/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// vim: tabstop=2:shiftwidth=2:expandtab
// -*- C++ -*-//
/**************************************************************************
**
**   File: Trigger/TrigHypothesis/TrigBPhysHypo/TrigEFBMuMuXHypo.cxx
**
**   Description: EF hypothesis algorithms for 
**                B^0_{s,d},\Lambda_b \to X \mu^+ \mu^-
**                
**
**   Author: J.Kirk
**   Author: Semen Turchikhin <Semen.Turchikhin@cern.ch>
**
**   Created:   12.09.2007
**   Modified:  08.04.2012
**
**   Modified:  26.09.2012 : "B_c -> D_s(*) (->Phi(->K+K-)) mu+mu-" added (Leonid Gladilin <gladilin@mail.cern.ch>)
**
***************************************************************************/

//#include "TrigMuonEvent/CombinedMuonFeature.h"

#include "TrigEFBMuMuXHypo.h"
#include "TrigParticle/TrigEFBphysContainer.h"

#include "EventInfo/EventInfo.h"
#include "EventInfo/EventID.h"
// #include "TrigSteeringEvent/TrigPassBits.h"
#include "xAODTrigger/TrigPassBits.h"

// additions of xAOD objects
#include "xAODEventInfo/EventInfo.h"

//#include "TrigVKalFitter/TrigVKalFitter.h"
//#include "TrigVKalFitter/VKalVrtAtlas.h"

class ISvcLocator;

TrigEFBMuMuXHypo::TrigEFBMuMuXHypo(const std::string & name, ISvcLocator* pSvcLocator):
  HLT::HypoAlgo(name, pSvcLocator)
