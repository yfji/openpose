#ifndef SRC_SERVER_H_
#define SRC_SERVER_H_
#include <iostream>
#include <vector>
#include <fstream>
#include <sys/socket.h>
#include <sys/types.h>
#include <stdlib.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <string.h>
#include <opencv2/opencv.hpp>

#define IMAGE_W	1080
#define IMAGE_H	1080

#define MAX_LEN 65536
#define INETADDR  "10.106.20.8"

#define COMPRESS
#define SIMPLE

using namespace std;


class Server {
public:
	Server(string ip, int p);
	Server();
	virtual ~Server();
	
	//static std::vector<std::string> poseInfo;
private:
	int listensd;
	int connState;
	int width;
	int height;
	int channels;
	int totalSize;
	const int queue=10;
	int port;
  int frameIndex;
  string inetAddr;
	struct sockaddr_in server_socket;
	struct sockaddr_in client_socket;
	unsigned char buff[MAX_LEN];
	char message[100];
	unsigned char* fullImageBuff;
	vector<uchar> dataToRecover;
	bool hasImage;
  bool bConnected;
	char stop;

public:
	void setIpAndPort(string ip, int port);
	bool startListen();
	bool startAccept();
  inline int getFrameIndex(){
    return frameIndex;
  }
  inline int isConnected(){
    return bConnected;
  }
	inline void setConnected(bool c){
		bConnected=c;
	}
	void sendMessage(const char* message);
	cv::Mat receiveFrame();
	string receiveMessage();
  void finishReceive();
  void reset(string ip, int p);
};

//static void* onImageProcess(void* args);
//static void* onInfoBackfeed(void* args);
#endif /* SRC_SERVER_H_ */

