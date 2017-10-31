/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/**
 * @file PersistentDataModel/AthenaAttributeList.h
 *
 * @brief An AttributeList represents a logical row of attributes in a
 * metadata table.  The name and type of each attribute is given in an
 * AttributeListSpecification.  AthenaAttributeList is an
 * AttributeList which is also a DataObject and thus can be saved in
 * an Athena data store, e.g. the Event Store.
 *
 * @author RD Schaffer <R.D.Schaffer@cern.ch>
 * @author Kristo Karr <kkarr@hep.anl.gov>
 * @author Richard Hawkings <richard.hawkings@cern.ch>
 *
 * $Id: AthenaAttributeList.h,v 1.4 2008-09-19 21:59:15 cranshaw Exp $
 */

#ifndef PERSISTENTDATAMODEL_ATHENAATTRIBUTELIST_H
#define PERSISTENTDATAMODEL_ATHENAATTRIBUTELIST_H 

//<<<<<< INCLUDES                                                       >>>>>>

#include "CoralBase/Attribute.h"
#include "CoralBase/AttributeList.h"
#include "CoralBase/AttributeListSpecification.h"
#include "AthenaKernel/CLASS_DEF.h"

#include "GaudiKernel/DataObject.h"

/**
 * @class AthenaAttributeList
 *
 * @brief An AttributeList represents a logical row of attributes in a
 * metadata table.  The name and type of each attribute is given in an
 * AttributeListSpecification.  AthenaAttributeList is an
 * AttributeList which is also a DataObject and thus can be saved in
 * an Athena data store, e.g. the Event Store.
 *
 */

class AthenaAttributeList : public coral::AttributeList,
                            public DataObject
{
public:

    /// Construct empty attribute list with empty specification.
    AthenaAttributeList();

    /// Copy attribute lists.
    AthenaAttributeList(const coral::AttributeList& rhs);
        
    /// Construct attribute list according to the specification.
    /// The link to the specification is kept internally.
    /// User must make sure that the lifetime of the specification covers
    /// the lifetime of the attribute list.
    explicit AthenaAttributeList(const coral::AttributeListSpecification& rhs);

    /// print to simulate function provided by old POOL AttributeList
    void print(std::ostream& os) const;

    coral::AttributeList coralList() const {return static_cast<coral::AttributeList>(*this);}

};

CLASS_DEF(AthenaAttributeList, 40774348, 0)
#include "AthenaKernel/CondCont.h"
CONDCONT_DEF(AthenaAttributeList, 211215482);

#endif // PERSISTENTDATAMODEL_ATHENAATTRIBUTELIST_H
