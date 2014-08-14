/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/***************************************************************************
    Authors: Natalia Panikashvili
             Shlomit Tarem
***************************************************************************/

#include "TrigBphysHypo/TrigL2DiMuHypo.h"
#include "TrigT1Interfaces/TrigT1Interfaces_ClassDEF.h"
#include "GaudiKernel/IDataProviderSvc.h"
#include "GaudiKernel/IIncidentSvc.h"
#include "GaudiKernel/PropertyMgr.h"
#include "GaudiKernel/SmartDataPtr.h"
#include "TrigInDetEvent/TrigInDetTrack.h"
#include "TrigParticle/TrigL2Bphys.h"
#include "TrigParticle/TrigL2BphysContainer.h"
#include "TrigSteeringEvent/TrigPassBits.h"
#include "TrigNavigation/Navigation.h"

using namespace std;

TrigL2DiMuHypo::TrigL2DiMuHypo(const std::string& name, ISvcLocator* pSvcLocator) :
    HLT::HypoAlgo(name, pSvcLocator),
    m_roiNum          (0),
    m_jpsiNum         (0)
{
  declareProperty("MuMuMassMin", m_MassMin = 2800.);
  declareProperty("MuMuMassMax", m_MassMax = 4000.);
  declareProperty("ApplyOppositeCharge", m_ApplyOppCharge = true);
  declareProperty("ApplyMuMuMassMax", m_ApplyMassMax = true);
  declareProperty("AcceptAll",   m_acceptAll = false);
  declareProperty("NHits",  m_NHits  =  3);
  declareProperty("ApplyChi2Cut",  m_ApplyChi2Cut  =  false);
  declareProperty("Chi2VtxCut",  m_Chi2VtxCut  =  20);

  // monitoring variables
  declareMonitoredVariable(    "NBphys",   mon_nbphys);
  declareMonitoredStdContainer("MuMumass", mon_dimumass, AutoClear);
  declareMonitoredStdContainer("MuEta"   , mon_muEta   , AutoClear);
  declareMonitoredStdContainer("MuPhi"   , mon_muPhi   , AutoClear);
}

TrigL2DiMuHypo::~TrigL2DiMuHypo()
{}

HLT::ErrorCode TrigL2DiMuHypo::hltInitialize()
{
  msg() << MSG::INFO << "Initializing TrigL2DiMuHypo" << endreq;
  return HLT::OK;
}

HLT::ErrorCode TrigL2DiMuHypo::hltFinalize()
{
  msg() << MSG::INFO << "Finalizing TrigL2DiMuHypo" << endreq;
  return HLT::OK;
}

