/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "LArCOOLConditions/LArPedestalBlob.h"
#include "AthenaPoolUtilities/CondAttrListCollection.h"
#include "CoralBase/Blob.h"
#include "CoralBase/Attribute.h"
#include "CoralBase/AttributeList.h"
#include "CoralBase/AttributeListSpecification.h"

LArPedestalBlob::LArPedestalBlob():
  m_nChannels(0)
  {}

LArPedestalBlob::~LArPedestalBlob() {}

void LArPedestalBlob::readBlob(const CondAttrListCollection* attrList, MsgStream& msg) {
  m_nChannels=0;
  m_pPedestal.clear();
  m_pPedestalRMS.clear();
    
  if (!attrList) return;

  CondAttrListCollection::const_iterator gainIt=attrList->begin();
  CondAttrListCollection::const_iterator gainIt_e=attrList->end();
  
  m_pPedestal.resize(attrList->size());
  m_pPedestalRMS.resize(attrList->size());
  msg << MSG::DEBUG << "Found data for " << attrList->size() << " gains." << endmsg;
  
  int blobSize=0; 

  for(;gainIt!=gainIt_e;++gainIt) {
    const unsigned gain=gainIt->first;
    if (gain>=attrList->size()) {
      msg << MSG::ERROR << "Found unexpected COOL-channel (=gain) number:" << gain << endmsg;
      return; //ERROR
    }
    const coral::AttributeList& attr=gainIt->second;
    const coral::Blob& pedBlob = attr["Pedestal"].data<coral::Blob>();
    const coral::Blob& rmsBlob = attr["PedestalRMS"].data<coral::Blob>();
    if (blobSize==0) blobSize=pedBlob.size();
    
    //Sanity checks:
    if (blobSize!=pedBlob.size() || blobSize!=rmsBlob.size()) {
      msg << MSG::ERROR << "Unequal blob size (" << blobSize << "/" 
	  << pedBlob.size() << "/" << rmsBlob.size() << ")" <<endmsg;
      return;
    }
    m_pPedestal[gain]=static_cast<const float*>(pedBlob.startingAddress());
    m_pPedestalRMS[gain]=static_cast<const float*>(rmsBlob.startingAddress());
  }// end loop over COOL channels

  
  m_nChannels=blobSize/sizeof(float);
  msg << MSG::DEBUG << "Found data for " << m_nChannels << endmsg;
  return;
}
