/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/********************************************************************
 *
 * NAME:     HLTCaloPreSelHypo.cxx
 * PACKAGE:  Trigger/TrigHypothesis/TrigTauHypo
 *
 * AUTHORS:   Marcelo Vogel (mavogel@cern.ch)
 *            Soshi Tsuno
 *            P.O. DeViveiros
 * CREATED:  July 18, 2013 
 *           April 30, 2014 (simplify & adapt to xAOD)
 *
 * DESCRIPTION:
 *
 * Implementation of calorimeter based pre-selection on
 * tau candidates starting from L1 seeds
 *
 *********************************************************************/

#include <iterator>
#include <sstream>
#include <bitset>

//
#include "TLorentzVector.h"

#include "GaudiKernel/IToolSvc.h"
#include "GaudiKernel/StatusCode.h"
#include "GaudiKernel/ListItem.h"
//
#include "TrigTauHypo/HLTCaloPreSelHypo.h"
//
#include "TrigSteeringEvent/TrigRoiDescriptor.h"
//
#include "CaloEvent/CaloCell.h"
//
#include "xAODJet/Jet.h"
#include "xAODJet/JetContainer.h"
#include "xAODCaloEvent/CaloCluster.h"
#include "xAODCaloEvent/CaloClusterContainer.h"

class ISvcLocator;

/////////////////////////////////////////////////////////////////////
// CONSTRUCTOR:
/////////////////////////////////////////////////////////////////////
//
HLTCaloPreSelHypo::HLTCaloPreSelHypo(const std::string& name, 
				     ISvcLocator* pSvcLocator):
  HLT::HypoAlgo(name, pSvcLocator),
  m_seedPt(-9999.9),
  m_fCore(-9999.9),
  m_hadRadius(9999.9),
  m_dRBaryCenterRoI(-9999.9)
  
{

  // Cluster-based pT cut
  declareProperty("LowerPtCut",           m_lowerPtCut=20000.0);
  declareProperty("ClusterEnergySumCone", m_clusterCone=0.2);

  // Cell-based shower requirement
  declareProperty("UseCellCut",        m_cellCut=1);
  declareProperty("CellVariableCore",  m_cellCore=0.1);
  declareProperty("CellVariableOuter", m_cellOuter=0.2);
  declareProperty("CoreFractionCut",   m_coreFractionCut=0.9);
  declareProperty("HadRadiusCut",      m_hadRadiusCut=0.2);

  declareMonitoredVariable("CutCounter"        , m_cutCounter);
  declareMonitoredVariable("seedPt"            , m_seedPt);
  declareMonitoredVariable("fCore"             , m_fCore);
  declareMonitoredVariable("hadRadius"         , m_hadRadius);
  declareMonitoredVariable("dRBaryCenterRoI"   , m_dRBaryCenterRoI);
}

/////////////////////////////////////////////////////////////////////
// DESTRUCTOR:
/////////////////////////////////////////////////////////////////////
//
HLTCaloPreSelHypo::~HLTCaloPreSelHypo()
{  
}

/////////////////////////////////////////////////////////////////////
// INITIALIZE:
/////////////////////////////////////////////////////////////////////

// ----------------------------------------------------------------------
HLT::ErrorCode HLTCaloPreSelHypo::hltInitialize()
// ----------------------------------------------------------------------
{
  
  msg() << MSG::INFO << "in initialize()" << endmsg;
  
  msg() << MSG::INFO << " REGTEST: HLTCaloPreSelHypo will cut on "                   << endmsg;
  msg() << MSG::INFO << " REGTEST: LowerPtCut " << m_lowerPtCut << endmsg;  
  msg() << MSG::INFO << " REGTEST: ------ "                                       << endmsg;


  msg() << MSG::INFO << "Initialization of EFHadCalibHypo completed successfully" << endmsg;
  return HLT::OK;
}

/////////////////////////////////////////////////////////////////////
// FINALIZE:
/////////////////////////////////////////////////////////////////////

// ----------------------------------------------------------------------
HLT::ErrorCode HLTCaloPreSelHypo::hltFinalize(){
// ----------------------------------------------------------------------
  
  msg() << MSG::INFO << "in finalize()" << endmsg;
  return HLT::OK;
}


/////////////////////////////////////////////////////////////////////
// EXECUTE:
/////////////////////////////////////////////////////////////////////

