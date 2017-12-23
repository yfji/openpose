#include <openpose/wrapper/wrapperReader.hpp>

namespace op
{
  
    WrapperReader::WrapperReader(const std::string& inetAddr_, const int port_, const bool readKeypointJson_, const bool readKeypointJsonSocket_):
	inetAddr{inetAddr_},
	port{port_},
	readKeypointJson{readKeypointJson_},
	readKeypointJsonSocket{readKeypointJsonSocket_}
    {}
}
