/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#include "LArTools/LArMCSymTool.h"
#include "LArCabling/LArCablingLegacyService.h"
#include "AthenaKernel/errorcheck.h"
#include "CaloIdentifier/LArID.h"

LArMCSymTool::LArMCSymTool(const std::string& type, 
                               const std::string& name, 
                               const IInterface* parent) 
  : AthAlgTool(type, name, parent), 
    m_cablingService("LArCablingLegacyService"),
    m_ncellem(0),
    m_ncellhec(0),
    m_ncellfcal(0),
    m_lar_em_id(nullptr),
    m_lar_hec_id(nullptr),
    m_lar_fcal_id(nullptr)
{
declareInterface<ILArMCSymTool>(this);
}

LArMCSymTool::~LArMCSymTool() {}

StatusCode LArMCSymTool::initialize()  {

 ATH_MSG_DEBUG("LArMCSymTool::initialize ");
 CHECK(m_cablingService.retrieve());
 CHECK(detStore()->retrieve(m_lar_em_id,"LArEM_ID"));
 CHECK(detStore()->retrieve(m_lar_hec_id,"LArHEC_ID"));
 CHECK(detStore()->retrieve(m_lar_fcal_id,"LArFCAL_ID"));
  

  m_ncellem=m_lar_em_id->channel_hash_max();
  m_ncellhec=m_lar_hec_id->channel_hash_max();
  m_ncellfcal=m_lar_fcal_id->channel_hash_max();


  const unsigned larHashMax=m_ncellem+m_ncellhec+m_ncellfcal;


  m_listOnline.resize(larHashMax);
  ATH_MSG_DEBUG("total number of cells " << larHashMax);

 CHECK( detStore()->regFcn(&LArCablingLegacyService::iovCallBack,&(*m_cablingService),
                           &LArMCSymTool::iovCallBack,
                           this,true) );

  ATH_MSG_DEBUG("end of initialize of LArMCSymTool");

 return StatusCode::SUCCESS;
}

StatusCode LArMCSymTool::initData() {
  ATH_MSG_DEBUG("in LArMCSymTool::initData ");

  ATH_MSG_DEBUG("Start looping over EM calo cells");
  
  for ( unsigned int idhash=0; idhash<m_lar_em_id->channel_hash_max(); 
	idhash++){
    const Identifier id=m_lar_em_id->channel_id (idhash);

    const int barrel_ec = abs( m_lar_em_id->barrel_ec(id) ) ;
    const int sampling  = m_lar_em_id->sampling(id);
    const int region    = m_lar_em_id->region(id);
    const Identifier regId = m_lar_em_id->region_id(barrel_ec, sampling, region );
    const int eta       = m_lar_em_id->eta(id);
    const int phi       = m_lar_em_id->phi_min(regId);
    const Identifier SymOffId = m_lar_em_id->channel_id(barrel_ec, sampling, region, eta, phi );
    const HWIdentifier SymOnId = m_cablingService->createSignalChannelID(SymOffId);
    const IdentifierHash idHash = m_lar_em_id->channel_hash(id);
    m_listOnline[idHash] = SymOnId;
  }

  ATH_MSG_DEBUG("start loop over HEC calo");

  for ( unsigned int idhash=0; idhash<m_lar_hec_id->channel_hash_max(); 
       idhash++){
    const Identifier id=m_lar_hec_id->channel_id (idhash);

    const int pos_neg   = abs( m_lar_hec_id->pos_neg(id) ) ;
    const int sampling  = m_lar_hec_id->sampling(id);
    const int region    = m_lar_hec_id->region(id);
    const Identifier regId = m_lar_hec_id->region_id(pos_neg, sampling, region );
    const int eta       = m_lar_hec_id->eta(id);
    const int phi       = m_lar_hec_id->phi_min(regId);
    const Identifier SymOffId = m_lar_hec_id->channel_id(pos_neg, sampling, region, eta, phi );
    const HWIdentifier SymOnId = m_cablingService->createSignalChannelID(SymOffId);
    const IdentifierHash idHash=m_lar_hec_id->channel_hash(id);
    m_listOnline[m_ncellem+idHash] = SymOnId;
  }

  ATH_MSG_DEBUG("start loop over FCAL calo");

  const bool isTB= m_lar_fcal_id->dictionaryVersion() == "H6TestBeam"; 
  for ( unsigned int idhash=0; idhash<m_lar_fcal_id->channel_hash_max();
       idhash++){
    const Identifier id=m_lar_fcal_id->channel_id (idhash);

    const int pos_neg   = abs( m_lar_fcal_id->pos_neg(id) ) ;
    const int module    = m_lar_fcal_id->module(id);

// symmetry for FCAL: save all y>0 part of the +side module
//                             y<0 side deduced by point symmetry around x=y=0
    const int eta  = m_lar_fcal_id->eta(id);
    int phi  = m_lar_fcal_id->phi(id);
    if (!isTB) {
      if (phi > 7) phi = phi-8;
    }
    Identifier SymOffId = m_lar_fcal_id->channel_id(pos_neg, module, eta, phi );
    HWIdentifier SymOnId = m_cablingService->createSignalChannelID(SymOffId);
    IdentifierHash idHash=m_lar_fcal_id->channel_hash(id);
    m_listOnline[m_ncellem+m_ncellhec+idHash] = SymOnId;
  }
  ATH_MSG_DEBUG("end of initData of LArMCSymTool ");

  return StatusCode::SUCCESS;
}

StatusCode LArMCSymTool::finalize()
{
 return StatusCode::SUCCESS;
}

HWIdentifier LArMCSymTool::symOnline(const HWIdentifier & id)  const
{

  const Identifier offid = m_cablingService->cnvToIdentifier(id);
  if (!offid.is_valid()) {
    return HWIdentifier();
  }
  return symOnline(offid);
}

HWIdentifier LArMCSymTool::symOnline(const Identifier & id) const
{

  
 IdentifierHash idHash;
 int offset=-1;
 if(m_lar_em_id->is_lar_em(id)) 
 {
    idHash=m_lar_em_id->channel_hash(id);
    offset=0;
 }
 else if(m_lar_hec_id->is_lar_hec(id))
 {
    idHash=m_lar_hec_id->channel_hash(id);
    offset = m_ncellem;
 }
 else if(m_lar_fcal_id->is_lar_fcal(id))
 {
    idHash=m_lar_fcal_id->channel_hash(id);
    offset = m_ncellem+m_ncellhec;
 }
 if (offset <0) ATH_MSG_ERROR( "problem offset " << offset  );
 const unsigned int index = idHash+offset;
 return  m_listOnline[index];
}


StatusCode LArMCSymTool::iovCallBack(IOVSVC_CALLBACK_ARGS) {
  ATH_MSG_INFO("IOV callback");
  ATH_CHECK( initData() );

  return StatusCode::SUCCESS;

}
