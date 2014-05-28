/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/*----------------------------------------
        Authors: Arik Kreisel & Erez Ra
------------------------------------------*/
#include "GaudiKernel/ISvcLocator.h"
#include "GaudiKernel/IDataProviderSvc.h" 
#include "TrigMuGirl/TrigMuGirl.h"
#include "TrigMuGirl/TrigMuGirlNtuple.h"
#include <math.h>

TrigMuGirlNtuple::TrigMuGirlNtuple(HLT::FexAlgo* pTrigMuGirl, INTupleSvc* pNTupleSvc):
m_max_pair    (100),
m_max_dig     (200),
m_max_idtrack (200),
m_maxTruthMuons(200),
m_pTrigMuGirl(pTrigMuGirl),
m_pNtuple     (NULL),
m_pNTupleSvc  (pNTupleSvc)
{}

StatusCode TrigMuGirlNtuple::book(const std::string& ntupleName, const std::string& ntupleTitle)
{
    MsgStream log(m_pTrigMuGirl->messageService(), m_pTrigMuGirl->name());
    log << MSG::INFO << " CandidateNtuple initialization" << endreq;

    m_ntupleName = ntupleName;
    m_pNtuple = m_pNTupleSvc->book(m_ntupleName, CLID_ColumnWiseTuple, ntupleTitle);
    if (m_pNtuple == NULL)
    {
        log << MSG::ERROR << " Cannot book Ntuple " << endreq;
        return StatusCode::FAILURE;
    }
    if ( m_pNtuple->addItem("TRIGMUGIRL/NumTruthMuon",     m_NumTruthMuon, 0, m_maxTruthMuons).isFailure() ||
         m_pNtuple->addItem("TRIGMUGIRL/TruthMuPt" ,       m_NumTruthMuon, m_TruthMuonPt).isFailure()||
         m_pNtuple->addItem("TRIGMUGIRL/TruthMuPhi" ,      m_NumTruthMuon, m_TruthMuonPhi).isFailure()||
         m_pNtuple->addItem("TRIGMUGIRL/TruthMuCot" ,      m_NumTruthMuon, m_TruthMuonCot).isFailure()||
         m_pNtuple->addItem("TRIGMUGIRL/TruthMuEta" ,      m_NumTruthMuon, m_TruthMuonEta).isFailure()||
         m_pNtuple->addItem("TRIGMUGIRL/TruthMuPdg" ,      m_NumTruthMuon, m_TruthMuonPdg).isFailure()||
         m_pNtuple->addItem("TRIGMUGIRL/TruthMuParentPdg" ,m_NumTruthMuon, m_TruthMuonParentPdg).isFailure())
    {
        log << MSG::ERROR << "Cannot addItem(Truth Muons)" << endreq;
        return StatusCode::FAILURE;
    }

    if (  m_pNtuple->addItem("TRIGMUGIRL/NumRecoMuon",     m_NumRecoMuon, 0, m_maxTruthMuons).isFailure() ||
         m_pNtuple->addItem("TRIGMUGIRL/RecoMuPt" ,       m_NumRecoMuon, m_RecoMuonPt).isFailure()||
         m_pNtuple->addItem("TRIGMUGIRL/RecoMuPhi" ,      m_NumRecoMuon, m_RecoMuonPhi).isFailure()||
         m_pNtuple->addItem("TRIGMUGIRL/RecoMuCot" ,      m_NumRecoMuon, m_RecoMuonCot).isFailure()||
         m_pNtuple->addItem("TRIGMUGIRL/RecoMuEta" ,      m_NumRecoMuon, m_RecoMuonEta).isFailure()||
	  m_pNtuple->addItem("TRIGMUGIRL/RecoMuCharge" ,      m_NumRecoMuon, m_RecoMuonCharge).isFailure()||
	  m_pNtuple->addItem("TRIGMUGIRL/RecoMuNN0" ,      m_NumRecoMuon, m_RecoMuonNN0).isFailure()||
	  m_pNtuple->addItem("TRIGMUGIRL/RecoMuNN1" ,      m_NumRecoMuon, m_RecoMuonNN1).isFailure()||
	  m_pNtuple->addItem("TRIGMUGIRL/RecoMuNN2" ,      m_NumRecoMuon, m_RecoMuonNN2).isFailure()||
	  m_pNtuple->addItem("TRIGMUGIRL/RecoMuNN3" ,      m_NumRecoMuon, m_RecoMuonNN3).isFailure()||
	  m_pNtuple->addItem("TRIGMUGIRL/RecoMuNN4" ,      m_NumRecoMuon, m_RecoMuonNN4).isFailure()||
	  m_pNtuple->addItem("TRIGMUGIRL/RecoMuNN5" ,      m_NumRecoMuon, m_RecoMuonNN5).isFailure()||
	  m_pNtuple->addItem("TRIGMUGIRL/RecoLHR" ,      m_NumRecoMuon, m_RecoMuonLHR).isFailure()||
	  m_pNtuple->addItem("TRIGMUGIRL/RecoIso" ,      m_NumRecoMuon, m_RecoMuonIso).isFailure()||
	  m_pNtuple->addItem("TRIGMUGIRL/RecodR" ,      m_NumRecoMuon, m_RecoMuondR).isFailure())
    {
        log << MSG::ERROR << "Cannot addItem(Reco Muons)" << endreq;
        return StatusCode::FAILURE;
    }

    if (  m_pNtuple->addItem("TRIGMUGIRL/Event",  m_Event).isFailure() ||
	  m_pNtuple->addItem("TRIGMUGIRL/Run",  m_Run).isFailure()) 
    {
        log << MSG::ERROR << "Cannot addItem(event number)" << endreq;
        return StatusCode::FAILURE;
    }



    return StatusCode::SUCCESS;
}


