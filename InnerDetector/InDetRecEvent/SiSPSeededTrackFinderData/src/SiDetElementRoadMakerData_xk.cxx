#include "SiSPSeededTrackFinderData/SiDetElementRoadMakerData_xk.h"
#include <algorithm>

void InDet::SiDetElementRoadMakerData_xk::resetUsageTracker(){
    for (auto & outerVec : elementUsageTracker){    /// loop over detector regions 
        for (auto & innerVec : outerVec){           /// loop over layers in region
            std::fill(innerVec.begin(), innerVec.end(), false);   /// loop over elements on layer
        }
    }
}