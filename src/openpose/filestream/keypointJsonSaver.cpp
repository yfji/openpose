#include <openpose/filestream/fileStream.hpp>
#include <openpose/filestream/keypointJsonSaver.hpp>
#include <iostream>
using namespace std;

namespace op
{
    KeypointJsonSaver::KeypointJsonSaver(const std::string& directoryPath) :
        FileSaver{directoryPath}
    {
    }

    void KeypointJsonSaver::save(const std::vector<std::pair<Array<float>, std::string>>& keypointVector,
                                 const std::string& fileName, const bool humanReadable) const
    {
        try
        {
            // Record json
            const auto finalFileName = getNextFileName(fileName) + ".json";
            saveKeypointsJson(keypointVector, finalFileName, humanReadable);
	    //Array<float> keypoints=keypointVector.first;
	    string msg=keypointVector[0].second;
	    cout<<msg<<endl;
            
        }
        catch (const std::exception& e)
        {
            error(e.what(), __LINE__, __FUNCTION__, __FILE__);
        }
    }
}
