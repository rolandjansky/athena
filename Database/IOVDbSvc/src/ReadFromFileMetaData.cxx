/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/

#include "ReadFromFileMetaData.h"
#include "CoralBase/AttributeList.h"
#include "IOVDbCoolFunctions.h"
#include "IOVDbDataModel/IOVMetaDataContainer.h"
#include "IOVDbDataModel/IOVPayloadContainer.h"
#include "AthenaPoolUtilities/CondAttrListCollection.h"
#include "AthenaPoolUtilities/AthenaAttributeList.h"

using IOVDbNamespace::makeEpochOrRunLumi;

namespace IOVDbNamespace{
  ReadFromFileMetaData::ReadFromFileMetaData( const IOVMetaDataContainer* container, 
        const IOVTime & refTime, const bool useEpochTimestamp):
        m_metaContainer(container), 
        m_referenceTime(refTime),
        m_isEpochTime(useEpochTimestamp){
    m_payload = (container) ? (container->payloadContainer()) : (nullptr);
    if (m_payload){
      IOVPayloadContainer::const_iterator pitr=m_payload->find(m_referenceTime);
      if (pitr!=m_payload->end()){
        m_pptr=*pitr;
      } else {
        m_pptr = new CondAttrListCollection(not useEpochTimestamp);
        m_newedPtr=true;
      }
      m_folderType=IOVDbNamespace::determineFolderType(m_pptr);
    }
  }
  //
  ReadFromFileMetaData::ReadFromFileMetaData( const IOVMetaDataContainer* container, 
        const cool::ValidityKey & refTimeKey, const bool useEpochTimestamp)
        :ReadFromFileMetaData(container,makeEpochOrRunLumi(refTimeKey,useEpochTimestamp),useEpochTimestamp){
  std::cout<<"delegated constructor"<<std::endl;//nop, we delegated the constructor
  }
  //
  ReadFromFileMetaData::~ReadFromFileMetaData(){
    if (m_newedPtr) delete m_pptr;
  }
  
  //
  bool 
  ReadFromFileMetaData::isValid(){
    return (m_metaContainer and m_payload);
  }
  
  //
  IOVDbNamespace::FolderType 
  ReadFromFileMetaData::folderType(){
    return m_folderType;
  }
  //
  unsigned int 
  ReadFromFileMetaData::numberOfObjects(){
    return m_pptr->size();
  }
  //
  std::string 
  ReadFromFileMetaData::stringAddress(){
    std::string strAddress{};
    if ((m_folderType==PoolRef) or (m_folderType==AttrList)) {
      const coral::AttributeList& payload1=m_pptr->begin()->second;
      if (m_folderType==PoolRef) {
        // single channel with PoolRef
        strAddress=payload1[0].data<std::string>();
      } else {
        // create an AthenaAttributeList for this data
        strAddress="POOLContainer_AthenaAttributeList][CLID=x";
      }
    }
    return strAddress;
  }

  //
  bool 
  ReadFromFileMetaData::poolPayloadRequested(){
    return (m_folderType==PoolRef) or (m_folderType==PoolRefColl);
  }
  
  IOVRange
  ReadFromFileMetaData::range(){
    if ((m_folderType==PoolRef) or (m_folderType==AttrList)) {
      return (m_pptr->iov_begin())->second;
    } else {
      return m_pptr->minRange();
    }
  }
  
  CondAttrListCollection *
  ReadFromFileMetaData::attrListCollection(){
    if ((m_folderType!=PoolRef) and (m_folderType!=AttrList)) {
      m_attrListColl=new CondAttrListCollection(*m_pptr);
    }
    return m_attrListColl;
  }
  
  AthenaAttributeList *
  ReadFromFileMetaData::attributeList(){
    if (m_folderType==AttrList) m_attrList = new AthenaAttributeList(m_pptr->begin()->second);
    return m_attrList;
  }




}