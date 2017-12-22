/*
 * Server.cpp
 *
 *  Created on: 2017年10月16日
 *      Author: yfji
 */

/*
 * Server.cpp
 *
 *      Author: yufeng
 */

#include "openpose/producer/Server.hpp"

extern std::string poseInfo;

Server::Server(const std::string& ip, int p) {
	reset(ip, p);
}

Server::Server(){
	listensd=socket(AF_INET, SOCK_STREAM, 0);
	if(listensd==-1){
		cout<<"Socket error"<<endl;
		return;
	}
}

Server::~Server() {
	// TODO Auto-generated destructor stub
	if(fullImageBuff!=NULL){
		delete fullImageBuff;
	}
	finishReceive();
}

void Server::setIpAndPort(const std::string& ip, int port){
	reset(ip, port);
}

void Server::reset(const std::string& ip, int p){
	totalSize=-1;
	fullImageBuff=NULL;
	port=p;
	inetAddr=ip;
	stop=0;
	frameIndex=0;
	listensd=-1;
	connState=-1;
	server_socket.sin_family=AF_INET;
	server_socket.sin_port=htons(port);
	server_socket.sin_addr.s_addr=inet_addr(inetAddr.c_str());
	cout<<"Image inetAddr: "<<inetAddr.c_str()<<", port: "<<port<<endl;
	hasImage=false;
	bConnected=false;
	bListening=false;
}

bool Server::startListen(){
	listensd=socket(AF_INET, SOCK_STREAM, 0);
	if(listensd==-1){
		cout<<"Socket error"<<endl;
		return false;
	}
	if(bind(listensd, (struct sockaddr*)(&server_socket), sizeof(server_socket))==-1){
		cout<<"Bind socket error"<<endl;
		close(listensd);
		return false;
	}
	if(listen(listensd, queue)==-1){
		cout<<"Listen port error"<<endl;
		close(listensd);
		return false;
	}
	cout<<"bind successfully"<<endl;
	bListening=true;
	return true;
}

bool Server::startAccept(){
	socklen_t length=sizeof(struct sockaddr_in);
	connState=accept(listensd, (struct sockaddr*)(&client_socket), &length);
	if(connState<0){
		bConnected=false;
		close(connState);
		close(listensd);
		bListening=false;
		return false;
	}
	bConnected=true;
	return true;
}

void Server::sendMessage(const char* message){
	int len=strlen(message);
	send(connState, message, len, 0);
}

cv::Mat Server::receiveFrame(){
	if(!bConnected){
		cout<<"No connection, exit"<<endl;
		return cv::Mat();
	}
/*
	if(frameIndex>0 and frameIndex%3!=0){
		//cout<<"empty frame "<<frameIndex<<endl;
		++frameIndex;
		return cv::Mat();
	}
*/
	char message[50];
	strcpy(message, "");
	sendMessage("frame");
	while(1){
		int rn=recv(connState, message, sizeof(message), 0);
		message[rn]='\0';
		if(message[0]=='s' && message[1]=='z'){
#ifdef COMPRESS
			char info[20];
			int i=3,offset=3;
			while(message[i]!='\0'){
				info[i-offset]=message[i];++i;
			}
			int t_size=atoi(info);
#else
			char info[3][20];
			int i=3, k=0, offset=3;
			while(message[i]!='\0'){
				if(message[i]==','){
					offset=i+1;++i;++k;
					continue;
				}
				info[k][i-offset]=message[i];
				++i;
			}
			width=atoi(info[0]);
			height=atoi(info[1]);
			channels=atoi(info[2]);
			int t_size=width*height*channels;
#endif
			if(totalSize!=t_size){
				totalSize=t_size;
				if(fullImageBuff!=NULL)
					delete fullImageBuff;
				fullImageBuff=new unsigned char[totalSize];
			}
			hasImage=true;
			sendMessage("sz");
			break;
		}
		else if(strcmp(message, "stop")==0){
			sendMessage("stop");
			close(connState);
			bConnected=false;
			return cv::Mat();
		}
		else if(strcmp(message, "empty")==0){
			return cv::Mat();
		}
	}
	int index=0;
	bool frameReceived=false;
	cv::Mat dataFrame;
	strcpy(message, "");

	while(not frameReceived){
#ifndef SIMPLE
		if(strcmp(message, "image")==0){
			sendMessage("image");
#endif
			int cnt=0;
			int total=0;
			index=0;
			while(1){
				int rn=recv(connState, fullImageBuff+total, min(totalSize,MAX_LEN), 0);
				//rn default 1448 or 2896, why?
				/*
				if(rn<=MAX_LEN){
					for(int k=index;k<rn+index;++k)
						fullImageBuff[k]=buff[k-index];
					index+=rn;
				}
				*/
				total+=rn;
				if(total==totalSize){
					frameReceived=true;
					break;
				}
				++cnt;
			}
#ifdef COMPRESS
			dataToRecover=vector<uchar>(fullImageBuff, fullImageBuff+totalSize);
			dataFrame=cv::imdecode(dataToRecover, 1);
#else
			dataFrame=cv::Mat(height,width,CV_8UC3,fullImageBuff);
#endif
			++frameIndex;
#ifndef SIMPLE
		}
#endif
		if(frameReceived)
			break;
		int rn=recv(connState,message,sizeof(message),0);
		message[rn]='\0';
		usleep(5);
	}
	return dataFrame;
}

string Server::receiveMessage(){
	strcpy(message, "");
	int rn=recv(connState, message, sizeof(message), 0);
	message[rn]='\0';
	return string(message);
}

void Server::finishReceive(){
	if(bConnected){
		sendMessage("stop");
	}
	bConnected=false;
	bListening=false;
	close(connState);
	close(listensd);
}