//counters
,m_lastEvent(-1)
,m_lastEventPassed(-1)
,m_countTotalEvents(0)
,m_countTotalRoI(0)
,m_countPassedEvents(0)
,m_countPassedRoIs(0)
,m_countPassedBplus(0)
,m_countPassedBd(0)
,m_countPassedBs(0)
,m_countPassedLb(0)
,m_countPassedBc(0)
,m_countPassedBcDplus(0)
,m_countPassedBcDstar(0)
,m_countPassedBcD0(0)
{
  declareProperty("bphysCollectionKey", m_bphysCollectionKey  = "EFBMuMuXFex" );
  declareProperty("AcceptAll",    m_acceptAll=true); // Should we just accept all events
  
  declareProperty("AcceptBplus",    m_acceptBplus=true); 
  declareProperty("AcceptBd",       m_acceptBd=true);    
  declareProperty("AcceptBs",       m_acceptBs=true);    
  declareProperty("AcceptLb",       m_acceptLb=true);    
  declareProperty("AcceptBc",       m_acceptBc=true); // BcDs
  declareProperty("AcceptBcDplus",  m_acceptBcDplus=false);    
  declareProperty("AcceptBcDstar",  m_acceptBcDstar=false);    
  declareProperty("AcceptBcD0",     m_acceptBcD0=false);    
  
  declareProperty("LowerBplusMassCut", m_lowerBplusMassCut = 4500.);                    //4500.00
  declareProperty("UpperBplusMassCut", m_upperBplusMassCut = 5900.);               //5900.00
  declareProperty("BplusChi2Cut", m_BplusChi2Cut = 60.);               //5900.00
  declareProperty("LowerBdMassCut", m_lowerBdMassCut = 4600.);                  //4600.00
  declareProperty("UpperBdMassCut", m_upperBdMassCut = 6000.);             //6000.00
  declareProperty("BdChi2Cut", m_BdChi2Cut = 60.);             //6000.00
  declareProperty("LowerKstarMassCut", m_lowerKstarMassCut = 600.);                  //4600.00
  declareProperty("UpperKstarMassCut", m_upperKstarMassCut = 1200.);             //6000.00
  declareProperty("KstarChi2Cut", m_KstarChi2Cut = 60.);             //6000.00
  declareProperty("LowerBsMassCut", m_lowerBsMassCut = 4600.);                  //4600.00
  declareProperty("UpperBsMassCut", m_upperBsMassCut = 6000.);             //6000.00
  declareProperty("BsChi2Cut", m_BsChi2Cut = 60.);             //6000.00
  declareProperty("LowerPhi1020MassCut", m_lowerPhi1020MassCut = 990.);                  //4600.00
  declareProperty("UpperPhi1020MassCut", m_upperPhi1020MassCut = 1050.);             //6000.00
  declareProperty("Phi1020Chi2Cut", m_Phi1020Chi2Cut = 60.);             //6000.00
  declareProperty("LowerLbMassCut", m_lowerLbMassCut = 4600.);                  //4600.00
  declareProperty("UpperLbMassCut", m_upperLbMassCut = 6400.);             //6400.00
  declareProperty("LbChi2Cut", m_LbChi2Cut = 100.);             //6400.00
  declareProperty("LowerLambdaMassCut", m_lowerLambdaMassCut = 1040.);                  //4600.00
  declareProperty("UpperLambdaMassCut", m_upperLambdaMassCut = 1220.);             //6400.00
  declareProperty("LambdaChi2Cut", m_LambdaChi2Cut = 100.);             //6400.00

  declareProperty("LowerBcMassCut", m_lowerBcMassCut = 1800.);             //1800.
  declareProperty("UpperBcMassCut", m_upperBcMassCut = 7050.);             //7050.
  declareProperty("BcChi2Cut", m_BcChi2Cut = 120.);                        // 120.
  //  declareProperty("LowerPhiDsMassCut", m_lowerPhiDsMassCut =  980.);       // 980.
  //  declareProperty("UpperPhiDsMassCut", m_upperPhiDsMassCut = 1080.);       //1080.
  declareProperty("LowerDsMassCut", m_lowerDsMassCut = 1600.);             //1600.
  declareProperty("UpperDsMassCut", m_upperDsMassCut = 2400.);             //2400.
  declareProperty("DsChi2Cut", m_DsChi2Cut = 90.);                         //  90.
  
  declareProperty("LowerBcDplusMassCut", m_lowerBcDplusMassCut = 5450.);             //1800.
  declareProperty("UpperBcDplusMassCut", m_upperBcDplusMassCut = 7050.);             //7050.
  declareProperty("BcDplusChi2Cut", m_BcDplusChi2Cut = 180.);                        // 120.
  declareProperty("LowerDplusMassCut", m_lowerDplusMassCut = 1500.);             //1600.
  declareProperty("UpperDplusMassCut", m_upperDplusMassCut = 2300.);             //2400.
  declareProperty("DplusChi2Cut", m_DplusChi2Cut = 90.);                         //  90.
  
  declareProperty("LowerBcDstarMassCut", m_lowerBcDstarMassCut = 5450.);             //1800.
  declareProperty("UpperBcDstarMassCut", m_upperBcDstarMassCut = 7050.);             //7050.
  declareProperty("BcDstarChi2Cut", m_BcDstarChi2Cut = 120.);                        // 120.
  declareProperty("LowerDstarMassCut", m_lowerDstarMassCut = 1500.);             //1600.
  declareProperty("UpperDstarMassCut", m_upperDstarMassCut = 2300.);             //2400.
  declareProperty("DstarChi2Cut", m_DstarChi2Cut = 90.);                         //  90.
  
  declareProperty("LowerBcD0MassCut", m_lowerBcD0MassCut = 5250.);             //1800.
  declareProperty("UpperBcD0MassCut", m_upperBcD0MassCut = 6911.);             //7050.
  declareProperty("BcD0Chi2Cut", m_BcD0Chi2Cut = 120.);                        // 120.
  declareProperty("LowerD0MassCut", m_lowerD0MassCut = 1500.);             //1600.
  declareProperty("UpperD0MassCut", m_upperD0MassCut = 2300.);             //2400.
  declareProperty("D0Chi2Cut", m_D0Chi2Cut = 90.);                         //  90.

}

TrigEFBMuMuXHypo::~TrigEFBMuMuXHypo()
{ }

