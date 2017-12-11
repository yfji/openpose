#ifndef IMAGE_FETCHER_SOCKET_HPP
#define IMAGE_FETCHER_SOCKET_HPP

#include <opencv2/core/core.hpp> // cv::Mat
#include <opencv2/highgui/highgui.hpp> // cv::VideoCapture
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <openpose/core/common.hpp>
#include <openpose/producer/producer.hpp>
#include <openpose/producer/Server.hpp>

namespace op{

class OP_API ImageFetcherSocket : public Producer
{
public:
  	explicit ImageFetcherSocket(const std::string& ip, const int port);
	virtual ~ImageFetcherSocket();

	inline std::string getFrameName(){
		frameIndex=server.getFrameIndex();
		char frameName[20];
		sprintf(frameName, "frame_%d", frameIndex);
		return string(frameName);
  	}

public:
	inline bool isOpened() const{
		//return (bConnected==true);
		return true;
	}

	inline void release(){
		server.finishReceive();
		bConnected=false;
	}

	inline double get(const int capProperty){
		return 0;
	}

	inline void set(const int capProperty, const double value){
		;
	}

	inline double get(const ProducerProperty property){
		return 0.0;
	}

	inline void set(const ProducerProperty property, const double value){
		;
	}	

private:
  	bool bConnected;
	int frameIndex;
	int numEmptyFrames;
	bool firstConnection;
  	Server server;
  	inline bool isConnected(){
    	return server.isConnected();
  	}
  	cv::Mat getRawFrame();
};
  
}
#endif
