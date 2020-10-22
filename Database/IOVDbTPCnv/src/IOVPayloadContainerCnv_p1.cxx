/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "IOVDbDataModel/IOVPayloadContainer.h"
#include "AthenaPoolUtilities/CondAttrListCollection.h"
#include "IOVDbTPCnv/IOVPayloadContainerCnv_p1.h"
#include "IOVDbTPCnv/IOVPayloadContainer_p1.h"
#include "CoralBase/Date.h"
#include "CoralBase/TimeStamp.h"

IOVPayloadContainerCnv_p1::IOVPayloadContainerCnv_p1()
{}


void
IOVPayloadContainerCnv_p1::persToTrans(const IOVPayloadContainer_p1* persObj, 
                                       IOVPayloadContainer* transObj, MsgStream &log)
{

    if (log.level() <= MSG::DEBUG) {
        log << MSG::DEBUG << "IOVPayloadContainerCnv_p1::persToTrans - entering "
            << endreq;
    }

    for( auto & offset : m_objIndexOffset ) {
        offset = 1; // this is just the initial value before first use 
    }

    IOVPayloadContainer::payloadVec& payloadVec = transObj->m_payloadVec;
    
    // Make sure transient container is empty - may be reused
    payloadVec.clear();

    // fill map from type name to int
    if (m_attributeTypes.size() == 0)fillAttributeTypeMap();

    // Check which version we have:
    //   1) earlier version took the attr spec from first attribute list
    //   2) more recent versions "correctly" checks ALL attribute lists for attr spec -
    //      attribute list spec may be extended as payloads get merged together
    // Use a non-empty m_attrType vector to detect the earlier version
    bool isEarlyVersion = persObj->m_attrType.size() > 0;

    // Loop over persistent attribute list collections
    payloadVec.reserve(persObj->m_payloadVec.size());
    for (const IOVPayloadContainer_p1::CondAttrListCollection_p1& persColl : 
           persObj->m_payloadVec)
    {
        CondAttrListCollection* transColl = new CondAttrListCollection(persColl.m_hasRunLumiBlockTime);

        // Add to transient payload container
        payloadVec.push_back(transColl);
        
        // Loop over attribute lists
        typedef std::vector<IOVPayloadContainer_p1::CondAttrListEntry_p1>::const_iterator attrListIt_t;

        attrListIt_t itattrList   = persColl.m_attrLists.begin();
        attrListIt_t lastAttrList = persColl.m_attrLists.end();
        for (; itattrList != lastAttrList; ++itattrList) {

            const IOVPayloadContainer_p1::CondAttrListEntry_p1& entry = (*itattrList);

            coral::AttributeList attrList;
            // Add in attributes

            unsigned int inameOld = 0;
            for (unsigned int iattr = entry.m_firstIndex; 
                 iattr < entry.m_lastIndex; ++iattr, ++inameOld) {

                // Check that we have the correct number of attributes
                if (iattr >= persObj->m_attrIndexes.size()) {
                    log << MSG::ERROR
                        << "IOVPayloadContainerCnv_p1::persToTrans attribute index bigger than array size " 
                        << iattr << " " << persObj->m_attrIndexes.size() << " last index "
                        << entry.m_lastIndex
                        << endreq;
                    return;
                }

                if (!isEarlyVersion) {
                    // version with name index saved in AttrListIndexes
                    
                    // Get indexes to type/value
                    const AttrListIndexes& indexes = persObj->m_attrIndexes[iattr];
                    // Get attribute name and type name
                    unsigned int iname           = indexes.nameIndex();
                    unsigned int itype           = indexes.typeIndex();
                    const std::string& name      = persObj->m_attrName[iname];
                    const std::string& typeName  = m_attributeTypes[itype];
                    // Extend attribute list with new type/name
                    attrList.extend(name, typeName);
                    fillAttributeData(persObj, indexes, name, attrList, log);
                }
                else {
                    // Earlier version - did not allow for attr spec
                    // to vary across the collection of attribute
                    // lists
                    bool indexesOK = true;
                    if (inameOld >= persObj->m_attrName.size() ||
                        inameOld >= persObj->m_attrType.size()) {
                        log << MSG::WARNING << "IOVPayloadContainerCnv_p1::persToTrans name index bigger than name/type size. index " 
                            << inameOld << " name size " << persObj->m_attrName.size()  
                            << " attr size " << persObj->m_attrType.size() 
                            << " last index " << entry.m_lastIndex 
                            << " Most likely unfilled attribute list. Skipping to the next attrlist entry."
                            << endreq;
                        indexesOK = false;

                    }
                
                    // Get indexes to type/value
                    const AttrListIndexes& indexes = persObj->m_attrIndexes[iattr];
                    // Get attribute name and type name
                    if (indexesOK) {
                        const std::string& name      = persObj->m_attrName[inameOld];
                        unsigned int itype           = persObj->m_attrType[inameOld];
                        const std::string& typeName  = m_attributeTypes[itype];
                        // Extend attribute list with new type/name
                        attrList.extend(name, typeName);
                        fillAttributeData(persObj, indexes, name, attrList, log);
                    }
                    else {
                        // Error, print out info
                        std::string typeName;
                        unsigned int itype = indexes.typeIndex();
                        typedef std::map<std::string, unsigned int>::const_iterator it_type;
                        it_type typeIt  = m_attributeTypeMap.begin();
                        it_type endType = m_attributeTypeMap.end();
                        for (; typeIt != endType; ++typeIt) {
                            if (itype == typeIt->second) {
                                typeName = typeIt->first;
                                break;
                            }
                        }
                        std::string name = "unknown";
                        attrList.extend(name, typeName);
                        fillAttributeData(persObj, indexes, name, attrList, log);
                    }
                }
            }


//             std::ostringstream attrStr;
//             attrList.toOutputStream(attrStr);       
//             log << MSG::DEBUG << "IOVPayloadContainerCnv_p1::persToTrans - 3 "
//                 << "start/stop " << entry.m_range.m_start << " "
//                 << entry.m_range.m_stop << " attrList " << attrStr.str()
//                 << endreq;
//             log << MSG::DEBUG << "IOVPayloadContainerCnv_p1::persToTrans - 3 "
//                 << "attrList size " << attrList.size()
//                 << endreq;
//             for (unsigned int i = 0; i < attrList.size(); ++i) {
//                 log << MSG::DEBUG << attrList[i].specification().name() << " " 
//                     << attrList[i].specification().typeName() << " " 
//                     << endreq;
//             }
            

            // Add in AttributeList
            transColl->add(entry.m_channelNumber, attrList);

            bool hasIOV = !(entry.m_range.m_start == 0 && entry.m_range.m_stop == 0);

            // Add in IOV
            if (hasIOV) {
                if (persColl.m_hasRunLumiBlockTime) {
                    // run/lumiblock
                    IOVTime start;
                    IOVTime stop;
                    start.setRETime(entry.m_range.m_start);
                    stop.setRETime(entry.m_range.m_stop);
                    transColl->add(entry.m_channelNumber, IOVRange(start, stop));
                }
                else {
                    // time stamp
                    IOVTime start;
                    IOVTime stop;
                    start.setTimestamp(entry.m_range.m_start);
                    stop.setTimestamp(entry.m_range.m_stop);
                    transColl->add(entry.m_channelNumber, IOVRange(start, stop));
                }
            }
            
            // Add in name
            if (entry.m_name.size()) {
                transColl->add(entry.m_channelNumber, entry.m_name);
            }
        }

        // Set start if needed:
        
        // Set stop which may be smaller than the min of all channels
        IOVTime start;
        IOVTime stop;
        bool    setStart = (persColl.m_start != static_cast<uint64_t>(-1));
        if (persColl.m_hasRunLumiBlockTime) {
            // run/lumiblock
            if (setStart) start.setRETime   ( persColl.m_start );
            stop.setRETime   ( persColl.m_stop );
        }
        else {
            // time stamp
            if (setStart) start.setTimestamp( persColl.m_start );
            stop.setTimestamp( persColl.m_stop );
        }
        if (setStart) transColl->addNewStart(start);
        transColl->addNewStop(stop);
  }
}

