///////////////////////// -*- C++ -*- /////////////////////////////

/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

// PyJobOptionsCatalogue.h 
// Header file for class PyJobOptionsCatalogue
// Author: S.Binet<binet@cern.ch>
/////////////////////////////////////////////////////////////////// 
#ifndef ATHENAPYTHON_PYJOBOPTIONSCATALOGUE_H
#define ATHENAPYTHON_PYJOBOPTIONSCATALOGUE_H 1

// STL includes
#include <vector>
#include <string>
#include <unordered_map>

// Gaudi includes
#include "GaudiKernel/StatusCode.h"
#include "GaudiKernel/Property.h"
#include "CxxUtils/checker_macros.h"

// Forward declaration


class ATLAS_NOT_THREAD_SAFE PyJobOptionsCatalogue
{ 
  /////////////////////////////////////////////////////////////////// 
  // Public typedefs
  /////////////////////////////////////////////////////////////////// 
public: 
  
  typedef Gaudi::Details::PropertyBase Property_t;
  typedef std::vector<const Property_t*> Properties_t;
  typedef std::unordered_map<std::string, Properties_t> Objects_t;
  
  /////////////////////////////////////////////////////////////////// 
  // Public methods: 
  /////////////////////////////////////////////////////////////////// 
public: 

  /// Default constructor: 
  PyJobOptionsCatalogue();

  /// Constructor with parameters: 

  /// Destructor: 
  virtual ~PyJobOptionsCatalogue(); 

  /////////////////////////////////////////////////////////////////// 
  // Const methods: 
  ///////////////////////////////////////////////////////////////////

  /// retrieve the list of all properties for a given client
  const Properties_t* properties (const std::string& client) const;

  /// retrieve the list of names of all clients
  /// FIXME: slow and inefficient
  std::vector<std::string> clients() const;

  const Property_t* getProperty(const std::string& client, 
                                const std::string& prop_name) const;
  
  /////////////////////////////////////////////////////////////////// 
  // Non-const methods: 
  /////////////////////////////////////////////////////////////////// 

  /// add a property to the catalogue
  StatusCode
  add_property (const std::string& client, const Property* prop);

  /// remove a property from the catalogue
  StatusCode
  remove_property (const std::string& client, const std::string& prop_name);

  /////////////////////////////////////////////////////////////////// 
  // Private methods
  /////////////////////////////////////////////////////////////////// 
 private: 

  /// Copy constructor: 
  PyJobOptionsCatalogue( const PyJobOptionsCatalogue& rhs );

  /// Assignment operator: 
  PyJobOptionsCatalogue& operator=( const PyJobOptionsCatalogue& rhs ); 

  /// retrieve the list of properties of a given client
  Properties_t* find_properties (const std::string& client) const;
  
  /// find a property among a list of properties
  bool find_property (Properties_t* props,
		      const std::string& name,
		      Properties_t::iterator& result);

  /////////////////////////////////////////////////////////////////// 
  // Private data: 
  /////////////////////////////////////////////////////////////////// 
private: 

  Objects_t m_catalogue;
}; 

/////////////////////////////////////////////////////////////////// 
// Inline methods: 
/////////////////////////////////////////////////////////////////// 
//std::ostream& operator<<( std::ostream& out, const PyJobOptionsCatalogue& o );



#endif //> !ATHENAPYTHON_PYJOBOPTIONSCATALOGUE_H
