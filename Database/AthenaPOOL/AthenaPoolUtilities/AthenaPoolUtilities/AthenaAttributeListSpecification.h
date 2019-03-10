/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#ifndef ATHENAPOOLUTILITIES_ATHENAATTRIBUTELISTSPECIFICATION_H
#define ATHENAPOOLUTILITIES_ATHENAATTRIBUTELISTSPECIFICATION_H

#include <string>
#include <iostream>
#include <sstream>
#include <typeinfo>
#include <vector>
#include <map>
#include <set>
#include <atomic>
#include "CoralBase/AttributeSpecification.h"
#include "CoralBase/AttributeListException.h"
#include "CoralBase/AttributeListSpecification.h"
//#include "CoralBase/Blob.h"
//#include "CoralBase/Date.h"
//#include "CoralBase/TimeStamp.h"

#include "GaudiKernel/System.h"

#include "AthenaPoolUtilities/AthenaAttributeSpecification.h"

/////////////////////////////////////////////////
//
// Class which acts like coral::AttributeSpecification 
//   BUT allows annotation like pool::CollectionDescription
//   BUT which is encased in name,value pair interface
//   which is not accessible through the constructor
//
////////////////////////////////////////////////


/**
   @class AthenaAttributeListSpecification AthenaAttributeListSpecification.h CoralBase/AthenaAttributeListSpecification.h

   The specification of an AttributeList
*/
class AthenaAttributeListSpecification {
public:
  /// Constructor
  AthenaAttributeListSpecification();

  /// Copy constructor.
  AthenaAttributeListSpecification( const AthenaAttributeListSpecification& rhs );

  /// Copy constructor.
  AthenaAttributeListSpecification( const coral::AttributeListSpecification& rhs );
 
  /// Decrements the reference counter
  void release() const;

  /// Appends a new specification
  template<typename T> void extend( const std::string& name,const std::string& unitName ="UNKNOWN", const std::string& groupName ="UNKNOWN" );

  void extend( const std::string& name,
               const std::string& typeName,
               const std::string& unitName="UNKNOWN", 
               const std::string& groupName="UNKNOWN");
  void extend( const std::string& name,
               const std::type_info& type,
               const std::string& unitName="UNKNOWN", 
               const std::string& groupName="UNKNOWN");
  void extend( const std::string& name,
               const AthenaAttributeType& type );
  void extend( const AthenaAttributeSpecification& row );

  /// Returns the size of the specification
  size_t size() const;

  /// Equal operator. Compares only types and values. Not the attribute names
  bool operator==( const AthenaAttributeListSpecification& rhs ) const;

  /// Comparison operator. Compares only types and values. Not the attribute names
  bool operator!=( const AthenaAttributeListSpecification& rhs ) const;

  /// Returns the index of a specification given its name. If the name is not found it returns -1.
  int index( const std::string& name ) const;

  /// Bracket operator
  const AthenaAttributeSpecification& operator[]( size_t index ) const;

  /// Returns the specification given an index
  const AthenaAttributeSpecification& specificationForAttribute( int index ) const;

  /// Fills a coral Specification
  void coralSpec(coral::AttributeListSpecification&) const;

  /// The iterator class
  class const_iterator
  {
  public:
    ~const_iterator() {};

    const_iterator( const const_iterator& rhs );
    const_iterator& operator=( const const_iterator& rhs );

  private:
    friend class AthenaAttributeListSpecification;
    const_iterator( std::vector< AthenaAttributeSpecification* >::const_iterator theIterator );

  public:
    const AthenaAttributeSpecification* operator->() const;
    const AthenaAttributeSpecification& operator*() const;
    void operator++();
    bool operator==( const const_iterator& rhs ) const;
    bool operator!=( const const_iterator& rhs ) const;

  private:
    std::vector< AthenaAttributeSpecification* >::const_iterator m_iterator;
  };


  /// Returns a forward iterator
  const_iterator begin() const;
  const_iterator end() const;


protected:
  /// Increments the reference counter only the AttributeList can call this method
  friend class AthenaAttributeList;
  void addRef() const;

  /// The destructor is private.
  ~AthenaAttributeListSpecification();

  /// No assignment operator
  AthenaAttributeListSpecification& operator=( const AthenaAttributeListSpecification& );

private:
  /// The reference counter
  mutable std::atomic<int> m_counter;

  /// The specifications
  std::vector< AthenaAttributeSpecification* > m_attributeSpecifications;

