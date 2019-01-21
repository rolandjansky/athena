/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/

// ********************************************************************
//
// NAME:     TrigEFMissingETHypo.cxx
// PACKAGE:  Trigger/TrigHypothesis/TrigEFMissingETHypo
//
// AUTHOR:  Kyle Cranmer, Florian Bernlochner
//
//  $Id: TrigEFMissingETHypo.cxx,v 1.26 2009-03-25 17:04:03 casadei Exp $
// ********************************************************************
//
#include <list>
#include <iterator>
#include <sstream>
#include <cstdio>
//
#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/StatusCode.h"
#include "GaudiKernel/ListItem.h"

#include "TrigMissingETHypo/TrigEFMissingETHypo.h"

//#include "TrigCaloEvent/TrigMissingET.h"
#include "TrigMissingEtEvent/TrigMissingET.h"
#include "xAODTrigMissingET/TrigMissingET.h"

#include "CLHEP/Units/SystemOfUnits.h"

class ISvcLocator;

/////////////////////////////////////////////////////////////////////
// CONSTRUCTOR:
/////////////////////////////////////////////////////////////////////
//
TrigEFMissingETHypo::TrigEFMissingETHypo(const std::string& name, ISvcLocator* pSvcLocator):
    HLT::HypoAlgo(name, pSvcLocator) {

  declareProperty("METLabel", m_featureLabel = "TrigEFMissingET", "label for the MET feature in the HLT Navigation");

  declareProperty("MissingETCut", m_MEtCut   =  200*CLHEP::GeV, "cut value for the MissingEt [MeV]");
  declareProperty("SumETCut",     m_SumEtCut = 1000*CLHEP::GeV, "cut value for the SumEt [MeV]");
  declareProperty("SigCut",       m_SigCut   = 30, "cut value for the SumEt");
  declareProperty("CutType",      m_CutType  = -3.0,     "val<-1.5 => MissingEt OR SumEt; -1.5<val<-0.5 => MissingEt AND SumEt; -0.5<val<0.5 => reject all events .5<val => Significance" );

  declareProperty("doMuonCorrection", m_doMuonCorrection = false, "switch on/off muon correction" );
  declareProperty("doEMScaleTC", m_doEMScaleTC = false, "switch on/off the use of EM scale information (for topo. clusters only!)" );
  declareProperty("doOnlyCalcCentralMET", m_doOnlyCalcCentralMET = false, "only use central MET" );

  declareProperty("doL1L2FEBTest", m_doL1L2FEBTest = false, "Use L2=L1 values to Trigger FEB if MET values disagree by more than L1L2FEBTolerance GeV (for FEB only!)" );
  declareProperty("L1L2FEBTolerance", m_L1L2FEBTolerance = 100*CLHEP::GeV, "L2=L1 vs FEB tolerance in GeV" );

  declareProperty("doLArH11off", m_doLArH11off = false, "LAr H11 crate is off" );
  declareProperty("doLArH12off", m_doLArH12off = false, "LAr H12 crate is off" );
  declareProperty("doMETphicut", m_doMETphicut = false, "remove event when MET phi in the region of LAr H11 or/both H12");




  // xs = MET/[a sqrt(SumET) - b] 

  declareProperty("significanceOffset", m_significanceOffset = 0, "offset of xs");
  declareProperty("significanceSlope",  m_significanceSlope  = 1, "slope of xs");
  declareProperty("significanceQuadr",  m_significanceQuadr  = 0, "quadratic term of xs");

  declareProperty("xsMETmin",  m_xsMETmin = 10*CLHEP::GeV, "Minimum Value for MET in xs chains");  
  declareProperty("xsSETmin",  m_xsSETmin = 16*CLHEP::GeV, "Minimum Value for MET in xs chains");  

  declareProperty("xsMETok",   m_xsMETok = 64*CLHEP::GeV,   "MET value for acceptance in xs chains" );
  declareProperty("xsSETok",   m_xsSETok = 6500*CLHEP::GeV, "SET value for acceptance in xs chains" );

  m_bitMask = 0;
  declareProperty("bitMaskComp", m_bitMaskComp = 0, "bit mask to enable rejection based on the component level status flag" );
  declareProperty("bitMaskGlob", m_bitMaskGlob = 0, "bit mask to enable rejection based on the global part of the status flag" );

//   declareProperty("doMonitoring",     m_doMonitoring = false,    "switch on/off monitoring" );
//   declareProperty("doTimers",         m_doTimers = true,         "switch on/off internal timers");

  /// extended use ///
  declareProperty("Central", m_central = 0,  "[GeV] if <0 subtract FCAL; if >0 require FCAL_MET<Central" );
  declareProperty("Forward", m_forward = 0,  "[GeV] if <0 require calo_MET<|Forward|; if >0 require calo_MET<Forward AND muon_MET<Forward" );

  declareProperty("MonitorMeasuredEnergies", m_monitorMeasuredEnergies = false, "Heavy Ion Monitoring"  );

  declareMonitoredVariable("Hypo_MET_cuts",  m_cutCounter); // passed thresholds
  declareMonitoredVariable("Hypo_MEx_log",   m_ex_log);     // signed log10 of Missing Ex [GeV]
  declareMonitoredVariable("Hypo_MEy_log",   m_ey_log);     // signed log10 of Missing Ey [GeV]
  declareMonitoredVariable("Hypo_MEz_log",   m_ez_log);     // signed log10 of Missing Ez [GeV]
  declareMonitoredVariable("Hypo_MET_log",   m_et_log);     // log10 of Missing Et [GeV]
  declareMonitoredVariable("Hypo_SumEt_log", m_sumet_log);  // log10 of SumEt [GeV]
  declareMonitoredVariable("Hypo_MEx_lin",   m_ex_lin);     // Missing Ex [GeV]
  declareMonitoredVariable("Hypo_MEy_lin",   m_ey_lin);     // Missing Ey [GeV]
  declareMonitoredVariable("Hypo_MEz_lin",   m_ez_lin);     // Missing Ez [GeV]
  declareMonitoredVariable("Hypo_MET_lin",   m_et_lin);     // Missing Et [GeV]
  declareMonitoredVariable("Hypo_SumEt_lin", m_sumet_lin);  // SumEt [GeV]
  declareMonitoredVariable("Hypo_XS",        m_xs);         // XS [GeV^1/2]
  declareMonitoredVariable("Hypo_XS2",       m_xs2);        // XS [GeV^1/2]
  declareMonitoredVariable("Hypo_MET_phi",   m_phi);        // MET phi [rad]

  m_totTime=0;

  m_accepted=0;
  m_rejected=0;
  m_errors=0;

}


