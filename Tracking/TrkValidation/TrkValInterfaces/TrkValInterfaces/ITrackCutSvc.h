/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

//////////////////////////////////////////////////////////////////
// ITrackCutSvcr.h
///////////////////////////////////////////////////////////////////
// (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////

#ifndef TRK_ITRACKCUTSVC_H
#define TRK_ITRACKCUTSVC_H

#include "GaudiKernel/IInterface.h"

namespace Trk {

struct TrackCuts {
    double maxD0;
    double maxZ0;
    double maxZ0sinTheta;
    double maxEta;
    int    minSiHits;
    int    minPixelHits; 
    int    minSCTHits;

    TrackCuts() :
      maxD0(2.),
      maxZ0(250.),
      maxZ0sinTheta(1.5),
      maxEta(4.),
      minSiHits(9),
      minPixelHits(0),
      minSCTHits(0)
   {}
       
};

static const InterfaceID IID_ITrackCutSvc("ITrackCutSvcs",1,0);

/** @class ITrackTCutSvc
    @brief services that allows to centrally modify track cuts
    for ITk 1.6 reconstructions
*/

class ITrackCutSvc : virtual public IInterface {
  public:
    /**Interface ID, declared here, and defined below*/
    static const InterfaceID& interfaceID();
    
    virtual TrackCuts trackCuts() const = 0;

  };

inline const InterfaceID& Trk::ITrackCutSvc::interfaceID() {
    return Trk::IID_ITrackCutSvc;
}

} // end namespace

#endif
