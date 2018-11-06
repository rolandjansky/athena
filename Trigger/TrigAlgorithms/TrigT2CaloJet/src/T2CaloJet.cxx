/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// ********************************************************************
//
// NAME:     T2CaloJet.cxx
// PACKAGE:  Trigger/TrigAlgorithms/TrigT2CaloJet
//
// AUTHOR:   Kyle Cranmer
// ********************************************************************


#include "GaudiKernel/IToolSvc.h"
//#include "GaudiKernel/StatusCode.h"

//#include "TrigT1Interfaces/RecEmTauRoI.h"
#include "TrigT1Interfaces/TrigT1Interfaces_ClassDEF.h"
#include "TrigSteeringEvent/TrigRoiDescriptor.h"

#include "TrigT2CaloJet/T2CaloJetBaseTool.h"
#include "TrigT2CaloJet/T2CaloJet.h"

#include "TrigCaloEvent/TrigT2Jet.h"
#include "TrigCaloEvent/TrigT2JetContainer.h"

#include "CaloGeoHelpers/CaloSampling.h"

#include "TrigT2CaloCommon/phiutils.h"

#include <math.h>

class ISvcLocator;

T2CaloJet::T2CaloJet(const std::string & name, ISvcLocator* pSvcLocator)
  : T2CaloBase(name, pSvcLocator),
    m_jet(0),
    m_layerInfo(0),
    m_tools(this),
    m_clearJetGrid(true),
    m_fillLayerInfo(false),
    m_timeLArUnpck(0),
    m_timeTileUnpck(0),
    m_timeHECUnpck(0)

{
  declareProperty("jetOutputKey",m_jetOutputKey = "TrigT2CaloJet");
  declareProperty("clusterOutputKey",m_clusterOutputKey = "TrigT2Calo");
  //  declareProperty("T2JetTools", m_toolNames);
  declareProperty("T2JetTools", m_tools, "list of Jet tools");
  declareProperty("clearJetGrid",m_clearJetGrid = true);
  declareProperty("fillLayerInfo",m_fillLayerInfo = false);
  declareProperty("PhiIDWidth", m_phiIDWidth);
  declareProperty("EtaIDWidth", m_etaIDWidth);

  declareMonitoredVariable("dR",  m_dR);
  declareMonitoredVariable("E",   m_e);
  declareMonitoredVariable("Et",  m_et);
  declareMonitoredVariable("Eem", m_eem0);
  declareMonitoredVariable("Ehad",m_ehad0);
  declareMonitoredVariable("Etem", m_etem0);
  declareMonitoredVariable("Ethad",m_ethad0);
  declareMonitoredVariable("Eta", m_eta);
  declareMonitoredVariable("Phi", m_phi);
  declareMonitoredVariable("ConversionErrors", m_conversionError);
  declareMonitoredVariable("AlgorithmErrors", m_algorithmError);
  m_roiEtaLimit = 4.8;
}