// ----------------------------------------------------------------------
HLT::ErrorCode TrigEFMissingETHypo::hltInitialize()
// ----------------------------------------------------------------------
{
  if(msgLvl() <= MSG::DEBUG) {
    msg() << MSG::DEBUG << "hypo initialize()" << endmsg;
  }

  // Initialize timing service
  //------------------------------
  if(doTiming() ) {
    m_totTime=addTimer("TrigEFMissingETHypoInternalTotalTime");
    if (m_totTime==0) {
      msg() << MSG::WARNING << "not able to initialize timer!" << endmsg;
    }
  }

  m_accepted=0;
  m_rejected=0;
  m_errors=0;

  // creating the global status mask
  m_bitMask = m_bitMaskGlob<<16 | m_bitMaskComp;
  if(msgLvl() <= MSG::DEBUG) {
    char buff[512];
    snprintf(buff, 512, "REGTEST: bitMask = 0x%08x", m_bitMask);
    msg() << MSG::DEBUG << buff << endmsg;
  }

  return HLT::OK;
}


// ----------------------------------------------------------------------
HLT::ErrorCode TrigEFMissingETHypo::hltFinalize(){
// ----------------------------------------------------------------------

   if(msgLvl() <= MSG::DEBUG) {
     msg() << MSG::DEBUG << "in finalize()" << endmsg;
     msg() << MSG::DEBUG << "Events accepted/rejected/errors:  "<< m_accepted <<" / "<<m_rejected<< " / "<< m_errors<< endmsg;
   }

  return HLT::OK;
}


