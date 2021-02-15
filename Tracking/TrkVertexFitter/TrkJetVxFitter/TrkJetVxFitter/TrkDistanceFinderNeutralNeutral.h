/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#ifndef DISTNEUNEU_H
#define DISTNEUNEU_H

#include "AthenaBaseComps/AthAlgTool.h"
#include <vector>

#include "TrkParameters/TrackParameters.h"
#include "TrkJetVxFitter/NeutralTrack.h"

static const InterfaceID IID_TrkDistanceFinderNeutralNeutral("TrkDistanceFinderNeutralNeutral", 1,0);

namespace Trk {

class TrkDistanceFinderNeutralNeutral : public AthAlgTool
{

  
 public:
  TrkDistanceFinderNeutralNeutral(const std::string& t, const std::string& n, const IInterface*  p);
  ~TrkDistanceFinderNeutralNeutral();
  
  std::pair<Amg::Vector3D,double> getPointAndDistance(const Trk::NeutralTrack&,const Trk::NeutralTrack&,
						    double &) const;
  
  static const InterfaceID& interfaceID()
    {
      return IID_TrkDistanceFinderNeutralNeutral;
    };

};

//no error possible: an analytic solution is possible here!
//namespace Error {
//  struct NewtonProblem {
//    const char* p;
//    NewtonProblem(const char* q) {p=q; }
//  };	
//}

}
#endif
