///////////////////////// -*- C++ -*- /////////////////////////////

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// PyJobOptionsCatalogue.cxx 
// Implementation file for class PyJobOptionsCatalogue
// Author: S.Binet<binet@cern.ch>
/////////////////////////////////////////////////////////////////// 

// Python includes
#include "Python.h"

// AthenaPython includes
#include "PyJobOptionsCatalogue.h"

// STL includes



/////////////////////////////////////////////////////////////////// 
// Public methods: 
/////////////////////////////////////////////////////////////////// 

// Constructors
////////////////
PyJobOptionsCatalogue::PyJobOptionsCatalogue() :
  m_catalogue ()
{}

// Destructor
///////////////
PyJobOptionsCatalogue::~PyJobOptionsCatalogue()
{
  for ( Objects_t::const_iterator 
	  icat     = m_catalogue.begin(), 
	  icat_end = m_catalogue.end();
	icat != icat_end;
	++icat ) {
    for ( Properties_t::const_iterator
	    iprop     = icat->second.begin(),
	    iprop_end = icat->second.end();
	  iprop != iprop_end;
	  ++iprop ) {
      delete *iprop; 
    }
  }
}

/////////////////////////////////////////////////////////////////// 
// Const methods: 
///////////////////////////////////////////////////////////////////

const PyJobOptionsCatalogue::Properties_t*
PyJobOptionsCatalogue::properties (const std::string& client) const
{
  return find_properties (client);
}

std::vector<std::string>
PyJobOptionsCatalogue::clients() const
{
  std::vector<std::string> result;
  result.reserve (m_catalogue.size());
  for ( Objects_t::const_iterator 
	  icat     = m_catalogue.begin(), 
	  icat_end = m_catalogue.end();
	icat != icat_end;
	++icat ) {
    result.push_back (icat->first);
  }
  return result;
}

/////////////////////////////////////////////////////////////////// 
// Non-const methods: 
/////////////////////////////////////////////////////////////////// 

/// add a property to the catalogue
StatusCode
PyJobOptionsCatalogue::add_property (const std::string& client,
				     const Gaudi::Details::PropertyBase* prop)
{
  if (!prop) {
    return StatusCode::FAILURE;
  }

  Properties_t* props = find_properties (client);
  if (props) {
    remove_property (client, prop->name()).ignore();
    props->push_back (prop);
  } else {
    m_catalogue[client] = Properties_t(1, prop);
  }
  return StatusCode::SUCCESS;
}

/// remove a property from the catalogue
StatusCode
PyJobOptionsCatalogue::remove_property (const std::string& client,
					const std::string& prop_name)
{
  Properties_t* props = find_properties (client);
  if (props) {
    Properties_t::iterator itr;
    if (find_property (props, prop_name, itr)) {
      delete *itr;
      props->erase (itr);
    }
  }
  return StatusCode::SUCCESS;
}

/// find a particular property by  name
const PyJobOptionsCatalogue::Property_t*
PyJobOptionsCatalogue::getProperty (const std::string& client,
                                    const std::string& prop_name) const {

  Properties_t* props = find_properties (client);
  if (!props) { return nullptr; }

  Properties_t::const_iterator itr;
  for (itr = props->begin(); itr!=props->end(); ++itr) {
    if (prop_name == (*itr)->name()) {
      return *itr;
    }
  }
  return nullptr;
}

/////////////////////////////////////////////////////////////////// 
// Protected methods: 
/////////////////////////////////////////////////////////////////// 

/// retrieve the list of properties of a given client
PyJobOptionsCatalogue::Properties_t*
PyJobOptionsCatalogue::find_properties (const std::string& client) const
{
  PyJobOptionsCatalogue *self = const_cast<PyJobOptionsCatalogue*>(this);
  Objects_t::iterator result = self->m_catalogue.find (client);
  return result==self->m_catalogue.end()
    ? NULL
    : &result->second;
}

  
/// find a property among a list of properties
bool
PyJobOptionsCatalogue::find_property (Properties_t* props,
				      const std::string& name,
				      Properties_t::iterator& result)
{
  if (0==props) {
    return false;
  }

  for (result = props->begin(); result!=props->end(); ++result) {
    // case sensitive...
    if (name == (*result)->name()) {
      return true;
    }
  }
  return false;
}