void
IOVPayloadContainerCnv_p1::transToPers(const IOVPayloadContainer* transObj, 
                                       IOVPayloadContainer_p1* persObj, MsgStream &log)
{

    if (log.level() <= MSG::DEBUG) {
        log << MSG::DEBUG << "IOVPayloadContainerCnv_p1::transToPers "
            << endreq;
    }
    

    // fill map from type name to int
    if (m_attributeTypes.size() == 0)fillAttributeTypeMap();

    // Create attribute list specification
    fillPersAttrSpec(transObj, persObj, log);

    // Nothing to do if specification was not filled
    if (!persObj->m_attrName.size()) {
        if (log.level() <= MSG::DEBUG) {
            log << MSG::DEBUG << "IOVPayloadContainerCnv_p1::transToPers - empty attr list spec "
                << endreq;
        }
        return;
    }

    // Now fill pers object
    persObj->m_payloadVec.resize(transObj->size());

    // Loop over transient CondAttrListCollection
    unsigned int ipcoll = 0;
    IOVPayloadContainer::const_iterator it   = transObj->begin();
    IOVPayloadContainer::const_iterator last = transObj->end();
    for (; it != last; ++it, ++ipcoll) {
        const CondAttrListCollection* tColl     = *it;
        IOVPayloadContainer_p1::CondAttrListCollection_p1& pColl = 
            persObj->m_payloadVec[ipcoll];
        pColl.m_attrLists.resize(tColl->size());

        //  minimal IOVRange
        bool isRunLumiBlock = tColl->minRange().start().isRunEvent();
        if (isRunLumiBlock) {
            // run/lumiblock
            pColl.m_hasRunLumiBlockTime = true;
            // Save start and stop which may be form an IOV smaller than the min of all channels
            pColl.m_start = tColl->minRange().start().re_time();
            pColl.m_stop  = tColl->minRange().stop().re_time();
        }
        else {
            // time stamp
            pColl.m_hasRunLumiBlockTime = false;
            // Save start and stop which may be form an IOV smaller than the min of all channels
            pColl.m_start = tColl->minRange().start().timestamp();
            pColl.m_stop  = tColl->minRange().stop().timestamp();
        }

        // Loop over transient AttributeLists
        CondAttrListCollection::const_iterator itAttr   = tColl->begin();
        CondAttrListCollection::const_iterator lastAttr = tColl->end();
        unsigned int ientry = 0;
        for (; itAttr != lastAttr; ++itAttr, ++ientry) {
            IOVPayloadContainer_p1::CondAttrListEntry_p1&  entry = pColl.m_attrLists[ientry];

            unsigned int chan     = (*itAttr).first;
            entry.m_channelNumber = chan;
            entry.m_firstIndex    = persObj->m_attrIndexes.size();
            // make sure last index is set in case of error
            entry.m_lastIndex     = persObj->m_attrIndexes.size(); 
            entry.m_range.m_start = 0;
            entry.m_range.m_stop  = 0;

            // Look for IOV per channel
            CondAttrListCollection::iov_const_iterator iovIt = tColl->chanIOVPair(chan);
            if (iovIt != tColl->iov_end()) {
                // this channel has an IOV of its own
                if (isRunLumiBlock) {
                    entry.m_range.m_start = iovIt->second.start().re_time();
                    entry.m_range.m_stop  = iovIt->second.stop().re_time();
                }
                else {
                    entry.m_range.m_start = iovIt->second.start().timestamp();
                    entry.m_range.m_stop  = iovIt->second.stop().timestamp();
                }
            }
            
            // Add in name
            std::string name = tColl->chanName(chan);
            if (name.size()) {
                entry.m_name = name;
            }

            // Add in each attribute value and save indexes

            const coral::AttributeList& attrList = (*itAttr).second;

            coral::AttributeList::const_iterator itAttr = attrList.begin();
            coral::AttributeList::const_iterator itLast = attrList.end();
            for (; itAttr != itLast; ++itAttr) {

                std::string typeName = itAttr->specification().typeName();
                std::map<std::string, unsigned int>::const_iterator attIt = 
                    m_attributeTypeMap.find(typeName);
                if (attIt == m_attributeTypeMap.end()) {
                    log << MSG::ERROR << "IOVPayloadContainerCnv_p1::transToPers - cannot find attibute type in map - name "
                        << typeName << endreq;
                    return;
                }
                unsigned int itype = attIt->second;

                std::string name = itAttr->specification().name();
                attIt = m_attrNameMap.find(name);
                if (attIt == m_attrNameMap.end()) {
                    log << MSG::ERROR << "IOVPayloadContainerCnv_p1::transToPers - cannot find attibute name in map - name "
                        << name << endreq;
                    return;
                }
                unsigned int iname = attIt->second;
                
                // fill persistent object with attribute data:
                //    name, type, attribute list, persObj
                fillAttributeData(iname, itype,(*itAttr), persObj, log);
            }
            
            // Now save the last index into the index vector
            entry.m_lastIndex = persObj->m_attrIndexes.size();
        }
    }
}

