#include <openpose/producer/imageFetcherSocket.hpp>

namespace op{

ImageFetcherSocket::ImageFetcherSocket(const std::string& ip, const int port):
	Producer{ProducerType::Socket}
{
	bConnected=false;
	numEmptyFrames=0;
	server.setIpAndPort(ip, port);
	if(server.startListen()){
		// if(server.startAccept()){
		// 	bConnected=true;
		// 	server.setConnected(true);
		// }
		std::cout<<"Listening..."<<std::endl;
		
	}
	else
		error("bind socket error, exit!", __LINE__, __FUNCTION__, __FILE__);
}

ImageFetcherSocket::~ImageFetcherSocket(){
	std::cout<<"Destroy ImageFetcherSocket"<<std::endl;	
}

cv::Mat ImageFetcherSocket::getRawFrame(){
	//cout<<"get frame"<<endl;
	if(not bConnected){
		std::cout<<"accepting for another connection..."<<std::endl;
		if(server.startAccept()){
			bConnected=true;
		}
		else{
			std::cout<<"Accept error"<<std::endl;
			return cv::Mat();
		}
	}
	cv::Mat img=server.receiveFrame();
	if(img.empty()){
		// std::cout<<"Empty frame received from imageFetcherSocket"<<std::endl;
		++numEmptyFrames;
	}
	if(numEmptyFrames==10){
		// sever.finishReceive();
		bConnected=false;
		numEmptyFrames=0;
		std::cout<<"Empty frame received from imageFetcherSocket"<<std::endl;
	}
	return img;
}

}
