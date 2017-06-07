/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "LArRecUtils/LArFEBConfigReader.h"
#include "LArIdentifier/LArOnlineID.h"

LArFEBConfigReader::LArFEBConfigReader(const std::string& type, const std::string& name, const IInterface* parent) :
  AthAlgTool(type,name,parent),m_onlineID(NULL) {

  declareInterface<ILArFEBConfigReader>(this);
  declareProperty("ListOfFolders",m_listOfFolders);
  m_lastIt=m_attrPerFeb.end(); 
}


LArFEBConfigReader::~LArFEBConfigReader() {}

StatusCode LArFEBConfigReader::initialize() {
  
  if (detStore()->retrieve(m_onlineID,"LArOnlineID").isFailure()) {
    msg(MSG::ERROR) << "Failed to retrieve LArOnlineID" << endmsg;
    return StatusCode::FAILURE;
  }   

  if (m_listOfFolders.size()==0) {
    msg(MSG::WARNING) << "List of folders is emtpy, do nothing" << endmsg;
    return StatusCode::SUCCESS;
  }

  const size_t nFolders=m_listOfFolders.size();
  m_handles.resize(nFolders);
 
  for (size_t i=0;i<nFolders;++i) {
    const std::string& fn=m_listOfFolders[i];
    const DataHandle<CondAttrListCollection>& chdl=m_handles[i]; 
    ATH_MSG_DEBUG("Registering callback for folder " << fn);
    StatusCode sc=detStore()->regFcn(&ILArFEBConfigReader::loadData,dynamic_cast<ILArFEBConfigReader*>(this),
				     chdl,fn);

    if (sc.isFailure()) {
      msg(MSG::ERROR) << "Failed to register callback for DB folder " << fn << endmsg;
      return StatusCode::FAILURE;
    }      
  }//end loop over folders
     
  msg(MSG::DEBUG) << "Successfully initialized LArFEBConfigReader" << endmsg;
  return StatusCode::SUCCESS;
} 

  

StatusCode LArFEBConfigReader::finalize() {
  return StatusCode::SUCCESS;
}

  
StatusCode LArFEBConfigReader::loadData(IOVSVC_CALLBACK_ARGS) {
  msg(MSG::INFO) << "In IOV callback method..." << endmsg;
  m_attrPerFeb.clear();
  unsigned nFebs=0;
  const size_t nFolders=m_listOfFolders.size();
  for (size_t i=0;i<nFolders;++i) {
    ATH_MSG_DEBUG("Working on folder " << m_listOfFolders[i]);  
    const DataHandle<CondAttrListCollection> dh;
    if (detStore()->retrieve(dh,m_listOfFolders[i]).isFailure()) {
      msg(MSG::ERROR) << "Failed to retrieve DataHandle for folder " << m_listOfFolders[i] << endmsg;
      return StatusCode::FAILURE;
    }
    CondAttrListCollection::const_iterator chanit=dh->begin();
    CondAttrListCollection::const_iterator chanit_e=dh->end();
    for (;chanit!=chanit_e;++chanit) {
      if (chanit->first==0) {
	ATH_MSG_DEBUG("Invalid channel number 0, ignoring...");
	continue;
      }  
      const HWIdentifier fid(chanit->first); //COOL channel number == FEB identifier
      //const coral::AttributeList& attr = chanit->second;
      ATH_MSG_DEBUG("Working on FEB 0x" << std::hex << fid.get_compact() << std::dec << "  " << m_onlineID->channel_name(fid));
      ++nFebs;
      m_attrPerFeb[fid]=&(chanit->second);
    }//End loop over COOL channels
  }//End loop over folders
  m_lastIt=m_attrPerFeb.end(); 
  msg(MSG::INFO) << "Read gain thresholds for " << nFebs << " Febs from " << m_listOfFolders.size() << " database folders." << endmsg;
  return StatusCode::SUCCESS;
}


short LArFEBConfigReader::getThreshold(const char* MedLow, const HWIdentifier& chid) const {

  if (m_attrPerFeb.size()==0) {
    msg(MSG::WARNING) << "FEB treshold cache is empty. Callback not fired?" << endmsg;
    return ERRORCODE;
  }

  const HWIdentifier fid=m_onlineID->feb_Id(chid);
  const int channel=m_onlineID->channel(chid); 
  if (m_lastIt==m_attrPerFeb.end() || m_lastIt->first!=fid)
    m_lastIt=m_attrPerFeb.find(fid);

  if (m_lastIt==m_attrPerFeb.end()) 
    return ERRORCODE;
  else {
    char channame[16];
    snprintf(channame,15,"%s%i",MedLow,channel+1);
    channame[15]='\n';// Just to be sure...
    return (short)(*(m_lastIt->second))[channame].data<int32_t>(); //Will throw and exception if channel does not exist
  } 
}
