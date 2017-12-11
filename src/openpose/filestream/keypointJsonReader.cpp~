#include <openpose/filestream/keypointJsonReader.hpp>
#include <sstream>
#include <iostream>


std::string poseInfo="";

namespace op
{

	KeypointJsonReader::KeypointJsonReader()
	{
	}

	std::string KeypointJsonReader::read(const std::vector<std::pair<Array<float>, std::string>>& keypointVector, const bool humanReadable) const
	{
	  try
	  {
	    auto numberPeople=0;
	    poseInfo="";
	    std::stringstream ss;
	    for(auto i=0;i<keypointVector.size();++i)
	      numberPeople=std::max(numberPeople, keypointVector[i].first.getSize(0));
	    for(auto person=0;person<numberPeople; ++person){
	      for(auto i=0;i<keypointVector.size();++i){
		const auto& keypoints=keypointVector[i].first;
		const auto& keypointName=keypointVector[i].second;
		const auto numberBodyParts=keypoints.getSize(1);
		if(strcmp(keypointName.c_str(), "pose_keypoints")==0){
		    for (auto j=0;j<numberBodyParts;++j){
		      const auto index=3*(person*numberBodyParts+j);
		      ss<<keypoints[index]<<","<<keypoints[index+1]<<","<<keypoints[index+2];
		      if(j<numberBodyParts-1)  ss<<",";
		    }
		}
	      }
	      if(person<numberPeople-1)	ss<<";"; 
	    }
	    poseInfo=ss.str();
	    return poseInfo;
	  }
	  catch (const std::exception& e)
	  {
	    error(e.what(), __LINE__, __FUNCTION__, __FILE__);
	    return "";
	  }
	}


}
