// -*- C++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/**************************************************************************
 **
 **   File: Trigger/TrigHypothesis/TrigEammaHypo/TrigL2CaloLayersHypo.cxx
 **
 **   Description: Hypothesis algorithms to be run at Level 2 after T2Calo 
 **
 **   Authour: Denis Oliveira Damazio 
 **
 **   Created:      Sep 06 2010
 **
 **************************************************************************/ 

#include "TrigEgammaHypo/TrigL2CaloLayersHypo.h"

#include<cmath>
#include "TrigSteeringEvent/TrigRoiDescriptor.h"
//#include "TrigCaloEvent/TrigEMCluster.h"
#include "xAODTrigCalo/TrigEMCluster.h"

class ISvcLocator;


TrigL2CaloLayersHypo::TrigL2CaloLayersHypo(const std::string & name, ISvcLocator* pSvcLocator)
  : HLT::HypoAlgo(name, pSvcLocator)
{
  // Read cuts - should probably get these from an xml file
  declareProperty("AcceptAll",      m_acceptAll=true);
  declareProperty("EnergyFractionCut",          m_EnergyFracCut);
  declareProperty("AbsoluteEnergyCut",         m_EnergyAbsCut);

  // declare monitoring histograms for all cut variables
  declareMonitoredVariable("Eta", m_monEta);
  declareMonitoredVariable("Phi", m_monPhi);
  declareMonitoredVariable("Energy",m_Energy);
  declareMonitoredVariable("PreSampler_Energy",m_preSamp);
  declareMonitoredVariable("PreSampler_fracEnergy",m_preSampFrac);
  declareMonitoredVariable("CutCounter", m_PassedCuts);
  m_EnergyAbsCut.clear();
  m_EnergyFracCut.clear();
  for(int i=0;i<4;i++){
  	m_EnergyAbsCut.push_back(999999.0);
	m_EnergyFracCut.push_back(1.0);
  }
}


TrigL2CaloLayersHypo::~TrigL2CaloLayersHypo()
{ }


HLT::ErrorCode TrigL2CaloLayersHypo::hltInitialize()
{
  if ( msgLvl() <= MSG::DEBUG ) {
    msg() << MSG::DEBUG << "Initialization completed successfully"   << endmsg;
    msg() << MSG::DEBUG << "AcceptAll           = "
	<< (m_acceptAll==true ? "True" : "False") << endmsg; 
    msg() << MSG::DEBUG << "EnergyFractionCut per layer = " 
	<< m_EnergyFracCut << endmsg;  
    msg() << MSG::DEBUG << "AbsoluteEnergyCut per layer = " 
	<< m_EnergyAbsCut << endmsg;  
  }

  if ( m_EnergyFracCut.size() != 4 ) {
    msg() << MSG::ERROR << " EnergyFracCut size is " <<  m_EnergyFracCut.size() << " but needs 4" << endmsg;
    return HLT::BAD_JOB_SETUP;
  }
  
  if ( m_EnergyAbsCut.size() != 4 ) {
    msg() << MSG::ERROR << " EnergyAbsCut size is " <<  m_EnergyAbsCut.size() << " but needs 4" << endmsg;
    return HLT::BAD_JOB_SETUP;
  }
  
  return HLT::OK;
}

HLT::ErrorCode TrigL2CaloLayersHypo::hltFinalize()
{
  if ( msgLvl() <= MSG::INFO )
    msg() << MSG::INFO << "In TrigL2CaloLayersHypo::finalize()" << endmsg;

  return HLT::OK;
}


