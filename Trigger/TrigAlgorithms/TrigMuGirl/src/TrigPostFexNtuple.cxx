/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/*----------------------------------------
        Authors: Arik Kreisel & Erez Ra
------------------------------------------*/
#include "GaudiKernel/ISvcLocator.h"
#include "GaudiKernel/IDataProviderSvc.h" 
#include "TrigMuGirl/TrigPostFex.h"
#include "TrigMuGirl/TrigPostFexNtuple.h"
#include <math.h>

TrigPostFexNtuple::TrigPostFexNtuple(TrigPostFex* pTrigPostFex, INTupleSvc* pNTupleSvc):
m_max_pair    (100),
m_max_dig     (200),
m_max_idtrack (200),
m_maxTruthMuons(200),
m_pTrigPostFex(pTrigPostFex),
m_pNtuple     (NULL),
m_pNTupleSvc  (pNTupleSvc)
{}

StatusCode TrigPostFexNtuple::book(const std::string& ntupleName, const std::string& ntupleTitle)
{
    MsgStream log(m_pTrigPostFex->messageService(), m_pTrigPostFex->name());
    log << MSG::INFO << " CandidateNtuple initialization" << endreq;

    m_ntupleName = ntupleName;
    m_pNtuple = m_pNTupleSvc->book(m_ntupleName, CLID_ColumnWiseTuple, ntupleTitle);
    if (m_pNtuple == NULL)
    {
        log << MSG::ERROR << " Cannot book Ntuple " << endreq;
        return StatusCode::FAILURE;
    }
    if ( m_pNtuple->addItem("POSTFEX/NumTruthMuon",     m_NumTruthMuon, 0, m_maxTruthMuons).isFailure() ||
         m_pNtuple->addItem("POSTFEX/TruthMuPt" ,       m_NumTruthMuon, m_TruthMuonPt).isFailure()||
         m_pNtuple->addItem("POSTFEX/TruthMuPhi" ,      m_NumTruthMuon, m_TruthMuonPhi).isFailure()||
         m_pNtuple->addItem("POSTFEX/TruthMuCot" ,      m_NumTruthMuon, m_TruthMuonCot).isFailure()||
         m_pNtuple->addItem("POSTFEX/TruthMuEta" ,      m_NumTruthMuon, m_TruthMuonEta).isFailure()||
         m_pNtuple->addItem("POSTFEX/TruthMuPdg" ,      m_NumTruthMuon, m_TruthMuonPdg).isFailure()||
         m_pNtuple->addItem("POSTFEX/TruthMuParentPdg" ,m_NumTruthMuon, m_TruthMuonParentPdg).isFailure())
    {
        log << MSG::ERROR << "Cannot addItem(Truth Muons)" << endreq;
        return StatusCode::FAILURE;
    }

    if (  m_pNtuple->addItem("POSTFEX/NumRecoMuon",     m_NumRecoMuon, 0, m_maxTruthMuons).isFailure() ||
         m_pNtuple->addItem("POSTFEX/RecoMuPt" ,       m_NumRecoMuon, m_RecoMuonPt).isFailure()||
         m_pNtuple->addItem("POSTFEX/RecoMuPhi" ,      m_NumRecoMuon, m_RecoMuonPhi).isFailure()||
         m_pNtuple->addItem("POSTFEX/RecoMuCot" ,      m_NumRecoMuon, m_RecoMuonCot).isFailure()||
         m_pNtuple->addItem("POSTFEX/RecoMuEta" ,      m_NumRecoMuon, m_RecoMuonEta).isFailure()||
	  m_pNtuple->addItem("POSTFEX/RecoMuCharge" ,      m_NumRecoMuon, m_RecoMuonCharge).isFailure())
    {
        log << MSG::ERROR << "Cannot addItem(Reco Muons)" << endreq;
        return StatusCode::FAILURE;
    }

    if (  m_pNtuple->addItem("POSTFEX/Event",  m_Event).isFailure() ||
	  m_pNtuple->addItem("POSTFEX/Run",  m_Run).isFailure()) 
    {
        log << MSG::ERROR << "Cannot addItem(event number)" << endreq;
        return StatusCode::FAILURE;
    }



    return StatusCode::SUCCESS;
}


