/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/
//
// ITrkToLeptonPV.h - Description
//
/*
   Interaface for xAOD/DAOD tools to match a track to a vertex assumed to be a Primary Vertex.
   This PV can be taken elsewhere or obtained with the tool itsels.
   Example:  check compatibility of a track to PV obtained with 2 energetic leptons from Z decay.
   See README fro more details

    Author: Vadim Kostyukhin
    e-mail: vadim.kostyukhin@cern.ch
*/
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
  static const InterfaceID IID_TrkToLeptonPV("ITrkToLeptonPV", 1, 0);

  class ITrkToLeptonPV : virtual public IAlgTool {
    public:
      static const InterfaceID& interfaceID() { return IID_TrkToLeptonPV;}
//---------------------------------------------------------------------------
//Interface itself

      virtual std::unique_ptr<const xAOD::Vertex> matchTrkToPV( const xAOD::TrackParticle*, const xAOD::Vertex*, const xAOD::EventInfo*) const =0;
      virtual std::unique_ptr<const xAOD::Vertex> npartVertex(const std::vector<const xAOD::TrackParticle*>&, const xAOD::EventInfo *) const =0;
  };


#endif