void
IOVPayloadContainerCnv_p1::fillPersAttrSpec(const IOVPayloadContainer* transObj, 
                                            IOVPayloadContainer_p1* persObj, 
                                            MsgStream &log)
{

    if (log.level() <= MSG::DEBUG) {
        log << MSG::DEBUG << "IOVPayloadContainerCnv_p1::fillPersAttrSpec IOVPayloadContainer size "
            << transObj->size() << endreq;
    }
    
    // Clear name list from previous conversion
    m_attrNameMap.clear();

    // Loop over all attribute lists and collect all possible
    // names - note that some attribute lists may have more names
    // (elements) than others, so we loop over all attribute lists
    // for a complete set
    std::set<std::string> names;
    IOVPayloadContainer::const_iterator itPayload    = transObj->begin();
    IOVPayloadContainer::const_iterator itPayloadEnd = transObj->end();
    // Only if there is at least one attribute list
    for (; itPayload != itPayloadEnd; ++itPayload) {
        
        const CondAttrListCollection* coll            = *itPayload;
        CondAttrListCollection::const_iterator  itColl    =  coll->begin();
        CondAttrListCollection::const_iterator  itCollEnd =  coll->end();
        for (; itColl != itCollEnd; ++itColl) {
            const coral::AttributeList&   attrList    = itColl->second;
            // iterate over attribute list and save types
            coral::AttributeList::const_iterator it   = attrList.begin();
            coral::AttributeList::const_iterator last = attrList.end();
            for (; it != last; ++it) {
                const coral::Attribute&   attr    = *it;
                std::string name     = attr.specification().name();
                std::string typeName = attr.specification().typeName();

                // Fill map of names for future lookup for conversion
                names.insert(name);
            }
        }

    }
    
    // Fill map for conversion toPers, and pers lookup table to be
    // saved for readback
    unsigned int iname = 0;
    persObj->m_attrName.reserve(names.size());
    std::set<std::string>::iterator itName    = names.begin();
    std::set<std::string>::iterator itNameEnd = names.end();
    for (; itName != itNameEnd; ++itName) {
        // Save the attr name
        persObj->m_attrName.push_back(*itName);
        m_attrNameMap.insert(attrNameMapValue_t(*itName, iname));
        ++iname;
    }

    if (log.level() <= MSG::DEBUG) {
        log << MSG::DEBUG << "IOVPayloadContainerCnv_p1::fillPersAttrSpec number of attribute names " 
            << names.size() << endreq;
    }
}