// ----------------------------------------------------------------------
HLT::ErrorCode TrigEFMissingETHypo::hltExecute(const HLT::TriggerElement* outputTE, bool & pass){
// ----------------------------------------------------------------------


///////// logics ////////
// if m_forward < 0 then
//   reject if centralMET > |m_forward|  # includes muons
// else if m_forward > 0 then
//   reject if (caloCentralMET > m_forward) OR (muonMET > m_forward)
// endif
// # centralMET is defined without FCAL
// # caloCentralMET is defined without FCAL and muons
// 
// if m_central < 0 then
//   MET := centralMET;
//   SumET := centralSET
// else if m_central > 0 then
//   reject if FCalMET > central
// endif
// 
// if !m_doMuonCorrection then
//   subtract muon contribution from MET, SumET
// endif
// 
// compare to MET and/or SumET thresholds



  // Time total TrigEFCaloHypo execution time.
  // -------------------------------------
  if (doTiming() && m_totTime) {
    m_totTime->start();
  }
  m_cutCounter = -1; // -1: error; 0: rejected; 1: MET passed; 2: SumET passed; 3: both passed; 4: Forward event;
  m_ex_log = -9e9;
  m_ey_log = -9e9;
  m_ez_log = -9e9;
  m_et_log = -9e9;
  m_sumet_log = -9e9;
  m_ex_lin = -9e9;
  m_ey_lin = -9e9;
  m_ez_lin = -9e9;
  m_et_lin = -9e9;
  m_sumet_lin = -9e9;
  m_xs = -9e9;
  m_phi = -9e9;


  // get MissingETCollections from the trigger element:

  pass = false;
  bool accepted = false;

  std::vector<const xAOD::TrigMissingET*> vectorMissingET;

  HLT::ErrorCode stat = getFeatures(outputTE, vectorMissingET, m_featureLabel);




  if(stat != HLT::OK) {
    msg() << MSG::WARNING << " Failed to get vectorMissingETs " << endmsg;
    if (doTiming() && m_totTime) {
      m_totTime->stop();
    }
    return HLT::OK;
  }

  //check size of MissingET
  if( vectorMissingET.size() == 0 ){
    if(msgLvl() <= MSG::DEBUG) {
      msg() << MSG::DEBUG<< " Got no MissingETs associated to the TE! " << endmsg;
    }
    m_errors++;
    if (doTiming() && m_totTime ) {
      m_totTime->stop();
    }
    return HLT::OK;
  } else {
    if(msgLvl() <= MSG::DEBUG) {
      msg() << MSG::DEBUG << " Got " << vectorMissingET.size()
	    << " vectorMissingET size associated to the TE" << endmsg;
    }
  }

  const xAOD::TrigMissingET* met = vectorMissingET.front();

  if (met==0){
    if(msgLvl() <= MSG::DEBUG) {
      msg() << MSG::DEBUG<< " MissingET pointer is null" << endmsg;
    }
    m_errors++;
    if (doTiming() && m_totTime) {
      m_totTime->stop();
    }
    return HLT::OK;
  }


  // early rejection based on status flag
  if ( m_bitMask & met->flag() ) {
    pass = false;
    m_rejected++;
    return HLT::OK;
  }


  m_cutCounter = 0; // monitoring the decision

  // Fex result:
  float MET=sqrt(met->ex()*met->ex()+met->ey()*met->ey());    // MeV
  float SET=met->sumEt(); // MeV
  float Ex=met->ex();     // MeV
  float Ey=met->ey();     // MeV
  float Ez=met->ez();     // MeV
   
  double epsilon=1e-6; // GeV (never compare a float for equality!)
  
  // calo. only (no muons)
  float caloMET=MET;
  float caloSET=SET;
  float caloEx=Ex;
  float caloEy=Ey;
  float caloEz=Ez;

  // central MET (no FCAL)
  float centralMET=MET;
  float centralSET=SET;
  float centralEx=Ex;
  float centralEy=Ey;
  float centralEz=Ez;

  // caloCentral MET (no muons, no FCAL)
  float caloCentralMET=MET;
  float caloCentralSET=SET;
  float caloCentralEx=Ex;
  float caloCentralEy=Ey;
  float caloCentralEz=Ez;

  // muon only
  float muonMET=0;
  float muonSET=0;
  float muonEx=0;
  float muonEy=0;
  float muonEz=0;

  unsigned int muIndex = met->getNumberOfComponents()-1; // muons in last component
  
  if( (met->nameOfComponent(muIndex)).substr(0,4)=="Muon") { // Fetch Muon Components
    float muEx = met->exComponent(muIndex);
    float muEy = met->eyComponent(muIndex);
    float muEz = met->ezComponent(muIndex);
    float muSumEt = met->sumEtComponent(muIndex);
    float sumOfSigns = met->sumOfSignsComponent(muIndex);
    float c0 = met->calib0Component(muIndex);
    float c1 = met->calib1Component(muIndex);
    // (check TrigEFMissingET/EFMissingETFromHelper) //
    muonEx  = sumOfSigns * c0 + c1 * muEx;
    muonEy  = sumOfSigns * c0 + c1 * muEy;
    muonEz  = sumOfSigns * c0 + c1 * muEz;
    muonSET = sumOfSigns * c0 + c1 * muSumEt;
    muonMET = sqrt(muonEx*muonEx + muonEy*muonEy);
  }
  else {
    if(msgLvl() <= MSG::WARNING) {
      msg() << MSG::WARNING << 
	"ERROR: cannon find 'Muon' in component name - muon not uncorrected!"
	    << endmsg;
    }
  } //end of muon name test

  if ( m_doEMScaleTC ) {
  	 
   // clear Fex result
   MET = SET = Ex = Ey = Ez = 0;
   
   unsigned int lowTCIndex  =  met->getNumberOfComponents()-5;
   unsigned int highTCIndex =  met->getNumberOfComponents()-1;
   
   if(m_doOnlyCalcCentralMET)
    highTCIndex = lowTCIndex + 2;
    
   for(unsigned int i = lowTCIndex; i < highTCIndex; i++) {
   	   	
   	Ex += met->exComponent(i);
   	Ey += met->eyComponent(i);
   	Ez += met->ezComponent(i);   	
   	SET += met->sumEtComponent(i);
   } 
   
    centralEx = caloEx = caloCentralEx = Ex;
    centralEy = caloEy = caloCentralEy = Ey;
    centralEz = caloEz = caloCentralEz = Ez;
   
    caloSET = centralSET = caloCentralSET = SET;
    MET = caloMET = centralMET = caloCentralMET = sqrt(Ex*Ex + Ey*Ey);
  	
  }
  
  if ( m_doOnlyCalcCentralMET && !m_doEMScaleTC ) {

   // clear Fex result
   MET = SET = Ex = Ey = Ez = 0;
     
   for(unsigned int i = 0; i < 2; i++) { 	
   	Ex += met->exComponent(i);
   	Ey += met->eyComponent(i);
   	Ez += met->ezComponent(i);   	
   	SET += met->sumEtComponent(i);
   } 
   
    centralEx = caloEx = caloCentralEx = Ex;
    centralEy = caloEy = caloCentralEy = Ey;
    centralEz = caloEz = caloCentralEz = Ez;
   
    caloSET = centralSET = caloCentralSET = SET;
    MET = caloMET = centralMET = caloCentralMET = sqrt(Ex*Ex + Ey*Ey);  
  
  }
  
  if ( m_doL1L2FEBTest ) {
  
   float L1MET =  sqrt( pow(met->exComponent(0),2.0) +  pow(met->eyComponent(0),2.0) );
    	
   if(MET - L1MET > m_L1L2FEBTolerance ) {    
    	
     MET = SET = Ex = Ey = Ez = 0; // clear Fex result
    
     Ex += met->exComponent(0);
     Ey += met->eyComponent(0);
     Ez += met->ezComponent(0);
     SET +=  met->sumEtComponent(0);

     centralEx = caloEx = caloCentralEx = Ex;
     centralEy = caloEy = caloCentralEy = Ey;
     centralEz = caloEz = caloCentralEz = Ez;
    
     caloSET = centralSET = caloCentralSET = SET;
     MET = caloMET = centralMET = caloCentralMET = sqrt(Ex*Ex + Ey*Ey);    
  	
   }
    	
  }

  if ( m_doMuonCorrection || fabs(m_forward) > epsilon ) { // add muons?
    // add muons
    Ex  += muonEx;
    Ey  += muonEy;
    Ez  += muonEz;
    SET += muonSET;
    MET = sqrt(Ex*Ex + Ey*Ey);
    // 

    // add muons to central MET
    centralEx  += muonEx;
    centralEy  += muonEy;
    centralEz  += muonEz;
    centralSET += muonSET;
    centralMET = sqrt(centralEx*centralEx + centralEy*centralEy);

  } 
  
  // FCAL only
  float fcalMET=0;
  float fcalSET=0;
  float fcalEx=0;
  float fcalEy=0;
  float fcalEz=0;

  if ( fabs(m_central) > epsilon || fabs(m_forward) > epsilon ) { // subtract FCAL?
    // address the 3 FCAL samplings:
    unsigned int Imin = met->getNumberOfComponents()-4;
    unsigned int Imax = met->getNumberOfComponents()-2;
    for (unsigned int i=Imin; i<=Imax; ++i) {
      if( (met->nameOfComponent(i)).substr(0,4)!="FCal") {
      	if(msgLvl() <= MSG::WARNING) {
      	  msg() << MSG::WARNING << 
      	    "ERROR: cannot find 'FCal' in component name - FCal not subtracted!"
      		<< endmsg;
      	}
        break;
      }
      float ex = met->exComponent(i);
      float ey = met->eyComponent(i);
      float ez = met->ezComponent(i);
      float SumEt = met->sumEtComponent(i);
      float sumOfSigns = met->sumOfSignsComponent(i);
      float c0 = met->calib0Component(i);
      float c1 = met->calib1Component(i);
      // (check TrigEFMissingET/EFMissingETFromHelper) //
      fcalEx  += sumOfSigns * c0 + c1 * ex;
      fcalEy  += sumOfSigns * c0 + c1 * ey;
      fcalEz  += sumOfSigns * c0 + c1 * ez;
      fcalSET += sumOfSigns * c0 + c1 * SumEt;
    } // loop on FCal samplings

    fcalMET = sqrt(fcalEx*fcalEx + fcalEy*fcalEy);
    // subtract FCAL
    centralEx  -= fcalEx;
    centralEy  -= fcalEy;
    centralEz  -= fcalEz;
    centralSET -= fcalSET;
    centralMET = sqrt(centralEx*centralEx + centralEy*centralEy);
    //
    caloCentralEx  -= fcalEx;
    caloCentralEy  -= fcalEy;
    caloCentralEz  -= fcalEz;
    caloCentralSET -= fcalSET;
    caloCentralMET = sqrt(caloCentralEx*caloCentralEx + caloCentralEy*caloCentralEy);
  }

  if( msgLvl() <= MSG::DEBUG &&
      ( !m_doMuonCorrection || fabs(m_forward) > epsilon || fabs(m_central) > epsilon )
    ) {

    msg() << MSG::DEBUG << "doMuonCorrection=" << m_doMuonCorrection
	       << ", Forward=" << m_forward << ", Central=" << m_central << endmsg;
  	msg() << MSG::DEBUG << "doEMScaleTC="<<m_doEMScaleTC << endmsg;  

    char buff[2000];
    std::snprintf(buff,sizeof(buff),
     "REGTEST:            global       calo        muon       central       fcal   caloCentral");
    msg() << MSG::DEBUG << buff << endmsg;

    std::snprintf(buff,sizeof(buff),"REGTEST: Ex/MeV    %11.2f %11.2f %11.2f %11.2f %11.2f %11.2f", 
      Ex ,  caloEx ,  muonEx ,  centralEx ,  fcalEx ,  caloCentralEx);
    msg() << MSG::DEBUG << buff << endmsg;

    std::snprintf(buff,sizeof(buff),"REGTEST: Ey/MeV    %11.2f %11.2f %11.2f %11.2f %11.2f %11.2f", 
      Ey ,  caloEy ,  muonEy ,  centralEy ,  fcalEy ,  caloCentralEy);
    msg() << MSG::DEBUG << buff << endmsg;

    std::snprintf(buff,sizeof(buff),"REGTEST: Ez/MeV    %11.2f %11.2f %11.2f %11.2f %11.2f %11.2f", 
      Ez ,  caloEz ,  muonEz ,  centralEz ,  fcalEz ,  caloCentralEz);
    msg() << MSG::DEBUG << buff << endmsg;

    std::snprintf(buff,sizeof(buff),"REGTEST: MET/MeV   %11.2f %11.2f %11.2f %11.2f %11.2f %11.2f", 
      MET,  caloMET,  muonMET,  centralMET,  fcalMET,  caloCentralMET);
    msg() << MSG::DEBUG << buff << endmsg;

    std::snprintf(buff,sizeof(buff),"REGTEST: SumET/MeV %11.2f %11.2f %11.2f %11.2f %11.2f %11.2f", 
      SET,  caloSET,  muonSET,  centralSET,  fcalSET,  caloCentralSET);
    msg() << MSG::DEBUG << buff << endmsg;
  }

  /// shall we use calorimeters only?
  if (!m_doMuonCorrection) {
    MET=caloMET;
    SET=caloSET;
    Ex=caloEx;
    Ey=caloEy;
    Ez=caloEz;
  }

  /// shall we veto on the central region?
  if ( m_forward < -epsilon ) {
    // reject if central_MET>|m_forward|
    if ( centralMET > std::abs(m_forward) ) {
      pass = false;
      m_rejected++;
      return HLT::OK;
    } else {
      accepted = true; // pass the event
      m_accepted++;
      m_cutCounter = 4;
    }
  }
  else if ( m_forward > epsilon ) {
    // reject if central_MET>|m_forward| OR muon_MET>|m_forward|
    if ( centralMET > m_forward ) {
      pass = false;
      m_rejected++;
      return HLT::OK;
    } 
    else if ( muonMET > m_forward ) {
      pass = false;
      m_rejected++;
      return HLT::OK;
    } 
    else {
      accepted = true; // pass the event
      m_accepted++;
      m_cutCounter = 4;
    }
  }



  /// shall we restrict to the central region?
  if ( !accepted && m_central < -epsilon ) {
    // subtract FCAL before normal checks
    if (m_doMuonCorrection) {
      MET = centralMET;
      SET = centralSET;
      Ex  = centralEx;
      Ey  = centralEy;
      Ez  = centralEz;
    } else { // no muons
      MET = caloCentralMET;
      SET = caloCentralSET;
      Ex  = caloCentralEx;
      Ey  = caloCentralEy;
      Ez  = caloCentralEz;
    }
  }
  else if ( !accepted && m_central > epsilon ) {
    // discard event also if FCAL_MET>m_central
    if ( fcalMET > m_central ) {
      pass = false;
      m_rejected++;
      return HLT::OK;
    }
  }

  // Calculate significance 
  // MET/[a + b*sqrt(SumET) + c*(SumET)]
  float SIG = 0;
  if( SET > 0 )
    {
      float SIG_numerator   = MET/CLHEP::GeV;
      float SIG_denominator = m_significanceOffset + m_significanceSlope*sqrt(SET/CLHEP::GeV) + m_significanceQuadr*(SET/CLHEP::GeV);
      SIG = SIG_numerator/SIG_denominator;
    }
  else SIG=0;
  

  // If the L1 overflow bits are set, 
  // automatically accept 

  // Check num components to determine
  // if HYPO is L2 or EF
  unsigned int componentCutoff = 5;

  if( !accepted && met->getNumberOfComponents() < componentCutoff ) {

    unsigned short maskL1OverflowExEy     = 0x0010; // bit  4
    unsigned short maskL1OverflowSumEt    = 0x0020; // bit  5
    unsigned short overflowMask = maskL1OverflowExEy | maskL1OverflowSumEt;
    
    if( met->flag() & overflowMask ) {
      m_cutCounter = 3;
      accepted = true;
    }
  }

  if (!accepted && m_CutType<-1.5) { // OR of MET and SumET thresholds
    if (MET > m_MEtCut) {
      m_cutCounter = 1;
      accepted = true;
    }
    else if (!accepted && SET > m_SumEtCut) {
      m_cutCounter = 2;
      accepted = true;
    }
  }
  else if (!accepted && m_CutType<-0.5) { // AND of MET and SumET thresholds
    if( MET > m_MEtCut && SET > m_SumEtCut) {
      m_cutCounter = 3;
      accepted = true;
    }
  }
  else if(!accepted && m_CutType>.5) {
    if( MET < m_xsMETmin || SET < m_xsSETmin ) { // reject
      accepted = false;
    }
    else if( MET > m_xsMETok || SET > m_xsSETok ) {
      m_cutCounter = 5;
      accepted = true;
    }
    else if( SIG > m_SigCut ) {
      m_cutCounter = 5;
      accepted = true;
    }
  }

  /*
  else {
    // This version not yet implemented
    if( !accepted && MET > m_CutType*met->SigmaET()) {
      m_cutCounter = 5; // change limits of monitoring histo!
      accepted = true;  // change comments above and in .h!
    }
  }
  */
  else if(!accepted){
    msg() << MSG::WARNING << " WRONG Cut Type.  Event will Fail MET Cut " << endmsg;
  }

  
  if(m_doMETphicut){
	m_phi = atan2f(Ey, Ex); 
	if(m_doLArH12off && m_phi > -2.5 && m_phi < -2.0 ) accepted = false;
	if(m_doLArH11off && m_phi > -2.0 && m_phi < -1.5 ) accepted = false;
  }

  if(accepted) {
    pass = true;
    m_accepted++;

    m_phi = atan2f(Ey, Ex);  // rad

    m_ez_lin = met->ez()*1e-3; // GeV
    m_ex_lin = 1e-3*Ex;        // GeV
    m_ey_lin = 1e-3*Ey;        // GeV
    m_et_lin = 1e-3*MET;       // GeV
    m_sumet_lin = 1e-3*SET;    // GeV

    if ( m_sumet_lin > epsilon) m_xs = m_et_lin / sqrt(m_sumet_lin);
    
    m_xs2 = SIG;
        
    if (m_ex_lin!=0)    m_ex_log = copysign(log10(fabsf(m_ex_lin)), m_ex_lin);
    if (m_ey_lin!=0)    m_ey_log = copysign(log10(fabsf(m_ey_lin)), m_ey_lin);
    if (m_ez_lin!=0)    m_ez_log = copysign(log10(fabsf(m_ez_lin)), m_ez_lin);
    if (m_et_lin!=0)    m_et_log = log10(fabsf(m_et_lin));
    if (m_sumet_lin!=0) m_sumet_log = log10(fabsf(m_sumet_lin));

    if(msgLvl() <= MSG::DEBUG) {
      msg() << MSG::DEBUG << " Event accepted ! " << endmsg;
      msg() << MSG::DEBUG << " REGTEST: Cut Type, MET Thresh/MeV, SumET Thresh/MeV = " << m_CutType
            <<  " " <<  m_MEtCut << " " << m_SumEtCut << endmsg;
      msg() << MSG::DEBUG << " REGTEST: et = " << MET << " MeV" << endmsg;
      msg() << MSG::DEBUG << " REGTEST: ex = " << Ex << " MeV" << endmsg;
      msg() << MSG::DEBUG << " REGTEST: ey = " << Ey << " MeV" << endmsg;
      msg() << MSG::DEBUG << " REGTEST: ez = " << Ez << " MeV" << endmsg;
      msg() << MSG::DEBUG << " REGTEST: SumEt = " << SET << " MeV" << endmsg;
      msg() << MSG::DEBUG << " REGTEST: Sig = " << SIG << endmsg;
    }
  }
  else{
    pass = false;
    if(msgLvl() <= MSG::DEBUG) {
      msg() << MSG::DEBUG << " Event rejected !" << endmsg;
      msg() << MSG::DEBUG << " REGTEST: Cut Type, MET Thresh/MeV, SumET Thresh/MeV = " << m_CutType
            <<  " " <<  m_MEtCut << " " << m_SumEtCut << endmsg;
      msg() << MSG::DEBUG << " REGTEST: et = " << MET << " MeV" << endmsg;
      msg() << MSG::DEBUG << " REGTEST: ex = " << Ex << " MeV" << endmsg;
      msg() << MSG::DEBUG << " REGTEST: ey = " << Ey << " MeV" << endmsg;
      msg() << MSG::DEBUG << " REGTEST: ez = " << Ez << " MeV" << endmsg;
      msg() << MSG::DEBUG << " REGTEST: SumEt = " << SET << " MeV" << endmsg;
      msg() << MSG::DEBUG << " REGTEST: Sig = " << SIG << endmsg;
    }
    m_rejected++;
  }

  // Time total TrigEFCaloHypo execution time.

  if (doTiming() && m_totTime) {
    m_totTime->stop();
  }

  return HLT::OK;
}
 