HLT::ErrorCode TrigL2CaloLayersHypo::hltExecute(const HLT::TriggerElement* outputTE, bool& pass)
{

  // Accept-All mode: temporary patch; should be done with force-accept 
  if (m_acceptAll) {
    pass = true;
    if ( msgLvl() <= MSG::DEBUG ) 
	msg() << MSG::DEBUG << "AcceptAll property is set: taking all events" 
	    << endmsg;
    return HLT::OK;
  } else {
    pass = false;
    if ( msgLvl() <= MSG::DEBUG ) {
      msg() << MSG::DEBUG << "AcceptAll property not set: applying selection" 
	  << endmsg;
    }
  }

  ///////////// get RoI descriptor ///////////////////////////////////////////////////////
  const TrigRoiDescriptor* roiDescriptor = 0;
  if (getFeature(outputTE, roiDescriptor) != HLT::OK) roiDescriptor = 0;

  if ( !roiDescriptor ) {
    ATH_MSG_WARNING("No RoI for this Trigger Element! ");
    return HLT::NAV_ERROR;
  }
  
  if ( fabs(roiDescriptor->eta() ) > 2.6 ) {
      ATH_MSG_DEBUG("The cluster had eta coordinates beyond the EM fiducial volume : " << roiDescriptor->eta() << "; stop the chain now");
      pass=false; // special case 
      return HLT::OK; 
  } 

  ATH_MSG_DEBUG( "Using outputTE("<< outputTE <<")->getId(): " << outputTE->getId()
          << "; RoI ID = "   << roiDescriptor->roiId()
          << ": Eta = "      << roiDescriptor->eta()
          << ", Phi = "      << roiDescriptor->phi());

  // fill local variables for RoI reference position
  
  // retrieve TrigEMCluster from the TE: must retrieve vector first
  std::vector< const xAOD::TrigEMCluster* > vectorOfClusters;  
  HLT::ErrorCode stat = getFeatures( outputTE, vectorOfClusters );

  if ( stat != HLT::OK ) {
    if ( msgLvl() <= MSG::WARNING)
      msg() << MSG::WARNING << "Failed to get TrigEMClusters" << endmsg;
 
    return HLT::OK;
  }

  if ( msgLvl() <= MSG::DEBUG ) {
    msg() << MSG::DEBUG << "Found vector with " << vectorOfClusters.size() 
	<< " TrigEMClusters" << endmsg;
  }
  
  // should be only 1 cluster, normally!
  if (vectorOfClusters.size() != 1) {
    msg() << MSG::DEBUG << "Size of vector of TrigEMClusters is not 1!" 
	<< endmsg;
    return HLT::OK;
  }

  // get cluster
  const xAOD::TrigEMCluster* pClus = vectorOfClusters.front();
  m_preSampFrac=m_preSamp=m_monEta=m_monPhi=m_Energy=-9999.0;

  if ( !pClus && (pClus->energy()>0.1) && (fabsf(pClus->eta())<2.1) ) {
    msg() << MSG::WARNING << "No EM cluster in RoI" << endmsg;
    return HLT::OK;
  }
  m_monEta = pClus->eta();
  m_monPhi = pClus->phi();

  // increment event counter 
  m_PassedCuts++; //// the ROI at least contais the cluster

  std::vector<double> fracs;
  for(int i=0;i<4;i++){
	fracs.push_back(0);
	fracs[i] = (pClus->energy( ((CaloSampling::CaloSample)i) )+pClus->energy( ((CaloSampling::CaloSample)(i+4)) ))/pClus->energy();
  }
  m_Energy = pClus->energy();

  if ( fracs[0] > m_EnergyFracCut[0] ) return HLT::OK;
  m_PassedCuts++; //// 
  m_preSampFrac = fracs[0];

  if ( (pClus->energy( ((CaloSampling::CaloSample)0) ) + pClus->energy( ((CaloSampling::CaloSample)4) ) ) > m_EnergyAbsCut[0] ) return HLT::OK;
  m_PassedCuts++; //// 
  m_preSamp = (fracs[0])*pClus->energy();

  // got this far => passed!
  pass = true;

  // Reach this point successfully  
  if ( msgLvl() <= MSG::DEBUG )
    msg() << MSG::DEBUG << "pass = " << pass << endmsg;

  return HLT::OK;
}

