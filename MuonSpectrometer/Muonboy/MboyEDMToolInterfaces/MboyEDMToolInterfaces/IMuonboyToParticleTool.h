/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef IMuonboyToParticleTool_H
#define IMuonboyToParticleTool_H

#include "GaudiKernel/IAlgTool.h" 

//Can not be forwarded due to typedef
#include "TrkTrack/Track.h"
#include "TrkTrack/TrackCollection.h"

namespace Rec {
  class  TrackParticleContainer;
}

/////////////////////////////////////////////////////////////////////////////
#include "MboyEDMToolInterfaces/IMboyEDMToolMisc.h"

  /**
   @class IMuonboyToParticleTool

   This tools converts Muonboy tracks into Rec::TrackParticle
   
  @author samusog@cern.ch
  
  */

static const InterfaceID IID_IIMuonboyToParticleTool("IMuonboyToParticleTool", 1, 0);

class IMuonboyToParticleTool :virtual public IAlgTool {

public:
///////////////////////////////////

   static const InterfaceID& interfaceID();

   /** Convert collection */
   virtual StatusCode  convertCollection(
                                 const TrackCollection*       pTrackCollection,
                                 Rec::TrackParticleContainer* pTrackParticleContainer
                                ) = 0;

   /** Convert single object */
   virtual Rec::TrackParticle* convert(
                               const Trk::Track* pTrack
			      ) = 0;

};

inline const InterfaceID& IMuonboyToParticleTool::interfaceID() { return IID_IIMuonboyToParticleTool; }

#endif 




