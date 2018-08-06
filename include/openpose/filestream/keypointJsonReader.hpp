#ifndef OPENPOSE_FILESTREAM_KEYPOINT_JSON_READER_HPP
#define OPENPOSE_FILESTREAM_KEYPOINT_JSON_READER_HPP

#include <openpose/core/common.hpp>
#include <vector>

namespace op
{
    class OP_API KeypointJsonReader
    {
    public:
        KeypointJsonReader();

        std::string read(const std::vector<std::pair<Array<float>, std::string> >& keypointVector, const bool humanReadable = true) const;

    };
}

#endif // OPENPOSE_FILESTREAM_KEYPOINT_JSON_READER_HPP
