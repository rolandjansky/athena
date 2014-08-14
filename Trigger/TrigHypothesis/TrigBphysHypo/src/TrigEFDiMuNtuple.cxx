/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/*----------------------------------------
        Authors: Arik Kreisel & Erez Ra
------------------------------------------*/
#include "GaudiKernel/ISvcLocator.h"
#include "GaudiKernel/IDataProviderSvc.h" 
#include "TrigBphysHypo/TrigEFDiMuFex.h"
#include "TrigBphysHypo/TrigEFDiMuNtuple.h"
#include <math.h>

TrigEFDiMuNtuple::TrigEFDiMuNtuple(TrigEFDiMuFex* pTrigEFDiMuFex, INTupleSvc* pNTupleSvc):
m_max_pair    (100),
m_max_dig     (200),
m_max_idtrack (200),
m_maxTruthMuons(200),
m_pTrigEFDiMuFex(pTrigEFDiMuFex),
m_pNtuple     (NULL),
m_pNTupleSvc  (pNTupleSvc)
{}

StatusCode TrigEFDiMuNtuple::book(const std::string& ntupleName, const std::string& ntupleTitle)
{
    MsgStream log(m_pTrigEFDiMuFex->messageService(), m_pTrigEFDiMuFex->name());
    log << MSG::INFO << " CandidateNtuple initialization" << endreq;

    m_ntupleName = ntupleName;
    m_pNtuple = m_pNTupleSvc->book(m_ntupleName, CLID_ColumnWiseTuple, ntupleTitle);
    if (m_pNtuple == NULL)
    {
        log << MSG::ERROR << " Cannot book Ntuple " << endreq;
        return StatusCode::FAILURE;
    }
    if ( m_pNtuple->addItem("EFDIMU/NumTruthMuon",     m_NumTruthMuon, 0, m_maxTruthMuons).isFailure() ||
         m_pNtuple->addItem("EFDIMU/TruthMuPt" ,       m_NumTruthMuon, m_TruthMuonPt).isFailure()||
         m_pNtuple->addItem("EFDIMU/TruthMuPhi" ,      m_NumTruthMuon, m_TruthMuonPhi).isFailure()||
         m_pNtuple->addItem("EFDIMU/TruthMuCot" ,      m_NumTruthMuon, m_TruthMuonCot).isFailure()||
         m_pNtuple->addItem("EFDIMU/TruthMuEta" ,      m_NumTruthMuon, m_TruthMuonEta).isFailure()||
         m_pNtuple->addItem("EFDIMU/TruthMuPdg" ,      m_NumTruthMuon, m_TruthMuonPdg).isFailure()||
         m_pNtuple->addItem("EFDIMU/TruthMuParentPdg" ,m_NumTruthMuon, m_TruthMuonParentPdg).isFailure())
    {
        log << MSG::ERROR << "Cannot addItem(Truth Muons)" << endreq;
        return StatusCode::FAILURE;
    }

    if (  m_pNtuple->addItem("EFDIMU/NumRecoMuon",     m_NumRecoMuon, 0, m_maxTruthMuons).isFailure() ||
         m_pNtuple->addItem("EFDIMU/RecoMuPt" ,       m_NumRecoMuon, m_RecoMuonPt).isFailure()||
         m_pNtuple->addItem("EFDIMU/RecoMuPhi" ,      m_NumRecoMuon, m_RecoMuonPhi).isFailure()||
         m_pNtuple->addItem("EFDIMU/RecoMuCot" ,      m_NumRecoMuon, m_RecoMuonCot).isFailure()||
         m_pNtuple->addItem("EFDIMU/RecoMuEta" ,      m_NumRecoMuon, m_RecoMuonEta).isFailure()||
         m_pNtuple->addItem("EFDIMU/RecoMuType" ,      m_NumRecoMuon, m_RecoMuonType).isFailure()||
	  m_pNtuple->addItem("EFDIMU/RecoMuCharge" ,      m_NumRecoMuon, m_RecoMuonCharge).isFailure())
    {
        log << MSG::ERROR << "Cannot addItem(Reco Muons)" << endreq;
        return StatusCode::FAILURE;
    }

    if (  m_pNtuple->addItem("EFDIMU/Event",  m_Event).isFailure() ||
	  m_pNtuple->addItem("EFDIMU/Run",  m_Run).isFailure()) 
    {
        log << MSG::ERROR << "Cannot addItem(event number)" << endreq;
        return StatusCode::FAILURE;
    }



    return StatusCode::SUCCESS;
}


