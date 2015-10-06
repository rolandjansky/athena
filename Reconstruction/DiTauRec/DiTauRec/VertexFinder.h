/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef DITAUREC_VERTEXFINDER_H
#define DITAUREC_VERTEXFINDER_H

#include "DiTauToolBase.h"
#include "JetEDM/TrackVertexAssociation.h"

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

 virtual StatusCode initialize();

 virtual StatusCode execute(DiTauCandidateData * data);

 virtual StatusCode eventFinalize(DiTauCandidateData *data);

 ElementLink<xAOD::VertexContainer> getPV_TJVA(const xAOD::DiTauJet*, const xAOD::VertexContainer*);

 float getJetVertexFraction(const xAOD::Vertex*, const std::vector<const xAOD::TrackParticle*>&, const jet::TrackVertexAssociation*) const;

 virtual void cleanup(DiTauCandidateData *) { }


private:
	std::string m_primVtxContainerName;
	std::string m_assocTracksName;
	std::string m_trackVertexAssocName;
	float m_maxJVF;


};

#endif  /* VERTEXFINDER_H */