HLT::ErrorCode TrigEFBMuMuXHypo::hltInitialize()
{
  msg() << MSG::INFO << "Running TrigEFBMuMuXHypo::hltInitialize" << endmsg;

  if (msgLvl() <= MSG::DEBUG) {
    msg() << MSG::DEBUG << " AcceptAll=          " << m_acceptAll << endmsg;
    /*
    msg() << MSG::DEBUG << "LowerBMassCut      = " << m_lowerBMassCut << endmsg;
    msg() << MSG::DEBUG << "UpperBMassCut      = " << m_upperBMassCut << endmsg;
    */
  }
  m_lastEvent=-1;
  m_lastEventPassed=-1;
  m_countTotalEvents=0;
  m_countTotalRoI=0;
  m_countPassedEvents=0;
  m_countPassedRoIs=0;
  m_countPassedBplus=0;
  m_countPassedBd=0;
  m_countPassedBs=0;
  m_countPassedLb=0;
  m_countPassedBc=0;
  m_countPassedBcDplus=0;
  m_countPassedBcDstar=0;
  m_countPassedBcD0=0;
   
  return HLT::OK;
}


HLT::ErrorCode TrigEFBMuMuXHypo::hltFinalize()
{

  msg() << MSG::INFO << "Running TrigEFBMuMuXHypo::hltFinalize" << endmsg;
  MsgStream log(msgSvc(), name());

  msg() << MSG::INFO << "|----------------------- SUMMARY FROM TrigEFBMuMuXHypo -------------|" << endmsg;
  msg() << MSG::INFO << "Run on events/RoIs       " << m_countTotalEvents << "/" << m_countTotalRoI <<  endmsg;
  msg() << MSG::INFO << "Passed events/RoIs       " << m_countPassedEvents << "/" << m_countPassedRoIs <<  endmsg;
  msg() << MSG::INFO << "RoIs Passed B+:          " << m_countPassedBplus << endmsg;
  msg() << MSG::INFO << "RoIs Passed Bd:          " << m_countPassedBd << endmsg;
  msg() << MSG::INFO << "RoIs Passed Bs:          " << m_countPassedBs << endmsg;
  msg() << MSG::INFO << "RoIs Passed Lb:          " << m_countPassedLb << endmsg;
  msg() << MSG::INFO << "RoIs Passed Bc:          " << m_countPassedBc << endmsg;
  msg() << MSG::INFO << "RoIs Passed BcDplus:     " << m_countPassedBcDplus << endmsg;
  msg() << MSG::INFO << "RoIs Passed BcDstar:     " << m_countPassedBcDstar << endmsg;
  msg() << MSG::INFO << "RoIs Passed BcD0:        " << m_countPassedBcD0 << endmsg;

  return HLT::OK;
}


