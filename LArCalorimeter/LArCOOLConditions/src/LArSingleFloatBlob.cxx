/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "LArCOOLConditions/LArSingleFloatBlob.h"
#include "AthenaPoolUtilities/CondAttrListCollection.h"
#include "CoralBase/Blob.h"
#include "GaudiKernel/MsgStream.h"


LArSingleFloatBlob::LArSingleFloatBlob():
  m_nChannels(0)
  {}

void LArSingleFloatBlob::readBlob(const CondAttrListCollection* attrList, const char* attrName, MsgStream& msg) {
  m_nChannels=0;

  if (!attrList) return;
  
  CondAttrListCollection::const_iterator gainIt=attrList->begin();
  CondAttrListCollection::const_iterator gainIt_e=attrList->end();
  
  m_pValues.resize(attrList->size());
  msg << MSG::DEBUG << "Found data for " << attrList->size() << " gains." << endmsg;
  
  int blobSize=0;  //FIXME Force size to hash-max??? m_onlineHelper->channelHashMax()

  for(;gainIt!=gainIt_e;++gainIt) {
    unsigned gain=gainIt->first;
    //Special case: Single-gain constant in channel 1
    if (gain==1 && attrList->size()==1) gain=0;

    if (gain>=attrList->size() || gain>2) { 
      msg << MSG::ERROR << "Found unexpected COOL-channel (=gain) number:" << gain << endmsg;
      return; //ERROR
    }

    const coral::AttributeList& attr=gainIt->second;
    const coral::Blob& myBlob = attr[attrName].data<coral::Blob>();
    m_pValues[gain]=static_cast<const float*>(myBlob.startingAddress());
    if (blobSize==0) {
      blobSize=myBlob.size();
    }
    else {
      if (blobSize!=myBlob.size()) 
	msg << MSG::ERROR << "Unequal blob size (" << blobSize << "/" << myBlob.size() << ")" << endmsg;
    }
  }// end loop over COOL channels

  m_nChannels=blobSize/sizeof(float);
  msg << MSG::DEBUG << "Found data for " << m_nChannels << " Channels" << endmsg;
  return;
}
 
