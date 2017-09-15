///////////////////////// -*- C++ -*- /////////////////////////////

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// DsoDb.cxx 
// Implementation file for class DsoDb
// Author: S.Binet<binet@cern.ch>
/////////////////////////////////////////////////////////////////// 

// AthenaKernel includes
#include "AthenaKernel/DsoDb.h"

//#define ATH_DSODB_VERBOSE

// STL includes
#include <cstdlib>    /* getenv */
#include <algorithm>
#include <iostream>
#include <fstream>
#include <set>

// boost includes
#include "boost/filesystem.hpp"
#include "boost/filesystem/path.hpp"
#include "boost/filesystem/exception.hpp" /*filesystem_error*/
#include "boost/tokenizer.hpp"

#include "boost/algorithm/string.hpp"
//#include "boost/algorithm/string/predicate.hpp"
//#include "boost/algorithm/string/replace.hpp"

#include <boost/regex.hpp>

// ROOT includes
#include "TClassEdit.h"

// fwk includes
#include "GaudiKernel/System.h"

namespace fs = boost::filesystem;

namespace {
  typedef std::vector<std::string> Strings_t;
  const std::string RootMap = ".rootmap";
  const std::string DsoMap  = ".dsomap";
  const std::string Components = ".components";
  const std::string PluginNs= "__pf__";

#ifdef _WIN32
  const std::string SHLIB_PREFIX = "lib";
  const std::string SHLIB_SUFFIX = ".dll";
#else
  const std::string SHLIB_PREFIX = "lib";
  const std::string SHLIB_SUFFIX = ".so";
#endif


  bool is_rootcint_dict(const std::string& libname)
  {
    if (libname == "liblistDict.so") return true;
    if (libname == "libmapDict.so") return true;
    if (libname == "libmap2Dict.so") return true;
    if (libname == "libmultimapDict.so") return true;
    if (libname == "libsetDict.so") return true;
    if (libname == "libvectorDict.so") return true;
    if (libname == "libCore.so") return true;
    if (libname == "libMathCore.so") return true;

    static const std::string dll = ".dll";
    if (libname == dll) {
      return false;
    }
    static const boost::regex e("\\w*?.dll");
    return !boost::algorithm::starts_with(libname, SHLIB_PREFIX) &&
            boost::regex_match(libname, e);
  }

  inline
  std::string to_string(const fs::path& p) 
  {
#if BOOST_FILESYSTEM_VERSION == 3 
    return p.c_str();
#else
    return p.native_file_string();
#endif
  }

  std::string getlibname(const std::string& libname)
  {
    std::string lib = libname;
    if (!boost::algorithm::starts_with(lib, "lib")) {
      lib = std::string("lib") + lib;
    }
    if (!boost::algorithm::ends_with(lib, SHLIB_SUFFIX)) {
      lib = lib + SHLIB_SUFFIX;
    }
    return lib;
  }

  const std::set<std::string>& 
  s_cxx_builtins()
  {
    static const std::set<std::string> s = {
      "char",
      "unsigned char",
      "signed char",
      
      "signed",
      
      "short int",
      "short signed",
      "short signed int",
      
      "short",
      "unsigned short",
      "signed short",

      "int",
      "unsigned int",

      "long int",
      "long signed int",
      "signed long int",
      
      "long",
      "long signed",
      "signed long",
      "unsigned long",
      "unsigned long int",

      "long long",
      "long long int",
      "unsigned long long",
      "longlong",
      
      "ulonglong",

      "float",
      "double",
      "long double",
      "bool",
    };
    return s;
  }

  const std::map<std::string, std::string>&
  s_cxx_aliases()
  {
    static const std::map<std::string, std::string> s =
      {{"ElementLinkInt_p1",             "ElementLink_p1<unsigned int>"},
       {"basic_string<char>",            "string"},
       {"vector<basic_string<char> >",   "vector<string>"},
       {"INavigable4MomentumCollection", "DataVector<INavigable4Momentum>"},
       {"IParticleContainer",            "DataVector<IParticle>"},
      };
    return s;
  }

  const std::map<std::string, std::string>&
  s_cxx_typedefs()
  {
    static const std::map<std::string, std::string> s =
      {{"INavigable4MomentumCollection", "DataVector<INavigable4Momentum>"},
       {"IParticleContainer",            "DataVector<IParticle>"},
      };
    return s;
  }

