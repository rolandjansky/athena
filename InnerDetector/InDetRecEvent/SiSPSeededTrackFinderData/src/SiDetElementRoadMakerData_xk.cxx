#include "SiSPSeededTrackFinderData/SiDetElementRoadMakerData_xk.h"
#include <algorithm>

void InDet::SiDetElementRoadMakerData_xk::resetUsageTracker(){
    for (auto & outerVec : elementUsageTracker){
        for (auto & innerVec : outerVec){
            std::for_each(innerVec.begin(), innerVec.end(), [](InDet::SiDetElementRoadMakerData_xk::UsedFlag & flag){flag.reset();});
        }
    }
}