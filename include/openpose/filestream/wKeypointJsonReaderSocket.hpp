//#ifndef OPENPOSE_FILESTREAM_W_KEYPOINT_JSON_READER_SOCKET_HPP
//#define OPENPOSE_FILESTREAM_W_KEYPOINT_JSON_READER_SOCKET_HPP

#include <openpose/core/common.hpp>
#include <openpose/filestream/keypointJsonReader.hpp>
#include <openpose/thread/workerConsumer.hpp>
#include <iostream>
#include <sys/socket.h>
#include <sys/types.h>
#include <stdlib.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <string.h>
#include <mutex>
#include <fstream>
//#define DEBUG

namespace op
{
    template<typename TDatums>
    class WKeypointJsonReaderSocket : public WorkerConsumer<TDatums>
    {
    public:
        explicit WKeypointJsonReaderSocket(const std::shared_ptr<KeypointJsonReader>& keypointJsonReader, const std::string& ip, const int p);
				virtual ~WKeypointJsonReaderSocket();

        void initializationOnThread();

        void workConsumer(const TDatums& tDatums);
        
       	inline void setBlockingSharedPtr(const std::shared_ptr<std::atomic<bool>>& isBlockingSharedPtr){
       		spIsBlocking=isBlockingSharedPtr;
       	}

    private:
        const std::shared_ptr<KeypointJsonReader> spKeypointJsonReader;

        DELETE_COPY(WKeypointJsonReaderSocket);

		public:
				inline void setIpAndPort(const std::string& ip, const int p){
					inetAddr=ip;
					port=p;
				}
				bool startListen();
				bool startAccept();
				inline int isConnected(){
					return bConnected;
				}
				inline void sendMessage(const char* message){
					send(connState, message, strlen(message), 0);
				}
				inline void acceptError(){
					bConnected=false;
					close(connState);
					close(listensd);
					bListening=false;
				}
				void reset(const std::string& ip, const int p);
				inline void finishSend(){
					if(bConnected){
#ifdef DEBUG
						log_file<<bConnected<<" "<<std::endl;
#endif						
						sendMessage("stop");
						close(connState);
					  	bConnected=false;
#ifdef DEBUG
					  	log_file<<"Socket closed successfully "<<std::endl;
#endif
					}
				}
		private:
				int listensd;
				int connState;
				std::string inetAddr;
				int port;
				int frameIndex;
				int numEmptyFrames;
				int numConnectTimes;
				char message[100];
				char handcheckBuff[5];
				char responseBuff[5];
				const int cmd_len=4;
				const char* handcheckMessage="yuge";
				const char* responseMessage="data";
				struct sockaddr_in server_socket;
				struct sockaddr_in client_socket;
				bool bConnected;
				bool bListening;
				bool firstFrameArrived;
#ifdef DEBUG
				std::ofstream log_file;
#endif
				
				std::shared_ptr<std::atomic<bool>> spIsBlocking;
    };
}


// Implementation
#include <openpose/utilities/pointerContainer.hpp>
namespace op
{
    template<typename TDatums>
    WKeypointJsonReaderSocket<TDatums>::WKeypointJsonReaderSocket(const std::shared_ptr<KeypointJsonReader>& keypointJsonReader, const std::string& ip, const int p) :
    spKeypointJsonReader{keypointJsonReader}
    {
		reset(ip, p);
		server_socket.sin_family=AF_INET;
		server_socket.sin_port=htons(port);
		server_socket.sin_addr.s_addr=inet_addr(inetAddr.c_str());
		if(startListen()){
			std::cout<<"Listening for reader"<<std::endl;
		}
		else{
			std::cout<<"Listening for reader error"<<std::endl;
		}
#ifdef DEBUG
		log_file.open("/home/yfji/GitProjects/openpose-master_blockable/log/run.log", std::ios::out);
#endif
    }
	template<typename TDatums>
    WKeypointJsonReaderSocket<TDatums>::~WKeypointJsonReaderSocket(){
		finishSend();
#ifdef DEBUG
		log_file.close();
#endif
		close(listensd);
	}
    template<typename TDatums>
    void WKeypointJsonReaderSocket<TDatums>::initializationOnThread()
    {
    }
	template<typename TDatums>
    bool WKeypointJsonReaderSocket<TDatums>::startListen(){
	    int on=1;
		listensd=socket(AF_INET, SOCK_STREAM, 0);
		int ret=setsockopt(listensd, SOL_SOCKET, SO_REUSEADDR, &on, sizeof(on));
		if(ret==-1){
			return false;
		}
		if(listensd==-1){
			std::cout<<"Reader socket error: "<<std::endl;
			return false;
		}
		if(bind(listensd, (struct sockaddr*)(&server_socket), sizeof(server_socket))==-1){
			std::cout<<"Bind reader socket error: "<<std::endl;
			if(errno==EADDRINUSE){
				std::cout<<"Reader port in use"<<std::endl;
			}
			close(listensd);
			return false;
		}
		if(listen(listensd, 10)==-1){
			std::cout<<"Reader listen error"<<std::endl;
			close(listensd);
			return false;
		}
		bListening=true;
		return true;
	}

