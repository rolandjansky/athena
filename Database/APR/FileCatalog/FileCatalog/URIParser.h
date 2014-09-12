/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

//$Id: URIParser.h 457829 2011-09-08 13:13:25Z mnowak $

#ifndef POOL_URIPARSER_H
#define  POOL_URIPARSER_H

#include <string>
#include "FileCatalog/FCException.h"
namespace pool{
  /**@class URIParser URIParser.h pool/URIParser.h
     URI parsing utility class.
     The URI string can be read from environment variable POOL_CATALOG 
     or from the URIParser constructor. 
     Protocol, host, path MUST be specified. 
     @Author Zhen Xie
  */
  
  class URIParser{
  public:
    /** Constructor. Contact string will be read from the environment variable
	POOL_CATALOG if no argument is given to the constructor.
	@param contactstr the contact string to the catalog
    */
    URIParser(const std::string& contactstr);
    ///Default constructor
    URIParser();
    ///Destructor
    ~URIParser();
    ///Print the parsing result
    void dump() const;
    void setURI(const std::string& contactstr);
    const std::string contactstring() const;
    const std::string prefix() const;
    const std::string url() const;
    void parse();
  private:    
    std::string m_contactstr;
    std::string m_prefix;
    std::string m_url;
  };
}/* ns pool */
#endif




