/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef PackageLoader_H
#define PackageLoader_H
//

#include<string>

#include<map>

namespace FADS {

class PackageLoader {

public:
  // default constructor
  PackageLoader(){}

  // construct and load
  explicit PackageLoader(const std::string & packlist, const std::string& version=getDefaultVersion()) {
    (*this)(packlist,version);
  }

  // load
  bool operator()(const std::string & packlist, const std::string& version=getDefaultVersion()) const;

  bool unload(const std::string & packlist);

public:
  ///
  static void confirm(const std::string & pack, const std::string & version="Unknown");
  ///
  static void setDefaultVersion(const char * version);
  ///
  static void setDefaultVersion(const std::string & version);

  static const std::string &  getDefaultVersion();

  static void dump(std::ostream & co);

private:

  static std::string defaultVersion; 

public:
  typedef std::pair<std::string,void *> Elem;
  typedef std::map<std::string, Elem, std::less<std::string> > Dict;
  typedef Dict::iterator DI;
  typedef Dict::const_iterator CDI;

private:
  /// return a static dict...
  static Dict & loaded();


};

}	// end namespace

#endif