HLT::ErrorCode T2CaloJet::hltInitialize()
{

  if (timerSvc()) {
    m_timer.push_back(addTimer("TimeTot"));
  }

  // Create helper tools
  if ( m_tools.retrieve().isFailure() ) {
    msg() << MSG::ERROR << "Failed to retreive helper tools: " << m_tools << endmsg;
  } else {
    msg() << MSG::INFO << "Retrieved " << m_tools << endmsg;
  }

  if (timerSvc()) {
    ToolHandleArray< T2CaloJetBaseTool >::iterator itTool = m_tools.begin(),
      itToolEnd = m_tools.end();
    for (  ; itTool != itToolEnd; ++itTool ) {

      if(msgLvl() <= MSG::DEBUG)
	msg() << MSG::DEBUG <<"Tool with name " << (*itTool).name()<< endmsg;

      m_timer.push_back(addTimer((*itTool).name()));

      if(((*itTool).name() == "T2CaloJetGridFromCells")||((*itTool).name() == "T2CaloJetGridFromFEBHeader")){

	m_timeLArUnpck = (*itTool)->getTimer("LArUnpck");
	if(m_timeLArUnpck !=0){
	  declareMonitoredObject("LArUnpck",m_timeLArUnpck, 
				 &TrigTimer::lastElapsed);
	  if(msgLvl() <= MSG::DEBUG)
	    msg() << MSG::DEBUG << "Found LArUnpck timer. Added to monitoring "
		  << endmsg;
	}
	
	m_timeHECUnpck = (*itTool)->getTimer("HECUnpck");
	if(m_timeHECUnpck !=0){
	  declareMonitoredObject("HECUnpck",m_timeHECUnpck, 
				 &TrigTimer::lastElapsed);
	  if(msgLvl() <= MSG::DEBUG)
	    msg() << MSG::DEBUG << "Found HECUnpck timer. Added to monitoring "
		  << endmsg;
	}

	m_timeTileUnpck = (*itTool)->getTimer("TileUnpck");
	if(m_timeTileUnpck !=0){
	  declareMonitoredObject("TileUnpck",m_timeTileUnpck, 
				 &TrigTimer::lastElapsed);
	  if(msgLvl() <= MSG::DEBUG)
	    msg() << MSG::DEBUG << "Found HECUnpck timer. Added to monitoring "
		  << endmsg;
	}
      }
    }
  }


  return HLT::OK;
}

T2CaloJet::~T2CaloJet(){
}

