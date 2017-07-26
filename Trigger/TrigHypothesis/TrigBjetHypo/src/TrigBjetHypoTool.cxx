/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// ************************************************
//
// NAME:     TrigBjetHypoTool.cxx
// PACKAGE:  Trigger/TrigHypothesis/TrigBjetHypoTool
//
// AUTHOR:   Lidija Zivkovic
// EMAIL:    Lidija.Zivkovic@cern.ch
// 
// ************************************************

#include "DecisionHandling/HLTIdentifier.h"

#include "TrigBjetHypo.h"

TrigBjetHypoTool::TrigBjetHypoTool( const std::string& type, 
		    const std::string& name, 
		    const IInterface* parent ) :
  AthAlgTool( type, name, parent ),
  m_id( name ),
  m_monTool( "GenericMonitoringTool/MOnTool", this )
{
  declareProperty ("AcceptAll", m_acceptAll         );
  declareProperty ("JetKey",    m_jetKey     = ""   ); //"EFJet" or "" needed for default config, "SplitJet" for new config
  declareProperty ("CutMV2c20", m_xcutMV2c20 = -20  );
  declareProperty ("MethodTag", m_methodTag  = ""   );
  //declareProperty ("Instance",  m_instance          );

  declareProperty ("UseBeamSpotFlag", m_useBeamSpotFlag = false);

  declareProperty( "MonTool", m_monTool=ToolHandle<GenericMonitoringTool>( "", this ) );


}


// -----------------------------------------------------------------------------------------------------------------


TrigBjetHypo::~TrigBjetHypo() {}


// -----------------------------------------------------------------------------------------------------------------


HLT::ErrorCode TrigBjetHypo::hltInitialize() {

  // Get message service 
 
  ATH_MSG_INFO("Initializing TrigBjetHypoTool");

 
  ATH_MSG_DEBUG(  "declareProperty review:"  );
  ATH_MSG_DEBUG(  " AcceptAll = "   <<     m_acceptAll        ); 
  ATH_MSG_DEBUG(  " MethodTag = "   <<     m_methodTag        ); 
  //  ATH_MSG_DEBUG(  " Instance = "    <<     m_instance         ); 
  ATH_MSG_DEBUG(  " UseBeamSpotFlag = " <<  m_useBeamSpotFlag  ); 
  ATH_MSG_DEBUG(  " JetKey = "     <<      m_jetKey           ); 

  if (m_xcutMV2c20 != -20) ATH_MSG_DEBUG( " CutMV2c20 = " <<  m_xcutMV2c20  ); 
 
  ATH_MSG_DEBUG( "Tool configured for chain/id: " << m_id  );
  return StatusCode::SUCCESS;

}


// ----------------------------------------------------------------------------------------------------------------- 

