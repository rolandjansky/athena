/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#ifndef DISTNEUCHARGED_H
#define DISTNEUCHARGED_H

#include "AthenaBaseComps/AthAlgTool.h"
#include <vector>
#include "TrkParameters/TrackParameters.h"
#include "GaudiKernel/ToolHandle.h"
#include "TrkJetVxFitter/NeutralTrack.h"

static const InterfaceID IID_TrkDistanceFinderNeutralCharged("TrkDistanceFinderNeutralCharged", 1,0);

namespace MagField {
   class AtlasFieldCache;
}

namespace Trk {

class TrkDistanceFinderNeutralCharged : public AthAlgTool
{

  
 public:
  TrkDistanceFinderNeutralCharged(const std::string& t, const std::string& n, const IInterface*  p);
  ~TrkDistanceFinderNeutralCharged();

  std::pair<Amg::Vector3D,double> getPointAndDistance(const Trk::NeutralTrack&,const Trk::Perigee&,
                                                      double &,
                                                      MagField::AtlasFieldCache &fieldCache) const;

  static const InterfaceID& interfaceID()
    {
      return IID_TrkDistanceFinderNeutralCharged;
    };

    StatusCode initialize();
    StatusCode finalize();

 private:

  //parameters for precision
  double m_precision;//as job option
  double m_maxloopnumber;//as job option
};

namespace Error {
  struct NewtonProblem {
    const char* p;
    NewtonProblem(const char* q) {p=q; }
  };	
}

}
#endif