HLT::ErrorCode TrigL2DiMuHypo::hltExecute(const HLT::TriggerElement* outputTE, bool& pass)
{
  pass=false;
  mon_nbphys=0;
  if (msgLvl() <= MSG::DEBUG)
    msg() << MSG::DEBUG << " Executing TrigL2DiMuHypo " << endreq;

  m_pStoreGate = store();

  if (m_acceptAll)
  {
    pass = true;
    if (msgLvl() <= MSG::DEBUG)
      msg() << MSG::DEBUG << "Accept property is set: taking all the events" << endreq;
    return HLT::OK;
  }

  ++m_roiNum;
  int pair_number = 0;
  bool result = false;

    // get vector for TrigL2Bphys particles
  const TrigL2BphysContainer* trigBphysColl;
  HLT::ErrorCode status = getFeature(outputTE, trigBphysColl);
  if (status != HLT::OK )
  {
    if (msgLvl() <= MSG::WARNING)
      msg() << MSG::WARNING << "Failed to get TrigBphysics collection" << endreq;
    return HLT::OK;
  }

  if (trigBphysColl == NULL )
  {
    if (msgLvl() <= MSG::DEBUG )
      msg() << MSG::DEBUG << "No Bphys particles found" << endreq;
    return HLT::OK;
  }

  if (msgLvl() <= MSG::DEBUG )
    msg() << MSG::DEBUG << "Got TrigBphys collection with " << trigBphysColl->size() << endreq;

  TrigPassBits *bits = HLT::makeTrigPassBits(trigBphysColl);

  mon_nbphys=trigBphysColl->size();

  TrigL2BphysContainer::const_iterator
      thePair = trigBphysColl->begin(),
  endPair = trigBphysColl->end();
  for (; thePair != endPair;  thePair++)
  {
    if ((*thePair)->particleType() == TrigL2Bphys::JPSIMUMU)
    {
      float mass = (*thePair)->mass();
      if (m_MassMin < mass && (mass < m_MassMax || (!m_ApplyMassMax)) )
      {
        // apply chi2 cut if requested
        if( m_ApplyChi2Cut && (*thePair)->fitchi2() > m_Chi2VtxCut) continue;
        
        pair_number++;
        if (msgLvl() <= MSG::DEBUG)
          msg() << MSG::DEBUG << " Pair_number " << pair_number << " Invariant mass after cut = " << mass << endreq;
        const ElementLinkVector<TrigInDetTrackCollection> trackVector = (*thePair)->trackVector();
        if (msgLvl() <= MSG::DEBUG)
          msg() << MSG::DEBUG << "Number of tracks in vertex:  " << trackVector.size() << endreq;

        //check that number of tracks is 2
        if(trackVector.size()!=2) {
          msg() << MSG::WARNING << "Number of trigger tracks in J/psi vertex differs from 2 " << endreq;
          continue;
        }

        // check broken ElementLinks
        if( !trackVector.at(0).isValid() || !trackVector.at(1).isValid() ) {
          msg() << MSG::WARNING << "Broken ElementLink." << endreq;
          return HLT::ErrorCode(HLT::Action::CONTINUE, HLT::Reason::USERDEF_1);
        }

        ElementLinkVector<TrigInDetTrackCollection>::const_iterator
            it=trackVector.begin(),
        itEnd=trackVector.end();
        int goodTracks = 0;
        std::vector<float> pT, eta, phi;
        for (int itrk=0; it != itEnd; ++it, ++itrk)
        {
          const TrigInDetTrackFitPar* p_param=(*(*it))->param();
          if (p_param == NULL )
          {
            if (msgLvl() <= MSG::WARNING )
              msg() << MSG::WARNING << "No track parameters for track  " << itrk << endreq;
            continue;
          }
          if (msgLvl() <= MSG::DEBUG)
            msg() << MSG::DEBUG << "Parameters for track " << itrk
                << " pT = " << p_param->pT()
                << " phi = " << p_param->phi0()
                << " eta = " << p_param->eta() << endreq;

          pT .push_back(p_param->pT());
          eta.push_back(p_param->eta());
          phi.push_back(p_param->phi0());
          if (msgLvl() <= MSG::DEBUG)
            msg() << MSG::DEBUG << " HitPattern() : " << (*(*it))->HitPattern() << endreq;
          int32_t word = (*(*it))->HitPattern();
          Encoding* pEncoding = (Encoding*)&word;

          if (msgLvl() <= MSG::DEBUG)
            msg() << MSG::DEBUG << " mdt : " << pEncoding->mdt
                << " tgc : " << pEncoding->tgc
                << " rpc : " << pEncoding->rpc << endreq;

          int32_t mdt_hits = pEncoding->mdt;
          int32_t tgc_hits = pEncoding->tgc;
          int32_t rpc_hits = pEncoding->rpc;

          if ((mdt_hits + rpc_hits) >= m_NHits || (mdt_hits + tgc_hits) >= m_NHits)
          {
            goodTracks++;
            if (msgLvl() <= MSG::DEBUG)
              msg() << MSG::DEBUG << " goodTracks : " << goodTracks << endreq;
          }
        }
        if (goodTracks == 2)
        {
          if (msgLvl() <= MSG::DEBUG)
            msg() << MSG::DEBUG << " goodTracks == 2, pTs: " << pT[0] << " " << pT[1] << " save mass  " << goodTracks << endreq;

          if (m_ApplyOppCharge) {
            if ((pT[0] * pT[1]) < 0.) {
              result = true;
              HLT::markPassing(bits, *thePair, trigBphysColl);
              mon_dimumass.push_back( mass/CLHEP::GeV );
              mon_muEta.push_back(eta[0]);
              mon_muEta.push_back(eta[1]);
              mon_muPhi.push_back(phi[0]);
              mon_muPhi.push_back(phi[1]);
            }
          } else {
            result=true;
            HLT::markPassing(bits, *thePair, trigBphysColl);
            mon_dimumass.push_back( mass/CLHEP::GeV );
            mon_muEta.push_back(eta[0]);
            mon_muEta.push_back(eta[1]);
            mon_muPhi.push_back(phi[0]);
            mon_muPhi.push_back(phi[1]);
          }
        }
      }
    }
  }

  // store result
  if ( attachBits(outputTE, bits) != HLT::OK ) {
    msg() << MSG::ERROR << "Problem attaching TrigPassBits! " << endreq;
  }

  pass = result;
  return HLT::OK;

}