void
IOVPayloadContainerCnv_p1::fillAttributeTypeMap()
{
    // fill type name to number map
    m_attributeTypeMap["bool"]               = IOVPayloadContainer_p1::ATTR_BOOL;
    m_attributeTypeMap["char"]               = IOVPayloadContainer_p1::ATTR_CHAR;
    m_attributeTypeMap["unsigned char"]      = IOVPayloadContainer_p1::ATTR_UNSIGNED_CHAR;
    m_attributeTypeMap["short"]              = IOVPayloadContainer_p1::ATTR_SHORT;
    m_attributeTypeMap["unsigned short"]     = IOVPayloadContainer_p1::ATTR_UNSIGNED_SHORT;
    m_attributeTypeMap["int"]                = IOVPayloadContainer_p1::ATTR_INT;
    m_attributeTypeMap["unsigned int"]       = IOVPayloadContainer_p1::ATTR_UNSIGNED_INT; 
    m_attributeTypeMap["long"]               = IOVPayloadContainer_p1::ATTR_LONG;
    m_attributeTypeMap["unsigned long"]      = IOVPayloadContainer_p1::ATTR_UNSIGNED_LONG;
    m_attributeTypeMap["long long"]          = IOVPayloadContainer_p1::ATTR_LONG_LONG;
    m_attributeTypeMap["unsigned long long"] = IOVPayloadContainer_p1::ATTR_UNSIGNED_LONG_LONG;
    m_attributeTypeMap["float"]              = IOVPayloadContainer_p1::ATTR_FLOAT;
    m_attributeTypeMap["double"]             = IOVPayloadContainer_p1::ATTR_DOUBLE;
    m_attributeTypeMap["long double"]        = IOVPayloadContainer_p1::ATTR_LONG_DOUBLE;
    m_attributeTypeMap["string"]             = IOVPayloadContainer_p1::ATTR_STRING;
    m_attributeTypeMap["blob"]               = IOVPayloadContainer_p1::ATTR_BLOB;
    m_attributeTypeMap["date"]               = IOVPayloadContainer_p1::ATTR_DATE;
    m_attributeTypeMap["time stamp"]         = IOVPayloadContainer_p1::ATTR_TIME_STAMP;

    // vector of types
    m_attributeTypes.push_back("bool");
    m_attributeTypes.push_back("char");
    m_attributeTypes.push_back("unsigned char");
    m_attributeTypes.push_back("short");
    m_attributeTypes.push_back("unsigned short");
    m_attributeTypes.push_back("int");
    m_attributeTypes.push_back("unsigned int");
    m_attributeTypes.push_back("long");
    m_attributeTypes.push_back("unsigned long");
    m_attributeTypes.push_back("long long");
    m_attributeTypes.push_back("unsigned long long");
    m_attributeTypes.push_back("float");
    m_attributeTypes.push_back("double");
    m_attributeTypes.push_back("long double");
    m_attributeTypes.push_back("string");
    m_attributeTypes.push_back("blob");
    m_attributeTypes.push_back("date");
    m_attributeTypes.push_back("time stamp");

}

