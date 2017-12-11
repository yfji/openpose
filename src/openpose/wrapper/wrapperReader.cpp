#include <openpose/wrapper/wrapperReader.hpp>

namespace op
{
  
    WrapperReader::WrapperReader(const std::string& serverAddr_, const int port_, const bool readKeypointJson_, const bool readKeypointJsonSocket_):
	serverAddr{serverAddr_},
	port{port_},
	readKeypointJson{readKeypointJson_},
	readKeypointJsonSocket{readKeypointJsonSocket_}
    {}
}