  /** @brief helper method to massage a typename into something
   *         understandable by rootmap files
   */
  std::string 
  to_rootmap_name(const std::string& type_name_)
  {
    std::string type_name = type_name_;
    boost::algorithm::replace_all(type_name, ", ", ",");
    // first, the easy case: builtins
    if (s_cxx_builtins().find(type_name) != s_cxx_builtins().end()) {
      return type_name;
    }

    // known missing aliases ?
    if (s_cxx_aliases().find(type_name) != s_cxx_aliases().end()) {
      return ::to_rootmap_name(s_cxx_aliases().find(type_name)->second);
    }
   
    type_name = TClassEdit::ShortType(type_name.c_str(), 
                                      TClassEdit::kDropDefaultAlloc|
                                      TClassEdit::kDropStd);
    boost::algorithm::replace_all(type_name, "basic_string<char> >", "string>");
    boost::algorithm::replace_all(type_name, "basic_string<char>", "string");
    return type_name;
  }

  /** @brief helper function to massage a typename into something
   *         understandable by Reflex.
   */
  std::string
  to_rflx_name(const std::string& type_name_)
  {
    std::string type_name = type_name_;
    boost::algorithm::replace_all(type_name, ", ", ",");
    // first the easy case: builtins
    if (s_cxx_builtins().find(type_name) != s_cxx_builtins().end()) {
      return type_name;
    }

    // known missing typedefs ?
    if (s_cxx_typedefs().find(type_name) != s_cxx_typedefs().end()) {
      return ::to_rflx_name(s_cxx_typedefs().find(type_name)->second);
    }
    type_name = TClassEdit::ShortType(type_name.c_str(),
                                      TClassEdit::kDropDefaultAlloc);
    // !! order matters !! (at least in C++03. C++11 should be good)
    boost::algorithm::replace_all(type_name, 
                                  "std::string>", 
                                  "std::basic_string<char> >");
    boost::algorithm::replace_all(type_name, 
                                  "std::string", 
                                  "std::basic_string<char>");
    return type_name;
  }
}

