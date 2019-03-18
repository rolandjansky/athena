// -*- C++ -*-

/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

/**************************************************************************
 **
 **   File: Trigger/TrigHypothesis/TrigBPhysHypo/TrigL2BMuMuHypo.cxx
 **
 **   Description: L2 hypothesis algorithms for B0_s -> mu+ mu-
 **                Create TrigL2Bphys
 **
 **   Author: S.Sivoklokov (on the base of Carlo Schiavi example)
 **
 **   Created:   05.092006
 **   Modified:  04.05.2007 Add monitoring cutcounter (from Julie's Kirk example)
 **
 **************************************************************************/

#include "TrigL2BMuMuHypo.h"

#include <math.h>
#include "xAODTrigger/TrigPassBits.h"
#include "TrigNavigation/Navigation.h"

#include "TrigBphysHelperUtilsTool.h"
#include "xAODTrigBphys/TrigBphys.h"
#include "xAODTrigBphys/TrigBphysContainer.h"

TrigL2BMuMuHypo::TrigL2BMuMuHypo(const std::string & name, ISvcLocator* pSvcLocator):
    HLT::HypoAlgo(name, pSvcLocator)
,m_bphysHelperTool("TrigBphysHelperUtilsTool")
,m_lastEvent(-1)
,m_lastEventPassed(-1)
,m_countTotalEvents(0)
,m_countTotalRoI(0)
,m_countPassedEvents(0)
,m_countPassedRoIs(0)
,m_countPassedBsMass(0)
,m_countPassedChi2Cut(0)
{

  // Read cuts
  declareProperty("AcceptAll",    m_acceptAll=true);
  declareProperty("AcceptSameMuon",    m_acceptSameMuon=false);
  declareProperty("OppositeSign", m_oppositeCharge=true);
  declareProperty("LowerMassCut", m_lowerMassCut=4000.0);
  declareProperty("UpperMassCut", m_upperMassCut=6000.0);
  declareProperty("ApplyUpperMassCut", m_ApplyupperMassCut=true);
  declareProperty("ApplyChi2Cut", m_ApplyChi2Cut=true);
  declareProperty("Chi2VtxCut", m_Chi2VtxCut=20.0);

  declareMonitoredVariable(    "CutCounter", m_mon_cutCounter);
  declareMonitoredStdContainer("MuMumass",   m_mon_MuMumass   , AutoClear);


}

TrigL2BMuMuHypo::~TrigL2BMuMuHypo()
{ }

HLT::ErrorCode TrigL2BMuMuHypo::hltInitialize()
{

  ATH_MSG_DEBUG("AcceptAll            = " << (m_acceptAll==true ? "True" : "False"));
  ATH_MSG_DEBUG("OppositeCharge       = " << (m_oppositeCharge==true ? "True" : "False"));
  ATH_MSG_DEBUG("LowerMassCut         = " << m_lowerMassCut);
  ATH_MSG_DEBUG("UpperMassCut         = " << m_upperMassCut);
  ATH_MSG_DEBUG("ApplyUpperMassCut         = " << m_ApplyupperMassCut);
  ATH_MSG_DEBUG("ApplyChi2Cut         = " << m_ApplyChi2Cut);
  ATH_MSG_DEBUG("Chi2Cut         = " << m_Chi2VtxCut);

  m_lastEvent = -1;
  m_lastEventPassed = -1;
  m_countTotalEvents =0;
  m_countTotalRoI =0;
  m_countPassedEvents =0;
  m_countPassedRoIs =0;
  m_countPassedBsMass =0;
  m_countPassedChi2Cut =0;

  if (m_bphysHelperTool.retrieve().isFailure()) {
     ATH_MSG_ERROR("Can't find TrigBphysHelperUtilsTool");
     return HLT::BAD_JOB_SETUP;
  } else {
     ATH_MSG_DEBUG("TrigBphysHelperUtilsTool found");
  }
    
  return HLT::OK;
}

HLT::ErrorCode TrigL2BMuMuHypo::hltFinalize()
{
  ATH_MSG_INFO("in finalize()");
  ATH_MSG_INFO("|----------------------- SUMMARY FROM TrigL2BMuMuHypo -------------|");
  ATH_MSG_INFO("Run on events/2xRoIs " << m_countTotalEvents << "/" << m_countTotalRoI);
  ATH_MSG_INFO("Passed events/2xRoIs " << m_countPassedEvents << "/" << m_countPassedRoIs);
  ATH_MSG_INFO("RoIs Passed BsMass: "  << m_countPassedBsMass);

  return HLT::OK;
}

