/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/** @file AthenaRootStreamerSvc.cxx
 *  @brief This file contains the implementation for the AthenaRootStreamerSvc class.
 *  @author Marcin Nowak
 *  $Id: 
 **/

#include "AthenaRootStreamerSvc.h"
#include "AthenaRootStreamer.h"
#include "AthenaRootConverterHandle.h"
#include "AthenaPoolServices/T_AthenaRootConverter.h"

#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/IService.h"

#include "StorageSvc/DbSession.h"
#include "StorageSvc/IOODatabase.h"

#include "DataModelRoot/RootType.h"
#include "TInterpreter.h"

#include "RVersion.h"
#if ROOT_VERSION_CODE < ROOT_VERSION(5,19,0)
using namespace ROOT;
#endif

#include <iostream>
#include <vector>


AthenaRootStreamerSvc::AthenaRootStreamerSvc(const std::string& name, ISvcLocator* pSvcLocator)
      : AthService(name, pSvcLocator),
	m_streamerMap()
{
   declareProperty( "Streamers", m_streamerClassNames );
}


AthenaRootStreamerSvc::~AthenaRootStreamerSvc()
{
}


StatusCode AthenaRootStreamerSvc::initialize()
{
   ATH_CHECK( AthService::initialize() );

   pool::DbSession	session;
   // initialize the session
   // using NULL context, hopefully not a problem
   pool::DbStatus       rc = session.open(0);
   // create ROOT-type database object -
   // this loads the ROOT storage service plugin and
   // initializes POOL/ROOT class loader needed by GetClass()
   if( ! session.db(pool::ROOT_StorageType) ) {
      ATH_MSG_ERROR ( "Failed to open POOL/ROOT session" );
      return StatusCode::FAILURE;
   }
   ATH_MSG_INFO ( "POOL/ROOT class loader initialized" );
   session.close();
   
   StatusCode status = StatusCode::SUCCESS;
   if( m_streamerClassNames.value().size() ) {
      ATH_MSG_INFO ( m_streamerClassNames.value().size() << " ROOT streamers declared" );

      std::vector< std::string >::const_iterator i = m_streamerClassNames.value().begin();
      while( i !=  m_streamerClassNames.value().end() ) {
         ATH_MSG_INFO ( "  - Streamer name:" << *i );
	 if( AddStreamer( *i++ ).isFailure() )
	    status = StatusCode::FAILURE; 
      }
   }

   return status;
}


StatusCode AthenaRootStreamerSvc::finalize()
{
   // Destroy converter Objects created by the service
   for( ConverterVector_t::iterator i = m_createdConverters.begin();
	i !=  m_createdConverters.end(); i++ )
   {
     i->first.Destruct (i->second);
   }
   m_createdConverters.clear();
   
   // do NOT delete the streamers created by the service - they are owned by TClass
   m_streamerMap.clear();

   return AthService::finalize(); 
}


StatusCode AthenaRootStreamerSvc::queryInterface(const InterfaceID& riid, void** ppvInterface)
{
  if ( IAthenaRootStreamerSvc::interfaceID().versionMatch(riid) ) {
     *ppvInterface = dynamic_cast<AthenaRootStreamerSvc*>(this);
   } else {
      // Interface is not directly available: try out a base class
      return(AthService::queryInterface(riid, ppvInterface));
   }
   addRef();
   return(StatusCode::SUCCESS);
}


const InterfaceID& AthenaRootStreamerSvc::type() const
{ 
    return IAthenaRootStreamerSvc::interfaceID();
}


StatusCode AthenaRootStreamerSvc::AddStreamer(const std::string& converter_classname, bool adopt)
{
   RootType streamer_class( converter_classname );
   if( !streamer_class ) {
      // Class not yet known to Reflex
      // try autoloading
      static bool first = true;
      if (first) {
         gInterpreter->EnableAutoLoading();
         first = false;
      }
      // int ntypesBefore = Reflex::Type::TypeSize();
      gInterpreter->AutoLoad( converter_classname.c_str() );

      MsgStream log(msgSvc(), name());
      streamer_class = RootType( converter_classname );
      if( ! streamer_class ) {
         ATH_MSG_ERROR ( "Could not find <" << converter_classname
                         << "> in the dictionary, and autoloading failed" );
         return(StatusCode::FAILURE);
      }
      log << MSG::DEBUG << "Loaded dictionary for class " << converter_classname
 //         << " ntypes before load " << ntypesBefore
 //         << " ntypes loaded " << Reflex::Type::TypeSize() - ntypesBefore
          << endmsg;
   }
   void* obj = streamer_class.Construct();
   m_createdConverters.emplace_back( streamer_class, obj );
   return AddStreamer( (T_AthenaRootConverterBase*)obj, adopt); 
}


StatusCode AthenaRootStreamerSvc::AddStreamer(T_AthenaRootConverterBase *converter, bool adopt)
{
   const std::string	&classname = converter->ClassName();
   if (m_streamerMap.find(classname) == m_streamerMap.end()) {
      // no streamer for this class yet
      m_streamerMap[classname] = new AthenaRootStreamer(classname, this);
   }
   AthenaRootConverterHandle *convH = new AthenaRootConverterHandle(converter);
   if( m_streamerMap[classname]->AddConverter(convH).isFailure() ) {
      ATH_MSG_ERROR ( "ROOT Streamer for "  << classname
                      << " already has a converter for checksum = "
                      << converter->StreamerChecksum() );
      return(StatusCode::FAILURE);
   }
   ATH_MSG_INFO ( "ROOT Streamer for " << classname
                  << " added converter for checksum = "
                  << converter->StreamerChecksum() );

   if( adopt ) {
      return AdoptStreamerForClass( classname );
   }
   return(StatusCode::SUCCESS);
}



StatusCode AthenaRootStreamerSvc::AdoptStreamerForClass(const std::string& class_name) {
   if (m_streamerMap.find(class_name) == m_streamerMap.end()) {
      ATH_MSG_ERROR ( "Attemt to adopt streamer for "
                      << class_name
                      << " with has no converters defined" );
      return(StatusCode::FAILURE);
   }
   if (m_streamerMap[class_name]->Adopt().isSuccess()) {
     ATH_MSG_INFO ( "Adopted streamer for class " << class_name );
     return(StatusCode::SUCCESS);
   }
   ATH_MSG_ERROR ( "Failed to adopt streamer for " << class_name );
   return(StatusCode::FAILURE);
}



StatusCode AthenaRootStreamerSvc::AdoptAllStreamers()
{
   StatusCode	sc = StatusCode::SUCCESS;
   for(StreamerMap::iterator i = m_streamerMap.begin();
       i != m_streamerMap.end();  i++ ) {
      if( i->second->Adopt().isFailure() ) {
         ATH_MSG_INFO ( "Failed to adopt streamer for class " << i->first );
	 sc = StatusCode::FAILURE;
      }
   }
   return sc; 
}


