/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/***************************************************************************
    Authors: Natalia Panikashvili
             Shlomit Tarem
***************************************************************************/

#include "TrigBphysHypo/TrigL2LooseMuonHypo.h"
#include "TrigT1Interfaces/TrigT1Interfaces_ClassDEF.h"
#include "GaudiKernel/IDataProviderSvc.h"
#include "GaudiKernel/IIncidentSvc.h"
#include "GaudiKernel/PropertyMgr.h"
#include "GaudiKernel/SmartDataPtr.h"
#include "TrigInDetEvent/TrigInDetTrack.h"
#include "TrigParticle/TrigL2Bphys.h"
#include "TrigParticle/TrigL2BphysContainer.h"

using namespace std;

TrigL2LooseMuonHypo::TrigL2LooseMuonHypo(const std::string& name, ISvcLocator* pSvcLocator) :
    HLT::HypoAlgo(name, pSvcLocator)
{
  declareProperty("AcceptAll",      m_acceptAll  = false);
}

TrigL2LooseMuonHypo::~TrigL2LooseMuonHypo()
{}

HLT::ErrorCode TrigL2LooseMuonHypo::hltInitialize()
{
  msg() << MSG::INFO << "Initializing TrigL2LooseMuonHypo" << endreq;
  return HLT::OK;
}

HLT::ErrorCode TrigL2LooseMuonHypo::hltFinalize()
{
  msg() << MSG::INFO << "Finalizing TrigL2LooseMuonHypo" << endreq;
  return HLT::OK;
}

HLT::ErrorCode TrigL2LooseMuonHypo::hltExecute(const HLT::TriggerElement* outputTE, bool& pass)
{
  pass=false;
  if (msgLvl() <= MSG::DEBUG)
    msg() << MSG::DEBUG << " Executing TrigL2LooseMuonHypo " << endreq;

  m_pStoreGate = store();

  if (m_acceptAll)
  {
    pass = true;
    if (msgLvl() <= MSG::DEBUG)
      msg() << MSG::DEBUG << "Accept property is set: taking all the events" << endreq;
    return HLT::OK;
  }

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

  // loop over nbphys
  TrigL2BphysContainer::const_iterator thePair = trigBphysColl->begin();
  for (; thePair != trigBphysColl->end();  thePair++) {
    if ((*thePair)->particleType() == TrigL2Bphys::JPSIMUMU) {

      const ElementLinkVector<TrigInDetTrackCollection> trackVector = (*thePair)->trackVector();

      if (msgLvl() <= MSG::DEBUG)
        msg() << MSG::DEBUG << "Number of tracks in TrigL2Bphys object:  " << trackVector.size() << endreq;

      if(trackVector.size() > 0) {
        // event accepted
        pass = true;
        return HLT::OK;
      }
    }
  }

  // event didn't pass
  return HLT::OK;

}




