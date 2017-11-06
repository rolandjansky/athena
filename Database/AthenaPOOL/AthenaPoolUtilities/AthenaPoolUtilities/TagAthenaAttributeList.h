/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/**
 * @file TagAthenaAttributeList.h
 *
 * @brief An AttributeList represents a logical row of attributes in a
 * metadata table.  The name and type of each attribute is given in an
 * AttributeListSpecification.  TagAthenaAttributeList is an
 * AttributeList which is also a DataObject and thus can be saved in
 * an Athena data store, e.g. the Event Store.
 *
 * @author RD Schaffer <R.D.Schaffer@cern.ch>
 * @author Kristo Karr <kkarr@hep.anl.gov>
 * @author Richard Hawkings <richard.hawkings@cern.ch>
 *
 * $Id: TagAthenaAttributeList.h,v 1.4 2008-10-15 15:13:10 cranshaw Exp $
 */

#ifndef DBDATAOBJECTS_TAGATHENAATTRIBUTELIST_H
# define DBDATAOBJECTS_TAGATHENAATTRIBUTELIST_H 

//<<<<<< INCLUDES                                                       >>>>>>

#include "CoralBase/Attribute.h"
#include "CoralBase/AttributeList.h"
#include "CoralBase/AttributeListSpecification.h"
#include "AthenaPoolUtilities/AthenaAttributeListSpecification.h"
#include "AthenaKernel/CLASS_DEF.h"

#include "GaudiKernel/DataObject.h"

/**
 * @class TagAthenaAttributeList
 *
 * @brief An AttributeList represents a logical row of attributes in a
 * metadata table.  The name and type of each attribute is given in an
 * AttributeListSpecification.  TagAthenaAttributeList is an
 * AttributeList which is also a DataObject and thus can be saved in
 * an Athena data store, e.g. the Event Store.
 *
 */

class TagAthenaAttributeList : public coral::AttributeList,
                               public DataObject
{
public:

    /// Construct empty attribute list with empty specification.
    TagAthenaAttributeList();

    /// Copy attribute lists.
    TagAthenaAttributeList(const coral::AttributeList& rhs);
    TagAthenaAttributeList(const TagAthenaAttributeList& rhs);
    /// Assignment.
    TagAthenaAttributeList& operator= (const TagAthenaAttributeList& rhs);
        
    /// Construct attribute list according to the specification.
    /// The link to the specification is kept internally.
    /// User must make sure that the lifetime of the specification covers
    /// the lifetime of the attribute list.
    explicit TagAthenaAttributeList(const coral::AttributeListSpecification& rhs);

    /// base on Spec with (name,type,info) tuple as well
    /// Otherwise it acts as coral version
    explicit TagAthenaAttributeList(const AthenaAttributeListSpecification& rhs);

    ~TagAthenaAttributeList();

    /// Extends the AttributeList by one attribute, given a specification. 
    /// In case the specification is shared, a copy is triggered.
    template<typename T> void extend( const std::string& name );
    void extend( const std::string& name,
                 const std::string& typeName );
    void extend( const std::string& name,
                 const std::type_info& type );
    void extend( const std::string& name,
                 const AthenaAttributeType& type );

    /// print to simulate function provided by old POOL AttributeList
    void print(std::ostream& os) const;

    bool hasDesc() const;
    const AthenaAttributeListSpecification* getDesc() const;

    coral::AttributeList coralList() const {return static_cast<coral::AttributeList>(*this);}

private:

    /// The specification (empty if created with coral version) 
    AthenaAttributeListSpecification* m_desc;


};

CLASS_DEF(TagAthenaAttributeList, 40774349, 0)

//<<<<<< INLINE PUBLIC FUNCTIONS                                        >>>>>>
//<<<<<< INLINE MEMBER FUNCTIONS                                        >>>>>>


inline
TagAthenaAttributeList::~TagAthenaAttributeList() {
  if(m_desc!=0) m_desc->release();
}
// CONSTRUCTORS - CORAL VERSIONS
inline
TagAthenaAttributeList::TagAthenaAttributeList() : coral::AttributeList(),
                                             m_desc(0)
{}

inline
TagAthenaAttributeList::TagAthenaAttributeList(const coral::AttributeList& rhs) : 
                                             coral::AttributeList(rhs),
                                             DataObject(*this),
                                             m_desc(0)
{}
        
inline
TagAthenaAttributeList::TagAthenaAttributeList(const coral::AttributeListSpecification& spec) : 
    coral::AttributeList(spec),
    m_desc(0)
{}

// CONSTRUCTORS - ATHENA MOD VERSIONS
inline
TagAthenaAttributeList::TagAthenaAttributeList(const TagAthenaAttributeList& rhs) : 
  coral::AttributeList(rhs),
  DataObject(rhs)
{
  if (rhs.hasDesc()) m_desc = new AthenaAttributeListSpecification( *(rhs.getDesc()) );
  else m_desc=0;
}
        
inline
TagAthenaAttributeList&
TagAthenaAttributeList::operator=(const TagAthenaAttributeList& rhs) 
{
  if (this != &rhs) {
    coral::AttributeList::operator= (rhs);
    DataObject::operator= (rhs);
    if (m_desc) m_desc->release();
    if (rhs.hasDesc()) m_desc = new AthenaAttributeListSpecification( *(rhs.getDesc()) );
    else m_desc=0;
  }
  return *this;
}
        
inline
TagAthenaAttributeList::TagAthenaAttributeList(const AthenaAttributeListSpecification& desc) : 
  coral::AttributeList(),
  m_desc( new AthenaAttributeListSpecification(desc) )
{
  for (AthenaAttributeListSpecification::const_iterator i = desc.begin();
       i != desc.end();
       ++i)
  {
    this->coral::AttributeList::extend( i->name(), i->attributeType().typeName() );
  }
}

// INLINE METHODS

template<typename T> void
TagAthenaAttributeList::extend( const std::string& name )
{
  this->extend( name, typeid(T) );
}

inline void
TagAthenaAttributeList::extend( const std::string& name,
                             const std::string& typeName )
{
  if (this->hasDesc()) m_desc->extend( name, typeName );
  this->coral::AttributeList::extend( name, typeName );
}


inline void
TagAthenaAttributeList::extend( const std::string& name,
                             const std::type_info& type )
{
  if (this->hasDesc()) m_desc->extend( name, type );
  this->coral::AttributeList::extend( name, type );
}

inline void
TagAthenaAttributeList::extend( const std::string& name,
                             const AthenaAttributeType& type )
{
  if (this->hasDesc()) m_desc->extend( name, type );
  this->coral::AttributeList::extend( name, type.typeName() );
}

inline
void TagAthenaAttributeList::print(std::ostream& os) const {
  // this->toOutputStream(os);
  os << "{";
  for (coral::AttributeList::const_iterator itr=this->begin();
      itr!=this->end();++itr) {
    if (itr!=this->begin()) os << ",";
    itr->toOutputStream(os);
  }
  os << "}";
}

inline bool TagAthenaAttributeList::hasDesc() const
{
  return m_desc!=0;
}

inline
const AthenaAttributeListSpecification* TagAthenaAttributeList::getDesc() const
{
  // User has to check for zero = no desc
  return m_desc;
}


#endif // DBDATAOBJECTS_TAGATHENAATTRIBUTELIST_H
