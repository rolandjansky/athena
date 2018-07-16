//Dear emacs, this is -*-c++-*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef MUONALIGNMENTERRORDATA_H
#define MUONALIGNMENTERRORDATA_H

#include <vector>
#include <boost/regex.hpp>
#include "GeoPrimitives/GeoPrimitives.h"

namespace Trk {
  class RIO_OnTrack;
}

// Struct for per-Station Deviations Information 

struct deviationStr {
  deviationStr();
  ~deviationStr();
  double traslation;
  double rotation;
  boost::regex stationName;
  boost::regex multilayer;
};    

class MuonAlignmentErrorData {
  
  friend class MuonAlignmentErrorDbAlg; 
  
public:

  MuonAlignmentErrorData();
  virtual ~MuonAlignmentErrorData();
  void setVec(const std::vector<deviationStr> vec);
  void getVec(std::vector<deviationStr>& vec) const;
  void clearVec();
  
private:
  
  int m_test;
  std::vector<deviationStr> m_vec;
    
};

// https://twiki.cern.ch/twiki/bin/view/AtlasComputing/MultiThreadingConditionsAccess

//#include "AthenaKernel/CondCont.h"
//#include "SGTools/BaseInfo.h"
//CONDCONT_DEF( CondCont<MuonAlignmentErrorData>, 265772564 );

// http://acode-browser.usatlas.bnl.gov/lxr/source/athena/LArCalorimeter/LArCabling/LArCabling/LArOnOffIdMapping.h

#include "CLIDSvc/CLASS_DEF.h"
CLASS_DEF( MuonAlignmentErrorData, 115867308, 1)
#include "AthenaKernel/CondCont.h"
CLASS_DEF( CondCont<MuonAlignmentErrorData>, 265772564, 0)


#endif
