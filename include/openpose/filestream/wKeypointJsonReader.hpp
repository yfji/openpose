//#ifndef OPENPOSE_FILESTREAM_W_KEYPOINT_JSON_READER_HPP
//#define OPENPOSE_FILESTREAM_W_KEYPOINT_JSON_READER_HPP

#include <openpose/core/common.hpp>
#include <openpose/filestream/keypointJsonReader.hpp>
#include <openpose/thread/workerConsumer.hpp>
#include <stdio.h>

namespace op
{
    template<typename TDatums>
    class WKeypointJsonReader : public WorkerConsumer<TDatums>
    {
    public:
        explicit WKeypointJsonReader(const std::shared_ptr<KeypointJsonReader>& keypointJsonReader);

        void initializationOnThread();

        void workConsumer(const TDatums& tDatums);

    private:
        const std::shared_ptr<KeypointJsonReader> spKeypointJsonReader;

        DELETE_COPY(WKeypointJsonReader);
    };
}


// Implementation
#include <openpose/utilities/pointerContainer.hpp>
namespace op
{
    template<typename TDatums>
    WKeypointJsonReader<TDatums>::WKeypointJsonReader(const std::shared_ptr<KeypointJsonReader>& keypointJsonReader) :
        spKeypointJsonReader{keypointJsonReader}
    {
    }

    template<typename TDatums>
    void WKeypointJsonReader<TDatums>::initializationOnThread()
    {
    }

    template<typename TDatums>
    void WKeypointJsonReader<TDatums>::workConsumer(const TDatums& tDatums)
    {
        try
        {
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
                    // Read and send keypoints
                    spKeypointJsonReader->read(keypointVector, humanReadable);
                }
                Profiler::timerEnd(profilerKey);
                Profiler::printAveragedTimeMsOnIterationX(profilerKey, __LINE__, __FUNCTION__, __FILE__);
                // Debugging log
                dLog("", Priority::Low, __LINE__, __FUNCTION__, __FILE__);
            }
        }
        catch (const std::exception& e)
        {
            this->stop();
            error(e.what(), __LINE__, __FUNCTION__, __FILE__);
        }
    }

    COMPILE_TEMPLATE_DATUM(WKeypointJsonReader);
}

//#endif // OPENPOSE_FILESTREAM_W_KEYPOINT_JSON_READER_HPP
