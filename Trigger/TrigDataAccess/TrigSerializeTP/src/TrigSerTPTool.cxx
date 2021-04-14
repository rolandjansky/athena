/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

// $Id$

// ROOT include(s):
#include <TClass.h>

// Gaudi/Athena include(s):
#include "GaudiKernel/System.h"
#include "AthenaKernel/IDictLoaderSvc.h"
#include "AthenaKernel/ITPCnvBase.h"
#include "AthenaKernel/errorcheck.h"
#include "CxxUtils/AthUnlikelyMacros.h"

// Local include(s):
#include "TrigSerializeTP/TrigSerTPTool.h"


TrigSerTPTool::TrigSerTPTool( const std::string& type,
                              const std::string& name,
                              const IInterface* parent) :
   AthAlgTool( type, name, parent ),
   m_msgsvcTP( "", name ),
   m_dictSvc( "AthDictLoaderSvc", name ),
   m_useAthDictLoader( true ),
   m_online( false ),
   m_tpcnvsvc( "AthTPCnvSvc", name ) {

   declareInterface< TrigSerTPTool >( this );

   declareProperty( "TPMap", m_TPmap, "map of T->P classes" );
   declareProperty( "ActiveClasses", m_activeClasses,
                    "allows to skip classes not relevant in the current "
                    "trigger level" );
   declareProperty( "debuggingMsgSvc", m_msgsvcTP,
                    "an instance of the messageSvc for debugging purposes" );
   declareProperty( "onlineMode", m_online,
                    "simpler setup for running online" );
   declareProperty( "useAthDictLoader", m_useAthDictLoader,
                    "use AthDictLoaderSvc instead of plain ROOT" );
}

StatusCode TrigSerTPTool::initialize(){

   // Greet the user:
   ATH_MSG_INFO( "Initializing" );

   // Retrieve a custom message service:
   if( ! m_msgsvcTP.empty() ) {
      if( m_msgsvcTP.retrieve().isFailure() ) {
         ATH_MSG_INFO( "Could not retrieve a separate MsgSvc for "
                       "the T/P debugging" );
      } else {
         ATH_MSG_DEBUG( "Using " << m_msgsvcTP << " for debugging" );
         IMessageSvc* msvc = m_msgsvcTP.operator->();
         m_logTP = std::make_unique< MsgStream >( msvc,
                                                  "TrigSerTPTool-T/P" );
      }
   }

   // Retrieve the used services:
   ATH_CHECK( m_dictSvc.retrieve() );
   ATH_CHECK( m_tpcnvsvc.retrieve() );

   return StatusCode::SUCCESS;
}

ITPCnvBase* TrigSerTPTool::getConverter( const std::string& persistent ) const
{
   std::lock_guard<std::mutex> lock(m_convertersCacheMutex);
   ITPCnvBase* cnvtr = m_convertesCache[persistent];
   if ( ATH_LIKELY( cnvtr ) )
      return cnvtr;

   // no converter, we need to find it, first the trigger specific one
   cnvtr = m_tpcnvsvc->p2t_cnv( persistent, Athena::TPCnvType::Trigger );
   if( ! cnvtr ) {
      // If there is no such thing, try a generic one:
      cnvtr = m_tpcnvsvc->p2t_cnv( persistent );
   }
   m_convertesCache[persistent] = cnvtr;
   return cnvtr;
}

