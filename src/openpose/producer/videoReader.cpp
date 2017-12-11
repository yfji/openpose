#include <openpose/utilities/fileSystem.hpp>
#include <openpose/producer/videoReader.hpp>
#include <iostream>

namespace op
{
    VideoReader::VideoReader(const std::string & videoPath) :
        VideoCaptureReader{videoPath, ProducerType::Video},
        mPathName{getFileNameNoExtension(videoPath)}
    {
    }

    std::string VideoReader::getFrameName()
    {
        try
        {
	    std::string name=mPathName + "_" + VideoCaptureReader::getFrameName();
	    std::cout<<name<<std::endl;
            return name;
        }
        catch (const std::exception& e)
        {
            error(e.what(), __LINE__, __FUNCTION__, __FILE__);
            return "";
        }
    }

    cv::Mat VideoReader::getRawFrame()
    {
        try
        {
            return VideoCaptureReader::getRawFrame();
        }
        catch (const std::exception& e)
        {
            error(e.what(), __LINE__, __FUNCTION__, __FILE__);
            return cv::Mat();
        }
    }
}