void
IOVPayloadContainerCnv_p1::fillAttributeData(unsigned int attrName,
                                             unsigned int attrType,
                                             const coral::Attribute& attr,
                                             IOVPayloadContainer_p1* persObj,
                                             MsgStream & log)
{
    // Fill persistent object with attribute data - must use switch
    // for all possible types
    switch (attrType) {
        case IOVPayloadContainer_p1::ATTR_BOOL:
            persObj->m_attrIndexes.push_back(
                AttrListIndexes(attrName, IOVPayloadContainer_p1::ATTR_BOOL, persObj->m_bool.size()));
            persObj->m_bool.push_back(attr.data<bool>());
            break;
        case IOVPayloadContainer_p1::ATTR_CHAR:                
            persObj->m_attrIndexes.push_back(
                AttrListIndexes(attrName, IOVPayloadContainer_p1::ATTR_CHAR, persObj->m_char.size()));
            persObj->m_char.push_back(attr.data<char>());
            break;
        case IOVPayloadContainer_p1::ATTR_UNSIGNED_CHAR:       
            persObj->m_attrIndexes.push_back(
                AttrListIndexes(attrName, IOVPayloadContainer_p1::ATTR_UNSIGNED_CHAR, 
                                persObj->m_unsignedChar.size()));
            persObj->m_unsignedChar.push_back(attr.data<unsigned char>());
            break;
        case IOVPayloadContainer_p1::ATTR_SHORT:
            persObj->m_attrIndexes.push_back(
                AttrListIndexes(attrName, IOVPayloadContainer_p1::ATTR_SHORT, persObj->m_short.size()));
            persObj->m_short.push_back(attr.data<short>());
            break;
        case IOVPayloadContainer_p1::ATTR_UNSIGNED_SHORT:      
            persObj->m_attrIndexes.push_back(
                AttrListIndexes(attrName, IOVPayloadContainer_p1::ATTR_UNSIGNED_SHORT, 
                                persObj->m_unsignedShort.size()));
            persObj->m_unsignedShort.push_back(attr.data<unsigned short>());
            break;
        case IOVPayloadContainer_p1::ATTR_INT:                 
            persObj->m_attrIndexes.push_back(
                AttrListIndexes(attrName, IOVPayloadContainer_p1::ATTR_INT, persObj->m_int.size()));
            persObj->m_int.push_back(attr.data<int>());
            break;
        case IOVPayloadContainer_p1::ATTR_UNSIGNED_INT:        
            persObj->m_attrIndexes.push_back(
                AttrListIndexes(attrName, IOVPayloadContainer_p1::ATTR_UNSIGNED_INT, 
                                persObj->m_unsignedInt.size()));
            persObj->m_unsignedInt.push_back(attr.data<unsigned int>());
            break;
        case IOVPayloadContainer_p1::ATTR_LONG:                
            persObj->m_attrIndexes.push_back(
                AttrListIndexes(attrName, IOVPayloadContainer_p1::ATTR_LONG, persObj->m_long.size()));
            persObj->m_long.push_back(attr.data<long>());
            break;
        case IOVPayloadContainer_p1::ATTR_UNSIGNED_LONG:       
            persObj->m_attrIndexes.push_back(
                AttrListIndexes(attrName, IOVPayloadContainer_p1::ATTR_UNSIGNED_LONG,
                                persObj->m_unsignedLong.size()));
            persObj->m_unsignedLong.push_back(attr.data<unsigned long>());
            break;
        case IOVPayloadContainer_p1::ATTR_LONG_LONG:           
            persObj->m_attrIndexes.push_back(
                AttrListIndexes(attrName, IOVPayloadContainer_p1::ATTR_LONG_LONG, persObj->m_longLong.size()));
            persObj->m_longLong.push_back(attr.data<long long>());
            break;
        case IOVPayloadContainer_p1::ATTR_UNSIGNED_LONG_LONG: 
            persObj->m_attrIndexes.push_back(
                AttrListIndexes(attrName, IOVPayloadContainer_p1::ATTR_UNSIGNED_LONG_LONG,
                                persObj->m_unsignedLongLong.size()));
            persObj->m_unsignedLongLong.push_back(attr.data<unsigned long long>());
            break;
        case IOVPayloadContainer_p1::ATTR_FLOAT:               
            persObj->m_attrIndexes.push_back(
                AttrListIndexes(attrName, IOVPayloadContainer_p1::ATTR_FLOAT, persObj->m_float.size()));
            persObj->m_float.push_back(attr.data<float>());
            break;
        case IOVPayloadContainer_p1::ATTR_DOUBLE:              
            persObj->m_attrIndexes.push_back(
                AttrListIndexes(attrName, IOVPayloadContainer_p1::ATTR_DOUBLE, persObj->m_double.size()));
            persObj->m_double.push_back(attr.data<double>());
            break;
        case IOVPayloadContainer_p1::ATTR_LONG_DOUBLE:         
//             persObj->m_attrIndexes.push_back(
//                 AttrListIndexes(attrName, IOVPayloadContainer_p1::ATTR_LONG_DOUBLE, persObj->m_longDouble.size()));
//             persObj->m_longDouble.push_back(attr.data<long double>());
            break;
        case IOVPayloadContainer_p1::ATTR_STRING:              
            persObj->m_attrIndexes.push_back(
                AttrListIndexes(attrName, IOVPayloadContainer_p1::ATTR_STRING, persObj->m_string.size()));
            persObj->m_string.push_back(attr.data<std::string>());
            break;
        case IOVPayloadContainer_p1::ATTR_BLOB:                
            log << MSG::ERROR 
                << "IOVPayloadContainerCnv_p1::fillAttributeData - cannot currently treat BLOB type " 
                << endreq;
            return;
        case IOVPayloadContainer_p1::ATTR_DATE:                
            persObj->m_attrIndexes.push_back(
                AttrListIndexes(attrName, IOVPayloadContainer_p1::ATTR_DATE, persObj->m_date.size()));
            persObj->m_date.push_back(coral::TimeStamp(attr.data<coral::Date>().time()).total_nanoseconds() );
            break;
        case IOVPayloadContainer_p1::ATTR_TIME_STAMP:
            persObj->m_attrIndexes.push_back(
                AttrListIndexes(attrName, IOVPayloadContainer_p1::ATTR_TIME_STAMP, 
                                persObj->m_timeStamp.size()));
            persObj->m_timeStamp.push_back( attr.data<coral::TimeStamp>().total_nanoseconds() );
            break;
    };
}


