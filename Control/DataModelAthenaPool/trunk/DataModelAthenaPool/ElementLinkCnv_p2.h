/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef DATAMODELATHENAPOOL_ELEMENTLINKCNV_P2_H
#define DATAMODELATHENAPOOL_ELEMENTLINKCNV_P2_H

/**
 *  @file ElementLinkCnv_p2.h
 *  @brief This file contains the class definition for the ElementLinkCnv_p2 class.
 *  @author R.D.Schaffer@cern.ch
 **/

#include "AthenaPoolCnvSvc/T_AthenaPoolTPConverter.h"
#include "ElementLink_p2.h"

/** @class ElementLinkCnv_p2<LINK>
 *  @brief Converter template for converters between transient
 *  ElementLink and its persistent representation. 
 *
 *  This is one of two optimized versions of persistent element
 *  link. The other version is ElementLinkCompactCnv_p2. Both versions
 *  work with an external class ElementLinkContNames_p2 which
 *  maintains a lookup table of the container names. These two
 *  versions should be used when there is a collection of persistent
 *  objects where each holds an element link. The top-level collection
 *  should then hold the ElementLinkContNames_p2 object with the
 *  container names.
 *
 *  This version holds both the index to the container name stored in
 *  ElementLinkContNames_p2 and the index to the element link. The
 *  other version (ElementLinkCompactCnv_p2) only holds the element
 *  link index - in this second case the ElementLinkContNames_p2 keeps
 *  track of which objects in the collection hold which container
 *  index.
 *
 *  This version should be used when there are many different
 *  container names, and ElementLinkCompactCnv_p2 should be used when
 *  the element links all point to the same container.
 *
 *  Template argument LINK is the type of the transient
 *  ElementLink. The type of the persistent link representation is
 *  automatically deducted - it can be integer index type, or string
 *  index type. More types can be added in ElementLink_p2.h
 *
 **/

class ElementLinkContNames_p2;

template <class LINK_TYPE>
class ElementLinkCnv_p2
    : public T_AthenaPoolTPCnvBase< LINK_TYPE, typename GeneratePersELinkType_p2<LINK_TYPE >::type > {
public:
    typedef	LINK_TYPE				                Link_t;
    typedef 	typename GeneratePersELinkType_p2< Link_t >::type	PersLink_t;

    ElementLinkCnv_p2();
    /// If client sets state of EL to persistible (i.e. with
    /// index,key), then set isPersistible to true (default is false)
    ElementLinkCnv_p2(bool isPersistible);
  
    void         resetForCnv(ElementLinkContNames_p2& lookupTable);
    void         resetForCnv(const ElementLinkContNames_p2& lookupTable);

    virtual void persToTrans(const PersLink_t* pers, Link_t* trans, MsgStream& log) ;
    virtual void transToPers(const Link_t* trans, PersLink_t* pers, MsgStream& log) ;

    virtual void persToTrans(const PersLink_t& pers, Link_t& trans, MsgStream& log) ;
    virtual void transToPers(const Link_t& trans, PersLink_t& pers, MsgStream& log) ;

private:

    /// Add name to lookup table and return index
    unsigned int getNameIndex    (const std::string& name);
    /// Get the container name from lookup table
    void         getContName     (unsigned int nameIndex, std::string*& name, MsgStream& log);
    std::string* defaultLastName () const;

    typedef std::map<std::string, unsigned int> IndexMap;

    /// Map to find index of container name in lookup table
    IndexMap                       m_nameIndexMap;
    /// Lookup table with list of container names and starting element
    /// link index for each container name change
    ElementLinkContNames_p2*       m_lookupTable;
    const ElementLinkContNames_p2* m_clookupTable;
    /// Last container name added - for persToTrans
    ///   must be string by value 
    std::string                    m_lastNameAdded;
    /// Last container name found - for transToPers
    ///   use string ptr to avoid string copy
    const std::string*             m_lastNameFound;
    /// Last name inded
    unsigned int                   m_lastNameIndex;

    /// Can avoid call to toPersistent if work already done by caller
    bool                           m_isPersistible;

};

#ifndef __REFLEX__
#include "ElementLinkCnv_p2.icc"
#endif // not __REFLEX__

#endif
