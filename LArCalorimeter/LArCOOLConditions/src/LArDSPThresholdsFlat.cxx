/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "LArCOOLConditions/LArDSPThresholdsFlat.h"
#include "AthenaPoolUtilities/AthenaAttributeList.h"
#include "CoralBase/Blob.h"
#include "CoralBase/Attribute.h"
#include "CoralBase/AttributeList.h"

LArDSPThresholdsFlat::LArDSPThresholdsFlat(): 
  m_nChannels(0),
  m_ptQThr(NULL),
  m_psamplesThr(NULL),
  m_ptrigSumThr(NULL)
  
{}

LArDSPThresholdsFlat::LArDSPThresholdsFlat(const AthenaAttributeList* attrList):
  m_nChannels(0),
  m_ptQThr(NULL),
  m_psamplesThr(NULL),
  m_ptrigSumThr(NULL) 
{


  StatusCode sc=initializeBase("LArDSPThresholdsFlat");
  if (sc.isFailure()) return;
 
  readBlob(attrList);
  return;
}




float LArDSPThresholdsFlat::tQThr(const HWIdentifier&  CellID) const {
  return this->tQThrByHash(m_onlineHelper->channel_Hash(CellID));
}
  
float LArDSPThresholdsFlat::tQThrByHash(const IdentifierHash& h) const {
  if (h>=m_nChannels) {
    (*m_log) << MSG::ERROR << "tQThrByHash: Hash out of range ( " << h << ", max=" << m_nChannels << ")" << endmsg;
    return 0;
  }
  return m_ptQThr[h];
}

float LArDSPThresholdsFlat::samplesThr(const HWIdentifier&  CellID) const {
  return this->samplesThrByHash(m_onlineHelper->channel_Hash(CellID));
}
  
float LArDSPThresholdsFlat::samplesThrByHash(const IdentifierHash& h) const {
  if (h>=m_nChannels) {
    (*m_log) << MSG::ERROR << "samplesThrByHash: Hash out of range ( " << h << ", max=" << m_nChannels << ")" << endmsg;
    return 0;
  }
  return m_psamplesThr[h];
}

float LArDSPThresholdsFlat::trigSumThr(const HWIdentifier&  CellID) const {
  return this->trigSumThrByHash(m_onlineHelper->channel_Hash(CellID));
}
  
float LArDSPThresholdsFlat::trigSumThrByHash(const IdentifierHash& h) const {
  if (h>=m_nChannels) {
    (*m_log) << MSG::ERROR << "trigSumThrByHash: Hash out of range ( " << h << ", max=" << m_nChannels << ")" << endmsg;
    return 0;
  }
  return m_ptrigSumThr[h];
}

void LArDSPThresholdsFlat::readBlob(const AthenaAttributeList* attrList) {
  m_nChannels=0;
  //m_setName.clear();

  if (!attrList) return;
  
  //const coral::AttributeList& attr= attrList->coralList();

  //m_setName=attr["Name"].data<std::string>();

  const coral::Blob& tQThrBlob = (*attrList)["tQThr"].data<coral::Blob>();
  const coral::Blob& samplesThrBlob = (*attrList)["samplesThr"].data<coral::Blob>();
  const coral::Blob& trigSumThrBlob = (*attrList)["trigSumThr"].data<coral::Blob>();
  //const coral::Blob *tQThrBlob = new coral::Blob(attr["tQThr"].data<coral::Blob>());
  //const coral::Blob *samplesThrBlob = new coral::Blob(attr["samplesThr"].data<coral::Blob>());
  //const coral::Blob *trigSumThrBlob = new coral::Blob(attr["trigSumThr"].data<coral::Blob>());

  if (!(tQThrBlob.size()==samplesThrBlob.size() && samplesThrBlob.size()==trigSumThrBlob.size())) {
    (*m_log) << MSG::ERROR << "Unequal blob size tQThr/samplesThr/trigSumThr = " 
	   << tQThrBlob.size() << "/" << samplesThrBlob.size()  << "/" << trigSumThrBlob.size() 
	   << endmsg;
    return;
  }

  m_nChannels=tQThrBlob.size()/sizeof(float);
  

  //(*m_log) << MSG::INFO << "Blob sizes:" << tQThrBlob.size() << "/" << samplesThrBlob.size()  << "/" << trigSumThrBlob.size() << endmsg;

  if (m_nChannels!=m_onlineHelper->channelHashMax()) {
    (*m_log) << MSG::WARNING << "Found data for " << m_nChannels << " but expected " << m_onlineHelper->channelHashMax() << endmsg;
  }

  (*m_log) << MSG::DEBUG << "Found data for " << m_nChannels << endmsg;
  m_ptQThr=static_cast<const float*>(tQThrBlob.startingAddress());
  m_psamplesThr=static_cast<const float*>(samplesThrBlob.startingAddress());
  m_ptrigSumThr=static_cast<const float*>(trigSumThrBlob.startingAddress());


  return;
}
