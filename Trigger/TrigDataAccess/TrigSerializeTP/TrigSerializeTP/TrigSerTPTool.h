// -*- C++ -*-

/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

// $Id$
#ifndef TRIGSERIALIZETP_TRIGSERTPTOOL_H
#define TRIGSERIALIZETP_TRIGSERTPTOOL_H 1

// System include(s):
#include <string>
#include <vector>
#include <map>
#include <memory>

// Gaudi/Athena include(s):
#include "GaudiKernel/ServiceHandle.h"
#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/IMessageSvc.h"
#include "AthenaKernel/IDictLoaderSvc.h"
#include "AthenaKernel/ITPCnvSvc.h"
#include "AthenaBaseComps/AthAlgTool.h"
#include "CxxUtils/checker_macros.h"
/**
 * @class  TrigSerTPTool
 * @author Jiri Masik <Jiri.Masik@cern.ch>
 * based on the code of AthenaROOTAccess by Scott Snyder
 */
class TrigSerTPTool : public AthAlgTool {

public:
   /// Declare the interface provided by the tool
   static const InterfaceID& interfaceID( ) {
      static const InterfaceID
         IID_TrigSerTPTool("TrigSerTPTool", 1, 0);
      return IID_TrigSerTPTool;
   }

   /// Standard AlgTool constructor
   TrigSerTPTool( const std::string& type, const std::string& name,
                  const IInterface* parent );

   /// Function initialising the tool
   virtual StatusCode initialize() override;

   /// Convert a transient object to its persistent self
   void* convertTP( const std::string& transName, void* trans,
                    std::string& persName ) const;
   /// Convert a persistent object to its transient self
   void* convertPT( const std::string& persName, void* pers,
                    std::string& transName ) const;

   /// Get the name of the persistent class belonging to a transient one
   const std::string& persClassName( const std::string& transClassName ) const;

private:
   /// Get the ROOT dictionary for a type
   TClass* getClass( const std::string& clname ) const;

   std::unique_ptr< MsgStream >             m_logTP;
   std::map< std::string, std::string >     m_TPmap; //configure - lookup of 
   std::vector< std::string >               m_activeClasses;
   ServiceHandle< IMessageSvc >             m_msgsvcTP;
   ServiceHandle< IDictLoaderSvc >          m_dictSvc;
   bool                                     m_useAthDictLoader;
   bool                                     m_online;
   ServiceHandle< ITPCnvSvc >               m_tpcnvsvc;
   mutable std::mutex                               m_convertersCacheMutex;
   mutable std::map<std::string, ITPCnvBase*>       m_convertesCache ATLAS_THREAD_SAFE; 

   ITPCnvBase* getConverter( const std::string& persistent ) const;

}; // class TrigSerTPTool

#endif // !TRIGSERIALIZETP_TRIGSERTPTOOL_H