StatusCode TrigPostFexNtuple::writeRecord()
{
    return m_pNTupleSvc->writeRecord(m_ntupleName);
}
StatusCode TrigPostFexNtuple::fillEvent(int event, int run)
{
  MsgStream log(m_pTrigPostFex->messageService(), m_pTrigPostFex->name());
  if (log.level() <= MSG::DEBUG)
    log << MSG::INFO << "TrigPostFexNtuple::fillEvent(" << event << ")" << endreq;
	
  m_Event=event;
  m_Run=run;

  if (log.level() <= MSG::DEBUG)
    log << MSG::INFO << "TrigPostFexNtuple::fillTruth done" << endreq;
  
return StatusCode::SUCCESS;

}

StatusCode TrigPostFexNtuple::fillTruth(int nMu, double phi, double cot, double eta, double pt, int pdg, int parentPdg)
{
        MsgStream log(m_pTrigPostFex->messageService(), m_pTrigPostFex->name());
        if (log.level() <= MSG::DEBUG)
            log << MSG::INFO << "TrigPostFexNtuple::fillTruth(" << nMu << ")" << endreq;

        if (nMu > m_maxTruthMuons)
            return StatusCode::FAILURE;

        m_NumTruthMuon = nMu;
        m_TruthMuonPt       [nMu-1] = pt;
        m_TruthMuonCot      [nMu-1] = cot;
        m_TruthMuonEta      [nMu-1] = eta;
        m_TruthMuonPhi      [nMu-1] = phi;
        m_TruthMuonPdg      [nMu-1] = pdg;
        m_TruthMuonParentPdg[nMu-1] = parentPdg;

	//        log << MSG::DEBUG << " m_NumTruthMuon = " << m_NumTruthMuon <<
	//                             " m_TruthMuonPt = " << m_TruthMuonPt[nMu-1] <<
	//                             " m_TruthMuonEta = " << m_TruthMuonEta[nMu-1] <<
	//                             " m_TruthMuonPhi = " << m_TruthMuonPhi[nMu-1] <<
	//                             " m_TruthMuonPdg = " << m_TruthMuonPdg[nMu-1] <<
	//                             " m_TruthMuonParentPdg = " << m_TruthMuonParentPdg[nMu-1] << endreq;

        if (log.level() <= MSG::DEBUG)
            log << MSG::INFO << "TrigPostFexNtuple::fillTruth done" << endreq;
        return StatusCode::SUCCESS;
}

StatusCode TrigPostFexNtuple::fillReco(int nMu, double phi, double cot, double eta, double pt, int charge)
{
        MsgStream log(m_pTrigPostFex->messageService(), m_pTrigPostFex->name());
        if (log.level() <= MSG::DEBUG)
            log << MSG::INFO << "TrigPostFexNtuple::fillReco(" << nMu << ")" << endreq;

        if (nMu > m_maxTruthMuons)
            return StatusCode::FAILURE;

        m_NumRecoMuon = nMu;
        m_RecoMuonPt       [nMu-1] = pt;
        m_RecoMuonEta      [nMu-1] = eta;
        m_RecoMuonCot      [nMu-1] = cot;
        m_RecoMuonPhi      [nMu-1] = phi;
        m_RecoMuonCharge   [nMu-1] = charge;

	//        log << MSG::DEBUG << " m_NumRecoMuon = " << m_NumRecoMuon <<
	//                             " m_RecoMuonPt = " << m_RecoMuonPt[nMu-1] <<
	//                             " m_RecoMuonEta = " << m_RecoMuonEta[nMu-1] <<
	//                             " m_RecoMuonPhi = " << m_RecoMuonPhi[nMu-1] <<
	//                             " m_RecoMuonCharge = " << m_RecoMuonCharge[nMu-1] << endreq;

        if (log.level() <= MSG::DEBUG)
            log << MSG::INFO << "TrigPostFexNtuple::fillReco done" << endreq;
        return StatusCode::SUCCESS;

}