  /// The map of names to indices
  std::map< std::string, int >  m_mapOfNameToIndex;
};




// Inline methods
template<typename T> void
AthenaAttributeListSpecification::extend( const std::string& name, const std::string& unitName, const std::string& groupName )
{
  this->extend( name, typeid(T), unitName, groupName );
}


inline size_t
AthenaAttributeListSpecification::size() const
{
  return m_attributeSpecifications.size();
}


inline AthenaAttributeListSpecification::const_iterator
AthenaAttributeListSpecification::begin() const
{
  return AthenaAttributeListSpecification::const_iterator( m_attributeSpecifications.begin() );
}


inline AthenaAttributeListSpecification::const_iterator
AthenaAttributeListSpecification::end() const
{
  return AthenaAttributeListSpecification::const_iterator( m_attributeSpecifications.end() );
}

inline
AthenaAttributeListSpecification::const_iterator::const_iterator( const AthenaAttributeListSpecification::const_iterator& rhs ):
  m_iterator( rhs.m_iterator )
{}
  
inline AthenaAttributeListSpecification::const_iterator&
AthenaAttributeListSpecification::const_iterator::operator=( const AthenaAttributeListSpecification::const_iterator& rhs )
{
  m_iterator = rhs.m_iterator;
  return *this;
}

inline
AthenaAttributeListSpecification::const_iterator::const_iterator( std::vector< AthenaAttributeSpecification* >::const_iterator theIterator ):
  m_iterator( theIterator )
{}


inline const AthenaAttributeSpecification*
AthenaAttributeListSpecification::const_iterator::operator->() const
{
  return *m_iterator;
}
  
inline const AthenaAttributeSpecification&
AthenaAttributeListSpecification::const_iterator::operator*() const
{
  return **m_iterator;
}

inline void
AthenaAttributeListSpecification::const_iterator::operator++()
{
  m_iterator++;
}

inline bool
AthenaAttributeListSpecification::const_iterator::operator==( const AthenaAttributeListSpecification::const_iterator& rhs ) const
{
  return m_iterator == rhs.m_iterator;
}

inline bool
AthenaAttributeListSpecification::const_iterator::operator!=( const AthenaAttributeListSpecification::const_iterator& rhs ) const
{
  return m_iterator != rhs.m_iterator;
}

inline const AthenaAttributeSpecification&
AthenaAttributeListSpecification::operator[]( size_t index ) const
{
  return this->specificationForAttribute( static_cast<int>( index ) );
}

inline bool
AthenaAttributeListSpecification::operator!=( const AthenaAttributeListSpecification& rhs ) const
{
  return ( ! ( *this == rhs ) );
}

inline
AthenaAttributeListSpecification::AthenaAttributeListSpecification():
  m_counter( 1 ),
  m_attributeSpecifications(),
  m_mapOfNameToIndex()
{}

inline
AthenaAttributeListSpecification::AthenaAttributeListSpecification( const AthenaAttributeListSpecification& rhs ):
  m_counter( 1 ),
  m_attributeSpecifications(),
  m_mapOfNameToIndex( rhs.m_mapOfNameToIndex )
{
  for ( std::vector< AthenaAttributeSpecification* >::const_iterator iSpecification = rhs.m_attributeSpecifications.begin();
        iSpecification != rhs.m_attributeSpecifications.end(); ++iSpecification ) {
    m_attributeSpecifications.push_back( new AthenaAttributeSpecification( **iSpecification ) );
  }
}

inline
AthenaAttributeListSpecification::AthenaAttributeListSpecification( const coral::AttributeListSpecification& rhs ):
  m_counter( 1 ),
  m_attributeSpecifications(),
  m_mapOfNameToIndex()
{
  //std::cerr << "AthenaAttributeListSpecification (coral version) with counter = " << m_counter << std::endl;
  for ( coral::AttributeListSpecification::const_iterator iSpecification = rhs.begin();
        iSpecification != rhs.end(); ++iSpecification ) 
  {
    this->extend(iSpecification->name(),iSpecification->typeName());
  }
}

inline
AthenaAttributeListSpecification::~AthenaAttributeListSpecification()
{
  //std::cerr << "In AthenaAttributeListSpecification destructor" << std::endl;
  for ( std::vector< AthenaAttributeSpecification* >::iterator iSpecification = m_attributeSpecifications.begin();
        iSpecification != m_attributeSpecifications.end(); ++iSpecification ) delete *iSpecification;
}


