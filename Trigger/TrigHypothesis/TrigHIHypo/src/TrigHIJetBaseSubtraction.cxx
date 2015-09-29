/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "TrigSteeringEvent/TrigRoiDescriptor.h"
#include "TrigCaloEvent/TrigT2Jet.h"
#include "TrigHIJetBaseSubtraction.h"
#include "TrigT1Interfaces/RecEnergyRoI.h"



using namespace HLT;

TrigHIJetBaseSubtraction::TrigHIJetBaseSubtraction(const std::string & name, ISvcLocator* pSvcLocator)
  : FexAlgo(name, pSvcLocator),
    m_lvl1_sumEt(-1),
    m_jet_background(-1){
  
  m_BgEtaReg.resize(100);
  m_BgWeights.resize(100);
  
  declareProperty("CollectionLabel", m_label="HIJets", "The label used to attach jets, has to match the TrigEDMConfig in order to be stored.");
  declareProperty("coneRadius",m_coneRadius = 0.4);
  declareProperty("CaloArea",m_CaloArea = 61.57522);
  declareProperty("BgEtaReg",m_BgEtaReg);
  declareProperty("BgWeights",m_BgWeights);
  declareProperty("BgnEtaBin",m_Bgnbin_eta = 50);
  
  declareMonitoredVariable("L1_SumEt",m_SumEt);
  declareMonitoredVariable("JetEt",m_Jet_Et);
  declareMonitoredVariable("Jet_Background",m_Background);
  declareMonitoredVariable("JetEta",m_Jet_Eta);
    
  declareProperty("tower_background",m_tower_background = -1);
  declareProperty("jet_weighted_background",m_jet_weighted_background = -1);
  declareProperty("ieta",m_ieta = -1);
}

TrigHIJetBaseSubtraction::~TrigHIJetBaseSubtraction() {}

ErrorCode TrigHIJetBaseSubtraction::hltInitialize() {

  m_jet_weighted_background = -1;
  m_tower_background = -1;
  m_ieta = -1;
  m_SumEt = -1; 
  m_Jet_Et = -1;
  m_Jet_Eta = -10;
  m_Background = -1;
    
  return OK;
}

ErrorCode TrigHIJetBaseSubtraction::hltFinalize() {
  return OK;
}

ErrorCode TrigHIJetBaseSubtraction::hltEndEvent() {
  m_jet_background = -1;
  m_lvl1_sumEt = -1;    
  return OK;
}


ErrorCode TrigHIJetBaseSubtraction::hltExecute(const TriggerElement* input, 
					       TriggerElement* output) {
  // easy piece: get input jet and create output jet object with Et corrected
  // the loop over RoIs is external to this
  double TwoPi = 6.283185; //=2*Pi


  if (m_jet_background<0) {
      const LVL1::RecEnergyRoI* lvl1_energyRoi = 0;
      if(msgLvl() <= MSG::DEBUG) 
        msg() << MSG::DEBUG << "Retrieving Total Energy" << endreq;
	  
	  HLT::Navigation* nav = config()->getNavigation();
      HLT::TriggerElement* initial = nav->getInitialNode();
      const std::vector<HLT::TriggerElement*>& rois = nav->getDirectSuccessors(initial);
      for (unsigned u=0; u<rois.size(); ++u) {
        if ( nav->getFeature( rois[u], lvl1_energyRoi ) ) {
			if (lvl1_energyRoi) break;
		}
	  }
      if(!lvl1_energyRoi) {
		msg() << MSG::WARNING << "No RecEnergyRoI object found!  Aborting" << endreq;
		return HLT::NAV_ERROR;
      }
      // Original SumET is in ~ GeV units (scale factor will be contained in LVL1ConfigSvc at one point ...) *1000 ->MeV
      m_lvl1_sumEt = static_cast<float>(lvl1_energyRoi->energyT()) * 1000;
      //Parameters for subtraction
      m_tower_background = m_lvl1_sumEt / m_CaloArea;
      m_jet_background = m_coneRadius*m_coneRadius*(TwoPi/2) * m_tower_background; 
  }
 
  // now deal with each jet
  if(msgLvl() <= MSG::DEBUG) {
    const TrigRoiDescriptor* roiDescriptor(0);
    if( getFeature(input, roiDescriptor, "initialRoI") != HLT::OK && roiDescriptor )
      msg() << MSG::DEBUG << "executing Jet Base subtraction on RoI :" << *roiDescriptor << endreq;
  }

  const TrigT2Jet* in_jet(0);
  ErrorCode ec = getFeature(output, in_jet);  
  if ( ec != OK ) {
    msg() << MSG::WARNING << "retrieving jet on roi failed" << endreq;
    return ec;
  }
  
  if ( !in_jet ) {
    // some INFO message that jet is missing
    if(msgLvl() <= MSG::DEBUG) 
      msg() << MSG::DEBUG << "No jet in RoI for subtraction"  << endreq;
    return OK;
  }
  
  if(msgLvl() <= MSG::DEBUG) 
    msg() << MSG::DEBUG << "Original jet" << *in_jet  << endreq;    

  if(msgLvl() <= MSG::DEBUG) msg() <<MSG::DEBUG << "Jet background before calib.: " << m_jet_background << endreq;
  	 	
  //Background weight   	
  for(int i=0; i < m_Bgnbin_eta;++i) {
    if( fabs(in_jet->eta()) >= m_BgEtaReg[i] && fabs(in_jet->eta()) < m_BgEtaReg[i+1] ) {
      	m_ieta = i;
      	break;
    }
  } 
  m_jet_weighted_background = m_jet_background * m_BgWeights[m_ieta];
  if(msgLvl() <= MSG::DEBUG){
  	msg() <<MSG::DEBUG << "Jet background after calib.: " << m_jet_weighted_background << endreq;
  }
   
  //Jet energy correction  
  TrigT2Jet* out_jet = corrected(in_jet, m_jet_weighted_background);    
  if ( out_jet ) {
    ec = attachFeature(output, out_jet, m_label);
    if(msgLvl() <= MSG::DEBUG) 
      msg() << MSG::DEBUG << "Subtracted jet attached" << *out_jet  << endreq;
  }
  
  //Just for monitoring purpose
  m_Background = m_jet_weighted_background; 
  m_SumEt = m_lvl1_sumEt;
  m_Jet_Et = out_jet->e()/cosh(out_jet->eta());
  m_Jet_Eta = out_jet->eta();
  
  return OK;
}

TrigT2Jet* TrigHIJetBaseSubtraction::corrected(const TrigT2Jet* in, double m_background ) const {
  TrigT2Jet* base_subtracted = new TrigT2Jet(*in);
  base_subtracted->setGrid(0);
  //Jet E has to be transfered in to Et before subtraction and than back
  base_subtracted->setE((in->e()/cosh(in->eta()) - m_background)*cosh(in->eta())); 
  return base_subtracted;
}

