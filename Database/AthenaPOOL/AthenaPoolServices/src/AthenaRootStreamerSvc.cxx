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
      : Service(name, pSvcLocator),
	m_streamerMap()
{
   declareProperty( "Streamers", m_streamerClassNames );
}


AthenaRootStreamerSvc::~AthenaRootStreamerSvc()
{
}


StatusCode AthenaRootStreamerSvc::initialize()
{
   MsgStream log(msgSvc(), name());

   if( !Service::initialize().isSuccess() ) {
      log << MSG::ERROR << "Can not initialize Service base class." << endreq;
      return StatusCode::FAILURE;
   }

   pool::DbSession	session;
   // initialize the session
   // using NULL context, hopefully not a problem
   pool::DbStatus       rc = session.open(0);
   // create ROOT-type database object -
   // this loads the ROOT storage service plugin and
   // initializes POOL/ROOT class loader needed by GetClass()
   if( ! session.db(pool::ROOT_StorageType) ) {
      log << MSG::ERROR << "Failed to open POOL/ROOT session" << endreq;
      return StatusCode::FAILURE;
   }
   log << MSG::INFO << "POOL/ROOT class loader initialized" << endreq;
   session.close();
   
   StatusCode status = StatusCode::SUCCESS;
   if( m_streamerClassNames.value().size() ) {
      log << MSG::INFO << m_streamerClassNames.value().size() << " ROOT streamers declared" <<  endreq;

      std::vector< std::string >::const_iterator i = m_streamerClassNames.value().begin();
      while( i !=  m_streamerClassNames.value().end() ) {
	 log << MSG::INFO << "  - Streamer name:" << *i << endreq;
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
	i !=  m_createdConverters.end(); i++ ) {
#if ROOT_VERSION_CODE < ROOT_VERSION(5,99,0)
      i->Destruct();
// PvG, FIXME: What about deletion fot ROOT 6?
#endif
   }
   m_createdConverters.clear();
   
   // do NOT delete the streamers created by the service - they are owned by TClass
   m_streamerMap.clear();

   return Service::finalize(); 
}


StatusCode AthenaRootStreamerSvc::queryInterface(const InterfaceID& riid, void** ppvInterface)
{
  if ( IAthenaRootStreamerSvc::interfaceID().versionMatch(riid) ) {
     *ppvInterface = dynamic_cast<AthenaRootStreamerSvc*>(this);
   } else {
      // Interface is not directly available: try out a base class
      return(Service::queryInterface(riid, ppvInterface));
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
         log << MSG::ERROR << "Could not find <" << converter_classname
             << "> in the dictionary, and autoloading failed" << endreq;
         return(StatusCode::FAILURE);
      }
      log << MSG::DEBUG << "Loaded dictionary for class " << converter_classname
 //         << " ntypes before load " << ntypesBefore
 //         << " ntypes loaded " << Reflex::Type::TypeSize() - ntypesBefore
          << endreq;
   }
#if ROOT_VERSION_CODE < ROOT_VERSION(5,99,0)
   RootObject convObj = streamer_class.Construct();
#else
   RootObject convObj(streamer_class);
#endif
   m_createdConverters.push_back( convObj );
   return AddStreamer( (T_AthenaRootConverterBase*)convObj.Address(), adopt); 
}


StatusCode AthenaRootStreamerSvc::AddStreamer(T_AthenaRootConverterBase *converter, bool adopt)
{
   MsgStream log(msgSvc(), name());
   const std::string	&classname = converter->ClassName();
   if (m_streamerMap.find(classname) == m_streamerMap.end()) {
      // no streamer for this class yet
      m_streamerMap[classname] = new AthenaRootStreamer(classname, this);
   }
   AthenaRootConverterHandle *convH = new AthenaRootConverterHandle(converter);
   if( m_streamerMap[classname]->AddConverter(convH).isFailure() ) {
      log << MSG::ERROR << "ROOT Streamer for "  << classname
	  << " already has a converter for checksum = "
	  << converter->StreamerChecksum() << endreq;
      return(StatusCode::FAILURE);
   }
   log << MSG::INFO << "ROOT Streamer for " << classname
       << " added converter for checksum = "
       << converter->StreamerChecksum() << endreq;

   if( adopt ) {
      return AdoptStreamerForClass( classname );
   }
   return(StatusCode::SUCCESS);
}



StatusCode AthenaRootStreamerSvc::AdoptStreamerForClass(const std::string& class_name) {
   MsgStream log(msgSvc(), name());
   if (m_streamerMap.find(class_name) == m_streamerMap.end()) {
      log << MSG::ERROR << "Attemt to adopt streamer for "
	      << class_name
	      << " with has no converters defined" << endreq;
      return(StatusCode::FAILURE);
   }
   if (m_streamerMap[class_name]->Adopt().isSuccess()) {
      log << MSG::INFO << "Adopted streamer for class " << class_name << endreq;
      return(StatusCode::SUCCESS);
   }
   log << MSG::ERROR << "Failed to adopt streamer for " << class_name << endreq;
   return(StatusCode::FAILURE);
}



StatusCode AthenaRootStreamerSvc::AdoptAllStreamers()
{
   MsgStream log(msgSvc(), name());
   StatusCode	sc = StatusCode::SUCCESS;
   for(StreamerMap::iterator i = m_streamerMap.begin();
       i != m_streamerMap.end();  i++ ) {
      if( i->second->Adopt().isFailure() ) {
	 log << MSG::INFO << "Failed to adopt streamer for class " << i->first << endreq;
	 sc = StatusCode::FAILURE;
      }
   }
   return sc; 
}


