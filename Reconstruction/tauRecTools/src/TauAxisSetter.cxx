/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#ifndef XAOD_ANALYSIS
#include "xAODTau/TauJetContainer.h"
#include "xAODTau/TauJetAuxContainer.h"
#include "xAODTau/TauJet.h"

#include "CaloUtils/CaloVertexedCluster.h"

#include "TauAxisSetter.h"
#include "tauRecTools/HelperFunctions.h"

/********************************************************************/
TauAxisSetter::TauAxisSetter(const std::string& name) :
TauRecToolBase(name) {
}

/********************************************************************/
TauAxisSetter::~TauAxisSetter() { }

/********************************************************************/
StatusCode TauAxisSetter::initialize()
{
    return StatusCode::SUCCESS;
}

/********************************************************************/
StatusCode TauAxisSetter::execute(xAOD::TauJet& pTau)
{

    const xAOD::Jet* pJetSeed = (*pTau.jetLink());
    if (!pJetSeed) {
        ATH_MSG_ERROR("Tau jet link is invalid.");
        return StatusCode::FAILURE;
    }

    xAOD::JetConstituentVector::const_iterator cItr = pJetSeed->getConstituents().begin();
    xAOD::JetConstituentVector::const_iterator cItrE = pJetSeed->getConstituents().end();

    ///////////////////////////////////////////////////////////////////////////
    //calculate barycenter
    TLorentzVector sumAllClusterVector;
    TLorentzVector tempClusterVector;
    for (; cItr != cItrE; ++cItr) {
      tempClusterVector.SetPtEtaPhiE( (*cItr)->pt(), (*cItr)->eta(), (*cItr)->phi(), (*cItr)->e() );

      sumAllClusterVector += tempClusterVector;
    }
    TLorentzVector BaryCenter; 
    BaryCenter.SetPtEtaPhiM(1., sumAllClusterVector.Eta(), sumAllClusterVector.Phi(), 0.);

    ATH_MSG_DEBUG("barycenter:" << BaryCenter.Pt()<< " " << BaryCenter.Eta() << " " << BaryCenter.Phi()  << " " << BaryCenter.E() );
    
    ///////////////////////////////////////////////////////////////////////////
    // calculate detector axis
    TLorentzVector tauDetectorAxis;
    // count number of constituents in core cone. could be zero!
    int nConstituents = 0;
    for (cItr = pJetSeed->getConstituents().begin(); cItr != cItrE; ++cItr) {
	tempClusterVector.SetPtEtaPhiE( (*cItr)->pt(), (*cItr)->eta(), (*cItr)->phi(), (*cItr)->e() );

	ATH_MSG_VERBOSE("cluster in detector axis loop:" << (*cItr)->pt()<< " " << (*cItr)->eta() << " " << (*cItr)->phi()  << " " << (*cItr)->e() );
	ATH_MSG_VERBOSE("delta R is " << BaryCenter.DeltaR(tempClusterVector) );

    if (BaryCenter.DeltaR(tempClusterVector) > m_clusterCone) continue;

	ElementLink<xAOD::IParticleContainer> linkToCluster;
	linkToCluster.toContainedElement( *(static_cast<const xAOD::IParticleContainer*> ((*cItr)->rawConstituent()->container())), (*cItr)->rawConstituent() );
	pTau.addClusterLink(linkToCluster);

	nConstituents++;
	tauDetectorAxis += tempClusterVector;
    }
    
    if  ( 0 == nConstituents )
    {
	    ATH_MSG_DEBUG("this tau candidate does not have any constituent clusters!");
	    return StatusCode::FAILURE;
    }
    
    // save values for detector axis.
    ATH_MSG_DEBUG("detector axis:" << tauDetectorAxis.Pt()<< " " << tauDetectorAxis.Eta() << " " << tauDetectorAxis.Phi()  << " " << tauDetectorAxis.E() );
    pTau.setP4(tauDetectorAxis.Pt(), tauDetectorAxis.Eta(), tauDetectorAxis.Phi(), pTau.m());
    pTau.setP4(xAOD::TauJetParameters::DetectorAxis, tauDetectorAxis.Pt(), tauDetectorAxis.Eta(), tauDetectorAxis.Phi(), tauDetectorAxis.M());

    ///////////////////////////////////////////////////////////////////////////
    // calculate tau intermediate axis (corrected for tau vertex), only used by offline
    if(m_doVertexCorrection)
    {
	TLorentzVector tauInterAxis;

	std::vector<const xAOD::CaloCluster*> clusterList;
	ATH_CHECK(tauRecTools::GetJetClusterList(pJetSeed, clusterList, m_incShowerSubtr, BaryCenter, m_clusterCone));
	for (auto cluster : clusterList){
	  if (pTau.vertexLink())
	    tauInterAxis += xAOD::CaloVertexedCluster(*cluster, (*pTau.vertexLink())->position()).p4();
	  else
	    tauInterAxis += xAOD::CaloVertexedCluster(*cluster).p4();
	}

	// save values for intermediate axis 
	ATH_MSG_DEBUG("tau axis:" << tauInterAxis.Pt()<< " " << tauInterAxis.Eta() << " " << tauInterAxis.Phi()  << " " << tauInterAxis.E() );
        pTau.setP4(tauInterAxis.Pt(), tauInterAxis.Eta(), tauInterAxis.Phi(), pTau.m());
	pTau.setP4(xAOD::TauJetParameters::IntermediateAxis, tauInterAxis.Pt(), tauInterAxis.Eta(), tauInterAxis.Phi(), tauInterAxis.M());
    
    }
    
    return StatusCode::SUCCESS;
}

//-----------------------------------------------------------------------------
// Finalize
//-----------------------------------------------------------------------------
StatusCode TauAxisSetter::finalize()
{
    return StatusCode::SUCCESS;
}
#endif
