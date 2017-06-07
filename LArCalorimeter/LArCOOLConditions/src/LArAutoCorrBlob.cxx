/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "LArCOOLConditions/LArAutoCorrBlob.h" 
#include "AthenaPoolUtilities/CondAttrListCollection.h"
#include "CoralBase/Blob.h"

LArAutoCorrBlob::LArAutoCorrBlob()
  :m_nChannels(0),
   m_nPoints(0)
{}

LArAutoCorrBlob::~LArAutoCorrBlob() 
{}


void LArAutoCorrBlob::readBlob(const CondAttrListCollection* attrList, MsgStream& msg) {
  m_nChannels=0;
  m_nPoints=0;
  m_pAutoCorr.clear();

  if (!attrList) return;

  CondAttrListCollection::const_iterator gainIt=attrList->begin();
  CondAttrListCollection::const_iterator gainIt_e=attrList->end();
  
  m_pAutoCorr.resize(attrList->size());
  msg << MSG::DEBUG << "Found data for " << attrList->size() << " gains." << endmsg;
  
  int blobSize=0;  

  for(;gainIt!=gainIt_e;++gainIt) {
    const unsigned gain=gainIt->first;
    if (gain>=attrList->size() || gain>2) {
      msg << MSG::ERROR << "Found unexpected COOL-channel (=gain) number:" << gain << endmsg;
      return; //ERROR
    }
    const coral::AttributeList& attr=gainIt->second;
    const coral::Blob& acBlob = attr["AutoCorrVec"].data<coral::Blob>();
    if (blobSize==0) blobSize=acBlob.size();
    if (m_nPoints==0) m_nPoints=attr["nPoints"].data<unsigned>();
    
    //Sanity checks:
    if (blobSize!=acBlob.size()) {
      msg << MSG::ERROR << "Unequal blob size (" << blobSize << "/" 
	       << acBlob.size() << ")" <<endmsg;
      return;
    }
    if (m_nPoints!=attr["nPoints"].data<unsigned>()) {
      msg << MSG::ERROR << "Unequal number of samples (" << m_nPoints << "/" 
	  << attr["nPoints"].data<unsigned>() << ")" << endmsg;
      return;
    }
    
    m_pAutoCorr[gain]=static_cast<const float*>(acBlob.startingAddress());
  }// end loop over COOL channels

  
  if (m_nPoints==0) {
    msg << MSG::ERROR << "Number of points is zero!" << endmsg;
    return;
  }
  m_nChannels=blobSize/(sizeof(float)*m_nPoints);
  msg << MSG::DEBUG << "Found data for " << m_nChannels << endmsg;
  return;
}