HLT::ErrorCode T2CaloJet::hltExecute(const HLT::TriggerElement* inputTE,
				     HLT::TriggerElement* outputTE)
{

  if(timerSvc()) m_timer[0]->start();

  m_dR = -99;
  m_e = -99;
  m_et = -99;
  m_eem0 = -99;
  m_ehad0 = -99;
  m_etem0 = -99;
  m_ethad0 = -99;
  m_eta = -99;
  m_phi = -99;
  m_conversionError=0;
  m_algorithmError=0;

  // Retrieve store.
  // Very important!
  //  m_storeGate = getStore(); // not needed anymore

  if(msgLvl() <= MSG::DEBUG)
     msg() << MSG::DEBUG << "in execute()" << endmsg;

  // Some debug output:
  if(msgLvl() <= MSG::DEBUG) {
    msg() << MSG::DEBUG
	  << "outputTE->getId(): "
	  << outputTE->getId()
	  << endmsg;

    msg() << MSG::DEBUG
	  << "inputTE->getId(): "
	  << inputTE->getId()
	  << endmsg;
  }
  // Note: new TriggerElement has no label() anymore !


  // =========================
  //       retrieve RoI:
  // =========================
  const IRoiDescriptor* roiDescriptor = 0;
  HLT::ErrorCode hltStatus;

  const TrigRoiDescriptor* trigRoiDescriptor = 0;
  hltStatus = getFeature(inputTE, trigRoiDescriptor);
  roiDescriptor = trigRoiDescriptor;


  if ( hltStatus == HLT::OK ) {
    if(msgLvl() <= MSG::DEBUG)
      msg() << MSG::DEBUG  << *roiDescriptor << endmsg;
    
  } else {
    msg() <<  MSG::WARNING << " Failed to find RoiDescriptor " << endmsg;
    return hltStatus;
  }

  // Some debug output:
  if(msgLvl() <= MSG::DEBUG) {
    msg() << MSG::DEBUG  << "Message to count events. LVL1 phi="
	  << roiDescriptor->phi()
	  << " & LVL1 eta="
	  << roiDescriptor->eta()
	  << endmsg;
  }

  /*** obsolete 
  // setup ROI bounds
  double etamin = std::max(-1.0 * m_roiEtaLimit, roiDescriptor->eta0() - m_etaWidth);
  double etamax = std::min( 1.0 * m_roiEtaLimit, roiDescriptor->eta0() + m_etaWidth);
  double phimin = roiDescriptor->phi() - m_phiWidth;
  double phimax = roiDescriptor->phi() + m_phiWidth;
  while (phimin < 0) phimin += 2. * M_PI;
  while (phimax > 2 * M_PI) phimax -= 2. * M_PI;



  //Look at RoIs in FCal - to what end ??
  if(fabs(roiDescriptor->eta())>3.2){
    if(roiDescriptor->eta()<-3.2){
      etamin = -m_fcalEtaMax;
      etamax = -m_fcalEtaMin;
    }
    if(roiDescriptor->eta()>3.2){
      etamin = m_fcalEtaMin;
      etamax = m_fcalEtaMax;
    }
  }

  if(msgLvl() <= MSG::DEBUG) {
    msg() << MSG::DEBUG  << " etamin = "<< etamin << endmsg;
    msg() << MSG::DEBUG  << " etamax = "<< etamax << endmsg;
    msg() << MSG::DEBUG  << " phimin = "<< phimin << endmsg;
    msg() << MSG::DEBUG  << " phimax = "<< phimax << endmsg;
  }

  ***/

  if(msgLvl() <= MSG::DEBUG) {
    msg() << MSG::DEBUG  << " roi = "<< *roiDescriptor << endmsg;
  }

  // =========================
  //       make Jet
  // =========================
  //  if(status) status = makeJet(outputTE);

  if(msgLvl() <= MSG::DEBUG)
     msg() << MSG::DEBUG  << " Making TrigT2Jet "<< endmsg;

  m_jet = new TrigT2Jet();

  // jetContainer was only retrieved from StoreGate ... but never used
  //TrigT2JetContainer* jetContainer;

/*
  StatusCode sc;

  if(!getStore()->transientContains<TrigT2JetContainer>(m_jetOutputKey)) {
    if(msgLvl() <= MSG::DEBUG){
       msg() << MSG::DEBUG
           <<" Create jetObjectMinimal container"
           <<endmsg;
    }
    jetContainer = new TrigT2JetContainer();
    sc = m_storeGate->record(jetContainer,m_jetOutputKey);

    if (sc.isFailure())
    {
       msg() << MSG::ERROR
           << "Could not record TrigT2JetContainer in TDS"
           << endmsg;
    }
  }
  else{
      sc = m_storeGate->retrieve(jetContainer,m_jetOutputKey);
      if(sc.isFailure()){
       msg() << MSG::ERROR
           << "Could not retrieve TrigT2JetContainer " << m_jetOutputKey
           << "from SG. Exiting..."
           << endmsg;
       return false;
      }
  }
*/
  //jetContainer->push_back(m_jet);


  // Add RoI word to Jet
  m_jet->set_RoIword(roiDescriptor->roiWord());

  // Set eta,phi
  m_jet->setEta(roiDescriptor->eta());
  m_jet->setPhi(roiDescriptor->phi());


  // do loop over tools
  ToolHandleArray< T2CaloJetBaseTool >::iterator it = m_tools.begin(),
    itToolEnd = m_tools.end();
  int courant=1;
  uint32_t error = 0;
  for (  ; it != itToolEnd; ++it ) {
    if (timerSvc()) m_timer[courant]->start();
    ///    if ((*it)->execute(m_jet, etamin, etamax, phimin, phimax).isFailure()){
    if ((*it)->execute(m_jet, *roiDescriptor, caloDDENull ).isFailure()){
      msg() << MSG::WARNING << "T2CaloJet AlgToolJets returned Failure" << endmsg;
      return HLT::ErrorCode(HLT::Action::ABORT_CHAIN,HLT::Reason::USERDEF_1);
    }
    uint32_t in_error = (*it)->report_error();
    if ( 0x0FFFFFFF & in_error ) m_conversionError++;
    if ( 0xF0000000 & in_error ) m_algorithmError++;
    error|=in_error;
    if (timerSvc()) m_timer[courant]->stop();
    courant++;
  }

  if(msgLvl() <= MSG::DEBUG){
     msg() << MSG::DEBUG << " Values of T2CaloJet produced: " << endmsg;
     msg() << MSG::DEBUG << " REGTEST: Jet eta = " << m_jet->eta() << endmsg;
     msg() << MSG::DEBUG << " REGTEST: Jet phi = " << m_jet->phi() << endmsg;
     msg() << MSG::DEBUG << " REGTEST: Jet energy = " << m_jet->e() << endmsg;
  }


  double phia=roiDescriptor->phi();
  if(phia>M_PI) phia=phia-2*M_PI;

  double phib=m_jet->phi();
  if(phib>M_PI) phib=phib-2*M_PI;

  double dphi= phia-phib;
  if(dphi>M_PI) dphi-=2*M_PI;
  if(dphi<-M_PI) dphi+=2*M_PI;

  m_dR = sqrt( (roiDescriptor->eta()-m_jet->eta())*(roiDescriptor->eta()-m_jet->eta()) + dphi*dphi );
  
  m_e = m_jet->e();
  m_eta = m_jet->eta();
  m_phi = m_jet->phi();
  
  m_ehad0 = m_jet->ehad0();
  m_ethad0 = m_ehad0/cosh(m_eta);

  m_eem0 = m_jet->eem0();
  m_etem0 = m_eem0/cosh(m_eta);
  
  m_et = m_e/cosh(m_eta);

  // should probably set the delete the jet's vector of Trig3Momentum
  // here, and set pointer to 0.

  //   std::string jetKey = key2keyStore::getKey(m_jet);
  //   sc = m_storeGate->record( m_jet, jetKey, false, false );
  //   if (sc.isFailure()){
  //     msg() << MSG::ERROR << "Record of individual T2CaloJet into StoreGate failed" << endmsg;
  //     return false;
  //   };

  std::string key = "";

  hltStatus = recordAndAttachFeature(outputTE, m_jet, key, m_jetOutputKey);
  if (hltStatus != HLT::OK) {
    if(msgLvl() <= MSG::DEBUG) {
      msg() << MSG::ERROR << "Write of TrigT2Jet into outputTE failed"
	    << endmsg;
    }
    return hltStatus;
  }

  // =========================
  //   make TrigCaloCluster
  // =========================

  if(msgLvl() <= MSG::DEBUG){
     msg() << MSG::DEBUG << "fillLayerInfo is set to "<< m_fillLayerInfo << endmsg;
  }

  if (m_fillLayerInfo) {
    if(msgLvl() <= MSG::DEBUG){
       msg() << MSG::DEBUG << "Making TrigCaloCluster "<< endmsg;
    }
    m_layerInfo = new TrigCaloCluster();

    std::vector<Trig3Momentum>::iterator gridIt = m_jet->grid()->begin();
    std::vector<Trig3Momentum>::iterator gridEnd = m_jet->grid()->end();
    int nCells = 0;
    double eLayer[CaloSampling::Unknown+1] = {0.0};//0 is CaloSampling::PreSamplerB, 24 is CaloSampling::Unknown

    // Loop in the jet cells:
    for (; gridIt != gridEnd; ++gridIt){
      // consider only cells that are used in the cone:
      if(! (*gridIt).inCone()) continue;
      nCells++;
      // get layer energy
      const CaloSampling::CaloSample s = (*gridIt).caloSample();
      double cellE = (*gridIt).e();  
      eLayer[s] = eLayer[s] + cellE; 
    }

    // fill TrigCaloCluster
    m_layerInfo->setRawEnergy(m_jet->eem0()+m_jet->ehad0());
    m_layerInfo->setRawEt((m_jet->eem0()+m_jet->ehad0())/cosh(m_jet->eta()));
    m_layerInfo->setRawEta(m_jet->eta());
    m_layerInfo->setRawPhi(m_jet->phi());
    m_layerInfo->setRoIword(m_jet->RoIword());
    m_layerInfo->setNCells(nCells);
    for (int iSample=CaloSampling::PreSamplerB; iSample<CaloSampling::Unknown; ++iSample){
      const CaloSampling::CaloSample s = static_cast<CaloSampling::CaloSample> (iSample);
      m_layerInfo->setRawEnergy(s,eLayer[s]);
    }
 
    if(msgLvl() <= MSG::DEBUG){
      msg() << MSG::DEBUG << " Values of TrigCaloCluster produced: " << endmsg;
      msg() << MSG::DEBUG << " REGTEST: Cluster raw eta = " << m_layerInfo->rawEta() << endmsg;
      msg() << MSG::DEBUG << " REGTEST: Cluster raw phi = " << m_layerInfo->rawPhi() << endmsg;
      msg() << MSG::DEBUG << " REGTEST: Cluster raw energy = " << m_layerInfo->rawEnergy() << endmsg;
      msg() << MSG::DEBUG << " REGTEST: Cluster nb. of cells = " << m_layerInfo->nCells() << endmsg;
      double totalE=0;
      for (int iSample=CaloSampling::PreSamplerB; iSample<=CaloSampling::Unknown; ++iSample){
        const CaloSampling::CaloSample s = static_cast<CaloSampling::CaloSample> (iSample);
        double layerE=m_layerInfo->rawEnergy(s);
        totalE=totalE+layerE;
        msg() << MSG::DEBUG << " REGTEST: Layer = " << iSample << " raw energy = " << layerE;
      }
      msg() << MSG::DEBUG << " REGTEST: Total = " << totalE << endmsg;
    }

    // attach TrigCaloCluster
    hltStatus = recordAndAttachFeature(outputTE, m_layerInfo, key, m_clusterOutputKey);
    if (hltStatus != HLT::OK) {
      if(msgLvl() <= MSG::DEBUG) {
        msg() << MSG::ERROR << "Write of TrigCaloCluster into outputTE failed"
	      << endmsg;
      }
      return hltStatus;
    }
  }

  // =========================
  //       make OutputTE
  // =========================

  // Create a new RoiDescriptor with updated eta and phi.
  // Note that the steering will propagate l1Id and roiId automatically
  // so no need to set them.
  // How big is the RoI ?????
  TrigRoiDescriptor* newRoiDescriptor =
    //new TrigRoiDescriptor(roiDescriptor->l1Id(), roiDescriptor->roiId(),
    //		      m_jet->eta(), m_jet->phi());
    new TrigRoiDescriptor(roiDescriptor->roiWord(), roiDescriptor->l1Id(), roiDescriptor->roiId(),
			  m_jet->eta(), m_jet->eta()-m_etaIDWidth, m_jet->eta()+m_etaIDWidth,
			  m_jet->phi(), 
			  HLT::wrap_phi(m_jet->phi()-m_phiIDWidth), 
			  HLT::wrap_phi(m_jet->phi()+m_phiIDWidth) ); 
			  // We forward the roiWord to the EF (??)


  hltStatus = attachFeature(outputTE, newRoiDescriptor, m_jetOutputKey);

  if ( hltStatus != HLT::OK ) {
    msg() << MSG::ERROR << "Write of update TrigRoiDescriptor into outputTE failed"
	  << endmsg;
    return hltStatus;
  }

  if(msgLvl() <= MSG::DEBUG) {
    msg() << MSG::DEBUG  << "Recorded an RoiDescriptor " << *newRoiDescriptor << endmsg;
  }

  // Some debug output:
  if(msgLvl() <= MSG::DEBUG) {
    msg() << MSG::DEBUG
	  << "We assume success, set TE with Id "
	  << outputTE->getId()
	  << " active to signal positive result."
	  << endmsg;
  }


  if(m_clearJetGrid) m_jet->clearGrid();

  if (timerSvc()) {
    m_timer[0]->stop();
  }  

  return HLT::OK;
}


HLT::ErrorCode T2CaloJet::hltFinalize()
{
  if ( msgLvl() <= MSG::INFO )
    msg() << MSG::INFO << "in finalize()" << endmsg;

  return HLT::OK;
}
