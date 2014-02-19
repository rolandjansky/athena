/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "FPTracker/CollimatorData.h"
#include "FPTracker/DataDumperHelpers.h"
#include "FPTracker/ConfigData.h"
#include "FPTracker/FPTrackerConstants.h"
#include <sstream>

namespace FPTracker{
  CollimatorData::CollimatorData(const ConfigData& cData): 
    coll_z   (std::vector<std::vector<double> >(2, std::vector<double>(ncoll))),
    coll_xap (std::vector<std::vector<double> >(2, std::vector<double>(ncoll))),
    coll_nmag(std::vector<std::vector<int> >   (2, std::vector<int>(ncoll))){
    // absolute values, collimator front faces
    coll_z[beam1][0] = 148.9900;
    coll_z[beam1][1] = 183.6170;
    coll_z[beam2][0] = 148.9900;
    coll_z[beam2][1] = 183.5240;
    coll_xap[beam1][0] = cData.xcol1;
    coll_xap[beam1][1] = cData.xcol2;
    coll_xap[beam2][0] = cData.xcol1;
    coll_xap[beam2][1] = cData.xcol2;
  }

  std::string CollimatorData::toString() const {
    std::ostringstream ost;
    ost<<"ncoll "<<ncoll<<'\n';
    ost<<"coll_z\n";
    ost<<array2DToString(coll_z)<<'\n';
    ost<<"coll_xap\n";
    ost<<array2DToString(coll_xap)<<'\n';
    ost<<"coll_nmag\n";
    ost<<array2DToString(coll_nmag)<<'\n';
    return ost.str();
  } 
  std::ostream& operator<<(std::ostream& os, const CollimatorData& cData){
    os<<cData.toString();
    return os;
  }
}
