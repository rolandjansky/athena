/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef FPTRACKER_COLLIMATORDATA_H
#define FPTRACKER_COLLIMATORDATA_H

#include <vector>
#include <ostream>

namespace FPTracker{
  class ConfigData;
  class CollimatorData{
  public:
    static const int ncoll = 2;
    std::vector< std::vector<double> > coll_z;
    std::vector< std::vector<double> > coll_xap;
    std::vector< std::vector<int> >    coll_nmag;
    CollimatorData(const ConfigData&);
    std::string toString() const;
  };
  std::ostream& operator<<(std::ostream&, const CollimatorData&);
}
#endif
