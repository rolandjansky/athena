//dear emacs, this is -*-c++-*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "LArMCSymCondAlg.h"
#include "LArIdentifier/LArOnlineID.h"
#include "CaloIdentifier/CaloCell_ID.h"


LArMCSymCondAlg::LArMCSymCondAlg(const std::string& name, ISvcLocator* pSvcLocator) :
  AthAlgorithm(name, pSvcLocator),
  m_readKey("LArOnOffIdMap"),
  m_writeKey("LArMCSym","LArMCSym"),
  m_condSvc("CondSvc",name) {
  
  declareProperty("ReadKey",m_readKey);
  declareProperty("WriteKey",m_writeKey);
  //declareProperty("isSuperCell",m_isSuperCell,"switch to true to use the SuperCell Identfier helper");
}

LArMCSymCondAlg::~LArMCSymCondAlg() {}

StatusCode LArMCSymCondAlg::initialize() {

  ATH_MSG_DEBUG("initializing");

  // CondSvc
  ATH_CHECK( m_condSvc.retrieve() );
  // Read Handles
  ATH_CHECK( m_readKey.initialize() );
  ATH_CHECK( m_writeKey.initialize() );
  // Register write handle
  if (m_condSvc->regHandle(this, m_writeKey).isFailure()) {
    ATH_MSG_ERROR("unable to register WriteCondHandle " << m_writeKey.fullKey() << " with CondSvc");
    return StatusCode::FAILURE;
  }
  return StatusCode::SUCCESS;
}


