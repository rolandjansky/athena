/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// OptionsStore
// 02-Oct-2003 Bill Seligman

// This template class is actually a simple wrapper around a map.
// It's purpose is to provide a common code base for store run options
// for the LArG4Model and LArG4 packages (although it could be used
// by other packages).

#ifndef __LArG4_OptionsStore_H__
#define __LArG4_OptionsStore_H__

#include <map>
#include <string>
#include <iostream>

#undef DEBUG_OPTIONS

namespace LArG4 {

  template< typename EnumType >
  class OptionsStore 
  {
  public:

    virtual ~OptionsStore() {;}

    // Store and retrieve user options.  Note that no attempt is made
    // to validate the option strings; if the user supplies "parital"
    // instead of "partial" this routine will not detect it.
    virtual void SetOption( EnumType e, std::string value );
    virtual std::string GetOption( EnumType e ) const; 

  protected:

    // The constructor is protected according to the standard
    // singleton design pattern.  However, any class that interits
    // from this template will have to implement its own GetInstance()
    // method.
    OptionsStore();

  private: 

    // Define a map type to hold detector options; some associated
    // types, and the map itself.
    typedef typename std::map< EnumType, std::string >    m_OptionsStoreMap_t;
    typedef typename m_OptionsStoreMap_t::iterator        m_OptionsStoreMap_ptr_t;
    typedef typename m_OptionsStoreMap_t::const_iterator  m_OptionsStoreMap_const_ptr_t;
    m_OptionsStoreMap_t m_OptionsStoreMap;
  };


  /////////////////////////////////////////////////////////////////////
  // Since this is a template class, its implementation also has to be
  // in the header.
  /////////////////////////////////////////////////////////////////////


  // Constructor
  template < typename EnumType >
  OptionsStore<EnumType>::OptionsStore() {;}


  // Store an option in the internal table.
  template < typename EnumType >
  void OptionsStore<EnumType>::SetOption( EnumType e, std::string value ) 
  { 
    m_OptionsStoreMap[e] = value; 
  }


  // Fetch an option from our internal table.  (We get a little fancy
  // here because, as an exercise, I want to maintain the "const"
  // nature of the method without generating compiler warnings.)
  template < typename EnumType >
  std::string OptionsStore<EnumType>::GetOption( EnumType e ) const
  {
    m_OptionsStoreMap_const_ptr_t i = m_OptionsStoreMap.find(e);

#ifdef DEBUG_OPTIONS
    std::cout << "OptionsStore: Searching for " << e;
#endif
    
    if ( i == m_OptionsStoreMap.end() )
      {
#ifdef DEBUG_OPTIONS
	std::cout << " and found nothing." << std::endl;
#endif

	return "";
      }

    else
      {
	// Reminder:
	// i == a pointer to one entry in OptionsStoreMap
	// (*i) = a pair<EnumType, std::string>
	// (*i).second = a string, the value of the option.
      
#ifdef DEBUG_OPTIONS
	std::cout << " and found '" 
		  << (*i).second
		  << "'"
		  << std::endl;
#endif

	return (*i).second;
      }
  } 

} // namespace LArG4

#endif // __LArG4_OptionsStore_H__