	template<typename TDatums>
  bool WKeypointJsonReaderSocket<TDatums>::startAccept(){
    struct timeval timeout={1,0};
  	socklen_t length=sizeof(struct sockaddr_in);
  	connState=accept(listensd, (struct sockaddr*)(&client_socket), &length);
  	int ret=setsockopt(connState, SOL_SOCKET, SO_RCVTIMEO, (char*)(&timeout), sizeof(timeout));
	if(ret==-1){
		std::cout<<"Setsockopt error"<<std::endl;
		acceptError();
		return false;
	}
  	if(connState<0){
  		acceptError();
  		return false;
	}
	int rn=recv(connState, handcheckBuff, cmd_len, MSG_WAITALL);
	if(rn<=0 or errno==11){
		std::cout<<"Reader time out, handcheck failed"<<std::endl;
		acceptError();
		return false;
	}
	handcheckBuff[rn]='\0';
	if(strcmp(handcheckBuff, handcheckMessage)!=0){
		std::cout<<"Reader command error, handcheck failed"<<std::endl;
		acceptError();
		return false;
	}
	sendMessage(handcheckMessage);
	bConnected=true;
	return true;
  }
  
	template<typename TDatums>
	void WKeypointJsonReaderSocket<TDatums>::reset(const std::string& ip, const int p){
		setIpAndPort(ip,p);
		frameIndex=0;
		numEmptyFrames=0;
		numConnectTimes=0;
		bConnected=false;
		bListening=false;
		firstFrameArrived=false;
		listensd=-1;
		connState=-1;
		std::cout<<"Pose data inetAddr: "<<inetAddr.c_str()<<", port: "<<port<<std::endl;
	}
  template<typename TDatums>
  void WKeypointJsonReaderSocket<TDatums>::workConsumer(const TDatums& tDatums)
  {
      try
      {
      	// std::cout<<"Socket running"<<std::endl;
			 	if(not bConnected){
			 		if(not bListening){
			 			std::cout<<"Reader listening for another connection..."<<std::endl;
						if(not startListen()){
							std::cout<<"Reader listening error"<<std::endl;
							sleep(1);
							return;
						}
					}
					std::cout<<"Reader accepting for another connection..."<<std::endl;
					if(startAccept()){
						bConnected=true;
						std::cout<<"Reader socket connect successfully"<<std::endl;
					}
					else{
						std::cout<<"Reader socket accept error"<<std::endl;
						close(listensd);
						bListening=false;
						return;
					}
				}
		    if (checkNoNullNorEmpty(tDatums))
		    {
		        dLog("", Priority::Low, __LINE__, __FUNCTION__, __FILE__);
		        const auto profilerKey = Profiler::timerInit(__LINE__, __FUNCTION__, __FILE__);
		        const bool humanReadable = true;
		        for (auto i = 0u ; i < tDatums->size() ; i++)
		        {
		          const auto& tDatum = (*tDatums)[i];

		          const std::vector<std::pair<Array<float>, std::string>> keypointVector{
		              std::make_pair(tDatum.poseKeypoints, "pose_keypoints"),
		              std::make_pair(tDatum.faceKeypoints, "face_keypoints"),
		              std::make_pair(tDatum.handKeypoints[0], "hand_left_keypoints"),
		              std::make_pair(tDatum.handKeypoints[1], "hand_right_keypoints")
		        };
						if(keypointVector[0].first.getSize(0)>0){
							firstFrameArrived=true;
		    			std::string poseInfo=spKeypointJsonReader->read(keypointVector, humanReadable);
							if(bConnected){ //frameIndex>2 and and frameIndex%5==0 
								sendMessage(poseInfo.c_str());
							}
						}
						else{
							sendMessage("nop");
						}
						while(1){
							int rn=recv(connState, responseBuff, cmd_len,MSG_WAITALL);
							if(rn<=0 or errno==11){
								std::cout<<"Wait response timeout or network error"<<std::endl;
								break;
							}
							responseBuff[rn]='\0';
							if(strcmp(responseBuff, responseMessage)==0)	break;
							usleep(5);
						}
		      }	
					++frameIndex;
		      Profiler::timerEnd(profilerKey);
		      Profiler::printAveragedTimeMsOnIterationX(profilerKey, __LINE__, __FUNCTION__, __FILE__);
		      // Debugging log
		      dLog("", Priority::Low, __LINE__, __FUNCTION__, __FILE__);
		  }
			else if(tDatums!=nullptr and tDatums->size()==0){
	#ifdef DEBUG
				log_file<<"Empty datum received from KeypointJsonReaderSocket "<<numEmptyFrames<<std::endl;
	#endif
				// std::cout<<"Empty datum"<<std::endl;
				++numEmptyFrames;
			}
			if(numEmptyFrames==10){
				numEmptyFrames=0;
				finishSend();
				*spIsBlocking= true;
				usleep(20);
			}
		}
		catch (const std::exception& e)
		{
	    this->stop();
	    error(e.what(), __LINE__, __FUNCTION__, __FILE__);
		}
	}

  COMPILE_TEMPLATE_DATUM(WKeypointJsonReaderSocket);
}

//#endif // OPENPOSE_FILESTREAM_W_KEYPOINT_JSON_READER_SOCKET_HPP