StatusCode LArMCSymCondAlg::execute() {
    

  SG::WriteCondHandle<LArMCSym> writeHandle{m_writeKey};
  
  if (writeHandle.isValid()) {
    writeHandle.updateStore(); //????
    ATH_MSG_WARNING("Found valid write handle");
    return StatusCode::SUCCESS;
  }  

  SG::ReadCondHandle<LArOnOffIdMapping> readHandle{m_readKey};
  const LArOnOffIdMapping* cabling{*readHandle};

  if (cabling==nullptr) {
    ATH_MSG_ERROR("Failed to retrieve CondAttributeListCollection with key " << m_readKey.key());
    return StatusCode::FAILURE;
  }

  const LArOnlineID* larOnlineID=nullptr;
  const CaloCell_ID* caloCellID=nullptr;
  const LArEM_ID* lar_em_id=nullptr;
  const LArHEC_ID* lar_hec_id=nullptr;
  const LArFCAL_ID* lar_fcal_id=nullptr;


  ATH_CHECK(detStore()->retrieve(lar_em_id,"LArEM_ID"));
  ATH_CHECK(detStore()->retrieve(lar_hec_id,"LArHEC_ID"));
  ATH_CHECK(detStore()->retrieve(lar_fcal_id,"LArFCAL_ID"));
  ATH_CHECK(detStore()->retrieve(caloCellID,"CaloCell_ID"));
  ATH_CHECK(detStore()->retrieve(larOnlineID,"LArOnlineID"));
  
  const unsigned ncellem=lar_em_id->channel_hash_max();
  const unsigned ncellhec=lar_hec_id->channel_hash_max();
  const unsigned ncellfcal=lar_fcal_id->channel_hash_max();

  const unsigned larHashMax=ncellem+ncellhec+ncellfcal;

  const unsigned onlHashMax=larOnlineID->channelHashMax();

  std::vector<HWIdentifier> oflHashtoSymOnl(larHashMax);
  std::vector<HWIdentifier> onlHashtoSymOnl(onlHashMax);
  
  ATH_MSG_DEBUG("Start looping over EM calo cells");
  for ( unsigned int idhash=0; idhash<ncellem;idhash++){
    const Identifier id=lar_em_id->channel_id (idhash);
    const HWIdentifier hwid=cabling->createSignalChannelID(id);
    const IdentifierHash hwid_hash=larOnlineID->channel_Hash(hwid);
    const int barrel_ec = abs( lar_em_id->barrel_ec(id) ) ;
    const int sampling  = lar_em_id->sampling(id);
    const int region    = lar_em_id->region(id);
    const Identifier regId = lar_em_id->region_id(barrel_ec, sampling, region );
    const int eta       = lar_em_id->eta(id);
    const int phi       = lar_em_id->phi_min(regId);
    const Identifier symOffId = lar_em_id->channel_id(barrel_ec, sampling, region, eta, phi );
    const HWIdentifier symOnId = cabling->createSignalChannelID(symOffId);
    const IdentifierHash idHash = lar_em_id->channel_hash(id);
    oflHashtoSymOnl[idHash] = symOnId;
    onlHashtoSymOnl[hwid_hash]= symOnId;
  }

  ATH_MSG_DEBUG("start loop over HEC calo");

  for ( unsigned int idhash=0; idhash<ncellhec;idhash++){
    const Identifier id=lar_hec_id->channel_id (idhash);
    const HWIdentifier hwid=cabling->createSignalChannelID(id);
    const IdentifierHash hwid_hash=larOnlineID->channel_Hash(hwid);

    const int pos_neg   = abs( lar_hec_id->pos_neg(id) ) ;
    const int sampling  = lar_hec_id->sampling(id);
    const int region    = lar_hec_id->region(id);
    const Identifier regId = lar_hec_id->region_id(pos_neg, sampling, region );
    const int eta       = lar_hec_id->eta(id);
    const int phi       = lar_hec_id->phi_min(regId);
    const Identifier symOffId = lar_hec_id->channel_id(pos_neg, sampling, region, eta, phi );
    const HWIdentifier symOnId = cabling->createSignalChannelID(symOffId);
    const IdentifierHash idHash=lar_hec_id->channel_hash(id);
    oflHashtoSymOnl[ncellem+idHash] = symOnId;
    onlHashtoSymOnl[hwid_hash]= symOnId;
  }

  ATH_MSG_DEBUG("start loop over FCAL calo");

  const bool isTB= lar_fcal_id->dictionaryVersion() == "H6TestBeam"; 
  for ( unsigned int idhash=0; idhash<lar_fcal_id->channel_hash_max();
       idhash++){
    const Identifier id=lar_fcal_id->channel_id (idhash);
    const HWIdentifier hwid=cabling->createSignalChannelID(id);
    const IdentifierHash hwid_hash=larOnlineID->channel_Hash(hwid);


    const int pos_neg   = abs( lar_fcal_id->pos_neg(id) ) ;
    const int module    = lar_fcal_id->module(id);

// symmetry for FCAL: save all y>0 part of the +side module
//                             y<0 side deduced by point symmetry around x=y=0
    const int eta  = lar_fcal_id->eta(id);
    int phi  = lar_fcal_id->phi(id);
    if (!isTB) {
      if (phi > 7) phi = phi-8;
    }
    Identifier symOffId = lar_fcal_id->channel_id(pos_neg, module, eta, phi );
    HWIdentifier symOnId = cabling->createSignalChannelID(symOffId);
    IdentifierHash idHash=lar_fcal_id->channel_hash(id);
    oflHashtoSymOnl[ncellem+ncellhec+idHash] = symOnId;
    onlHashtoSymOnl[hwid_hash]= symOnId;
  }


  std::unique_ptr<LArMCSym> mcSym=std::make_unique<LArMCSym>(larOnlineID,caloCellID,
							     std::move(oflHashtoSymOnl),
							     std::move(onlHashtoSymOnl));


  
  // Define validity of the output cond object and record it
  EventIDRange rangeW;
  if(!readHandle.range(rangeW)) {
    ATH_MSG_ERROR("Failed to retrieve validity range for " << readHandle.key());
    return StatusCode::FAILURE;
  }

  if(writeHandle.record(rangeW,mcSym.release()).isFailure()) {
    ATH_MSG_ERROR("Could not record LArMCSym object with " 
		  << writeHandle.key() 
		  << " with EventRange " << rangeW
		  << " into Conditions Store");
    return StatusCode::FAILURE;
  }
  ATH_MSG_INFO("recorded new " << writeHandle.key() << " with range " << rangeW << " into Conditions Store");

 
  return StatusCode::SUCCESS;
}

