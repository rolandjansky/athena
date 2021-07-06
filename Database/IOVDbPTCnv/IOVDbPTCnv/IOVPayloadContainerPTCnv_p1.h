/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef IOVDBPTCNV_IOVPAYLOADCONTAINERPTCNV_P1_H
#define IOVDBPTCNV_IOVPAYLOADCONTAINERPTCNV_P1_H
/**
 * @file IOVPayloadContainerPTCnv_p1.h
 *
 * @brief Header file for in-file metadata P/T converters (file peeking without Athena)
 *
 * @author  <vaniachine@anl.gov>
 */

//<<<<<< INCLUDES                                                       >>>>>>
#include "CoralBase/AttributeList.h"
#include "IOVDbTPCnv/IOVPayloadContainer_p1.h"
#include "IOVDbDataModel/IOVPayloadContainer.h"

class AttrListIndexes;

class IOVPayloadContainerPTCnv_p1  {
public:
    IOVPayloadContainerPTCnv_p1();
    virtual void   persToTrans(const IOVPayloadContainer_p1* persObj, IOVPayloadContainer* transObj);

private:
    void           fillPersAttrSpec(const IOVPayloadContainer* transObj, IOVPayloadContainer_p1* persObj);

    void           fillAttributeTypeMap();

    void           fillAttributeData(unsigned int attrName,
				     unsigned int attrType,
				     const coral::Attribute& attr,
				     IOVPayloadContainer_p1* persObj);

    void           fillAttributeData(const IOVPayloadContainer_p1* persObj,
				     const AttrListIndexes& index,
				     const std::string& name,
				     coral::AttributeList& attrList);

    
    // AttributeList map from attribute name to type index
    typedef std::map<std::string, unsigned int>::value_type    attrNameMapValue_t;
    std::map<std::string, unsigned int>    m_attrNameMap;

    // Attribute types
    std::map<std::string, unsigned int>    m_attributeTypeMap;
    std::vector<std::string>               m_attributeTypes;

    unsigned int m_objIndexOffset[IOVPayloadContainer_p1::ATTR_TIME_STAMP+1];

};

#endif // IOVDBPTCNV_IOVPAYLOADCONTAINERPTCNV_P1_H
