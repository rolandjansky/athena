/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef POOL_XMLMETADATACATALOG_H
#define POOL_XMLMETADATACATALOG_H
#include "FileCatalog/FCMetaImpl.h"
#include "FileCatalog/FCBuf.h"

#ifdef HAVE_GAUDI_PLUGINSVC
#include "Gaudi/PluginService.h"
#endif
#include "GAUDI_VERSION.h"

namespace pool {
class FCImpl;
class MetaDataEntry;
class PoolXMLFileCatalog;
class XMLFileCatalog;
class XMLMetaDataCatalog: public FCMetaImpl{
 public:
#ifdef HAVE_GAUDI_PLUGINSVC
  #if GAUDI_VERSION > CALC_GAUDI_VERSION(25, 3) 
    typedef Gaudi::PluginService::Factory<FCMetaImpl*, FCImpl*> Factory;
  #else  
    typedef Gaudi::PluginService::Factory1<FCMetaImpl*, FCImpl*> Factory;
  #endif
#endif

  /// Defsult Constructor
  XMLMetaDataCatalog( FCImpl* fc );
  /// Virtual Destructor
  virtual ~XMLMetaDataCatalog();
  
  virtual void insertMetaData(MetaDataEntry& mentry) const;
  
  /**create meta data definition in the catalog
     @param schemadef [IN] schema definition.
  */
  virtual void createMetaDataSpec(MetaDataEntry& schemadef) const;
  
  /**update meta data definition in the catalog
     @param schemadef [IN] schema definition to add
  */
  virtual void updateMetaDataSpec(MetaDataEntry& newschemadef,
				  const FileCatalog::MetaUpdateMode metamode) const;
  
  /**get the metadata schema of the catalog
     @param spec [OUT] catalog meta data schema
  */
  virtual void getMetaDataSpec(MetaDataEntry& spec) const;
  
  /**drop the meta data definition from the catalog
   */
  virtual void dropMetaDataSpec() const;
  
  virtual void deleteMetaData(const FileCatalog::FileID& guid) const;
  
  
  virtual bool retrievePFN(const std::string& query, 
			   FCBuf<PFNEntry>& buf, 
			   const size_t& start );

  virtual bool retrieveLFN(const std::string& query, 
			   FCBuf<LFNEntry>& buf, 
			   const size_t& start );

  virtual bool retrieveMetaData(const std::string& query, 
				FCBuf<MetaDataEntry>& buf, 
				const size_t& start );
  virtual bool retrieveGuid(const std::string& query, 
			    FCBuf<FileCatalog::FileID>& buf, 
			    const size_t& start );

 private:
  mutable XMLFileCatalog* x_fc;
  mutable PoolXMLFileCatalog* XMLcon;
  

};
}
#endif