HLT::ErrorCode TrigEFBMuMuXHypo::hltExecute(const HLT::TriggerElement* outputTE, bool& pass)
{
  
  if ( msgLvl() <= MSG::DEBUG )
    msg() << MSG::DEBUG << "Running TrigEFBMuMuXHypo::hltExecute" << endmsg;

  bool result = false;
  bool PassedBplus=false;
  bool PassedBd=false;
  bool PassedBs=false;
  bool PassedLb=false;
  bool PassedBc=false;
  bool PassedBcDplus=false;
  bool PassedBcDstar=false;
  bool PassedBcD0=false;

    // Retrieve event info
    int IdRun   = 0;
    int IdEvent = 0;
    
    // JW - Try to get the xAOD event info
    const EventInfo* pEventInfo(0);
    const xAOD::EventInfo *evtInfo(0);
    if ( store()->retrieve(evtInfo).isFailure() ) {
        if ( msgLvl() <= MSG::DEBUG ) msg()  << MSG::DEBUG << "Failed to get xAOD::EventInfo " << endmsg;
        // now try the old event ifo
        if ( store()->retrieve(pEventInfo).isFailure() ) {
            if ( msgLvl() <= MSG::DEBUG ) msg()  << MSG::DEBUG << "Failed to get EventInfo " << endmsg;
            //mon_Errors.push_back( ERROR_No_EventInfo );
        } else {
            IdRun   = pEventInfo->event_ID()->run_number();
            IdEvent = pEventInfo->event_ID()->event_number();
            if ( msgLvl() <= MSG::DEBUG ) msg() << MSG::DEBUG << " Run " << IdRun << " Event " << IdEvent <<  endmsg;
        }// found old event info
    }else { // found the xAOD event info
        if ( msgLvl() <= MSG::DEBUG ) msg() << MSG::DEBUG << " Run " << evtInfo->runNumber()
            << " Event " << evtInfo->eventNumber() << endmsg;
        IdRun   = evtInfo->runNumber();
        IdEvent = evtInfo->eventNumber();
    } // get event ifo
  if (IdEvent != m_lastEvent) {
    m_countTotalEvents++;
    m_lastEvent=IdEvent;
  }
  m_countTotalRoI++;

  // Accept-All mode: temporary patch; should be done with force-accept
  if (m_acceptAll) {
    if ( msgLvl() <= MSG::DEBUG )
      msg() << MSG::DEBUG << "AcceptAll property is set: taking all events" << endmsg;
    pass = true;
    return HLT::OK;
  } else {
    if ( msgLvl() <= MSG::DEBUG )
      msg() << MSG::DEBUG << "AcceptAll property not set: applying selection" << endmsg;
  }

  if ( msgLvl() <= MSG::DEBUG ){
    msg() << MSG::DEBUG
        << "Using outputTE ( " << outputTE << " ) ->getId(): " << outputTE->getId() << endmsg;
  }
  
  // create vector for TrigEFBphys particles
    //  const TrigEFBphysContainer* trigBphysColl = 0;
    const xAOD::TrigBphysContainer * xAODTrigBphysColl(0);
    
  // get vector of TrigEFBphys particles from outputTE
  HLT::ErrorCode status = getFeature(outputTE, xAODTrigBphysColl, m_bphysCollectionKey);
  if ( status != HLT::OK ) {
    if ( msgLvl() <= MSG::WARNING )
      msg() << MSG::WARNING << "Failed to get xAODTrigBphysColl collection" << endmsg;
    return HLT::OK;
  }
  
  if(msgLvl() <= MSG::DEBUG) msg() << MSG::DEBUG << " Retrieved Bphys collection  xAODTrigBphysColl = " << xAODTrigBphysColl << endmsg;
  if ( xAODTrigBphysColl == 0 ) {
    if ( msgLvl() <= MSG::DEBUG )
      msg() << MSG::DEBUG << "No Bphys particles to analyse, leaving!" << endmsg;
    return HLT::OK;
  }
  
  if ( msgLvl() <= MSG::DEBUG ) {
    msg() << MSG::DEBUG << "Got xAODTrigBphysColl collection with " << xAODTrigBphysColl->size()
        << " TrigBphys particles " << endmsg;
  }
  
  // if no Bphys particles were found, just leave TrigBphysColl. empty and leave
  if ( xAODTrigBphysColl->size() == 0 ) {
    if ( msgLvl() <= MSG::DEBUG )
      msg() << MSG::DEBUG << "No Bphys particles to analyse, leaving!" << endmsg;
//     delete trigBphysColl;
    return HLT::OK;
  }
  
  //TrigPassBits *bits = HLT::makeTrigPassBits(xAODTrigBphysColl);
  std::unique_ptr<xAOD::TrigPassBits> xBits = xAOD::makeTrigPassBits<xAOD::TrigBphysContainer>(xAODTrigBphysColl);

  // now loop over Bphys particles to see if one passes cuts
  for (xAOD::TrigBphysContainer::const_iterator bphysIter = xAODTrigBphysColl->begin();
       bphysIter !=  xAODTrigBphysColl->end(); ++bphysIter) {
    
    //determine decay mode
    std::string decayName("UNKNOWN");
    xAOD::TrigBphys::pType decayType = (*bphysIter)->particleType();
    if(decayType == xAOD::TrigBphys::BKMUMU)      decayName = "B+ -> mu mu K+";
    if(decayType == xAOD::TrigBphys::BDKSTMUMU)   decayName = "Bd -> mu mu K*";
    if(decayType == xAOD::TrigBphys::BSPHIMUMU)   decayName = "Bs -> mu mu Phi";
    if(decayType == xAOD::TrigBphys::LBLMUMU)     decayName = "Lambda_b -> mu mu Lambda";
    if(decayType == xAOD::TrigBphys::BCDSMUMU)    decayName = "Bc -> mu mu Ds";
    if(decayType == xAOD::TrigBphys::BCDPMUMU)    decayName = "Bc -> mu mu D+";
    if(decayType == xAOD::TrigBphys::BCDSTMUMU)   decayName = "Bc -> mu mu D*+ or Bc -> mu mu D0";

    if (msgLvl() <= MSG::DEBUG) msg() << MSG::DEBUG << "Bphys particle type: " << decayName << ", "
        << decayType << " with mass " << (*bphysIter)->mass() << endmsg;


//     if ((*bphysIter)->particleType() == TrigEFBphys::BMUMUX ) {
    if (decayType == xAOD::TrigBphys::BKMUMU    || decayType == xAOD::TrigBphys::BDKSTMUMU ||
        decayType == xAOD::TrigBphys::BSPHIMUMU || decayType == xAOD::TrigBphys::LBLMUMU ||
        decayType == xAOD::TrigBphys::BCDSMUMU || decayType == xAOD::TrigBphys::BCDPMUMU 
       || decayType == xAOD::TrigBphys::BCDSTMUMU ) {

      bool thisPassedBplus=false;
      bool thisPassedBd=false;
      bool thisPassedBs=false;
      bool thisPassedLb=false;
      bool thisPassedBc=false;
      bool thisPassedBcDplus=false;
      bool thisPassedBcDstar=false;
      bool thisPassedBcD0=false;
      
      float BMass = (*bphysIter)->mass();
      float BChi2 = (*bphysIter)->fitchi2();
      if ( msgLvl() <= MSG::DEBUG) msg() << MSG::DEBUG << "BMass = " << BMass << endmsg;
      if ( msgLvl() <= MSG::DEBUG) msg() << MSG::DEBUG << "BChi2 = " << BChi2 << endmsg;
      
      if( decayType == xAOD::TrigBphys::BKMUMU ) {
        if( m_acceptBplus ) {
          if ( BMass > m_lowerBplusMassCut && BMass < m_upperBplusMassCut ) {
            if ( msgLvl() <= MSG::DEBUG) msg() << MSG::DEBUG << " Mass =  " << BMass  << " -> B+ Mass passed " << endmsg;
            if( BChi2 < m_BplusChi2Cut /*&& BChi2 >= -0.0001*/ ) {  // allow -nve chi2 for not fitted
              if ( msgLvl() <= MSG::DEBUG) msg() << MSG::DEBUG << "  Chi2 =  " << BChi2  << " -> B+ Chi2 passed " << endmsg;
              PassedBplus=true;
              thisPassedBplus=true;
            }
          }
        }
      }
      else { // bD_to_Kstar, bS_to_Phi, lB_to_L, bC_to_Ds
        const xAOD::TrigBphys* trigPartX = (*bphysIter)->secondaryDecay();
        if(!trigPartX) {
          msg() << MSG::WARNING << "No secondary decay pointer in Bphys particle of type " << (*bphysIter)->particleType() << endmsg;
        } else {
          float XMass = trigPartX->mass();
          float XChi2 = trigPartX->fitchi2();
          int nXParticles = trigPartX->trackParticleLinks().size();
          if ( msgLvl() <= MSG::DEBUG) msg() << MSG::DEBUG << "XMass = " << XMass << endmsg;
          if ( msgLvl() <= MSG::DEBUG) msg() << MSG::DEBUG << "XChi2 = " << XChi2 << endmsg;
          if ( msgLvl() <= MSG::DEBUG) msg() << MSG::DEBUG << "nXParticles = " << nXParticles << endmsg;
          
          // Bd -> Mu Mu Kstar
          if( (decayType == xAOD::TrigBphys::BDKSTMUMU) && m_acceptBd ) {
            if ( BMass > m_lowerBdMassCut && BMass < m_upperBdMassCut ) {
              if ( msgLvl() <= MSG::DEBUG) msg() << MSG::DEBUG << " BMass =  " << BMass  << " -> Bd Mass passed " << endmsg;
              if ( XMass > m_lowerKstarMassCut && XMass < m_upperKstarMassCut ) {
                if ( msgLvl() <= MSG::DEBUG) msg() << MSG::DEBUG << "  XMass =  " << XMass  << " -> K* Mass passed " << endmsg;
                if( BChi2 < m_BdChi2Cut /*&& BChi2 >= -0.0001*/) { // allow -nve chi2 for not fitted
                  if ( msgLvl() <= MSG::DEBUG) msg() << MSG::DEBUG << "   BChi2 =  " << BChi2  << " -> Bd Chi2 passed " << endmsg;
                  if ( XChi2 < m_KstarChi2Cut /*&& XChi2 >= -0.0001*/) { // allow -nve chi2 for not fitted
                    if ( msgLvl() <= MSG::DEBUG) msg() << MSG::DEBUG << "    XChi2 =  " << XChi2  << " -> K* Chi2 passed " << endmsg;
                    PassedBd=true;
                    thisPassedBd=true;
                  }
                }
              }
            }
          }
          
          // Bs -> Mu Mu Phi
          if( (decayType == xAOD::TrigBphys::BSPHIMUMU) && m_acceptBs ) {
            if ( BMass > m_lowerBsMassCut && BMass < m_upperBsMassCut ) {
              if ( msgLvl() <= MSG::DEBUG) msg() << MSG::DEBUG << " BMass =  " << BMass  << " -> Bs Mass passed " << endmsg;
              if ( XMass > m_lowerPhi1020MassCut && XMass < m_upperPhi1020MassCut ) {
                if ( msgLvl() <= MSG::DEBUG) msg() << MSG::DEBUG << "  XMass =  " << XMass  << " -> Phi Mass passed " << endmsg;
                  if( BChi2 < m_BsChi2Cut /*&& BChi2 >= -0.0001*/ ) { // allow -nve chi2 for not fitted
                  if ( msgLvl() <= MSG::DEBUG) msg() << MSG::DEBUG << "   BChi2 =  " << BChi2  << " -> Bs Chi2 passed " << endmsg;
                  if ( XChi2 < m_Phi1020Chi2Cut /*&& XChi2 >= -0.0001*/) { // allow -nve chi2 for not fitted
                    if ( msgLvl() <= MSG::DEBUG) msg() << MSG::DEBUG << "    XChi2 =  " << XChi2  << " -> Phi Chi2 passed " << endmsg;
                    PassedBs=true;
                    thisPassedBs=true;
                  }
                }
              }
            }
          }
          
          // Lambda_b -> Mu Mu Lambda
          if( (decayType == xAOD::TrigBphys::LBLMUMU) && m_acceptLb ) {
            if ( BMass > m_lowerLbMassCut && BMass < m_upperLbMassCut ) {
              if ( msgLvl() <= MSG::DEBUG) msg() << MSG::DEBUG << " BMass =  " << BMass  << " -> Lambda_b Mass passed " << endmsg;
              if ( XMass > m_lowerLambdaMassCut && XMass < m_upperLambdaMassCut ) {
                if ( msgLvl() <= MSG::DEBUG) msg() << MSG::DEBUG << "  XMass =  " << XMass  << " -> Lambda Mass passed " << endmsg;
                if( BChi2 < m_LbChi2Cut /*&& BChi2 >= -0.0001*/) {// allow -nve chi2 for not fitted
                  if ( msgLvl() <= MSG::DEBUG) msg() << MSG::DEBUG << "   BChi2 =  " << BChi2  << " -> Lambda_b Chi2 passed " << endmsg;
                  if ( XChi2 < m_LambdaChi2Cut /*&& XChi2 >= -0.0001*/ ) { // allow -nve chi2 for not fitted
                    if ( msgLvl() <= MSG::DEBUG) msg() << MSG::DEBUG << "    XChi2 =  " << XChi2  << " -> Lambda Chi2 passed " << endmsg;
                    PassedLb=true;
                    thisPassedLb=true;
                  }
                }
              }
            }
          }
          
          // Bc -> Mu Mu Ds
          if( (decayType == xAOD::TrigBphys::BCDSMUMU) && m_acceptBc ) {
            if ( BMass > m_lowerBcMassCut && BMass < m_upperBcMassCut ) {
              if ( msgLvl() <= MSG::DEBUG) msg() << MSG::DEBUG << " BMass =  " << BMass  << " -> Bc Mass passed " << endmsg;
              if ( XMass > m_lowerDsMassCut && XMass < m_upperDsMassCut ) {
                if ( msgLvl() <= MSG::DEBUG) msg() << MSG::DEBUG << "  XMass =  " << XMass  << " -> Ds Mass passed " << endmsg;
                if( BChi2 < m_BcChi2Cut && BChi2 >= -0.0001) {
                  if ( msgLvl() <= MSG::DEBUG) msg() << MSG::DEBUG << "   BChi2 =  " << BChi2  << " -> Bc Chi2 passed " << endmsg;
                  if ( XChi2 < m_DsChi2Cut && XChi2 >= -0.0001) {
                    if ( msgLvl() <= MSG::DEBUG) msg() << MSG::DEBUG << "    XChi2 =  " << XChi2  << " -> Ds Chi2 passed " << endmsg;
                    PassedBc=true;
                    thisPassedBc=true;
                  }
                }
              }
            }
          }
          
          // Bc -> Mu Mu D+
          if( (decayType == xAOD::TrigBphys::BCDPMUMU) && m_acceptBcDplus ) {
            if ( BMass > m_lowerBcDplusMassCut && BMass < m_upperBcDplusMassCut ) {
              if ( msgLvl() <= MSG::DEBUG) msg() << MSG::DEBUG << " BMass =  " << BMass  << " -> BcDplus Mass passed " << endmsg;
              if ( XMass > m_lowerDplusMassCut && XMass < m_upperDplusMassCut ) {
                if ( msgLvl() <= MSG::DEBUG) msg() << MSG::DEBUG << "  XMass =  " << XMass  << " -> Dplus Mass passed " << endmsg;
                if( BChi2 < m_BcDplusChi2Cut && BChi2 >= -0.0001) {
                  if ( msgLvl() <= MSG::DEBUG) msg() << MSG::DEBUG << "   BChi2 =  " << BChi2  << " -> BcDplus Chi2 passed " << endmsg;
                  if ( XChi2 < m_DplusChi2Cut && XChi2 >= -0.0001) {
                    if ( msgLvl() <= MSG::DEBUG) msg() << MSG::DEBUG << "    XChi2 =  " << XChi2  << " -> Dplus Chi2 passed " << endmsg;
                    PassedBcDplus=true;
                    thisPassedBcDplus=true;
                  }
                }
              }
            }
          }
          
          // Bc -> Mu Mu D*
          if( (decayType == xAOD::TrigBphys::BCDSTMUMU) && nXParticles == 3 && m_acceptBcDstar ) {
            if ( BMass > m_lowerBcDstarMassCut && BMass < m_upperBcDstarMassCut ) {
              if ( msgLvl() <= MSG::DEBUG) msg() << MSG::DEBUG << " BMass =  " << BMass  << " -> BcDstar Mass passed " << endmsg;
              if ( XMass > m_lowerDstarMassCut && XMass < m_upperDstarMassCut ) {
                if ( msgLvl() <= MSG::DEBUG) msg() << MSG::DEBUG << "  XMass =  " << XMass  << " -> Dstar Mass passed " << endmsg;
                if( BChi2 < m_BcDstarChi2Cut && BChi2 >= -0.0001) {
                  if ( msgLvl() <= MSG::DEBUG) msg() << MSG::DEBUG << "   BChi2 =  " << BChi2  << " -> BcDstar Chi2 passed " << endmsg;
                  if ( XChi2 < m_DstarChi2Cut && XChi2 >= -0.0001) {
                    if ( msgLvl() <= MSG::DEBUG) msg() << MSG::DEBUG << "    XChi2 =  " << XChi2  << " -> Dstar Chi2 passed " << endmsg;
                    PassedBcDstar=true;
                    thisPassedBcDstar=true;
                  }
                }
              }
            }
          }
          
          // Bc -> Mu Mu D0
          if( (decayType == xAOD::TrigBphys::BCDSTMUMU) && nXParticles == 2 && m_acceptBcD0 ) {
            if ( BMass > m_lowerBcD0MassCut && BMass < m_upperBcD0MassCut ) {
              if ( msgLvl() <= MSG::DEBUG) msg() << MSG::DEBUG << " BMass =  " << BMass  << " -> BcD0 Mass passed " << endmsg;
              if ( XMass > m_lowerD0MassCut && XMass < m_upperD0MassCut ) {
                if ( msgLvl() <= MSG::DEBUG) msg() << MSG::DEBUG << "  XMass =  " << XMass  << " -> D0 Mass passed " << endmsg;
                if( BChi2 < m_BcD0Chi2Cut && BChi2 >= -0.0001) {
                  if ( msgLvl() <= MSG::DEBUG) msg() << MSG::DEBUG << "   BChi2 =  " << BChi2  << " -> BcD0 Chi2 passed " << endmsg;
                  if ( XChi2 < m_D0Chi2Cut && XChi2 >= -0.0001) {
                    if ( msgLvl() <= MSG::DEBUG) msg() << MSG::DEBUG << "    XChi2 =  " << XChi2  << " -> D0 Chi2 passed " << endmsg;
                    PassedBcD0=true;
                    thisPassedBcD0=true;
                  }
                }
              }
            }
          }
          
        } // end if(trigPartX)
      } // end bD_to_Kstar, bS_to_Phi, lB_to_L, bC_to_Ds

      if ( (thisPassedBplus) || 
           (thisPassedBd)    || 
           (thisPassedBs)    || 
           (thisPassedBc)    || 
           (thisPassedBcDplus)    || 
           (thisPassedBcDstar)    || 
           (thisPassedBcD0)    || 
           (thisPassedLb)      ) {
        result=true;
        //HLT::markPassing(bits, *bphysIter, xAODTrigBphysColl);
        xBits->markPassing((*bphysIter),xAODTrigBphysColl,true);
      }
      
    } // end if decayType is ok
  }
  
  if (PassedBplus) m_countPassedBplus++;
  if (PassedBd) m_countPassedBd++;
  if (PassedBs) m_countPassedBs++;
  if (PassedLb) m_countPassedLb++;
  if (PassedBc) m_countPassedBc++;
  if (PassedBcDplus) m_countPassedBcDplus++;
  if (PassedBcDstar) m_countPassedBcDstar++;
  if (PassedBcD0) m_countPassedBcD0++;
  
  if (result) {
    m_countPassedRoIs++;
    if (IdEvent!= (int) m_lastEventPassed) {
      m_countPassedEvents++;
      m_lastEventPassed=IdEvent;
    }
  }

  // Reach this point successfully
  if ( msgLvl() <= MSG::DEBUG) msg() << MSG::DEBUG << "Run: " << IdRun << " Event: " << IdEvent << "  result is " << result << endmsg;

  if (!m_acceptAll) pass = result;

  // store result
  //if ( attachBits(outputTE, bits) != HLT::OK ) {
  //  msg() << MSG::ERROR << "Problem attaching TrigPassBits! " << endmsg;
  //}
  if(attachFeature(outputTE, xBits.release(),"passbits") != HLT::OK)
      ATH_MSG_ERROR("Could not store TrigPassBits! ");
  
//   delete trigBphysColl;

  return HLT::OK;
}
