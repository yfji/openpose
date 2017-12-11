#ifndef OPENPOSE_WRAPPER_WRAPPER_READER_HPP
#define OPENPOSE_WRAPPER_WRAPPER_READER_HPP

#include <openpose/core/common.hpp>
#include <openpose/filestream/enumClasses.hpp>
#include <iostream>

namespace op
{
    /**
     * WrapperStructOutput: Output (small GUI, writing rendered results and/or pose data, etc.) configuration struct.
     * WrapperStructOutput allows the user to set up the input frames generator.
     */
    struct OP_API WrapperReader
    {
        bool readKeypointJson;
        bool readKeypointJsonSocket;
	std::string serverAddr;
	int port;
	WrapperReader(const std::string& serverAddr="10.106.18.187", const int port =8011, const bool readKeyPointJson=false, const bool readKeypointJsonSocket=false);
    };
}

#endif