inline void
AthenaAttributeListSpecification::addRef() const
{
  ++m_counter;
}


inline void
AthenaAttributeListSpecification::release() const
{
  //std::cerr << "Before decrement " << std::endl;
  //std::cerr << "AthenaAttributeListSpecification release called "
  //          << "with counter = " << m_counter << std::endl;
  if (--m_counter == 0) delete this;
}

inline void
AthenaAttributeListSpecification::extend( const std::string& name,
                                          const std::string& typeName,
                                          const std::string& unitName,
                                          const std::string& groupName )
{
  if ( m_mapOfNameToIndex.find( name ) != m_mapOfNameToIndex.end() )
    throw coral::AttributeListException( "An attribute with the name \"" + name + "\" exists already" );
  m_attributeSpecifications.push_back( new AthenaAttributeSpecification( name, typeName, unitName, groupName ) );
  m_mapOfNameToIndex.insert( std::make_pair( name, static_cast<int>( m_attributeSpecifications.size() - 1 ) ) );
}


inline void
AthenaAttributeListSpecification::extend( const std::string& name,
                                          const std::type_info& typeInfo,
                                          const std::string& unitName,
                                          const std::string& groupName)
{ 
  std::string typeName = System::typeinfoName(typeInfo);
  if ( m_mapOfNameToIndex.find( name ) != m_mapOfNameToIndex.end() )
    throw coral::AttributeListException( "An attribute with the name \"" + name + "\" exists already" );
  m_attributeSpecifications.push_back( new AthenaAttributeSpecification( name, typeName, unitName, groupName ) );
  m_mapOfNameToIndex.insert( std::make_pair( name, static_cast<int>( m_attributeSpecifications.size() - 1 ) ) );
}


inline void
AthenaAttributeListSpecification::extend( const std::string& name,
                                          const AthenaAttributeType& typeName )
{
  if ( m_mapOfNameToIndex.find( name ) != m_mapOfNameToIndex.end() )
    throw coral::AttributeListException( "An attribute with the name \"" + name + "\" exists already" );
  m_attributeSpecifications.push_back( new AthenaAttributeSpecification( name, typeName ) );
  m_mapOfNameToIndex.insert( std::make_pair( name, static_cast<int>( m_attributeSpecifications.size() - 1 ) ) );
}


inline void
AthenaAttributeListSpecification::extend( const AthenaAttributeSpecification& aas )
{
  if ( m_mapOfNameToIndex.find( aas.name() ) != m_mapOfNameToIndex.end() )
    throw coral::AttributeListException( "An attribute with the name \"" + aas.name() + "\" exists already" );
  m_attributeSpecifications.push_back( new AthenaAttributeSpecification( aas ) );
  m_mapOfNameToIndex.insert( std::make_pair( aas.name(), static_cast<int>( m_attributeSpecifications.size() - 1 ) ) );
}


inline int
AthenaAttributeListSpecification::index( const std::string& name ) const
{
  std::map<std::string, int>::const_iterator iSpecificationForAttribute = m_mapOfNameToIndex.find( name );
  if ( iSpecificationForAttribute == m_mapOfNameToIndex.end() ) return -1;
  else return iSpecificationForAttribute->second;
}


inline const AthenaAttributeSpecification&
AthenaAttributeListSpecification::specificationForAttribute( int index ) const
{
  if ( index < 0 || index >= static_cast<int>( m_attributeSpecifications.size() ) )
    throw coral::AttributeListException( "Invalid index for the attribute already" );
  return *( m_attributeSpecifications[ index ] );
}

// Utility function which 
//                        transforms an AthenaAttributeListSpecification 
//                              into a  coral::AttributeListSpecification
inline void 
AthenaAttributeListSpecification::coralSpec(coral::AttributeListSpecification& s) const
{
  for (AthenaAttributeListSpecification::const_iterator i = this->begin();
       i != this->end();
       ++i)
  {
    s.extend( i->name(), i->attributeType().typeName() );
  }
}


inline bool
AthenaAttributeListSpecification::operator==( const AthenaAttributeListSpecification& rhs ) const
{
  if ( this->size() != rhs.size() ) return false;
  size_t theSize = this->size();
  const AthenaAttributeListSpecification& lhs = *this;
  for ( size_t i = 0; i < theSize; ++i )
    //if ( (*this)[i] != rhs[i] )
    if ( lhs[i] != rhs[i] )
      return false;
  return true;
}

#endif
