/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "LArCabling/LArCablingBase.h"
#include "AthenaPoolUtilities/AthenaAttributeList.h"
#include "CoralBase/Blob.h"


//-------------------------------------------------------------
LArCablingBase::LArCablingBase( const std::string& type, 
				const std::string& name, 
				const IInterface* parent ) 
  : AthAlgTool(type,name,parent),
    m_onlineId(0),
    m_caloId(0),
    m_onOffValid(false),
    m_idEmpty(),
    m_hwidEmpty()
{
  declareProperty("LArOnOffIdFolder",m_onOffIdKey="/LAR/Identifier/OnOffIdMap");
}


LArCablingBase::~LArCablingBase() { } 

//---------------------------------------------------------
StatusCode LArCablingBase::finalize ()
{
  return StatusCode::SUCCESS;
}



bool LArCablingBase::readOnlOffMap() {
  msg(MSG::DEBUG) << "Start reading Online/Offline mapping" << endmsg;
  m_onOffValid=false;
  m_onlHashToOffline.clear();
  m_oflHashToOnline.clear();

  StatusCode sc=detStore()->retrieve(m_attrOnOff,m_onOffIdKey);
  if (sc.isFailure()) {
    msg() << MSG::ERROR << "Failed to read AthenaAttributeList with key " << m_onOffIdKey << endmsg;
    return false;
  }
  const coral::Blob& blobOnOff=(*m_attrOnOff)["OnlineHashToOfflineId"].data<coral::Blob>();
  unsigned nChan=blobOnOff.size()/sizeof(uint32_t);
  const uint32_t* pBlobOnOff=static_cast<const uint32_t*>(blobOnOff.startingAddress());
  

  if (nChan!=m_onlineId->channelHashMax()) {
    msg() << MSG::WARNING << "Number of channels read from DB (" << nChan << ") does not match online hash max (" 
	     <<m_onlineId->channelHashMax() << ")" << endmsg;
    nChan=std::min(nChan,(unsigned)m_onlineId->channelHashMax());
  }

  m_onlHashToOffline.assign(nChan,m_idEmpty);
  m_oflHashToOnline.assign(nChan,m_hwidEmpty);

  unsigned nConnected=0;

  for (unsigned i=0;i<nChan;++i) {
    const Identifier id=Identifier(Identifier32(pBlobOnOff[i]));
    //std::cout << "id[" << i <<"] " << std::hex << id.get_compact() << std::dec << std::endl;
    if (id.is_valid()) {
      const IdentifierHash oflHash=m_caloId->calo_cell_hash(id);
      const HWIdentifier hwid=m_onlineId->channel_Id(IdentifierHash(i));
      ++nConnected;
      m_onlHashToOffline[i]=id;
      m_oflHashToOnline[oflHash]=hwid;
    }
  }//end loop over channels

  msg() << MSG::INFO << "Done reading online/offline identifier mapping" << endmsg; 
  msg() << MSG::INFO << "Found " << nChan << " online identifier and " << nConnected << " offline identifier. " 
	<< nChan-nConnected << " disconnected channels." << endmsg;
  m_onOffValid=true;
  return true;
  
}
