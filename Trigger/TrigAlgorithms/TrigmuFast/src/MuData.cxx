/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "TrigmuFast/MuData.h"


MuonTDC::MuonTDC() {

  Ipart     = 0;
  DriftTime = 0.;
  //PeMDT     = NULL;
  next      = NULL;
}


MuonTDC::~MuonTDC() {

  //  if(PeMDT) delete PeMDT;
  if(next)  delete next;
}


MuonCham::MuonCham() {

  int i,j;

  type = 0;
  code = 0;
  for(i=0;i<MaxTDC;i++) for(j=0;j<TDCch;j++) TDC[i][j] = 0;
}


MuonCham::~MuonCham() {

  for(int i=0;i<MaxTDC;++i) for(int j=0;j<TDCch;++j) delete TDC[i][j];
}

MDT_ROB::MDT_ROB()
    : half(0), zindex(0), pindex(0)
{
    for(int i=0;i<MaxCham;++i) Ndigits[i] = 0;
    for(int i=0;i<MaxCham;++i) cham[i]    = 0;
    next = 0;
}


MDT_ROB::~MDT_ROB() {

    for (int i=0;i<MaxCham;++i) delete cham[i];
    delete next;
}


//RPC_ROB::RPC_ROB()
//    : type(0), threshold(0), DataPhi(0), triggerNum(0)
//{
//    for(int i=0;i<3;++i) {
//      rpc1[i] = 0.;
//      rpc2[i] = 0.;
//      rpc3[i] = 0.;
//    }
//}

MuData::MuData()
    : m_muon_roi(0),m_rpc_pad(0),m_tgc_rdo(0),m_event_number(0),m_rpc_digits(0),
      m_mdt_digits(0),m_ovl_digits(0)
{
    m_mdt_csm.clear();
    m_ovl_csm.clear();
}

MuData::MuData(int event)
    : m_muon_roi(0),m_rpc_pad(0), m_tgc_rdo(0),m_event_number(event),
      m_rpc_digits(0),m_mdt_digits(0),m_ovl_digits(0)
{
    m_mdt_csm.clear();
    m_ovl_csm.clear();
}

MuData::MuData(const MuData& mu_data)
    : m_muon_roi(mu_data.muon_roi()),m_rpc_pad(mu_data.rpc_pad()),
      m_tgc_rdo(mu_data.tgc_rdo()),m_mdt_csm(mu_data.mdt_csm()),
      m_ovl_csm(mu_data.ovl_csm()),
      m_event_number(mu_data.event()),m_rpc_digits(mu_data.rpc_digits()),
      m_mdt_digits(mu_data.mdt_digits()),m_ovl_digits(mu_data.ovl_digits())
{
}

MuData
MuData::operator=(const MuData& mu_data)
{
    m_muon_roi = mu_data.muon_roi();
    m_rpc_pad  = mu_data.rpc_pad();
    m_mdt_csm  = mu_data.mdt_csm();
    m_ovl_csm  = mu_data.ovl_csm();
    m_tgc_rdo  = mu_data.tgc_rdo();
    m_event_number = mu_data.event();
    m_rpc_digits = mu_data.rpc_digits();
    m_mdt_digits = mu_data.mdt_digits();
    m_ovl_digits = mu_data.ovl_digits();
    m_tgc_digits = mu_data.tgc_digits();
    return *this;
}

void
MuData::get_roi(const LVL1::RecMuonRoI* muon_roi)
{
    m_muon_roi = muon_roi;
}

void
MuData::get_csm(const MdtCsm* mdt_csm)
{
    m_mdt_csm.push_back(mdt_csm);
    m_mdt_digits += mdt_csm->size();
}

void
MuData::get_ovl(const MdtCsm* mdt_ovl)
{
    m_ovl_csm.push_back(mdt_ovl);
    m_ovl_digits += mdt_ovl->size();
}

void
MuData::get_pad(const RpcPad* rpc_csm)
{
    m_rpc_pad = rpc_csm;
    
    RpcPad::const_iterator it3 = m_rpc_pad->begin(); 
    RpcPad::const_iterator it4 = m_rpc_pad->end();
    for (; it3!=it4; ++it3) 
    {
        m_rpc_digits += (*it3)->size();
    }
}


void
MuData::get_tgc(const TgcRdo* tgc_rdo)
{
    m_tgc_rdo = tgc_rdo;
    
    TgcRdo::const_iterator it3 = m_tgc_rdo->begin(); 
    TgcRdo::const_iterator it4 = m_tgc_rdo->end();
    for (; it3!=it4; ++it3) 
    {
        //TgcRawData* pRawData = *it3;
	
	// check Hit or Coincidence
        if ((*it3)->isCoincidence()) continue;

        // select current Hits
        if ((*it3)->bcTag() != 0 && (*it3)->bcTag() != 2 ) continue;
        
	++m_tgc_digits;
    }
}
MuData::~MuData(void)
{
    // delete m_rpc_pad;
    m_mdt_csm.clear();
    m_ovl_csm.clear();
}
