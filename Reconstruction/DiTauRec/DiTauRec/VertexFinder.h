/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#ifndef DITAUREC_VERTEXFINDER_H
#define DITAUREC_VERTEXFINDER_H

#include "DiTauToolBase.h"
#include "JetEDM/TrackVertexAssociation.h"
#include "xAODTracking/VertexContainer.h"
#include "StoreGate/ReadHandleKey.h"

class VertexFinder : public DiTauToolBase {
public:

 //-------------------------------------------------------------
 //! Constructor
 //-------------------------------------------------------------
 VertexFinder(const std::string& type,
         const std::string& name,
         const IInterface * parent);

 //-------------------------------------------------------------
 //! Destructor
 //-------------------------------------------------------------
 virtual ~VertexFinder();

 virtual StatusCode initialize() override;

 virtual StatusCode execute(DiTauCandidateData * data,
                            const EventContext& ctx) const override;

 ElementLink<xAOD::VertexContainer> getPV_TJVA(const xAOD::DiTauJet*, const xAOD::VertexContainer*,
                                               float& maxJVF,
                                               const EventContext& ctx) const;

 float getJetVertexFraction(const xAOD::Vertex*, const std::vector<const xAOD::TrackParticle*>&, const jet::TrackVertexAssociation*) const;

 virtual void cleanup(DiTauCandidateData *) { }


private:
 SG::ReadHandleKey<xAOD::VertexContainer> m_primVtxContainerName
 { this, "PrimVtxContainerName", "PrimaryVertices", "" };
 std::string m_assocTracksName;
 SG::ReadHandleKey<jet::TrackVertexAssociation> m_trackVertexAssocName
 { this, "TrackVertexAssociation", "JetTrackVtxAssoc_forDiTaus", "" };
};

#endif  /* VERTEXFINDER_H */