//-------------------------------------------------------------------------------------
HLT::ErrorCode TrigL2BMuMuHypo::hltExecute(const HLT::TriggerElement* outputTE, bool& pass)
{

  //const double BSMASS = 5369.3;
  //const double BDMASS = 5279.2;

  //bool PassedMuMuPair=false;
  bool PassedBsMass=false;
  bool PassedChi2Cut=false;
  bool result = false;
  m_mon_cutCounter = -1;
    // Retrieve event info
    //int IdRun   = 0;
    int IdEvent = 0;
    
    // event info
    uint32_t runNumber(0), evtNumber(0), lbBlock(0);
    if (m_bphysHelperTool->getRunEvtLb( runNumber, evtNumber, lbBlock).isFailure()) {
        ATH_MSG_ERROR("Error retriving EventInfo");
    }
    //IdRun = runNumber;
    IdEvent = evtNumber;

    if (IdEvent != m_lastEvent) {
    m_countTotalEvents++;
    m_lastEvent=IdEvent;
  }
  m_countTotalRoI++;


  // Accept-All mode: temporary patch; should be done with force-accept
  if (m_acceptAll) {
    pass = true;
    //return HLT::OK;
  }
  ATH_MSG_DEBUG("AcceptAll is set to : " << (m_acceptAll ? "True, taking all events " : "False, applying selection" ));

    //  create vector for TrigL2Bphys particles
    // const TrigL2BphysContainer* trigBphysColl = 0;
    const xAOD::TrigBphysContainer* trigBphysColl(nullptr);

  HLT::ErrorCode status = getFeature(outputTE, trigBphysColl, "L2BMuMuFex");

  if ( status != HLT::OK ) {
      ATH_MSG_WARNING("Failed to get TrigBphysics collection");
      ATH_MSG_DEBUG(" End of HLTexecute, pass= " << pass);
      return HLT::OK;
  }

  ATH_MSG_DEBUG(" Retrieved Bphys collection  trigBphysColl = " << trigBphysColl);
  if ( trigBphysColl == 0 ) {
      ATH_MSG_DEBUG("No Bphys particles to analyse, leaving!");
      return HLT::OK;
  }
  ATH_MSG_DEBUG("Got TrigBphys collection with " << trigBphysColl->size() << " TrigBphys particles ");

  // if no Bphys particles were found, just leave TrigBphysColl. empty and leave
  if ( trigBphysColl->size() == 0 ) {
      ATH_MSG_DEBUG("No Bphys particles to analyse, leaving!");
      return HLT::OK;
  }

  m_mon_cutCounter = 0;

  //TrigPassBits *bits = HLT::makeTrigPassBits(trigBphysColl);
  std::unique_ptr<xAOD::TrigPassBits> xBits = xAOD::makeTrigPassBits<xAOD::TrigBphysContainer>(trigBphysColl);

    // now loop over Bphys particles to see if one passes cuts
    for (xAOD::TrigBphysContainer::const_iterator bphysIter = trigBphysColl->begin(); bphysIter !=  trigBphysColl->end(); ++bphysIter) {
        
        if ((*bphysIter)->particleType() == xAOD::TrigBphys::BMUMU ) {
            ATH_MSG_DEBUG("Got Bphys partcile with mass " <<  (*bphysIter)->mass() << " and chi2 " << (*bphysIter)->fitchi2());
            
            if(m_acceptSameMuon && (*bphysIter)->mass() < 0.) {
              PassedBsMass = true;
              PassedChi2Cut = true;
              //HLT::markPassing(bits, *bphysIter, trigBphysColl);
              xBits->markPassing((*bphysIter),trigBphysColl,true);
              ATH_MSG_DEBUG("Candidate with mass " <<  (*bphysIter)->mass() << " accepted as AcceptSameMuon is set");
            }
            
            float BsMass = (*bphysIter)->mass();
            bool thisPassedBsMass = (m_lowerMassCut < BsMass && (BsMass < m_upperMassCut || (!m_ApplyupperMassCut)));
            // PassedBsMass |= thisPassedBsMass;
            
            bool thisPassedChi2Cut = ((!m_ApplyChi2Cut) || ((*bphysIter)->fitchi2() < m_Chi2VtxCut && (*bphysIter)->fitchi2() != -99) );
            // PassedChi2Cut |= thisPassedChi2Cut;
            if (thisPassedBsMass || (m_acceptSameMuon && (*bphysIter)->mass() < 0.) )    m_mon_MuMumass.push_back((BsMass*0.001));
            
            if (thisPassedBsMass)  {
              m_countPassedBsMass++;
              m_mon_cutCounter++;
              if (thisPassedChi2Cut) { 
                m_countPassedChi2Cut++; 
                m_mon_cutCounter++; 
              }
            }
            
            if( thisPassedBsMass && thisPassedChi2Cut )
            {
                PassedBsMass = true;
                PassedChi2Cut = true;
                //HLT::markPassing(bits, *bphysIter, trigBphysColl);
                xBits->markPassing((*bphysIter),trigBphysColl,true);
            }
        }
    }
  
  /*
  if (PassedBsMass)  {
    m_countPassedBsMass++;
    m_mon_cutCounter++;
  }
  if (PassedChi2Cut) { m_countPassedChi2Cut++; m_mon_cutCounter++; }
  */
  
  if ( PassedBsMass && PassedChi2Cut ) {
    result = true;
  }

  if (result) {
    m_countPassedRoIs++;
    if (IdEvent!= m_lastEventPassed) {
      m_countPassedEvents++;
      m_lastEventPassed=IdEvent;
    }
    pass=true;
  }

  // store result
  if(attachFeature(outputTE, xBits.release(),"passbits") != HLT::OK)
      ATH_MSG_ERROR("Could not store TrigPassBits! ");
      ATH_MSG_DEBUG("End of hltExecute, passMass, passChi2, pass= " <<  PassedBsMass << "  " << PassedChi2Cut << "  " << pass);
      return HLT::OK;
}
