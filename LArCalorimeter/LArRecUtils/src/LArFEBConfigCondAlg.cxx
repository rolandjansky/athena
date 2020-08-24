/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#include "LArFEBConfigCondAlg.h"
#include "LArIdentifier/LArOnlineID.h"


LArFEBConfigCondAlg::LArFEBConfigCondAlg(const std::string& name, ISvcLocator* pSvcLocator) :
  AthReentrantAlgorithm(name,pSvcLocator),m_onlineID(nullptr) {}


LArFEBConfigCondAlg::~LArFEBConfigCondAlg() {}

StatusCode LArFEBConfigCondAlg::initialize() {
  
  ATH_MSG_DEBUG(" trying initialize");

  ATH_CHECK(detStore()->retrieve(m_onlineID,"LArOnlineID"));

  if (m_listOfFolders.size()==0) {
    ATH_MSG_WARNING( "List of folders is emtpy, do nothing");
    return StatusCode::SUCCESS;
  }
  
  ATH_CHECK(m_listOfFolders.initialize());
  ATH_CHECK(m_configKey.initialize());

  ATH_MSG_DEBUG("Successfully initialized LArFEBConfigCondAlg");
  return StatusCode::SUCCESS;
} 


  
StatusCode LArFEBConfigCondAlg::execute(const EventContext& ctx) const {
  ATH_MSG_DEBUG("executing");

  SG::WriteCondHandle<LArFebConfig> writeHandle{m_configKey, ctx};
  if (writeHandle.isValid()) {
      ATH_MSG_DEBUG("Found valid write LArFebConfig handle");
      return StatusCode::SUCCESS;
  }


  std::vector<const CondAttrListCollection*> attrvec;

  for (const auto& fldkey: m_listOfFolders) {
     SG::ReadCondHandle<CondAttrListCollection> cHdl(fldkey, ctx);
     const CondAttrListCollection* cattr = *cHdl;
     if(cattr) {
        ATH_MSG_DEBUG("Folder: "<<cHdl.key()<<" has size: "<<std::distance(cattr->begin(),cattr->end()));
        attrvec.push_back(cattr);
	writeHandle.addDependency(cHdl);
     } else {
         ATH_MSG_WARNING("Why do not have FEB config folder " << fldkey.fullKey());
     }
  } 

  // Fill LArFebConfig
  std::unique_ptr<LArFebConfig> febConfig=std::make_unique<LArFebConfig>(m_onlineID);
  LArFebConfig* p_febConfig = febConfig.get();

  unsigned nFebs=0;
  for(auto dh: attrvec){
    CondAttrListCollection::const_iterator chanit=dh->begin();
    CondAttrListCollection::const_iterator chanit_e=dh->end();
    for (;chanit!=chanit_e;++chanit) {
      if (chanit->first==0) {
	ATH_MSG_DEBUG("Invalid channel number 0, ignoring...");
	continue;
      }  
      const HWIdentifier fid(chanit->first); //COOL channel number == FEB identifier
      //const coral::AttributeList& attr = chanit->second;
      ATH_MSG_VERBOSE("Working on FEB 0x" << std::hex << fid.get_compact() << std::dec << "  " << m_onlineID->channel_name(fid));
      p_febConfig->add (fid, &chanit->second);
      ++nFebs;
    }//End loop over COOL channels
  }
  ATH_MSG_INFO("Read gain thresholds for " << nFebs << " Febs from " << m_listOfFolders.size() << " database folders.");

  // Record output
  if(writeHandle.record(std::move(febConfig)).isFailure()) {
      ATH_MSG_ERROR("Could not record LArFebConfig object with " << writeHandle.key()
		    << " with EventRange " << writeHandle.getRange() << " into Conditions Store");
      return StatusCode::FAILURE;
  }

  ATH_MSG_INFO("recorded new " << writeHandle.key() << " with range " << writeHandle.getRange() << " into Conditions Store");

  return StatusCode::SUCCESS;
}