StatusCode TrigEFDiMuNtuple::writeRecord()
{
    return m_pNTupleSvc->writeRecord(m_ntupleName);
}
StatusCode TrigEFDiMuNtuple::fillEvent(int event, int run)
{
  MsgStream log(m_pTrigEFDiMuFex->messageService(), m_pTrigEFDiMuFex->name());
  if (log.level() <= MSG::DEBUG)
    log << MSG::INFO << "TrigEFDiMuNtuple::fillEvent(" << event << ")" << endreq;
	
  m_Event=event;
  m_Run=run;

  if (log.level() <= MSG::DEBUG)
    log << MSG::INFO << "TrigEFDiMuNtuple::fillTruth done" << endreq;
  
return StatusCode::SUCCESS;

}

StatusCode TrigEFDiMuNtuple::fillTruth(int nMu, double phi, double cot, double eta, double pt, int pdg, int parentPdg)
{
        MsgStream log(m_pTrigEFDiMuFex->messageService(), m_pTrigEFDiMuFex->name());
        if (log.level() <= MSG::DEBUG)
            log << MSG::INFO << "TrigEFDiMuNtuple::fillTruth(" << nMu << ")" << endreq;

        if (nMu > m_maxTruthMuons)
            return StatusCode::FAILURE;

        m_NumTruthMuon = nMu;
        m_TruthMuonPt       [nMu-1] = pt;
        m_TruthMuonCot      [nMu-1] = cot;
        m_TruthMuonEta      [nMu-1] = eta;
        m_TruthMuonPhi      [nMu-1] = phi;
        m_TruthMuonPdg      [nMu-1] = pdg;
        m_TruthMuonParentPdg[nMu-1] = parentPdg;

	if (log.level() <= MSG::DEBUG)
	  log << MSG::DEBUG << " m_NumTruthMuon = " << m_NumTruthMuon <<
                             " m_TruthMuonPt = " << m_TruthMuonPt[nMu-1] <<
                             " m_TruthMuonEta = " << m_TruthMuonEta[nMu-1] <<
                             " m_TruthMuonPhi = " << m_TruthMuonPhi[nMu-1] <<
                             " m_TruthMuonPdg = " << m_TruthMuonPdg[nMu-1] <<
                             " m_TruthMuonParentPdg = " << m_TruthMuonParentPdg[nMu-1] << endreq;

        if (log.level() <= MSG::INFO)
            log << MSG::INFO << "TrigEFDiMuNtuple::fillTruth done" << endreq;
        return StatusCode::SUCCESS;
}

StatusCode TrigEFDiMuNtuple::fillReco(int nMu, double phi, double cot, double eta, double pt, int charge, int type)
{
        MsgStream log(m_pTrigEFDiMuFex->messageService(), m_pTrigEFDiMuFex->name());
        if (log.level() <= MSG::INFO)
            log << MSG::INFO << "TrigEFDiMuNtuple::fillReco(" << nMu << ")" << endreq;

        if (nMu > m_maxTruthMuons)
            return StatusCode::FAILURE;

        m_NumRecoMuon = nMu;
        m_RecoMuonPt       [nMu-1] = pt;
        m_RecoMuonEta      [nMu-1] = eta;
        m_RecoMuonCot      [nMu-1] = cot;
        m_RecoMuonPhi      [nMu-1] = phi;
        m_RecoMuonCharge   [nMu-1] = charge;
        m_RecoMuonType     [nMu-1] = type;

	if (log.level() <= MSG::DEBUG)
	  log << MSG::DEBUG << " m_NumRecoMuon = " << m_NumRecoMuon <<
                             " m_RecoMuonPt = " << m_RecoMuonPt[nMu-1] <<
                             " m_RecoMuonEta = " << m_RecoMuonEta[nMu-1] <<
                             " m_RecoMuonPhi = " << m_RecoMuonPhi[nMu-1] <<
                             " m_RecoMuonCharge = " << m_RecoMuonCharge[nMu-1] << endreq;

        if (log.level() <= MSG::INFO)
            log << MSG::INFO << "TrigEFDiMuNtuple::fillReco done" << endreq;
        return StatusCode::SUCCESS;

}
