/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

//@file ReadFromFileMetaData.h
//@brief Class to read from file metadata and give the results
//@author Shaun Roe
#ifndef IOVDbSvc_ReadFromFileMetaData_h
#define IOVDbSvc_ReadFromFileMetaData_h


#include "CoolKernel/ValidityKey.h"
#include "AthenaKernel/IOVTime.h"
#include "AthenaKernel/IOVRange.h"
#include <string>
#include "FolderTypes.h"
#include "IOVDbDataModel/IOVPayloadContainer.h"

//fwd declarations
class IOVMetaDataContainer;
class CondAttrListCollection;
class AthenaAttributeList;

namespace coral{
  class AttributeList;
}

namespace IOVDbNamespace{
  //@brief Encapsulates the functionality to read database quantities from file metadata
  class ReadFromFileMetaData{
  public:
    ReadFromFileMetaData()=delete;
    ReadFromFileMetaData(const IOVMetaDataContainer* m_metacon, const IOVTime & refTime, const bool useEpochTimestamp);
    ReadFromFileMetaData(const IOVMetaDataContainer* m_metacon, const cool::ValidityKey & refTimeKey, const bool useEpochTimestamp);
    ~ReadFromFileMetaData();
    //
    //
    bool isValid();
    IOVDbNamespace::FolderType folderType();
    CondAttrListCollection *attrListCollection();
    AthenaAttributeList *attributeList();
    unsigned int numberOfObjects();
    std::string stringAddress();
    bool poolPayloadRequested();
    IOVRange range();
    
  private:
    const IOVMetaDataContainer* m_metaContainer{};
    IOVTime m_referenceTime{};
    const IOVPayloadContainer* m_payload{};
    CondAttrListCollection * m_pptr{};
    AthenaAttributeList * m_attrList{};
    CondAttrListCollection * m_attrListColl{};
    IOVDbNamespace::FolderType m_folderType{UNKNOWN};
    bool m_newedPtr{};
  
  };
}//namespace
#endif
