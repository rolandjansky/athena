/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "FileCatalogMap.h"
#include "FileCatalog/IFileCatalog.h"
#include "FileCatalog/FCSystemTools.h"
#include "FileCatalog/URIParser.h"
#include "POOLCore/Exception.h"

using namespace std;

namespace pool {
  
  FileCatalogMap* FileCatalogMap::s_instance = 0;
  
  FileCatalogMap* FileCatalogMap::instance(){
    return s_instance ? s_instance : (s_instance = new FileCatalogMap());
  }
  

  
  IFileCatalog* FileCatalogMap::getFileCatalog(const std::string& uri){
    
    IFileCatalog* fc = m_fcMap[uri];
    
    if(!fc){
      if(uri=="")
        throw pool::Exception( "Default file catalog not available, it has to be set explicitly via FileCatalogMap::setDefaultFileCatalog",
                               "FileCatalogMap::getFileCatalog",
                               "RootCollection" );
      fc = createCatalog(uri);
      
      m_fcMap[uri] = fc;
    }
    
    return fc;
  }
  

  
  void FileCatalogMap::setDefaultFileCatalog( const std::string& uri){
    m_fcMap[""] = getFileCatalog(uri);
  }
  
  
  
  void FileCatalogMap::setDefaultFileCatalog(IFileCatalog* fc){
    m_fcMap[""] = fc;
  }
  

  
  IFileCatalog* FileCatalogMap::createCatalog( const std::string& uri){
    
    URIParser parser(uri);
    parser.parse();
    
    //IFileCatalog* fc = FCSystemTools::createCatalog( parser.prefix() );
    IFileCatalog* fc = new IFileCatalog();
    if (!fc) 
      throw pool::Exception( "cannot create file catalog from URI: "+uri,
                             "FileCatalogMap::getFileCatalog",
                             "ROOTCollection" );
    
    //fc->connect(parser.url());
    
    fc->setWriteCatalog(parser.contactstring());
    //fc->addReadCatalog(parser.contactstring());
    fc->connect();
    
    return fc;
  }
}
