/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef ROOTCOLLECTION_FILECATALOGMAP_H
#define ROOTCOLLECTION_FILECATALOGMAP_H

#include <map>
#include <string>

namespace pool {
  
  class IFileCatalog;
  
  
  /// Simple registry singelton for instances of FileCatalog used by RootCollection
  class FileCatalogMap {
    
    //! the instance of the singleton
    static FileCatalogMap* s_instance;
    
    //! File catalog lookup map
    std::map<std::string,IFileCatalog*> m_fcMap;
    
    /// undefined, no semantics
    FileCatalogMap (const FileCatalogMap &);
    /// undefined, no semantics
    FileCatalogMap &operator= (const FileCatalogMap &);
    
    //! create and connect a catalog
    IFileCatalog* createCatalog(const std::string& uri);
    
  protected:
    /// invisible constructor (singleton)
    FileCatalogMap(){};
    
  public:
    
    virtual ~FileCatalogMap(){};
    
    /// returns the instance of this singleton
    static FileCatalogMap* instance();
    
    /// If the FileCatalog defined by the given URI is already available in the map,
    /// the catalog from the map is returned, otherwise a new 
    /// catalog is created, connected to the given uri, stored in the 
    /// map  and returnd to the caller.
    IFileCatalog* getFileCatalog( const std::string& uri = "");
    
    /// set the default file catalog (urikey = "")
    /// caution: if there already is a catalog at key ""
    /// it is just replaced, it is not deleted!
    void setDefaultFileCatalog( const std::string& uri = "");
    
    /// set the default file catalog (urikey = "")
    /// caution: if there already is a catalog at key ""
    /// it is just replaced, it is not deleted!
    void setDefaultFileCatalog(IFileCatalog*);
    
  };
}
#endif
