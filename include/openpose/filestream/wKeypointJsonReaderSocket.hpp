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
					serverAddr=ip;
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
				void reset(const std::string& ip, const int p);
				inline void finishSend(){
					if(bConnected){
						#ifdef DEBUG
						log_file<<bConnected<<" "<<std::endl;
						#endif						
						sendMessage("stop");
						usleep(20);
					  	bConnected=false;
					  	#ifdef DEBUG
					  	log_file<<"Socket closed successfully "<<std::endl;
					  	#endif
					}
				}
		private:
				int listensd;
				int connState;
				std::string serverAddr;
				int port;
				int frameIndex;
				int numEmptyFrames;
				int numConnectTimes;
				char message[100];
				struct sockaddr_in server_socket;
				struct sockaddr_in client_socket;
				bool bConnected;
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
		server_socket.sin_addr.s_addr=inet_addr(serverAddr.c_str());
		if(startListen()){
			std::cout<<"Listening for reader"<<std::endl;
		}
		else{
			std::cout<<"Listening for reader error"<<std::endl;
		}
#ifdef DEBUG
		log_file.open("/home/yfji/GitProjects/openpose-master_nogui/log/run.log", std::ios::out);
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
		listensd=socket(AF_INET, SOCK_STREAM, 0);
		if(listensd==-1){
			std::cout<<"Socket error: "<<std::endl;
			return false;
		}
		if(bind(listensd, (struct sockaddr*)(&server_socket), sizeof(server_socket))==-1){
			std::cout<<"Bind reader socket error: "<<std::endl;
			close(listensd);
			return false;
		}
		if(listen(listensd, 10)==-1){
			std::cout<<"Listen error"<<std::endl;
			close(listensd);
		}
		return true;
	}

	template<typename TDatums>
    bool WKeypointJsonReaderSocket<TDatums>::startAccept(){
    	socklen_t length=sizeof(struct sockaddr_in);
    	connState=accept(listensd, (struct sockaddr*)(&client_socket), &length);
    	if(connState<0){
    		std::cout<<"Reader socket accept error"<<std::endl;
    		return false;
		}
		std::cout<<"Reader socket connect successfully"<<std::endl;
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
		firstFrameArrived=false;
	}
    template<typename TDatums>
    void WKeypointJsonReaderSocket<TDatums>::workConsumer(const TDatums& tDatums)
    {
        try
        {
        	// std::cout<<"Socket running"<<std::endl;
		   	if(not bConnected){
				std::cout<<"Reader accepting for another connection..."<<std::endl;
				if(startAccept()){
					bConnected=true;
				}
				else{
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
  // Read and send keypoints
      std::string poseInfo=spKeypointJsonReader->read(keypointVector, humanReadable);
						if(bConnected){ //frameIndex>2 and and frameIndex%5==0 
							sendMessage(poseInfo.c_str());
							//usleep(1);
							//sendMessage("99.9");
						}
					}
					else{
						sendMessage("nop");
					}
					while(1){
						int rn=recv(connState, message, sizeof(message),0);
						message[rn]='\0';
						if(strcmp(message, "data")==0)	break;
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
