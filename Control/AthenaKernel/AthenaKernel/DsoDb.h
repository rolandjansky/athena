///////////////////////// -*- C++ -*- /////////////////////////////

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// DsoDb.h 
// Header file for class DsoDb
// Author: S.Binet<binet@cern.ch>
/////////////////////////////////////////////////////////////////// 
#ifndef ATHENAKERNEL_ATH_DSODB_H
#define ATHENAKERNEL_ATH_DSODB_H 1

// STL includes
#include <iosfwd>
#include <string>
#include <vector>
#include <map>
#include <unordered_map>

#include "DataModelRoot/RootType.h"

// Forward declaration

namespace Ath {

/** @brief helper class to query rootmap files and extract their content
 */
class DsoDb
{ 
  /////////////////////////////////////////////////////////////////// 
  // Public typedefs: 
  /////////////////////////////////////////////////////////////////// 
public: 
  typedef std::vector<std::string> Libs_t;
  typedef std::unordered_map<std::string, Libs_t> DsoMap_t;
  //typedef std::map<std::string, Libs_t> DsoMap_t;

  // helper functions for pythonizations
  static
  std::vector<std::string> 
  py_keys_from(const Ath::DsoDb::DsoMap_t& m)
  {
    std::vector<std::string> k;
    k.reserve(m.size());
    for (Ath::DsoDb::DsoMap_t::const_iterator i = m.begin(), e = m.end();
         i != e;
         ++i) {
      k.push_back(i->first);
    }
    return k;
  }
  
  static
  std::vector<std::vector<std::string> >
  py_vals_from(const Ath::DsoDb::DsoMap_t& m)
  {
    std::vector<Libs_t> v;
    v.reserve(m.size());
    for (Ath::DsoDb::DsoMap_t::const_iterator i = m.begin(), e = m.end();
         i != e;
         ++i) {
      v.push_back(i->second);
    }
    return v;
  }
  
  /////////////////////////////////////////////////////////////////// 
  // Public methods: 
  /////////////////////////////////////////////////////////////////// 
 public: 

  /** factory for the DsoDb
   */
  static
  const DsoDb* instance();

  /// Destructor: 
  ~DsoDb(); 

  /////////////////////////////////////////////////////////////////// 
  // Const methods: 
  ///////////////////////////////////////////////////////////////////

  /// list of all dsomap/rootmap files we know about
  const std::vector<std::string>& dso_files() const
  { return m_dsofiles; }

  /// repository of plugin factories
  const DsoMap_t& pf() const { return m_pf; }

  /// repository of components
  const DsoMap_t& db() const { return m_db; }

  /////////////////////////////////////////////////////////////////// 
  // Non-const methods: 
  /////////////////////////////////////////////////////////////////// 

  bool has_type(const std::string& type_name) const;

  std::string load_type(const std::string& type_name) const;

  /// list of reflex-types associated with a library name
  std::vector<std::string> capabilities(const std::string& libname) const;

  /// list of libraries hosting duplicate reflex-types
  DsoMap_t duplicates(const std::string& libname, bool pedantic=false) const;

  /// table of dict-duplicates: {type: [lib1, lib2, ...]}
  DsoMap_t dict_duplicates(bool pedantic=false) const;

  /// table of plugin-factories-duplicates: {type: [lib1, lib2, ...]}
  DsoMap_t pf_duplicates(bool pedantic=false) const;

  /// list of all libraries we know about
  /// @param `detailed` if true, prints the full path to the library
  Libs_t libs(bool detailed=false) const;

  /// return the table {type: [lib1, ...]} - concatenation of all
  /// dict-entries and plugin-factories entries.
  /// @param `pedantic` if true, retrieves the library's full path
  DsoMap_t content(bool pedantic) const;

  /////////////////////////////////////////////////////////////////// 
  // Private data: 
  /////////////////////////////////////////////////////////////////// 
 private: 

  /// initialize the repository of dso file names
  void build_repository();

  /// get the duplicates for a given repository of components
  void
  get_dups(DsoMap_t& dups, const DsoMap_t& db, bool pedantic) const;

  /// load the reflex type after having loaded the hosting library
  RootType rflx_type(const std::string& type_name) const;

  /// Default constructor: 
  DsoDb();

  /// Copy constructor: 
  DsoDb( const DsoDb& rhs ); //> not implemented

  /// Assignment operator: 
  DsoDb& operator=( const DsoDb& rhs ); //> not implemented

  /// repository of plugin factories
  DsoMap_t m_pf;

  /// repository of components
  DsoMap_t m_db;

  /// list of dsofiles
  std::vector<std::string> m_dsofiles;

}; 

/////////////////////////////////////////////////////////////////// 
// Inline methods: 
/////////////////////////////////////////////////////////////////// 
//std::ostream& operator<<( std::ostream& out, const DsoDb& o );

} //> end namespace Ath

#endif //> !ATHENAKERNEL_ATH_DSODB_H
