/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: DbPrint.h 458019 2011-09-09 10:11:57Z mnowak $
//====================================================================
//  Storage Service Printer object
//--------------------------------------------------------------------
//
//  @author      M.Frank
//====================================================================
#ifndef POOL_STORAGESVC_DBPRINT_H
#define POOL_STORAGESVC_DBPRINT_H 1

// Framework customization file
#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/Bootstrap.h"
#include "GaudiKernel/ISvcLocator.h"

/*
 *   POOL namespace declaration
 */
namespace pool {

   namespace  DbPrintLvl {
      typedef   MSG::Level      MsgLevel;
      static const MsgLevel None        = MsgLevel::NIL;
      static const MsgLevel Verbose     = MsgLevel::VERBOSE;
      static const MsgLevel Debug       = MsgLevel::DEBUG;
      static const MsgLevel Info        = MsgLevel::INFO;
      static const MsgLevel Warning     = MsgLevel::WARNING;
      static const MsgLevel Error       = MsgLevel::ERROR;
      static const MsgLevel Fatal       = MsgLevel::FATAL;
      static const MsgLevel Always      = MsgLevel::ALWAYS;

      extern MsgLevel   outputLvl;
      inline void       setLevel( MsgLevel l )  { outputLvl=l; }
   }


   class DbPrint : public MsgStream
   {
  public:
     DbPrint( const std::string& name )
           : MsgStream( Gaudi::svcLocator()->service<IMessageSvc>( "MessageSvc" ).get(), name )
     {
        if( DbPrintLvl::outputLvl != DbPrintLvl::None ) {
           setLevel( DbPrintLvl::outputLvl );
        }
     }

     static MsgStream& endmsg( MsgStream& s ) { return ::endmsg(s); }
   };

}       // End namespace pool
#endif  // POOL_STORAGESVC_DBPRINT_H