StatusCode TrigMuGirlNtuple::writeRecord()
{
    return m_pNTupleSvc->writeRecord(m_ntupleName);
}
StatusCode TrigMuGirlNtuple::fillEvent(int event, int run)
{
  MsgStream log(m_pTrigMuGirl->messageService(), m_pTrigMuGirl->name());
  if (log.level() <= MSG::DEBUG)
    log << MSG::INFO << "TrigMuGirlNtuple::fillEvent(" << event << ")" << endreq;
	
  m_Event=event;
  m_Run=run;

  if (log.level() <= MSG::DEBUG)
    log << MSG::INFO << "TrigMuGirlNtuple::fillTruth done" << endreq;
  
return StatusCode::SUCCESS;

}

StatusCode TrigMuGirlNtuple::fillTruth(int nMu, double phi, double cot, double eta, double pt, int pdg, int parentPdg)
{
        MsgStream log(m_pTrigMuGirl->messageService(), m_pTrigMuGirl->name());
        if (log.level() <= MSG::DEBUG)
            log << MSG::INFO << "TrigMuGirlNtuple::fillTruth(" << nMu << ")" << endreq;

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
            log << MSG::INFO << "TrigMuGirlNtuple::fillTruth done" << endreq;
        return StatusCode::SUCCESS;
}

StatusCode TrigMuGirlNtuple::fillReco(int nMu, double phi, double cot, double eta, double pt, int charge, float NN0, float NN1, float NN2, float NN3, float NN4, float NN5, float LHR, float Iso ,float dR)
{
        MsgStream log(m_pTrigMuGirl->messageService(), m_pTrigMuGirl->name());
        if (log.level() <= MSG::DEBUG)
            log << MSG::INFO << "TrigMuGirlNtuple::fillReco(" << nMu << ")" << endreq;

	//        log << MSG::DEBUG << " nMu= " << nMu <<
	//                             " pt = " << pt <<
	//                             " eta = " << eta <<
	//                             " phi = " << phi <<
	//                             " charge = " << charge << 
	//                             " NN0 = " << NN0 <<
	//                             " NN1 = " << NN1 <<
	//                             " NN2 = " << NN2 <<
	//                             " NN3 = " << NN3 <<
	//                             " NN4 = " << NN4 <<
	//                             " NN5 = " << NN5 <<
	//                             " m_maxTruthMuons = " << m_maxTruthMuons <<endreq;

        if (nMu > m_maxTruthMuons)
            return StatusCode::FAILURE;

        m_NumRecoMuon = nMu;
        m_RecoMuonPt       [nMu-1] = pt;
        m_RecoMuonEta      [nMu-1] = eta;
        m_RecoMuonCot      [nMu-1] = cot;
        m_RecoMuonPhi      [nMu-1] = phi;
        m_RecoMuonCharge   [nMu-1] = charge;
	m_RecoMuonNN0      [nMu-1] = NN0;
	m_RecoMuonNN1      [nMu-1] = NN1;
	m_RecoMuonNN2      [nMu-1] = NN2;
	m_RecoMuonNN3      [nMu-1] = NN3;
	m_RecoMuonNN4      [nMu-1] = NN4;
	m_RecoMuonNN5      [nMu-1] = NN5;
	m_RecoMuonLHR      [nMu-1] = LHR;
	m_RecoMuonIso      [nMu-1] = Iso;
	m_RecoMuondR       [nMu-1] = dR;

	//        log << MSG::DEBUG << " m_NumRecoMuon = " << m_NumRecoMuon <<
	//                             " m_RecoMuonPt = " << m_RecoMuonPt[nMu-1] <<
	//                             " m_RecoMuonEta = " << m_RecoMuonEta[nMu-1] <<
	//                             " m_RecoMuonPhi = " << m_RecoMuonPhi[nMu-1] <<
	//                             " m_RecoMuonCharge = " << m_RecoMuonCharge[nMu-1] << endreq;

        if (log.level() <= MSG::DEBUG)
            log << MSG::INFO << "TrigMuGirlNtuple::fillReco done" << endreq;
        return StatusCode::SUCCESS;

}
