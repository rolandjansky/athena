/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#ifndef XAOD_ANALYSIS
#include "tauRecTools/TauEventData.h"

#include "xAODTau/TauJetContainer.h"
#include "xAODTau/TauJetAuxContainer.h"
#include "xAODTau/TauJet.h"

#include "CaloUtils/CaloVertexedCluster.h"

#include "TauAxisSetter.h"

/********************************************************************/
TauAxisSetter::TauAxisSetter(const std::string& name) :
TauRecToolBase(name),
m_clusterCone(0.2),
m_doVertexCorrection(true)
{
    declareProperty("ClusterCone", m_clusterCone);
    declareProperty("VertexCorrection", m_doVertexCorrection = true);
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
        ATH_MSG_WARNING("tau does not have jet seed for LC calibration");
        return StatusCode::SUCCESS;
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

    
    ///////////////////////////////////////////////////////////////////////////
    // calculate detector axis
    TLorentzVector tauDetectorAxis;
    // count number of constituents in core cone. could be zero!
    int nConstituents = 0;
    for (cItr = pJetSeed->getConstituents().begin(); cItr != cItrE; ++cItr) {
	tempClusterVector.SetPtEtaPhiE( (*cItr)->pt(), (*cItr)->eta(), (*cItr)->phi(), (*cItr)->e() );

	ATH_MSG_VERBOSE("cluster in detector axis loop:" << (*cItr)->pt()<< " " << (*cItr)->eta() << " " << (*cItr)->phi()  << " " << (*cItr)->e() );
	ATH_MSG_VERBOSE("delta R is " << BaryCenter.DeltaR(tempClusterVector) );

        if (BaryCenter.DeltaR(tempClusterVector) > m_clusterCone)
            continue;

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

    ATH_MSG_VERBOSE("jet axis:" << (*pTau.jetLink())->pt()<< " " << (*pTau.jetLink())->eta() << " " << (*pTau.jetLink())->phi()  << " " << (*pTau.jetLink())->e() );
    // save values for detector axis.
    ATH_MSG_VERBOSE("detector axis:" << tauDetectorAxis.Pt()<< " " << tauDetectorAxis.Eta() << " " << tauDetectorAxis.Phi()  << " " << tauDetectorAxis.E() );

    // detectorAxis (set default) 
    pTau.setP4(tauDetectorAxis.Pt(), tauDetectorAxis.Eta(), tauDetectorAxis.Phi(), pTau.m());
    // save detectorAxis 
    pTau.setP4(xAOD::TauJetParameters::DetectorAxis, tauDetectorAxis.Pt(), tauDetectorAxis.Eta(), tauDetectorAxis.Phi(), tauDetectorAxis.M());

    ///////////////////////////////////////////////////////////////////////////
    // calculate tau intermediate axis (corrected for tau vertex)
    // not needed at trigger level
    if(m_doVertexCorrection)
      {
	TLorentzVector tauInterAxis;
	
	for (cItr = pJetSeed->getConstituents().begin(); cItr != cItrE; ++cItr) {
	  tempClusterVector.SetPtEtaPhiE( (*cItr)->pt(), (*cItr)->eta(), (*cItr)->phi(), (*cItr)->e() );
	  if (BaryCenter.DeltaR(tempClusterVector) > m_clusterCone)
	    continue;
	  
	  const xAOD::CaloCluster* cluster = dynamic_cast<const xAOD::CaloCluster*>( (*cItr)->rawConstituent() ); 
	  if (!cluster) continue;
	  
	  if (pTau.vertexLink())
	    tauInterAxis += xAOD::CaloVertexedCluster(*cluster, (*pTau.vertexLink())->position()).p4();
	  else
	    tauInterAxis += xAOD::CaloVertexedCluster(*cluster).p4();
	}
	
	// save values for tau intermediate axis
	// energy will be overwritten by EnergyCalibrationLC (if correctEnergy is enabled)
	// direction will be overwritten by EnergyCalibrationLC (if correctAxis is enabled)
	
	// intermediate axis( set default) 
	pTau.setP4(tauInterAxis.Pt(), tauInterAxis.Eta(), tauInterAxis.Phi(), pTau.m());
	
	ATH_MSG_VERBOSE("tau axis:" << tauInterAxis.Pt()<< " " << tauInterAxis.Eta() << " " << tauInterAxis.Phi()  << " " << tauInterAxis.E() );
	
	// save intermediateAxis 
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
