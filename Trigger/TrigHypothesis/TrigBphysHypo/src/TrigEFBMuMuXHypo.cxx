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
#include "TrigSteeringEvent/TrigPassBits.h"

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
{
  declareProperty("AcceptAll",    m_acceptAll=true); // Should we just accept all events
  
  declareProperty("AcceptBplus",    m_acceptBplus=true); 
  declareProperty("AcceptBd",       m_acceptBd=true);    
  declareProperty("AcceptBs",       m_acceptBs=true);    
  declareProperty("AcceptLb",       m_acceptLb=true);    
  declareProperty("AcceptBc",       m_acceptBc=true);    
  
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

}

TrigEFBMuMuXHypo::~TrigEFBMuMuXHypo()
{ }

HLT::ErrorCode TrigEFBMuMuXHypo::hltInitialize()
{
  msg() << MSG::INFO << "Running TrigEFBMuMuXHypo::hltInitialize" << endreq;

  if (msgLvl() <= MSG::DEBUG) {
    msg() << MSG::DEBUG << " AcceptAll=          " << m_acceptAll << endreq;
    /*
    msg() << MSG::DEBUG << "LowerBMassCut      = " << m_lowerBMassCut << endreq;
    msg() << MSG::DEBUG << "UpperBMassCut      = " << m_upperBMassCut << endreq;
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
   
  return HLT::OK;
}


HLT::ErrorCode TrigEFBMuMuXHypo::hltFinalize()
{

  msg() << MSG::INFO << "Running TrigEFBMuMuXHypo::hltFinalize" << endreq;
  MsgStream log(msgSvc(), name());

  msg() << MSG::INFO << "|----------------------- SUMMARY FROM TrigEFBMuMuXHypo -------------|" << endreq;
  msg() << MSG::INFO << "Run on events/RoIs       " << m_countTotalEvents << "/" << m_countTotalRoI <<  endreq;
  msg() << MSG::INFO << "Passed events/RoIs       " << m_countPassedEvents << "/" << m_countPassedRoIs <<  endreq;
  msg() << MSG::INFO << "RoIs Passed B+:          " << m_countPassedBplus << endreq;
  msg() << MSG::INFO << "RoIs Passed Bd:          " << m_countPassedBd << endreq;
  msg() << MSG::INFO << "RoIs Passed Bs:          " << m_countPassedBs << endreq;
  msg() << MSG::INFO << "RoIs Passed Lb:          " << m_countPassedLb << endreq;
  msg() << MSG::INFO << "RoIs Passed Bc:          " << m_countPassedBc << endreq;

  return HLT::OK;
}


HLT::ErrorCode TrigEFBMuMuXHypo::hltExecute(const HLT::TriggerElement* outputTE, bool& pass)
{
  
  if ( msgLvl() <= MSG::DEBUG )
    msg() << MSG::DEBUG << "Running TrigEFBMuMuXHypo::hltExecute" << endreq;

  bool result = false;
  bool PassedBplus=false;
  bool PassedBd=false;
  bool PassedBs=false;
  bool PassedLb=false;
  bool PassedBc=false;

    // Retrieve event info
    int IdRun   = 0;
    int IdEvent = 0;
    
    // JW - Try to get the xAOD event info
    const EventInfo* pEventInfo(0);
    const xAOD::EventInfo *evtInfo(0);
    if ( store()->retrieve(evtInfo).isFailure() ) {
        if ( msgLvl() <= MSG::DEBUG ) msg()  << MSG::DEBUG << "Failed to get xAOD::EventInfo " << endreq;
        // now try the old event ifo
        if ( store()->retrieve(pEventInfo).isFailure() ) {
            if ( msgLvl() <= MSG::DEBUG ) msg()  << MSG::DEBUG << "Failed to get EventInfo " << endreq;
            //mon_Errors.push_back( ERROR_No_EventInfo );
        } else {
            IdRun   = pEventInfo->event_ID()->run_number();
            IdEvent = pEventInfo->event_ID()->event_number();
            if ( msgLvl() <= MSG::DEBUG ) msg() << MSG::DEBUG << " Run " << IdRun << " Event " << IdEvent <<  endreq;
        }// found old event info
    }else { // found the xAOD event info
        if ( msgLvl() <= MSG::DEBUG ) msg() << MSG::DEBUG << " Run " << evtInfo->runNumber()
            << " Event " << evtInfo->eventNumber() << endreq;
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
      msg() << MSG::DEBUG << "AcceptAll property is set: taking all events" << endreq;
    pass = true;
    return HLT::OK;
  } else {
    if ( msgLvl() <= MSG::DEBUG )
      msg() << MSG::DEBUG << "AcceptAll property not set: applying selection" << endreq;
  }

  if ( msgLvl() <= MSG::DEBUG ){
    msg() << MSG::DEBUG
        << "Using outputTE ( " << outputTE << " ) ->getId(): " << outputTE->getId() << endreq;
  }
  
  // create vector for TrigEFBphys particles
    //  const TrigEFBphysContainer* trigBphysColl = 0;
    const xAOD::TrigBphysContainer * xAODTrigBphysColl(0);
    
  // get vector of TrigEFBphys particles from outputTE
  HLT::ErrorCode status = getFeature(outputTE, xAODTrigBphysColl, "EFBMuMuXFex");
  if ( status != HLT::OK ) {
    if ( msgLvl() <= MSG::WARNING )
      msg() << MSG::WARNING << "Failed to get xAODTrigBphysColl collection" << endreq;
    return HLT::OK;
  }
  
  if(msgLvl() <= MSG::DEBUG) msg() << MSG::DEBUG << " Retrieved Bphys collection  xAODTrigBphysColl = " << xAODTrigBphysColl << endreq;
  if ( xAODTrigBphysColl == 0 ) {
    if ( msgLvl() <= MSG::DEBUG )
      msg() << MSG::DEBUG << "No Bphys particles to analyse, leaving!" << endreq;
    return HLT::OK;
  }
  
  if ( msgLvl() <= MSG::DEBUG ) {
    msg() << MSG::DEBUG << "Got xAODTrigBphysColl collection with " << xAODTrigBphysColl->size()
        << " TrigBphys particles " << endreq;
  }
  
  // if no Bphys particles were found, just leave TrigBphysColl. empty and leave
  if ( xAODTrigBphysColl->size() == 0 ) {
    if ( msgLvl() <= MSG::DEBUG )
      msg() << MSG::DEBUG << "No Bphys particles to analyse, leaving!" << endreq;
//     delete trigBphysColl;
    return HLT::OK;
  }
  
  TrigPassBits *bits = HLT::makeTrigPassBits(xAODTrigBphysColl);

  // now loop over Bphys particles to see if one passes cuts
  for (xAOD::TrigBphysContainer::const_iterator bphysIter = xAODTrigBphysColl->begin();
       bphysIter !=  xAODTrigBphysColl->end(); ++bphysIter) {
    
    //determine decay mode
    std::string decayName;
    xAOD::TrigBphys::pType decayType = (*bphysIter)->particleType();
    if(decayType == xAOD::TrigBphys::BKMUMU)      decayName = "B+ -> mu mu K+";
    if(decayType == xAOD::TrigBphys::BDKSTMUMU)   decayName = "Bd -> mu mu K*";
    if(decayType == xAOD::TrigBphys::BSPHIMUMU)   decayName = "Bs -> mu mu Phi";
    if(decayType == xAOD::TrigBphys::LBLMUMU)     decayName = "Lambda_b -> mu mu Lambda";
    if(decayType == xAOD::TrigBphys::BCDSMUMU)    decayName = "Bc -> mu mu Ds";

    if (msgLvl() <= MSG::DEBUG) msg() << MSG::DEBUG << "Bphys particle type: " << decayName << ", "
        << decayType << " with mass " << (*bphysIter)->mass() << endreq;


//     if ((*bphysIter)->particleType() == TrigEFBphys::BMUMUX ) {
    if (decayType == xAOD::TrigBphys::BKMUMU || decayType == xAOD::TrigBphys::BDKSTMUMU ||
        decayType == xAOD::TrigBphys::BSPHIMUMU || decayType == xAOD::TrigBphys::LBLMUMU ||
        decayType == xAOD::TrigBphys::BCDSMUMU ) {

      bool thisPassedBplus=false;
      bool thisPassedBd=false;
      bool thisPassedBs=false;
      bool thisPassedLb=false;
      bool thisPassedBc=false;
      
      float BMass = (*bphysIter)->mass();
      float BChi2 = (*bphysIter)->fitchi2();
      if ( msgLvl() <= MSG::DEBUG) msg() << MSG::DEBUG << "BMass = " << BMass << endreq;
      if ( msgLvl() <= MSG::DEBUG) msg() << MSG::DEBUG << "BChi2 = " << BChi2 << endreq;
      
      if( (decayType == xAOD::TrigBphys::BKMUMU) ) {
        if( m_acceptBplus ) {
          if ( BMass > m_lowerBplusMassCut && BMass < m_upperBplusMassCut ) {
            if ( msgLvl() <= MSG::DEBUG) msg() << MSG::DEBUG << " Mass =  " << BMass  << " -> B+ Mass passed " << endreq;
            if( BChi2 < m_BplusChi2Cut ) {
              if ( msgLvl() <= MSG::DEBUG) msg() << MSG::DEBUG << "  Chi2 =  " << BChi2  << " -> B+ Chi2 passed " << endreq;
              PassedBplus=true;
              thisPassedBplus=true;
            }
          }
        }
      }
      else { // bD_to_Kstar, bS_to_Phi, lB_to_L, bC_to_Ds
        const xAOD::TrigBphys* trigPartX = (*bphysIter)->secondaryDecay();
        if(!trigPartX) {
          msg() << MSG::WARNING << "No secondary decay pointer in Bphys particle of type " << (*bphysIter)->particleType() << endreq;
        } else {
          float XMass = trigPartX->mass();
          float XChi2 = trigPartX->fitchi2();
          if ( msgLvl() <= MSG::DEBUG) msg() << MSG::DEBUG << "XMass = " << XMass << endreq;
          if ( msgLvl() <= MSG::DEBUG) msg() << MSG::DEBUG << "XChi2 = " << XChi2 << endreq;
          
          // Bd -> Mu Mu Kstar
          if( (decayType == xAOD::TrigBphys::BDKSTMUMU) && m_acceptBd ) {
            if ( BMass > m_lowerBdMassCut && BMass < m_upperBdMassCut ) {
              if ( msgLvl() <= MSG::DEBUG) msg() << MSG::DEBUG << " BMass =  " << BMass  << " -> Bd Mass passed " << endreq;
              if ( XMass > m_lowerKstarMassCut && XMass < m_upperKstarMassCut ) {
                if ( msgLvl() <= MSG::DEBUG) msg() << MSG::DEBUG << "  XMass =  " << XMass  << " -> K* Mass passed " << endreq;
                if( BChi2 < m_BdChi2Cut ) {
                  if ( msgLvl() <= MSG::DEBUG) msg() << MSG::DEBUG << "   BChi2 =  " << BChi2  << " -> Bd Chi2 passed " << endreq;
                  if ( XChi2 < m_KstarChi2Cut ) {
                    if ( msgLvl() <= MSG::DEBUG) msg() << MSG::DEBUG << "    XChi2 =  " << XChi2  << " -> K* Chi2 passed " << endreq;
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
              if ( msgLvl() <= MSG::DEBUG) msg() << MSG::DEBUG << " BMass =  " << BMass  << " -> Bs Mass passed " << endreq;
              if ( XMass > m_lowerPhi1020MassCut && XMass < m_upperPhi1020MassCut ) {
                if ( msgLvl() <= MSG::DEBUG) msg() << MSG::DEBUG << "  XMass =  " << XMass  << " -> Phi Mass passed " << endreq;
                if( BChi2 < m_BsChi2Cut ) {
                  if ( msgLvl() <= MSG::DEBUG) msg() << MSG::DEBUG << "   BChi2 =  " << BChi2  << " -> Bs Chi2 passed " << endreq;
                  if ( XChi2 < m_Phi1020Chi2Cut ) {
                    if ( msgLvl() <= MSG::DEBUG) msg() << MSG::DEBUG << "    XChi2 =  " << XChi2  << " -> Phi Chi2 passed " << endreq;
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
              if ( msgLvl() <= MSG::DEBUG) msg() << MSG::DEBUG << " BMass =  " << BMass  << " -> Lambda_b Mass passed " << endreq;
              if ( XMass > m_lowerLambdaMassCut && XMass < m_upperLambdaMassCut ) {
                if ( msgLvl() <= MSG::DEBUG) msg() << MSG::DEBUG << "  XMass =  " << XMass  << " -> Lambda Mass passed " << endreq;
                if( BChi2 < m_LbChi2Cut ) {
                  if ( msgLvl() <= MSG::DEBUG) msg() << MSG::DEBUG << "   BChi2 =  " << BChi2  << " -> Lambda_b Chi2 passed " << endreq;
                  if ( XChi2 < m_LambdaChi2Cut ) {
                    if ( msgLvl() <= MSG::DEBUG) msg() << MSG::DEBUG << "    XChi2 =  " << XChi2  << " -> Lambda Chi2 passed " << endreq;
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
              if ( msgLvl() <= MSG::DEBUG) msg() << MSG::DEBUG << " BMass =  " << BMass  << " -> Bc Mass passed " << endreq;
              if ( XMass > m_lowerDsMassCut && XMass < m_upperDsMassCut ) {
                if ( msgLvl() <= MSG::DEBUG) msg() << MSG::DEBUG << "  XMass =  " << XMass  << " -> Ds Mass passed " << endreq;
                if( BChi2 < m_BcChi2Cut ) {
                  if ( msgLvl() <= MSG::DEBUG) msg() << MSG::DEBUG << "   BChi2 =  " << BChi2  << " -> Bc Chi2 passed " << endreq;
                  if ( XChi2 < m_DsChi2Cut ) {
                    if ( msgLvl() <= MSG::DEBUG) msg() << MSG::DEBUG << "    XChi2 =  " << XChi2  << " -> Ds Chi2 passed " << endreq;
                    PassedBc=true;
                    thisPassedBc=true;
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
           (thisPassedLb)      ) {
        result=true;
        HLT::markPassing(bits, *bphysIter, xAODTrigBphysColl);
      }
      
    } // end if decayType is ok
  }
  
  if (PassedBplus) m_countPassedBplus++;
  if (PassedBd) m_countPassedBd++;
  if (PassedBs) m_countPassedBs++;
  if (PassedLb) m_countPassedLb++;
  if (PassedBc) m_countPassedBc++;
  
  if (result) {
    m_countPassedRoIs++;
    if (IdEvent!= (int) m_lastEventPassed) {
      m_countPassedEvents++;
      m_lastEventPassed=IdEvent;
    }
  }

  // Reach this point successfully
  if ( msgLvl() <= MSG::DEBUG) msg() << MSG::DEBUG << "Run: " << IdRun << " Event: " << IdEvent << "  result is " << result << endreq;

  if (!m_acceptAll) pass = result;

  // store result
  if ( attachBits(outputTE, bits) != HLT::OK ) {
    msg() << MSG::ERROR << "Problem attaching TrigPassBits! " << endreq;
  }
  
//   delete trigBphysColl;

  return HLT::OK;
}
