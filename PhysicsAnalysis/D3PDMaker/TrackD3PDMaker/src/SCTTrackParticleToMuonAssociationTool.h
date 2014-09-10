/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/**
 * @file D3PDMaker/TrackD3PDMaker/src/SCTTrackParticleToMuonAssociationTool.cxx
 * @contact <laura.rehnisch@cern.ch>
 * @date Nov, 2012
 * @brief setting up an association of trackparticles
 *        and muons.
 */
#ifndef TRACKD3PDMAKER_SCTTRACKPARTICLETOMUONASSOCIATIONTOOL_H
#define TRACKD3PDMAKER_SCTTRACKPARTICLETOMUONASSOCIATIONTOOL_H
 

#include "D3PDMakerUtils/SingleAssociationTool.h"


namespace Analysis {
  class Muon;
}

namespace Rec {
  class TrackParticle;
}

namespace D3PD {
 
  class SCTTrackParticleToMuonAssociationTool
    : public SingleAssociationTool<Rec::TrackParticle, Analysis::Muon>
    {
    public:
      typedef SingleAssociationTool<Rec::TrackParticle, Analysis::Muon> Base; 

      SCTTrackParticleToMuonAssociationTool (const std::string& type,
                                             const std::string& name,
                                             const IInterface* parent);
 
      virtual StatusCode initialize();
      virtual StatusCode book();


      virtual const Analysis::Muon* get (const Rec::TrackParticle& trackparticle);

    private:
      // StoreGate keys
      std::string m_muonContainer;

    }; // class SCTTrackParticleToMuonAssociationTool
 
} // namespace D3PD
 
#endif // not TRACKD3PDMAKER_SCTTRACKPARTICLETOMUONASSOCIATIONTOOL_H

