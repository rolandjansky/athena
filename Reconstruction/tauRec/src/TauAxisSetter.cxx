/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "CLHEP/Vector/LorentzVector.h"
#include "CLHEP/Units/SystemOfUnits.h"
#include "FourMomUtils/P4Helpers.h"
#include "FourMom/P4EEtaPhiM.h"
#include "JetEvent/Jet.h"
#include "CaloEvent/CaloCluster.h"

//tau
#include "tauRec/TauCandidateData.h"

#include "xAODTau/TauJetContainer.h"
#include "xAODTau/TauJetAuxContainer.h"
#include "xAODTau/TauJet.h"

#include "CaloUtils/CaloVertexedCluster.h"

#include "tauRec/TauAxisSetter.h"

/********************************************************************/
TauAxisSetter::TauAxisSetter(const std::string& type,
        const std::string& name,
        const IInterface* parent):
TauToolBase(type, name, parent),
m_clusterCone(0.2),
m_doCellCorrection(false)
{
    declareInterface<TauToolBase > (this);
    declareProperty("ClusterCone", m_clusterCone);
    declareProperty("tauContainerKey", tauContainerKey = "TauJets");
    declareProperty("CellCorrection", m_doCellCorrection);
}

/********************************************************************/
TauAxisSetter::~TauAxisSetter() { }

/********************************************************************/
StatusCode TauAxisSetter::initialize()
{
    return StatusCode::SUCCESS;
}

StatusCode TauAxisSetter::eventInitialize(TauCandidateData * /*data*/) 
{
    return StatusCode::SUCCESS;
      
}

/********************************************************************/
StatusCode TauAxisSetter::execute(TauCandidateData *data)
{

  xAOD::TauJet *pTau = data->xAODTau;

    if (pTau == NULL) {
        ATH_MSG_ERROR("no candidate given");
        return StatusCode::FAILURE;
    }

    const xAOD::Jet* pJetSeed = (*pTau->jetLink());
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

	nConstituents++;
	tauDetectorAxis += tempClusterVector;
    }
    
    if  (nConstituents == 0)
      {
	ATH_MSG_WARNING("this tau candidate does not have any constituent clusters! breaking off tau tool chain and not recording this candidate!");
        return StatusCode::FAILURE;
      }

    ATH_MSG_VERBOSE("jet axis:" << (*pTau->jetLink())->pt()<< " " << (*pTau->jetLink())->eta() << " " << (*pTau->jetLink())->phi()  << " " << (*pTau->jetLink())->e() );
    // save values for detector axis.
    // FixMe: consider dropping these details variables as they are duplicated in the detector axis 4 vector
    pTau->setDetail(xAOD::TauJetParameters::LC_TES_precalib , static_cast<float>( tauDetectorAxis.Pt() ) );		  
    pTau->setDetail(xAOD::TauJetParameters::seedCalo_eta, static_cast<float>( tauDetectorAxis.Eta() ) );
    pTau->setDetail(xAOD::TauJetParameters::seedCalo_phi, static_cast<float>( tauDetectorAxis.Phi() ) );
    ATH_MSG_VERBOSE("detector axis:" << tauDetectorAxis.Pt()<< " " << tauDetectorAxis.Eta() << " " << tauDetectorAxis.Phi()  << " " << tauDetectorAxis.E() );

    // detectorAxis (set default) 
    pTau->setP4(tauDetectorAxis.Pt(), tauDetectorAxis.Eta(), tauDetectorAxis.Phi(), pTau->m());
    // save detectorAxis 
    pTau->setP4(xAOD::TauJetParameters::DetectorAxis, tauDetectorAxis.Pt(), tauDetectorAxis.Eta(), tauDetectorAxis.Phi(), tauDetectorAxis.M());

    ///////////////////////////////////////////////////////////////////////////
    // calculate tau intermediate axis (corrected for tau vertex)
    TLorentzVector tauInterAxis;
    
    for (cItr = pJetSeed->getConstituents().begin(); cItr != cItrE; ++cItr) {
      tempClusterVector.SetPtEtaPhiE( (*cItr)->pt(), (*cItr)->eta(), (*cItr)->phi(), (*cItr)->e() );
      if (BaryCenter.DeltaR(tempClusterVector) > m_clusterCone)
	continue;
      
      const xAOD::CaloCluster* cluster = dynamic_cast<const xAOD::CaloCluster*>( (*cItr)->rawConstituent() ); 
      if (!cluster) continue;
         
      if (pTau->vertexLink())
        tauInterAxis += xAOD::CaloVertexedCluster(*cluster, (*pTau->vertexLink())->position()).p4();
      else
        tauInterAxis += xAOD::CaloVertexedCluster(*cluster).p4();
    }

    // save values for tau intermediate axis
    // energy will be overwritten by EnergyCalibrationLC (if correctEnergy is enabled)
    // direction will be overwritten by EnergyCalibrationLC (if correctAxis is enabled)

    // intermediate axis( set default) 
    pTau->setP4(tauInterAxis.Pt(), tauInterAxis.Eta(), tauInterAxis.Phi(), pTau->m());

    ATH_MSG_VERBOSE("tau axis:" << tauInterAxis.Pt()<< " " << tauInterAxis.Eta() << " " << tauInterAxis.Phi()  << " " << tauInterAxis.E() );
    
    // save intermediateAxis 
    pTau->setP4(xAOD::TauJetParameters::IntermediateAxis, tauInterAxis.Pt(), tauInterAxis.Eta(), tauInterAxis.Phi(), tauInterAxis.M());

    return StatusCode::SUCCESS;
}

//-----------------------------------------------------------------------------
// Finalize
//-----------------------------------------------------------------------------
StatusCode TauAxisSetter::finalize()
{
    return StatusCode::SUCCESS;
}
