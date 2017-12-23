#include <openpose/producer/imageFetcherSocket.hpp>

namespace op{

ImageFetcherSocket::ImageFetcherSocket(const std::string& ip, const int port):
	Producer{ProducerType::Socket}
{
	bConnected=false;
	firstConnection=true;
	numEmptyFrames=0;
	server.setIpAndPort(ip, port);
	if(server.startListen()){
		std::cout<<"Listening..."<<std::endl;
	}
	else{
		error("Socket error, exit!", __LINE__, __FUNCTION__, __FILE__);
	}
}

ImageFetcherSocket::~ImageFetcherSocket(){
	std::cout<<"Destroy ImageFetcherSocket"<<std::endl;	
}

cv::Mat ImageFetcherSocket::getRawFrame(){
	// cout<<"get frame"<<endl;
	if(firstConnection){
		*spIsBlocking= {true};
	}
	if(not bConnected){
		if(not server.isListening()){
			std::cout<<"Listening for another connection..."<<std::endl;
			if(not server.startListen()){
				std::cout<<"Listening error"<<std::endl;
				return cv::Mat(1,1,CV_8UC1);
			}
		}
		std::cout<<"accepting for another connection..."<<std::endl;
		if(server.startAccept()){
			bConnected=true;
			cout<<"client connect successfully"<<endl;
			if(firstConnection){
				firstConnection=false;
			}
			*spIsBlocking = {false};
		}
		else{
			std::cout<<"Accept error"<<std::endl;
			/*** Do not release the block ***/
			// *spIsBlocking = {false};
			return cv::Mat(1,1,CV_8UC1);	// listen and accept again after {N--} empty frames
		}
	}
	cv::Mat img=server.receiveFrame();
	if(img.empty()){
		// std::cout<<"Empty frame received from imageFetcherSocket"<<std::endl;
		++numEmptyFrames;
	}
	if(numEmptyFrames==10){
		bConnected=false;
		numEmptyFrames=0;
		std::cout<<"Empty frame received from imageFetcherSocket"<<std::endl;
	}
	return img;
}

}