void* TrigSerTPTool::convertTP( const std::string &clname, void *ptr,
                                std::string &persName ) const
{
   ATH_MSG_DEBUG( "TrigSerTPTool::convertTP" );

   //pers and persName set only after successful conversion
   persName = "";
   void *pers( 0 );

   // Start by trying to find the correct persistent class from the TP map
   // Otherwise, use the internal logic of the TPCnvSvc
   const auto tpItr = m_TPmap.find( clname );
   if( tpItr == m_TPmap.end() ) {
      REPORT_MESSAGE( MSG::ERROR )
         << "Transient class " << clname 
         << " is not in the T/P Converter map";
      return 0;
   }
   persName = tpItr->second;

   ITPCnvBase* cnvtr = getConverter( persName );
   if( ! cnvtr ) {
      ATH_MSG_ERROR( "T/P Converter for transient class " << tpItr->first
                     << " persistent class " << persName << " could not be retrieved");
   }
 
   // Create a persistent object:
   const std::string persname =
      System::typeinfoName( cnvtr->persistentTInfo() );
   TClass *persObjCl = getClass( persname );
   void *persptr( 0 );
   if( persObjCl ) {
      persptr = persObjCl->New();
      ATH_MSG_DEBUG( "created object of " << persptr << " at " << persptr  );
   } else {
      REPORT_MESSAGE( MSG::ERROR )
         << "Couldn't find dictionary for type " << persname;
      return 0;
   }

   // Do the conversion:
   ATH_MSG_DEBUG( "invoking TP for " << clname  << " at " << ptr );
   try {
      cnvtr->transToPersUntyped( ptr, persptr, m_logTP ? *m_logTP : msg() );
      persName = persname;
      pers = persptr;
      ATH_MSG_DEBUG( "succeeded at " << persptr  );
   }
   catch( const std::runtime_error& e ){

      //delete persObjCl->New();
      const std::string issue = e.what();
      if( issue.find( "is deprecated" ) != std::string::npos ) {
         ATH_MSG_INFO( "An exception " << e.what() );
      } else {
         pers = 0;
         REPORT_MESSAGE( MSG::ERROR ) << "An exception occurred: " << e.what();
      }
   }

   // Return the converted object:
   return pers;
}

void* TrigSerTPTool::convertPT( const std::string &persName, void *pers,
                                std::string& transName ) const
{
   // First look for a trigger specific converter:
   ITPCnvBase* cnvtr =
      m_tpcnvsvc->p2t_cnv( persName, Athena::TPCnvType::Trigger );
   if( ! cnvtr ) {
      // If there is no such thing, try a generic one:
      cnvtr = m_tpcnvsvc->p2t_cnv( persName );
   }
   if( ! cnvtr ) {
      REPORT_MESSAGE( MSG::ERROR )
         << "T/P Converter for persistent class "
         << persName << " could not be retrieved";
      return 0;
   }

   // Get the name of the transient class:
   transName = System::typeinfoName( cnvtr->transientTInfo() );

   // Create the transient object:
   TClass *transCl = getClass( transName );
   void *trans( 0 );
   if( transCl ){
      trans = transCl->New();
      ATH_MSG_DEBUG( "trans  " << trans );
   }

   // Do the conversion:
   ATH_MSG_DEBUG( "invoking PT for " << transName );
   try {
      cnvtr->persToTransWithKeyUntyped( pers, trans, "", m_logTP ? *m_logTP : msg() );
      ATH_MSG_DEBUG( " succeeded at " << trans );
   }
   catch (const std::runtime_error& e){
      REPORT_MESSAGE( MSG::ERROR ) << "An exception occurred: " << e.what();
      //it should destruct trans here (is it in a good state?)
      trans = 0;
   }

   // Return the converted object:
   return trans;
}

const std::string&
TrigSerTPTool::persClassName( const std::string& transClassName ) const {

   // Try to extract from the TP map
   const auto tpItr = m_TPmap.find( transClassName );
   if( tpItr == m_TPmap.end() ) {
      REPORT_MESSAGE( MSG::ERROR )
         << "Transient class " << transClassName
         << " is not in the T/P Converter map";
      static const std::string dummy( "" );
      return dummy;
   }

   // Return the persistent class name:
   return tpItr->second;
}

TClass *TrigSerTPTool::getClass( const std::string &cname ) const {

   TClass *cl( 0 );
   if( m_useAthDictLoader && m_dictSvc ){
      m_dictSvc->load_type( cname );
   }
   cl = TClass::GetClass( cname.c_str() );
   return cl;
}
