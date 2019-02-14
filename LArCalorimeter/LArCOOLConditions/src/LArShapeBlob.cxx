/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#include "LArCOOLConditions/LArShapeBlob.h"
#include "AthenaPoolUtilities/CondAttrListCollection.h"
#include "CoralBase/Blob.h"

LArShapeBlob::LArShapeBlob():
  m_nChannels(0),
  m_nSamples(0)
  {}

LArShapeBlob::~LArShapeBlob() {}


void LArShapeBlob::readBlob(const CondAttrListCollection* attrList, MsgStream& msg) {
  m_nChannels=0;
  m_nSamples=0;
  m_pShape.clear();
  m_pShapeDer.clear();
  m_pTimeOffset.clear();
 
   if (!attrList) return;

  CondAttrListCollection::const_iterator gainIt=attrList->begin();
  CondAttrListCollection::const_iterator gainIt_e=attrList->end();
  
  m_pShape.resize(attrList->size());
  m_pShapeDer.resize(attrList->size());
  m_pTimeOffset.resize(attrList->size());
  msg << MSG::DEBUG << "Found data for " << attrList->size() << " gains." << endmsg;
  
  int blobSize=0;  //FIXME Force size to hash-max??? m_onlineHelper->channelHashMax()

 
  for(;gainIt!=gainIt_e;++gainIt) {
    const unsigned gain=gainIt->first;
    if (gain>=attrList->size() || gain>2) {
      msg << MSG::ERROR << "Found unexpected COOL-channel (=gain) number:" << gain << endmsg;
      return; //ERROR
    }
    const coral::AttributeList& attr=gainIt->second;
    const coral::Blob& shapeaBlob = attr["Shape"].data<coral::Blob>();
    const coral::Blob& shapebBlob = attr["ShapeDer"].data<coral::Blob>();
    const coral::Blob& toBlob = attr["TimeOffset"].data<coral::Blob>();
    if (blobSize==0) blobSize=shapeaBlob.size();
    if (m_nSamples==0) m_nSamples=attr["nSamples"].data<unsigned>();
    
    //Sanity checks:
    if (blobSize!=shapeaBlob.size() || blobSize!=shapebBlob.size()) {
      msg << MSG::ERROR << "Unequal blob size (" << blobSize << "/" 
	       << shapeaBlob.size() << "/" << shapebBlob.size() << ")" <<endmsg;
      return;
    }
    if (m_nSamples!=attr["nSamples"].data<unsigned>()) {
      msg << MSG::ERROR << "Unequal number of samples (" << m_nSamples << "/" 
	  << attr["nSamples"].data<unsigned>() << ")" << endmsg;
      return;
    }


    m_pShape[gain]=static_cast<const float*>(shapeaBlob.startingAddress());
    m_pShapeDer[gain]=static_cast<const float*>(shapebBlob.startingAddress());
    m_pTimeOffset[gain]=static_cast<const float*>(toBlob.startingAddress());
  }// end loop over COOL channels

  if (m_nSamples==0) {
    msg << MSG::ERROR << "Number of samples is zero!" << endmsg;
    return;
  }  
  m_nChannels=blobSize/(sizeof(float)*m_nSamples);
  msg << MSG::DEBUG << "Found data for " << m_nChannels << endmsg;
}