// ----------------------------------------------------------------------
HLT::ErrorCode HLTCaloPreSelHypo::hltExecute(const HLT::TriggerElement* inputTE, bool& pass){
// ----------------------------------------------------------------------
  
  // Get the messaging service, print where you are
  
  if( msgLvl() <= MSG::DEBUG )  
    msg() << MSG::DEBUG << "REGTEST:"<< name() << ": in execute()" << endmsg;
  
  // general reset
  pass = false;
  m_cutCounter = 0;

  //get RoI descriptor
  const TrigRoiDescriptor* roiDescriptor = 0;
  HLT::ErrorCode status = getFeature(inputTE, roiDescriptor);
  if ( status != HLT::OK || roiDescriptor == 0 ) {
    msg() <<  MSG::WARNING << " Failed to find RoiDescriptor " << endmsg;
    return status;
  }

  float roIEta = roiDescriptor->eta();
  float roIPhi = roiDescriptor->phi();


  //////////////////////////////////////////////////////////
  ///////////// PtCut implementation ///////////////////
  //////////////////////////////////////////////////////////

  // Retrieve Calocluster container
  std::vector<const xAOD::CaloClusterContainer*> vectorCaloClusterContainer;
  status = getFeatures(inputTE, vectorCaloClusterContainer);
  
  if(status!=HLT::OK ) {
    msg() << MSG::ERROR << " No CaloClusterContainers retrieved for the trigger element" << endmsg;
    return status;
  }
  
  if (vectorCaloClusterContainer.size() < 1) {
    msg() << MSG::ERROR
          << "  CaloCluster container is empty"
          << endmsg;
    return HLT::ERROR;
  }
  
  if( msgLvl() <= MSG::DEBUG )
    msg() << MSG::DEBUG << " CaloCluster container size is " << vectorCaloClusterContainer.size() << endmsg;
  
  // Grab the last cluster collection attached
  const xAOD::CaloClusterContainer* RoICaloClusterContainer = vectorCaloClusterContainer.back();
  
  if(RoICaloClusterContainer != NULL) {
    msg() << MSG::DEBUG
          << "REGTEST: Size of vector CaloCluster container is "
          << RoICaloClusterContainer->size()
          << endmsg;
    if(RoICaloClusterContainer->size()==0) {
      msg() << MSG::DEBUG
            << "Cannot proceed, size of vector CaloCluster container is "
            << RoICaloClusterContainer->size()
            << endmsg;
      //m_calo_errors.push_back(EmptyClustCont);
      return HLT::OK;
    }
  }
  else {
    msg() << MSG::ERROR << "no CaloCluster container found "<< endmsg;
    //m_calo_errors.push_back(NoClustCont);
    return HLT::ERROR;
  }

  // Use a jet to re-calculate quickly the new RoI center
  // Ideally we want to actually use fast-jet here in the case
  // we are provided with a fullscan collection (with anti-kT 0.4)
  // and then re-sum all constituents around 0.4 for the new RoI

  // The jet is not needed anymore, as we'll eventually use fastjet

  // xAOD::Jet aJet;
  // Now we just use the clusters directly

  // Make a minimal effort to speed things up ;)
  TLorentzVector myCluster;
  TLorentzVector TauBarycenter(0., 0., 0., 0.);
  

  xAOD::CaloClusterContainer::const_iterator clusterIt;
  for (clusterIt=RoICaloClusterContainer->begin(); clusterIt != RoICaloClusterContainer->end(); ++clusterIt) {
    if( msgLvl() <= MSG::DEBUG )
      msg()<< MSG::DEBUG <<" Cluster (e, eta, phi) : ("<< (*clusterIt)->e() << " , " <<(*clusterIt)->eta()<<" , "<<(*clusterIt)->phi()<< " )" << endmsg;
    
    if((*clusterIt)->e() < 0)
      {
	if( msgLvl() <= MSG::DEBUG )
	  msg()<< MSG::DEBUG <<" Negative energy cluster is rejected" << endmsg;
	continue;
      }
    
    myCluster.SetPtEtaPhiE((*clusterIt)->pt(), (*clusterIt)->eta(), (*clusterIt)->phi(), (*clusterIt)->e());
    //aJet.addConstituent(*clusterIt);
    
    TauBarycenter += myCluster;

  }
  
  if( msgLvl() <= MSG::DEBUG )
    msg() << MSG :: DEBUG << "Jet formed (pT, eta, phi, m): ("<< TauBarycenter.Pt()<<" , "<<TauBarycenter.Eta()<<" , "<<TauBarycenter.Phi()<<" , "<<TauBarycenter.M()<< " )" << endmsg;
  

  //TauBarycenter.SetPtEtaPhiM(aJet.pt(), aJet.eta(), aJet.phi(), aJet.m());

  // Determine the LC tau pT at detector axis
  TLorentzVector TauDetectorAxis(0.,0.,0.,0.);
  
  
  for (clusterIt=RoICaloClusterContainer->begin(); clusterIt != RoICaloClusterContainer->end(); ++clusterIt) {
    
    myCluster.SetPtEtaPhiE((*clusterIt)->pt(), (*clusterIt)->eta(), (*clusterIt)->phi(), (*clusterIt)->e());
    
    if(TauBarycenter.DeltaR(myCluster) > m_clusterCone)
      continue;
    
    TauDetectorAxis += myCluster;
  } // end loop on clusters

  m_seedPt = TauDetectorAxis.Pt();
  m_dRBaryCenterRoI = sqrt((roIEta-TauDetectorAxis.Eta())*(roIEta-TauDetectorAxis.Eta()) + (roIPhi-TauDetectorAxis.Phi())*(roIPhi-TauDetectorAxis.Phi()));
 
  // Offer quick shortcut if pT cut is not passed:
  if (m_seedPt < m_lowerPtCut)
    {
      if( msgLvl() <= MSG::DEBUG )
        msg() << MSG::DEBUG
              << " REGTEST: The cluster-based tau pT = " << m_seedPt
              << "did not pass the pt threshold: "
              << m_lowerPtCut
              << endmsg;
      return HLT::OK;
    } else if(m_cellCut==false)
    {
      // Not cutting on cells -> pass straight through
      pass = true;
      if( msgLvl() <= MSG::DEBUG )
	msg() << MSG::DEBUG << " REGTEST: TE accepted !! " << endmsg;
      return HLT::OK;
    }

  m_cutCounter++;

  //loop over the cells in the cluster collection
  
  std::bitset<200000> cellSeen;

  TLorentzVector myCell;
  float coreSum   =0;
  float outerSum  =0;
  float outerHad  =0;
  float outerHadW =0;
  
  for (clusterIt=RoICaloClusterContainer->begin(); clusterIt != RoICaloClusterContainer->end(); ++clusterIt)
    {
      
      CaloClusterCellLink::const_iterator firstcell = (*clusterIt)->getCellLinks()->begin();
      CaloClusterCellLink::const_iterator lastcell = (*clusterIt)->getCellLinks()->end();
      
      //loop over cells and calculate the variables
      for (; firstcell != lastcell; ++firstcell)
	{
	  // use hash code to prevent cell double counting
	  if (cellSeen.test((*firstcell)->caloDDE()->calo_hash()))
	    continue;
	  else
	    cellSeen.set((*firstcell)->caloDDE()->calo_hash());
	  
	  myCell.SetPtEtaPhiM((*firstcell)->et(), (*firstcell)->eta(), (*firstcell)->phi(), 0); 
    
	  if(TauBarycenter.DeltaR(myCell) < m_cellCore)
	    coreSum += myCell.Pt();
	  
	  if(TauBarycenter.DeltaR(myCell) < m_cellOuter)
	    outerSum += myCell.Pt();
	  
	  // Are we in the had calorimeter or 3rd layer of the EM?
	  if((*firstcell)->caloDDE()->getSampling() > 6 && (*firstcell)->caloDDE()->getSampling() < 20 && TauBarycenter.DeltaR(myCell) < m_cellOuter)
	    {
	      outerHadW += myCell.Pt()*TauBarycenter.DeltaR(myCell);
	      outerHad += myCell.Pt();
	    }
	}
    }
  
  if(outerSum >0)
    m_fCore = coreSum/outerSum;
  if(outerHad >0)
    m_hadRadius = outerHadW/outerHad;
  
  // Check the pre-selection shower requirements
  if(m_fCore < m_coreFractionCut || m_hadRadius > m_hadRadiusCut)
    {
      if( msgLvl() <= MSG::DEBUG )
        msg() << MSG::DEBUG
              << " REGTEST: The calo-based tau fCore = " << m_fCore
	      << " and hadRadius = " << m_hadRadius
              << " did not pass the thresholds: " << m_coreFractionCut << " , " << m_hadRadiusCut
              << endmsg;
      
      return HLT::OK;
    }
  
  // Success! All done...
  
  m_cutCounter++;
  pass = true;
  
  if( msgLvl() <= MSG::DEBUG )
    msg() << MSG::DEBUG << " REGTEST: TE accepted !! " << endmsg;
  
  return HLT::OK;
  
}
