/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef XAOD_ANALYSIS
//#include "CLHEP/Vector/LorentzVector.h"
//#include "CLHEP/Units/SystemOfUnits.h"
//#include "FourMomUtils/P4Helpers.h"
//#include "FourMom/P4EEtaPhiM.h"
//#include "JetEvent/Jet.h"
//#include "CaloEvent/CaloCluster.h"

//tau
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
m_doCellCorrection(false),
m_doAxisCorrection(true)
{
    declareProperty("ClusterCone", m_clusterCone);
    declareProperty("tauContainerKey", m_tauContainerKey = "TauJets");
    declareProperty("CellCorrection", m_doCellCorrection);
    declareProperty("AxisCorrection", m_doAxisCorrection = true);
}

/********************************************************************/
TauAxisSetter::~TauAxisSetter() { }

/********************************************************************/
StatusCode TauAxisSetter::initialize()
{
    return StatusCode::SUCCESS;
}

StatusCode TauAxisSetter::eventInitialize() 
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
    
    if  (nConstituents == 0)
      {
	StatusCode sc;
	bool isCosmics = false;
	if (tauEventData()->hasObject("IsCosmics?")) {
	  sc = tauEventData()->getObject("IsCosmics?", isCosmics);
	}
	 
	// If running cosmic triggers, don't worry about not having clusters in tau
	if(sc.isSuccess() && tauEventData()->inTrigger() && isCosmics){
	  ATH_MSG_WARNING("this tau candidate does not have any constituent clusters! breaking off tau tool chain and not recording this candidate!");
	} else {
	  ATH_MSG_DEBUG("this tau candidate does not have any constituent clusters! breaking off tau tool chain and not recording this candidate!");
	}
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
    if(m_doAxisCorrection)
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
