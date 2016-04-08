/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include <sstream>
#include "CoralBase/MessageStream.h"

#include "FileCatalog/FCImpl.h"
#include "FileCatalog/URIParser.h"
#include "FileCatalog/FCMetaImpl.h"
#include "FileCatalog/FCException.h"

#ifdef HAVE_GAUDI_PLUGINSVC
#include "Gaudi/PluginService.h"
#else
#include "Reflex/PluginService.h"
#endif
#include "GAUDI_VERSION.h"

namespace pool{
  FCImpl* FCImplBuilder::BuildFCImpl(const std::string& contact){
    FCImpl* fcimpl=0;
    URIParser p(contact);
    std::string uriprefix,url;
    try{
      p.parse();
      uriprefix=p.prefix();
      url=p.url();

      std::string comp_name = uriprefix;
      if( uriprefix.empty() || uriprefix=="XMLFileCatalog" )
         comp_name = "xmlcatalog";
#ifdef HAVE_GAUDI_PLUGINSVC
  #if GAUDI_VERSION > CALC_GAUDI_VERSION(25, 3) 
      fcimpl = Gaudi::PluginService::Factory< FCImpl* >::create(comp_name);
  #else  
	  fcimpl = Gaudi::PluginService::Factory0< FCImpl* >::create(comp_name);
  #endif
#else
      fcimpl = Reflex::PluginService::CreateWithId< FCImpl* >(comp_name);
#endif

      if(fcimpl==0) {
        std::stringstream msg;
        if ( uriprefix == "" )
          msg << "Failed to load file catalog plugin (XMLCatalog) for url='" << url << "'";
        else
          msg << "Failed to load file catalog plugin for url='" << url 
              << "' and uriprefix='" << uriprefix << "'";
        throw pool::Exception( msg.str(), "FCImplBuilder::BuildFCImpl", "FileCatalog" );
      }      
      fcimpl->setURL(url);
      fcimpl->setType(uriprefix);
      return fcimpl;
    }catch( ... ){
      throw;
    }
  }

  FCMetaImpl* FCImplBuilder::BuildFCMetaImpl( FCImpl* fc){
    coral::MessageStream fclog( "FCImplBuilder::BuildFCMetaImpl");
    FCMetaImpl* metaimpl=0;
    try{
      std::string mytype=fc->implType();
      std::string comp_name = mytype + "_meta";
      if( mytype.empty() || mytype=="XMLFileCatalog" )
         comp_name = "xmlcatalog_meta";      
#ifdef HAVE_GAUDI_PLUGINSVC
  #if GAUDI_VERSION > CALC_GAUDI_VERSION(25, 3) 
      metaimpl = Gaudi::PluginService::Factory< FCMetaImpl*, FCImpl* >::create(comp_name, fc);
  #else  
      metaimpl = Gaudi::PluginService::Factory1< FCMetaImpl*, FCImpl* >::create(comp_name, fc);
  #endif
#else
      metaimpl = Reflex::PluginService::CreateWithId< FCMetaImpl* >(comp_name, fc);
#endif
      if(!metaimpl){
        fclog<<coral::Info<<"no metadata catalog plugin found for "<<mytype<<coral::MessageStream::endmsg;
      }
    }catch(...){
      throw;
    }
    return metaimpl;
  }
}//ns pool