void
IOVPayloadContainerCnv_p1::fillAttributeData(const IOVPayloadContainer_p1* persObj,
                                             const AttrListIndexes& index,
                                             const std::string& name,
                                             coral::AttributeList& attrList,
                                             MsgStream & log)
{
    /*
       this offset calculation solves the problem reported in
       ATR-22116 trying to get past the limitation introduced by
       AttrListIndexes::m_objIndex being of type short, which doesn't
       cover the full length of the type-wise data vectors

       It is assumed that when reading the persistent object entries,
       the objIndex starts at 0, and increases by 1 each read for a given type.

       One must also not call fillAttributeData multiple times for the
       same (AttrListIndexes index), nor call them out of order

       * There is one offset per type, initialized to 1
       * In the first read of a type the offset is set to 0
       * In all later reads the offset is not changed unless the objIndex equals 0
         - if objIndex is fixed to be unsigned int, then objIndex should never be 0 in later reads
         - if objIndex is unsigned short, then objIndex==0 only when it runs into the 65536 boundary
           and the offset then gets increased by this amount

       So this will work also when objIndex is integer
     */

    unsigned int objIndex = index.objIndex();
    unsigned int & offset = m_objIndexOffset[index.typeIndex()];
    if(offset == 1) {
        offset = 0;
    } else {
        if(objIndex == 0) {
            offset += 65536;
        }
    }

    // Fill persistent object with attribute data - must use switch
    // for all possible types
    switch (index.typeIndex()) {
        case IOVPayloadContainer_p1::ATTR_BOOL:
            attrList[name].setValue(persObj->m_bool[objIndex + offset]);
            break;
        case IOVPayloadContainer_p1::ATTR_CHAR:                
            attrList[name].setValue(persObj->m_char[objIndex + offset]);
            break;
        case IOVPayloadContainer_p1::ATTR_UNSIGNED_CHAR:       
            attrList[name].setValue(persObj->m_unsignedChar[objIndex + offset]);
            break;
        case IOVPayloadContainer_p1::ATTR_SHORT:
            attrList[name].setValue(persObj->m_short[objIndex + offset]);
            break;
        case IOVPayloadContainer_p1::ATTR_UNSIGNED_SHORT:      
            attrList[name].setValue(persObj->m_unsignedShort[objIndex + offset]);
            break;
        case IOVPayloadContainer_p1::ATTR_INT:                 
            attrList[name].setValue(persObj->m_int[objIndex + offset]);
            break;
        case IOVPayloadContainer_p1::ATTR_UNSIGNED_INT:        
            attrList[name].setValue(persObj->m_unsignedInt[objIndex + offset]);
            break;
        case IOVPayloadContainer_p1::ATTR_LONG:                
            attrList[name].setValue(persObj->m_long[objIndex + offset]);
            break;
        case IOVPayloadContainer_p1::ATTR_UNSIGNED_LONG:       
            attrList[name].setValue(persObj->m_unsignedLong[objIndex + offset]);
            break;
        case IOVPayloadContainer_p1::ATTR_LONG_LONG:           
            attrList[name].setValue(persObj->m_longLong[objIndex + offset]);
            break;
        case IOVPayloadContainer_p1::ATTR_UNSIGNED_LONG_LONG: 
            attrList[name].setValue(persObj->m_unsignedLongLong[objIndex + offset]);
            break;
        case IOVPayloadContainer_p1::ATTR_FLOAT:               
            attrList[name].setValue(persObj->m_float[objIndex + offset]);
            break;
        case IOVPayloadContainer_p1::ATTR_DOUBLE:              
            attrList[name].setValue(persObj->m_double[objIndex + offset]);
            break;
        case IOVPayloadContainer_p1::ATTR_LONG_DOUBLE:         
//             attrList[name].setValue(persObj->m_longDouble[objIndex + offset]);
            break;
        case IOVPayloadContainer_p1::ATTR_STRING:
            attrList[name].setValue(persObj->m_string[objIndex + offset]);
            break;
        case IOVPayloadContainer_p1::ATTR_BLOB:                
            log << MSG::ERROR 
                << "IOVPayloadContainerCnv_p1::fillAttributeData - cannot currently treat BLOB type " 
                << endreq;
            return;
        case IOVPayloadContainer_p1::ATTR_DATE:
          {
            coral::TimeStamp::ValueType  ns( persObj->m_date[objIndex + offset] );
            attrList[name].setValue( coral::Date(coral::TimeStamp(ns).time()) );
            break;
          }
        case IOVPayloadContainer_p1::ATTR_TIME_STAMP:
          {
            coral::TimeStamp::ValueType  ns =
              coral::TimeStamp::ValueType( persObj->m_timeStamp[objIndex + offset] );
            attrList[name].setValue( coral::TimeStamp(ns) );
            break;
          }
    };
}


