#include "SiSPSeededTrackFinderData/SiDetElementRoadMakerData_xk.h"
#include <algorithm>

void InDet::SiDetElementRoadMakerData_xk::resetUsageTracker(){
    for (auto & outerVec : elementUsageTracker){    /// loop over detector regions 
        for (auto & innerVec : outerVec){           /// loop over layers in region
            std::for_each(innerVec.begin(), innerVec.end(), [](InDet::SiDetElementRoadMakerData_xk::UsedFlag & flag){flag.reset();});   /// loop over elements on layer
        }
    }
}