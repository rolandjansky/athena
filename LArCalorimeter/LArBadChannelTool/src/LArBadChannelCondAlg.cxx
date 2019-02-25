/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#include "LArBadChannelTool/LArBadChannelCondAlg.h"
#include "LArBadChannelTool/LArBadChanBlobUtils.h"
#include "LArIdentifier/LArOnlineID.h"
#include "LArBadChannelTool/LArBadChannelDecoder.h"


LArBadChannelCondAlg::LArBadChannelCondAlg(const std::string& name, ISvcLocator* pSvcLocator) :
  AthAlgorithm(name, pSvcLocator),
  m_BCInputKey("/LAR/BadChannelsOfl/BadChannels"),
  m_cablingKey("LArOnOffIdMap"),
  m_BCOutputKey("LArBadChannel","LArBadChannel"),
  m_condSvc("CondSvc",name)
{
  declareProperty("ReadKey",m_BCInputKey);
  declareProperty("CablingKey",m_cablingKey);
  declareProperty("WriteKey",m_BCOutputKey);
  declareProperty("InputFileName",m_inputFileName="");
}

LArBadChannelCondAlg::~LArBadChannelCondAlg() {}


StatusCode LArBadChannelCondAlg::initialize() {

  // CondSvc
  ATH_CHECK( m_condSvc.retrieve() );
  // Read Handles
  ATH_CHECK( m_BCInputKey.initialize() );
  ATH_CHECK( m_BCOutputKey.initialize() );
  ATH_CHECK(m_cablingKey.initialize());
 
  // Register write handle
  if (m_condSvc->regHandle(this, m_BCOutputKey).isFailure()) {
    ATH_MSG_ERROR("unable to register WriteCondHandle " << m_BCOutputKey.fullKey() << " with CondSvc");
    return StatusCode::FAILURE;
  }
  return StatusCode::SUCCESS;
}


StatusCode LArBadChannelCondAlg::execute() {
    
  SG::WriteCondHandle<LArBadChannelCont> writeHandle{m_BCOutputKey};
  
  if (writeHandle.isValid()) {
    msg(MSG::DEBUG) << "Found valid write handle" << endmsg;
    return StatusCode::SUCCESS;
  }  

  SG::ReadCondHandle<CondAttrListCollection> readHandle{m_BCInputKey};
  const CondAttrListCollection* attrListColl{*readHandle};


  SG::ReadCondHandle<LArOnOffIdMapping> cablingHdl{m_cablingKey};
  const LArOnOffIdMapping* cabling{*cablingHdl};

  if (attrListColl==nullptr) {
    msg(MSG::ERROR) << "Failed to retrieve CondAttributeListCollection with key " << m_BCInputKey.key() << endmsg;
    return StatusCode::FAILURE;
  }

  std::unique_ptr<LArBadChannelCont> badChannelCont(new LArBadChannelCont());
  
  //Loop over COOL channels:
   CondAttrListCollection::const_iterator chanIt=attrListColl->begin();
   CondAttrListCollection::const_iterator chanIt_e=attrListColl->end();
   for (;chanIt!=chanIt_e;++chanIt) {
     
     const coral::AttributeList& attrList = chanIt->second;
     const coral::Blob& blob = attrList["Blob"].data<coral::Blob>();
     unsigned int chanSize = attrList["ChannelSize"].data<unsigned int>();
     unsigned int stateSize = attrList["StatusWordSize"].data<unsigned int>();
     unsigned int endian = attrList["Endianness"].data<unsigned int>();
     unsigned int version = attrList["Version"].data<unsigned int>();
    
     std::vector<std::pair<HWIdentifier,LArBadChannel> > bcVec = 
       LArBadChanBlobUtils::decodeBlob<LArBadChannel>( &blob, chanSize, stateSize, endian,
						       version, msg());
   
     for (auto& idBC : bcVec) {
       badChannelCont->add(idBC.first,idBC.second);
     }
     
   }// end loop over COOL channels
   
   if (m_inputFileName.size()) {//Read supplemental data from ASCII file (if required)
     
     const LArOnlineID* onlineID;
     ATH_CHECK(detStore()->retrieve(onlineID,"LArOnlineID"));	       
     LArBadChannelDecoder decoder(&(*onlineID), msg());
     std::vector<std::pair<HWIdentifier,LArBadChannel> > bcVec = decoder.readASCII(m_inputFileName,LArBadChannelState::MAXCOOLCHAN);
     for (auto& idBC : bcVec) {
       badChannelCont->add(idBC.first,idBC.second);
     }
   } //end if have ASCII filename


   
   size_t nChanBeforeMege=badChannelCont->size();
   badChannelCont->sort(); //Sorts vector of bad channels and merges duplicate entries
   
   ATH_MSG_INFO("Read a total of " << badChannelCont->size() << " problematic channels from database");
   if (nChanBeforeMege!=badChannelCont->size()) {
     ATH_MSG_INFO("Merged " << nChanBeforeMege-badChannelCont->size() << " duplicate entries");
   }


   //Fill vector by offline id
   LArBadChannelCont::BadChanVec oflVec;
   for (const auto& entry : badChannelCont->fullCont()) {
     const Identifier id= cabling->cnvToIdentifier(HWIdentifier(entry.first));
     if (id.is_valid()) oflVec.emplace_back(id.get_identifier32().get_compact(),entry.second);
   }
   
   badChannelCont->setOflVec(oflVec);
   

  // Define validity of the output cond object and record it
  EventIDRange rangeW;
  if(!readHandle.range(rangeW)) {
    ATH_MSG_ERROR("Failed to retrieve validity range for " << readHandle.key());
    return StatusCode::FAILURE;
  }

  if(writeHandle.record(rangeW,badChannelCont.release()).isFailure()) {
    ATH_MSG_ERROR("Could not record LArBadChannelCont object with " 
		  << writeHandle.key() 
		  << " with EventRange " << rangeW
		  << " into Conditions Store");
    return StatusCode::FAILURE;
  }
  return StatusCode::SUCCESS;
}