////    I am here
HLT::ErrorCode TrigBjetHypo::hltExecute(const HLT::TriggerElement* outputTE, bool& pass) {
  
    ATH_MSG_DEBUG(  "Executing TrigBjetHypo"  );

  // AcceptAll declare property setting - no need to save it in TrigPassBit, but instead it will go into decission, where we can save also properties and attach them to objects
    if (m_acceptAll) 
      ATH_MSG_DEBUG(  "REGTEST: AcceptAll property is set: taking all events and applying the selection only to save the TrigPassBits"  );
    else 
      ATH_MSG_DEBUG(  "REGTEST: AcceptAll property not set: applying the selection and saving the TrigPassBits"  );
  
  // initialise monitoring variables 
  m_cutCounter = -1;

  // Retrieve beamspot information - chec if this is going to be changed
  if (m_useBeamSpotFlag) {

    IBeamCondSvc* iBeamCondSvc; 
    StatusCode sc = service("BeamCondSvc", iBeamCondSvc);
    
    if (sc.isFailure() || iBeamCondSvc == 0) {
      

      ATH_MSG_WARNING(  "Could not retrieve Beam Conditions Service. "  );
      
    } else {

      int beamSpotStatus = 0;
      int beamSpotBitMap = iBeamCondSvc->beamStatus();    

      beamSpotStatus = ((beamSpotBitMap & 0x4) == 0x4);  
      if (beamSpotStatus) beamSpotStatus = ((beamSpotBitMap & 0x3) == 0x3);
      
      if (!beamSpotStatus) {
	
	m_cutCounter=0;
	pass = false;
	
	return HLT::OK;
      }
    }
  }

  // Get RoI descriptor 
  const TrigRoiDescriptor* roiDescriptor = 0;
  HLT::ErrorCode stat = getFeature(outputTE, roiDescriptor, m_jetKey);
  
  if (stat == HLT::OK) {

      ATH_MSG_DEBUG(  "Using outputTE: " << *roiDescriptor  );
      //<< "RoI id " << roiDescriptor->roiId()
      //    << ", Phi = " <<  roiDescriptor->phi()
      //	    << ", Eta = " << roiDescriptor->eta() << );
    }
  } else {
    
      ATH_MSG_WARNING(  "No RoI for this Trigger Element "  );

    return HLT::NAV_ERROR;
  }

  bool result = false;

  // Define TrigPassBits for b-jets 
  //TrigPassBits *bitsEF=0;

  // Retrieve xAOD b-jet object 
  const xAOD::BTaggingContainer* trigBTaggingContainer=0;
 
  if((getFeature(outputTE, trigBTaggingContainer, "HLTBjetFex") != HLT::OK) && trigBTaggingContainer) {
    if (msgLvl() <= ATH_MSG_WARNING()
      msg()  ATH_MSG_WARNING(  "Failed to get BTaggingContainer"  );
    return HLT::OK;
  }
  if(msgLvl() <= ATH_MSG_DEBUG( && trigBTaggingContainer != 0) {
    msg()  ATH_MSG_DEBUG(  "Got BTaggingContainer with "  trigBTaggingContainer->size()  " BTagging object"  );
   }
  if(trigBTaggingContainer != 0 && trigBTaggingContainer->size() > 1) {
    if(msgLvl() <= MSG::ERROR)
      msg()  MSG::ERROR  "More than one BTagging object to analyse: this should never happen"  );
    return HLT::NAV_ERROR;
  }
  if(trigBTaggingContainer->size() == 0) {
    if(msgLvl() <= MSG::ERROR)
      msg()  MSG::ERROR  "No BTagging object to analyse: this should never happen"  );
    return HLT::NAV_ERROR;
  }

  // Add TrigPassBits for EF b-jets 
  //bitsEF = HLT::makeTrigPassBits(trigBTaggingContainer);
  std::unique_ptr<xAOD::TrigPassBits> xBits = xAOD::makeTrigPassBits<xAOD::BTaggingContainer>(trigBTaggingContainer);

  // to separate bad input TE and true behaviour 
  m_cutCounter=1;
  
  auto trigBTagging = trigBTaggingContainer->begin();
  auto trigBTaggingEnd = trigBTaggingContainer->end();

  // Loop over EFBjets and perform cut 
  if (m_methodTag == "MV2c20") {

    for ( ; trigBTagging != trigBTaggingEnd; trigBTagging++) { 

      double x = (*trigBTagging)->auxdata<double>("MV2c20_discriminant");

      if(msgLvl() <= ATH_MSG_DEBUG() msg()  ATH_MSG_DEBUG(  "MV2c20 x =  "  x;
      if(x>m_xcutMV2c20) {
	//HLT::markPassing(bitsEF, (*trigBTagging), trigBTaggingContainer);
	xBits->markPassing((*trigBTagging),trigBTaggingContainer,true);
      if(msgLvl() <= ATH_MSG_DEBUG() msg()  ATH_MSG_DEBUG(  " ==> Passed "  );
	result = true;
      }
      else {
	if(msgLvl() <= ATH_MSG_DEBUG() msg()  ATH_MSG_DEBUG(  " ==> Failed "  );
      }
    }
  } 


  if(result) m_cutCounter++;
  pass = result;
  
  // Print trigger decision 
  if (m_acceptAll) {
    if(msgLvl() <= ATH_MSG_DEBUG() 
      msg() << ATH_MSG_DEBUG( << "REGTEST: Trigger decision is 1" << );
  } else {
    if(msgLvl() <= ATH_MSG_DEBUG() 
      msg() << ATH_MSG_DEBUG( << "REGTEST: Trigger decision is " << pass << );
  }

  // Monitoring of method used to perform the cut 
  if (m_methodTag == "IP2D") m_monitorMethod = 1;
  else if (m_methodTag == "IP3D") m_monitorMethod = 2;
  else if (m_methodTag == "COMB") m_monitorMethod = 3;

  if (m_acceptAll) {
    m_cutCounter = 2;
    pass = true;
    //xBits->markPassing((*trigBTagging),trigBTaggingContainer,true);
    //return HLT::OK;
    for ( ; trigBTagging != trigBTaggingEnd; trigBTagging++)
      xBits->markPassing((*trigBTagging),trigBTaggingContainer,true);
  }

  // Print TrigPassBits to outputTE 
  //if (attachBits(outputTE, bitsEF, "EFBjets") != HLT::OK) {
  //  msg() << MSG::ERROR << "Problem attaching TrigPassBits for b-jets" << );
  //}
   if(attachFeature(outputTE, xBits.release(),"passbits") != HLT::OK)
    ATH_MSG_ERROR("Could not store TrigPassBits! ");

   if (msgLvl() <= ATH_MSG_DEBUG()
      msg() << ATH_MSG_DEBUG( << "TrigPassBits was saved" << ); 


  return HLT::OK;
}


// -----------------------------------------------------------------------------------------------------------------


StatusCode TrigBjetHypoTool::finalize()  {

  ATH_MSG_INFO( "Finalizing TrigBjetHypoTool" );
  return StatusCode::SUCCESS;
}

