/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include <math.h>
#include "TrigInterfaces/AlgoConfig.h"
#include "TrigNavigation/TriggerElement.h"
//#include "TrigNavigation/Navigation.h"
#include "TrigSteeringEvent/TrigRoiDescriptor.h"

#include "TrigGenericAlgs/ReverseRoI.h"

ReverseRoI::ReverseRoI(const std::string& name, ISvcLocator* pSvcLocator)
  : HLT::FexAlgo(name, pSvcLocator)
{
  declareProperty("PhiShift", m_phi_shift = M_PI, "Shift phi position of the RoI by some angle (default pi). Note that sign matters!");
  declareProperty("FlipEta", m_flip_eta=true, "Flip the RoI position in the eta (negate it). Default is yes.");
  declareProperty("InputRoILabel", m_input_label,"Take labeled RoI rather than most recent one (default behavoir).");
  declareProperty("OutputRoILabel", m_output_label="secondaryRoI_L2","Use this label for reversed RoI. Default suits algorithm running at L2.");
}


HLT::ErrorCode ReverseRoI::hltExecute(const HLT::TriggerElement* /* te_in */,
				     HLT::TriggerElement* te_out)
{
  if ( msgLvl() <= MSG::DEBUG ) 
    msg() << MSG::DEBUG << "Executing ReverseRoI " << name()<< endmsg;
  
  const TrigRoiDescriptor* roiDescriptor = 0;
  HLT::ErrorCode hltStatus = getFeature(te_out, roiDescriptor, m_input_label);
  
  if ( hltStatus == HLT::OK && roiDescriptor !=0 ) {
    if (msgLvl() <= MSG::DEBUG) msg() << MSG::DEBUG << "REGTEST: original RoI " << *roiDescriptor << endmsg;
  } else {
    msg() <<  MSG::WARNING << " No RoI Descriptor for this Trigger Element! " << endmsg;
    return HLT::ErrorCode(HLT::Action::ABORT_CHAIN, HLT::Reason::MISSING_FEATURE);
  }
  

  double newPhi = roiDescriptor->phi() + m_phi_shift;
  while (newPhi < -M_PI) newPhi += 2. * M_PI;  // this while is executed once -- it is cheaper than the "if" statement  !
  while (newPhi > M_PI)  newPhi -= 2. * M_PI;
  

  double newPhiMinus = roiDescriptor->phiMinus() + m_phi_shift;
  while (newPhiMinus < -M_PI) newPhiMinus += 2. * M_PI;  // this while is executed once -- it is cheaper than the "if" statement  !
  while (newPhiMinus > M_PI)  newPhiMinus -= 2. * M_PI;

  double newPhiPlus = roiDescriptor->phiPlus() + m_phi_shift;;
  while (newPhiPlus < -M_PI) newPhiPlus += 2. * M_PI;  // this while is executed once -- it is cheaper than the "if" statement  !
  while (newPhiPlus > M_PI)  newPhiPlus -= 2. * M_PI;
  


  double newEta = roiDescriptor->eta();
  if ( m_flip_eta ) newEta *= -1.;
    
  double newEtaMinus = roiDescriptor->etaMinus();
  if ( m_flip_eta ) newEtaMinus = -roiDescriptor->etaPlus();

  double newEtaPlus = roiDescriptor->etaPlus();
  if ( m_flip_eta ) newEtaPlus  = -roiDescriptor->etaMinus();


  TrigRoiDescriptor* newRoiDescriptor = new TrigRoiDescriptor(roiDescriptor->roiWord(),
							      roiDescriptor->l1Id(), 
							      roiDescriptor->roiId(),
							      newEta, newEtaMinus, newEtaPlus, 
							      newPhi, newPhiMinus, newPhiPlus);
  
  hltStatus = attachFeature(te_out, newRoiDescriptor, m_output_label);
  if (hltStatus != HLT::OK){
    msg() << MSG::ERROR << "Write of newRoiDescriptor into outputTE failed"
	  << endmsg;
    return hltStatus;
  }  
  if (msgLvl() <= MSG::DEBUG) msg() << MSG::DEBUG << "REGTEST: reversed RoI " << * newRoiDescriptor << endmsg;
 
 
 return HLT::OK;
}
