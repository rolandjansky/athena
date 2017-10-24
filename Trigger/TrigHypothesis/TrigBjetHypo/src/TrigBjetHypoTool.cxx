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

#include "TrigBjetHypoTool.h"
#include "InDetBeamSpotService/IBeamCondSvc.h"
#include "AthenaMonitoring/MonitoredScope.h"


TrigBjetHypoTool::TrigBjetHypoTool( const std::string& type, 
		    const std::string& name, 
		    const IInterface* parent ) :
  AthAlgTool( type, name, parent ),
  m_id( name ),
  m_monTool( "GenericMonitoringTool/MOnTool", this )
{
  declareProperty ("AcceptAll", m_acceptAll         );
  //  declareProperty ("JetKey",    m_jetKey     = ""   ); //"EFJet" or "" needed for default config, "SplitJet" for new config - not sure if needed, with some changes new configuration may be established
  declareProperty ("CutMV2c20", m_xcutMV2c20 = -20  );
  declareProperty ("CutMV2c10", m_xcutMV2c10 = -20  );
  declareProperty ("MethodTag", m_methodTag  = ""   );
  declareProperty ("Instance",  m_instance          );

  declareProperty ("UseBeamSpotFlag", m_useBeamSpotFlag = false);
  declareProperty ("OverrideBeamSpotValid", m_overRideBeamSpotValid = false);

  declareProperty( "MonTool", m_monTool=ToolHandle<GenericMonitoringTool>( "", this ) );


}


// -----------------------------------------------------------------------------------------------------------------


TrigBjetHypoTool::~TrigBjetHypoTool() {}


// -----------------------------------------------------------------------------------------------------------------

StatusCode TrigBjetHypoTool::initialize()  {

  // Get message service 
 
  ATH_MSG_INFO("Initializing TrigBjetHypoTool");

 
  ATH_MSG_DEBUG(  "declareProperty review:"  );
  ATH_MSG_DEBUG(  " AcceptAll = "   <<     m_acceptAll        ); 
  ATH_MSG_DEBUG(  " MethodTag = "   <<     m_methodTag        ); 
  ATH_MSG_DEBUG(  " Instance = "    <<     m_instance         ); 
  ATH_MSG_DEBUG(  " UseBeamSpotFlag = " <<  m_useBeamSpotFlag  ); 
  //  ATH_MSG_DEBUG(  " JetKey = "     <<      m_jetKey           ); 

  if (m_xcutMV2c20 != -20) ATH_MSG_DEBUG( " CutMV2c20 = " <<  m_xcutMV2c20  ); 
  if (m_xcutMV2c10 != -20) ATH_MSG_DEBUG( " CutMV2c10 = " <<  m_xcutMV2c10  ); 
 
  ATH_MSG_DEBUG( "Tool configured for chain/id: " << m_id  );
  return StatusCode::SUCCESS;

}


// ----------------------------------------------------------------------------------------------------------------- 

////    I am here
bool TrigBjetHypoTool::decide(  const xAOD::BTagging* bTag, const TrigRoiDescriptor* roiDescriptor )  const {
//bool TrigBjetHypoTool::decide(  const xAOD::BTaggingContainer* trigBTaggingContainer, const TrigRoiDescriptor* roiDescriptor )  const {
  bool pass = false;

  ATH_MSG_DEBUG(  "Executing TrigBjetHypoTool"  );

  using namespace Monitored;
  // initialise monitoring variables 
  auto PassedCuts   = MonitoredScalar::declare<int>( "CutCounter", -1 );
  auto monitorIt = MonitoredScope::declare( m_monTool, PassedCuts );
  // when leaving scope it will ship data to monTool
  PassedCuts = PassedCuts + 1; //got called (data in place)

  // AcceptAll declare property setting - no need to save it in TrigPassBit, but instead it will go into decission, where we can save also properties and attach them to objects
    if (m_acceptAll) 
      ATH_MSG_DEBUG(  "REGTEST: AcceptAll property is set: taking all events"  );
    else 
      ATH_MSG_DEBUG(  "REGTEST: AcceptAll property not set: applying the selection"  );
  

  // Retrieve beamspot information - check if this is going to be changed
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
      
      if (!beamSpotStatus && !m_overRideBeamSpotValid) {

	PassedCuts=0;
	pass = false;
	
	return pass;
      }
    }
  }

  // to protect if there is no RoI? Our RoI is jet, one should be careful here

  ATH_MSG_DEBUG( "RoI ID = "   << roiDescriptor->roiId()
                 << ": Eta = "      << roiDescriptor->eta()
                 << ", Phi = "      << roiDescriptor->phi() );

  //  here
  bool result = false;


  // Need to check if Btagging is retereived

  // to separate bad input TE and true behaviour 
  PassedCuts=1;
 
  // Let's try without a loop over container and see if this is done in Alg? 
  //  auto trigBTagging = trigBTaggingContainer->begin();
  //  auto trigBTaggingEnd = trigBTaggingContainer->end();

  // Loop over EFBjets and perform cut 
