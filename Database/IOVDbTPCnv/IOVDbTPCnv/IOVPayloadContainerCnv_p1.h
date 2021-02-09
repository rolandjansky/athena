/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef IOVDBDATAMODEL_IOVPAYLOADCONTAINERCNV_P1_H
#define IOVDBDATAMODEL_IOVPAYLOADCONTAINERCNV_P1_H


//<<<<<< INCLUDES                                                       >>>>>>
#include "AthenaPoolCnvSvc/T_AthenaPoolTPConverter.h"
#include "CoralBase/AttributeList.h"
#include "IOVDbTPCnv/IOVPayloadContainer_p1.h"
#include "IOVDbDataModel/IOVPayloadContainer.h"

class MsgStream;
class AttrListIndexes;

class IOVPayloadContainerCnv_p1  : public T_AthenaPoolTPCnvBase<IOVPayloadContainer, IOVPayloadContainer_p1>  {
public:
    IOVPayloadContainerCnv_p1();
    virtual void   persToTrans(const IOVPayloadContainer_p1* persObj, 
                               IOVPayloadContainer* transObj, MsgStream &log);
    virtual void   transToPers(const IOVPayloadContainer* transObj, 
                               IOVPayloadContainer_p1* persObj, MsgStream &log);
private:
    void           fillPersAttrSpec(const IOVPayloadContainer* transObj, 
                                    IOVPayloadContainer_p1* persObj, MsgStream &log);

    void           fillAttributeTypeMap();

    void           fillAttributeData(unsigned int attrName,
                                     unsigned int attrType,
                                     const coral::Attribute& attr,
                                     IOVPayloadContainer_p1* persObj,
                                     MsgStream & log);

    void           fillAttributeData(const IOVPayloadContainer_p1* persObj,
                                     const AttrListIndexes& index,
                                     const std::string& name,
                                     coral::AttributeList& attrList,
                                     MsgStream & log);

    
    // AttributeList map from attribute name to type index
    typedef std::map<std::string, unsigned int>::value_type    attrNameMapValue_t;
    std::map<std::string, unsigned int>    m_attrNameMap;

    // Attribute types
    std::map<std::string, unsigned int>    m_attributeTypeMap;
    std::vector<std::string>               m_attributeTypes;

    unsigned int m_objIndexOffset[IOVPayloadContainer_p1::ATTR_TIME_STAMP+1];

};

#endif
