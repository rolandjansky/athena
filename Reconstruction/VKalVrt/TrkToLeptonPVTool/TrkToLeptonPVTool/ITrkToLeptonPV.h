/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/
//
// ITrkToLeptonPV.h - Description
//
#ifndef ITrkToLeptonPV_H
#define ITrkToLepronPV_H

#include <vector>
#include <utility>
#include <memory>
#include "AthenaBaseComps/AthAlgTool.h"
#include "xAODTracking/TrackParticleContainer.h"
#include "xAODTracking/VertexContainer.h"
#include "xAODEventInfo/EventInfo.h"

//------------------------------------------------------------------------
  /** Interface for PV creation based on identified tracks */
  static const InterfaceID IID_TrkToLeptonPV("ITrkToLeptonPV", 1, 0);

  /**  @class ITrkToLeptonPV
  
   Interaface class for xAOD/DAOD tools to match a track to a vertex assumed to be a Primary Vertex.
   This PV can be taken elsewhere or obtained with the tool itsels.
   Example:  check compatibility of a track to PV obtained with 2 energetic leptons from Z decay.
   See README for more details

   xAOD::EventInfo object is needed only in case of DAOD, for xAOD case it can be omitted

    Author: Vadim Kostyukhin <vadim.kostyukhin@cern.ch>
  */

  class ITrkToLeptonPV : virtual public IAlgTool {
    public:
      static const InterfaceID& interfaceID() { return IID_TrkToLeptonPV;}
//---------------------------------------------------------------------------

      /** Method to match any tracks to a known Primary Vertex.
          A combined PV+track vertex is returned. Track compatibility to PV can be estimated with Chi2 of the PV+track vertex (NDF=2) 
      */
      virtual std::unique_ptr<xAOD::Vertex> matchTrkToPV( const xAOD::TrackParticle*, const xAOD::Vertex*, const xAOD::EventInfo* =0) const =0;
 
      /** Method to create a Primary Vertex using N(>=2) identified tracks (e.g. leptons) and beam constraint,
          The obtained N-track PV can be used as input for matchTrkToPV() to check another track.
	  However, better way to check another track compatibility  is to call again  npartVertex() with N+1 tracks and check the obtained Chi2 (NDF=2*N_trk-1)  
      */
      virtual std::unique_ptr<xAOD::Vertex> npartVertex(const std::vector<const xAOD::TrackParticle*>&, const xAOD::EventInfo * =0) const =0;
  };


#endif
