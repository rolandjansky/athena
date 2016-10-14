/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include <sstream>

#include "FileCatalog/FCImpl.h"
#include "FileCatalog/URIParser.h"
#include "FileCatalog/FCMetaImpl.h"
#include "FileCatalog/FCException.h"

#include "Gaudi/PluginService.h"

namespace pool
{

  FCImpl* FCImplBuilder::BuildFCImpl(const std::string& contact)
  {
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

      fcimpl = Gaudi::PluginService::Factory< FCImpl* >::create(comp_name);
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

   
  FCMetaImpl* FCImplBuilder::BuildFCMetaImpl( FCImpl* fc)
  {
    FCMetaImpl* metaimpl=0;
    try {
      std::string mytype=fc->implType();
      std::string comp_name = mytype + "_meta";
      if( mytype.empty() || mytype=="XMLFileCatalog" )
         comp_name = "xmlcatalog_meta";      

      metaimpl = Gaudi::PluginService::Factory< FCMetaImpl*, FCImpl* >::create(comp_name, fc);
      if(!metaimpl) {
         std::stringstream msg;
         msg << "no metadata catalog plugin found for "<<mytype;
         throw pool::Exception( msg.str(), "FCImplBuilder::BuildFCMetaImpl", "FileCatalog" );
      }
    }catch(...){
      throw;
    }
    return metaimpl;
  }
}//ns pool