// Loop over EFBjets and perform cut 
  if (m_methodTag == "MV2c20") {

    //    for ( ; trigBTagging != trigBTaggingEnd; trigBTagging++) { 

    //     double x = (*trigBTagging)->auxdata<double>("MV2c20_discriminant");
    double x = bTag->auxdata<double>("MV2c20_discriminant");

      ATH_MSG_DEBUG(" MV2c20 x =  " << x);
       if(x>m_xcutMV2c20) {
	//HLT::markPassing(bitsEF, (*trigBTagging), trigBTaggingContainer);
	 //	xBits->markPassing((*trigBTagging),trigBTaggingContainer,true);
	ATH_MSG_DEBUG("  ==> Passed ");
	result = true;
      }
      else {
	ATH_MSG_DEBUG("  ==> Failed ");
      }
    }
  //  }
  else if (m_methodTag == "MV2c10") {

    //    for ( ; trigBTagging != trigBTaggingEnd; trigBTagging++) {

    //      double x = (*trigBTagging)->auxdata<double>("MV2c10_discriminant");
    double x = bTag->auxdata<double>("MV2c10_discriminant");

       ATH_MSG_DEBUG(" MV2c10 x =  " << x);
      if(x>m_xcutMV2c10) {
        //HLT::markPassing(bitsEF, (*trigBTagging), trigBTaggingContainer);
	//        xBits->markPassing((*trigBTagging),trigBTaggingContainer,true);
	ATH_MSG_DEBUG("  ==> Passed ");
        result = true;
      }
      else {
	ATH_MSG_DEBUG("  ==> Failed ");
      }
    }
  //  } 
 

  if(result) PassedCuts=PassedCuts+1;
  pass = result;
  
  // Print trigger decision 
  if (m_acceptAll) {
    ATH_MSG_DEBUG("REGTEST: Trigger decision is 1");
  } else {
    ATH_MSG_DEBUG("REGTEST: Trigger decision is " << pass);
  }


  if (m_acceptAll) {
    PassedCuts = 2;
    pass = true;
    //xBits->markPassing((*trigBTagging),trigBTaggingContainer,true);
    //return pass;
  //   for ( ; trigBTagging != trigBTaggingEnd; trigBTagging++)
  //     xBits->markPassing((*trigBTagging),trigBTaggingContainer,true);
   }

  // Print TrigPassBits to outputTE 
  //if (attachBits(outputTE, bitsEF, "EFBjets") != pass) {
  //  msg() << MSG::ERROR << "Problem attaching TrigPassBits for b-jets" << );
  //}
    //   if(attachFeature(outputTE, xBits.release(),"passbits") != pass)
   //  ATH_MSG_ERROR("Could not store TrigPassBits! ");

   // if (msgLvl() <= ATH_MSG_DEBUG()
   //    msg() << ATH_MSG_DEBUG( << "TrigPassBits was saved" << ); 


  return pass;
}


// -----------------------------------------------------------------------------------------------------------------


StatusCode TrigBjetHypoTool::finalize()  {

  ATH_MSG_INFO( "Finalizing TrigBjetHypoTool" );
  return StatusCode::SUCCESS;
}