namespace Ath {

/////////////////////////////////////////////////////////////////// 
// Public methods: 
/////////////////////////////////////////////////////////////////// 

/** factory for the DsoDb
 */
const DsoDb* DsoDb::instance()
{
   static const DsoDb db;
   return &db;
}

// Constructors
////////////////
DsoDb::DsoDb() :
  m_pf(),
  m_db(),
  m_dsofiles()
{
  build_repository();
}

// Destructor
///////////////
DsoDb::~DsoDb()
{}

/////////////////////////////////////////////////////////////////// 
// Const methods: 
///////////////////////////////////////////////////////////////////

/////////////////////////////////////////////////////////////////// 
// Non-const methods: 
/////////////////////////////////////////////////////////////////// 

bool
DsoDb::has_type(const std::string& type_name) const
{
  if (s_cxx_builtins().find(type_name) != s_cxx_builtins().end()) {
    return true;
  }
  const std::string n = ::to_rootmap_name(type_name);
  if ( m_db.find(n) != m_db.end() ||
       m_pf.find(n) != m_pf.end() ) {
    return true;
  }
  return false;
}

std::string
DsoDb::load_type ATLAS_NOT_THREAD_SAFE (const std::string& type_name) const
{
  RootType t = this->rflx_type(type_name);
  if (t.Id()) {
    return t.Name(Reflex::SCOPED|Reflex::QUALIFIED);
  }
  return std::string();
}

/////////////////////////////////////////////////////////////////// 
// Protected methods: 
/////////////////////////////////////////////////////////////////// 

/// initialize the repository of dso file names
void 
DsoDb::build_repository()
{
 // std::cerr << "::build_repository...\n";

  typedef boost::tokenizer<boost::char_separator<char> > Tokenizer_t;
  typedef std::vector<fs::path> Paths_t;

  std::set<std::string> dsofiles;

  std::string dsopath = System::getEnv("LD_LIBRARY_PATH");
  std::string rootsys = System::getEnv("ROOTSYS");

  Tokenizer_t tok(dsopath, boost::char_separator<char>(":"));

  for (Tokenizer_t::iterator itr = tok.begin(), iend = tok.end();
       itr != iend;
       ++itr) {
    //std::cerr << "--[" << *itr << "]...\n";
    if (boost::algorithm::starts_with(*itr, rootsys)) {
      continue;
    }
    fs::path p(*itr);
    if (!boost::filesystem::exists(*itr)) {
      continue;
    }
    Paths_t dir_content;
    std::copy(fs::directory_iterator(p), 
              fs::directory_iterator(), 
              std::back_inserter(dir_content));
    std::sort(dir_content.begin(),
              dir_content.end());
    for (Paths_t::iterator ipath = dir_content.begin(),
           ipath_end = dir_content.end();
         ipath != ipath_end;
         ++ipath) {
      const fs::path& dsomap = *ipath;

      bool is_components = false;
      if (dsomap.extension() == Components)
        is_components = true;
      else if (dsomap.extension() != RootMap)
        continue;

      if (!fs::exists(dsomap)) continue;

      //std::cerr << "=== [" << dso << "] ===\n";
#if BOOST_FILESYSTEM_VERSION == 3 
      dsofiles.insert(dsomap.c_str());
      std::ifstream f(dsomap.c_str());
#else
      dsofiles.insert(dsomap.native_file_string().c_str());
      std::ifstream f(dsomap.native_file_string().c_str());
#endif
      int line_nbr = -1;
      std::string lastlib;
      while (f) {
        line_nbr += 1;
        std::string line;
        std::getline(f, line);
        boost::algorithm::trim(line);

        std::string libname;
        std::string dso_key;
        if ( line.empty() )
          continue;

        else if (line.substr (0, 8) == "Library.") {
          Strings_t ll;
          boost::algorithm::split(ll, line, 
                                  boost::is_any_of(" "),
                                  boost::token_compress_on);
          if (ll.size() < 2) {
            std::cerr << "DsoDb:: **error** could not parse " 
                      << dsomap << ":" << line_nbr
                      << "\n"
                      << "DsoDb:: (some) reflex-dicts may fail to be autoloaded"
                      << "\n";
            continue;
          }
          libname = ll[1];
          if (::is_rootcint_dict(libname)) {
            continue;
          }
          dso_key = ll[0];
          libname = ::getlibname(libname);
          boost::algorithm::replace_all(dso_key, "Library.", "");
          boost::algorithm::replace_all(dso_key, ":", "");
          boost::algorithm::replace_all(dso_key, "@", ":");
          boost::algorithm::replace_all(dso_key, "-", " ");
        }

        else if (line[0] == '[') {
          libname = line.substr (1, line.size()-2);
          boost::algorithm::trim (libname);
          if (::is_rootcint_dict(libname)) {
            lastlib.clear();
            continue;
          }
          if (!libname.empty())
            lastlib = ::getlibname (libname);
          continue;
        }

        else if (line.substr(0, 8) == "# --End ") {
           lastlib.clear();
           continue;
        }

        else if (line.substr(0, 6) == "class ") {
          libname = lastlib;
          line.erase (0, 6);
          dso_key = line;
        }

        else if (is_components && line.substr(0, 3) == "lib") {
          std::string::size_type pos = line.find (':');
          if (pos == std::string::npos) continue;
          libname = line.substr(0, pos);
          line.erase (0, pos+1);
          dso_key = line;

          if (dso_key.substr(0, 6) == "_PERS_" ||
              dso_key.substr(0, 7) == "_TRANS_")
            continue;

          if (libname.find ("AthenaPoolPoolCnv") != std::string::npos)
            continue;
        }

        if (libname.empty() || dso_key.empty()) continue;

        const std::string fullpath_libname = to_string(dsomap.parent_path() / fs::path(libname));

        // std::cerr << " [" << line << "] -> [" << dso_key << "] [" << libname << "]\n";

        DsoMap_t *db = NULL;
        if (boost::algorithm::starts_with(dso_key, PluginNs) || is_components) {
          db = &m_pf;
        } else {
          db = &m_db;
        }
        if (db->find(dso_key) == db->end()) {
          db->insert(std::make_pair(dso_key, Strings_t()));
        }
        (*db)[dso_key].push_back(fullpath_libname);
      }
      // std::cerr << "=== [" << dso << "] === [EOF]\n";
    }
    //std::cerr << "--[" << *itr << "]... [done]\n";
  }
  
  m_dsofiles.reserve(dsofiles.size());
  std::copy(dsofiles.begin(), dsofiles.end(), std::back_inserter(m_dsofiles));

  // std::cerr << "::build_repository... [done]\n";
  return;
}

std::vector<std::string>
DsoDb::capabilities(const std::string& libname_) const
{
  fs::path p(libname_);

  const std::string libname = ::getlibname(to_string(p.filename()));
  std::set<std::string> caps;
  const DsoMap_t* dbs[] = { &m_pf, &m_db };

  for (std::size_t i = 0, imax = 2; i < imax; ++i) {
    const DsoMap_t* db = dbs[i];
    for (DsoMap_t::const_iterator idb = db->begin(), iend=db->end();
         idb != iend;
         ++idb) {
      for (Libs_t::const_iterator ilib = idb->second.begin(),
             ilibend = idb->second.end();
           ilib != ilibend;
           ++ilib) {
        fs::path lib(*ilib);
        if (to_string(lib.filename()) == libname) {
          caps.insert(idb->first);
        }
      }
    }
  }

  return std::vector<std::string>(caps.begin(), caps.end());
}

/// list of libraries hosting duplicate reflex-types
DsoDb::DsoMap_t 
DsoDb::duplicates(const std::string& libname, bool pedantic) const
{
  DsoMap_t dups;
  const std::string basename_lib = to_string(fs::path(libname).filename());
  std::vector<std::string> caps = this->capabilities(libname);
  const DsoMap_t* dbs[] = { &m_pf, &m_db };

  for (std::size_t i = 0, imax = 2; i < imax; ++i) {
    DsoMap_t dup_db;
    this->get_dups(dup_db, *dbs[i], pedantic);
    for (DsoMap_t::const_iterator 
           idb = dup_db.begin(), 
           idbend = dup_db.end();
         idb != idbend;
         ++idb) {
      if (std::find(caps.begin(), caps.end(), idb->first) != caps.end()) {
        for (Libs_t::const_iterator 
               ilib = idb->second.begin(),
               ilibend=idb->second.end();
             ilib != ilibend;
             ++ilib) {
          fs::path p(*ilib);
          const std::string basename = to_string(p.filename());
          if (basename != libname) {
            dups[idb->first].push_back(*ilib);
          }
        }
      }
    }
  }  
  return dups;
}

/// table of dict-duplicates: {type: [lib1, lib2, ...]}
DsoDb::DsoMap_t 
DsoDb::dict_duplicates(bool pedantic) const
{
  DsoMap_t dups;
  get_dups(dups, m_db, pedantic);
  return dups;
}

/// table of plugin-factories-duplicates: {type: [lib1, lib2, ...]}
DsoDb::DsoMap_t 
DsoDb::pf_duplicates(bool pedantic) const
{
  DsoMap_t dups;
  get_dups(dups, m_pf, pedantic);
  return dups;
}

/// list of all libraries we know about
/// @param `detailed` if true, prints the full path to the library
DsoDb::Libs_t 
DsoDb::libs(bool detailed) const
{
  std::set<std::string> libs;
  const DsoMap_t* dbs[] = { &m_pf, &m_db };

  for (std::size_t i = 0, imax = 2; i < imax; ++i) {
    const DsoMap_t& db = *dbs[i];
    for (DsoMap_t::const_iterator idb = db.begin(), iend=db.end();
         idb != iend;
         ++idb) {
      for (Libs_t::const_iterator 
             ilib = idb->second.begin(),
             ilibend=idb->second.end();
           ilib != ilibend;
           ++ilib) {
        if (detailed) {
          libs.insert(*ilib);
        } else {
          libs.insert(to_string(fs::path(*ilib).filename()));
        }
      }
    }
  }
  return Libs_t(libs.begin(), libs.end());
}

/// return the table {type: [lib1, ...]} - concatenation of all
/// dict-entries and plugin-factories entries.
/// @param `pedantic` if true, retrieves the library's full path
DsoDb::DsoMap_t 
DsoDb::content(bool pedantic) const
{
  DsoMap_t db;

  const DsoMap_t* dbs[] = { &m_pf, &m_db };
  for (std::size_t i = 0, imax = 2; i < imax; ++i) {
    const DsoMap_t& d = *dbs[i];
    for (DsoMap_t::const_iterator idb = d.begin(), idbend=d.end();
         idb != idbend;
         ++idb) {
      if (pedantic) {
        db[idb->first] = idb->second;
      } else {
        Libs_t libs;
        std::set<std::string> baselibs;
        for (Libs_t::const_iterator 
               ilib = idb->second.begin(),
               ilibend= idb->second.end();
             ilib != ilibend;
             ++ilib) {
          const std::string baselib = to_string(fs::path(*ilib).filename());
          if (baselibs.find(baselib) == baselibs.end()) {
            libs.push_back(*ilib);
            baselibs.insert(baselib);
          }
        }
        db[idb->first] = libs;
      }
    }
  }

  return db;
}

/// get the duplicates for a given repository of components
void 
DsoDb::get_dups(DsoMap_t& dups, const DsoMap_t& db, bool pedantic) const
{
  for (DsoMap_t::const_iterator idb = db.begin(), iend = db.end();
       idb != iend;
       ++idb) {
    if (idb->second.size() == 1) {
      continue;
    }
    Libs_t libs;
    if (pedantic) {
      libs = idb->second;
    } else {
      std::set<std::string> baselibs;
      for (Libs_t::const_iterator 
             ilib = idb->second.begin(), 
             ilend= idb->second.end();
           ilib != ilend;
           ++ilib) {
        fs::path p(*ilib);
        const std::string baselib = to_string(p.filename());
        if (baselibs.find(baselib) == baselibs.end()) {
          baselibs.insert(baselib);
          libs.push_back(*ilib);
        }
      }
    }
    if (libs.size() > 1) {
      dups[idb->first] = Libs_t(libs.begin(), libs.end());
    }
  }
}

/// load the reflex type after having loaded the hosting library
RootType 
DsoDb::rflx_type ATLAS_NOT_THREAD_SAFE (const std::string& type_name) const
{
  const std::string rootmap_name = ::to_rootmap_name(type_name);
  const std::string rflx_name = ::to_rflx_name(type_name);

  // std::cerr << "---DsoDb::rflx_type---\n"
  //           << " tname: [" << type_name << "]\n"
  //           << " root:  [" << rootmap_name << "]\n"
  //           << " rflx:  [" << rflx_name << "]\n"
  //           << "----------------------\n";

  if (s_cxx_builtins().find(type_name) != s_cxx_builtins().end()) {
    return RootType(rflx_name);
  }

  if (!this->has_type(rootmap_name)) {
#ifdef ATH_DSODB_VERBOSE
    std::cerr << "DsoDb **error**: no such type [" << rootmap_name << "]"
              << " in rootmap files\n";
#endif
    return RootType();
  }
  DsoMap_t::const_iterator idb = m_db.find(rootmap_name);
  if (idb == m_db.end()) {
    // try plugin factories...
    idb = m_pf.find(rootmap_name);
    if (idb == m_pf.end()) {
      return RootType();
    }
  }
  const Libs_t& libs = idb->second;
  if (libs.empty()) {
#ifdef ATH_DSODB_VERBOSE
    std::cerr << "DsoDb **error**: no library hosting [" << type_name << "]\n";
#endif
    return RootType();
  }

  System::ImageHandle handle;
  std::string libname = ::to_string(fs::path(libs[0]).filename());
  boost::algorithm::trim(libname);
  if (boost::algorithm::starts_with(libname, SHLIB_PREFIX)) {
    libname = libname.substr(SHLIB_PREFIX.size(), std::string::npos);
  }
  if (boost::algorithm::ends_with(libname, SHLIB_SUFFIX)) {
    libname = libname.substr(0, libname.size()-SHLIB_SUFFIX.size());
  }

  unsigned long err = System::loadDynamicLib( libname, &handle );
  if ( err != 1 ) {
    std::cerr << "DsoDb **error**: could not load library [" 
              << libs[0] << "] (" << System::getLastErrorString() 
              << ")\n";
    return RootType();
  }

  return RootType(rflx_name);
}

} //> end namespace Ath
