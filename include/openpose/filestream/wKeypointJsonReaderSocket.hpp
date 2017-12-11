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

    private:
        const std::shared_ptr<KeypointJsonReader> spKeypointJsonReader;

        DELETE_COPY(WKeypointJsonReaderSocket);

		public:
				inline void setIpAndPort(const std::string& ip, const int p){
					serverAddr=ip;
					port=p;
					std::cout<<"Server address: "<<serverAddr<<", port: "<<port<<std::endl;
				}
				bool connectServer();
				inline int isConnected(){
					return bConnected;
				}
				inline void sendMessage(const char* message){
					send(clientsd, message, strlen(message), 0);
				}
				void reset(const std::string& ip, const int p);
				inline void finishSend(){
					if(bConnected){
						#ifdef DEBUG
						log_file<<bConnected<<" "<<std::endl;
						#endif						
						sendMessage("stop");
						close(clientsd);
					  	bConnected=false;
					  	#ifdef DEBUG
					  	log_file<<"Socket closed successfully "<<std::endl;
					  	#endif
					}
				}
		private:
				int clientsd;
				std::string serverAddr;
				int port;
				int frameIndex;
				int numEmptyFrames;
				int numConnectTimes;
				char message[100];
				struct sockaddr_in server_socket;
				bool bConnected;
				bool firstFrameArrived;
				#ifdef DEBUG
				std::ofstream log_file;
				#endif
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
//		std::cout<<"create socket"<<std::endl;
			reset(ip, p);
			server_socket.sin_family=AF_INET;
			server_socket.sin_port=htons(port);
			server_socket.sin_addr.s_addr=inet_addr(serverAddr.c_str());
			if(connectServer()){
				std::cout<<"reader server connect successfully"<<std::endl;
			}
			else{
				std::cout<<"connect server error"<<std::endl;
			}
			#ifdef DEBUG
			log_file.open("/home/yfji/GitProjects/openpose-master_nonstop/log/run.log", std::ios::out);
			#endif
    }
		template<typename TDatums>
    WKeypointJsonReaderSocket<TDatums>::~WKeypointJsonReaderSocket(){
			finishSend();
			#ifdef DEBUG
			log_file.close();
			#endif
		}
    template<typename TDatums>
    void WKeypointJsonReaderSocket<TDatums>::initializationOnThread()
    {
    }
		template<typename TDatums>
    bool WKeypointJsonReaderSocket<TDatums>::connectServer(){
    			clientsd=socket(AF_INET, SOCK_STREAM, 0);
			if(clientsd==-1){
				#ifdef DEBUG
				log_file<<"Socket error: "<<clientsd<<std::endl;
				#endif
				// close(clientsd);
				return false;
			}
			if(connect(clientsd, (struct sockaddr*)(&server_socket), sizeof(server_socket))<0){
				close(clientsd);
				return false;
			}
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
				   	if(not bConnected){
				   		#ifdef DEBUG
				   		log_file<<serverAddr<<": "<<port<<std::endl;
				   		#endif
				   		if(not connectServer()){
				   			++numConnectTimes;
				   			usleep(10);
				   		}
				   		else{
				   			#ifdef DEBUG
				   			log_file<<"reader server connect successfully, why?"<<std::endl;
				   			#endif
				   			;
				   		}
				   		if(numConnectTimes==10){
				   			#ifdef DEBUG
				   			log_file<<"Cannot connect to keypoint server!"<<std::endl;
				   			#endif
				   			numConnectTimes=0;
				   		}
							if(not bConnected)
								return;
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
											int rn=recv(clientsd, message, sizeof(message),0);
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
		else if(tDatums==nullptr){
			#ifdef DEBUG
			log_file<<"Empty datum received from KeypointJsonReaderSocket "<<numEmptyFrames<<std::endl;
			#endif
			++numEmptyFrames;
		}
		if(numEmptyFrames==10){
			numEmptyFrames=0;
			finishSend();
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
