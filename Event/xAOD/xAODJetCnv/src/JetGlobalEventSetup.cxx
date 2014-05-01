/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "JetGlobalEventSetup.h"

 // Gaudi includes
#include "GaudiKernel/IIncidentSvc.h"
#include "GaudiKernel/ServiceHandle.h"

#include "AthenaKernel/errorcheck.h" 
#include "JetEvent/JetMomentMap.h"

#include "StoreGate/StoreClearedIncident.h"
 
 ////////////////////////////////////////////////////////////////////////////////
 // Constructor
 ////////////////////////////////////////////////////////////////////////////////
 JetGlobalEventSetup::JetGlobalEventSetup( const std::string& name, ISvcLocator* pSvcLocator) :
   AthAlgorithm(name,pSvcLocator)   
 {
 }
 
 ////////////////////////////////////////////////////////////////////////////////
 // Initialize tool
 ////////////////////////////////////////////////////////////////////////////////
 StatusCode JetGlobalEventSetup::initialize()
 {
   
   msg(MSG::INFO) << "initialize()" << endreq;
 
   // Get handle on IncidentSvc
   ServiceHandle<IIncidentSvc> incidentSvc("IncidentSvc", name());
   CHECK(incidentSvc.retrieve());
   
   // Register listener for BeginRun
   incidentSvc->addListener(this, "StoreCleared");
 
   return StatusCode::SUCCESS;
 }
 
 
 ////////////////////////////////////////////////////////////////////////////////
 // Handle incidents
 ////////////////////////////////////////////////////////////////////////////////
 void JetGlobalEventSetup::handle(const Incident& inc)
 {

   if (inc.type()=="StoreCleared") {
     ATH_MSG(DEBUG) << " got store cleared event" << endreq;
     const StoreClearedIncident* sci
       = dynamic_cast<const StoreClearedIncident*> (&inc);
     if (sci && sci->store() == &*evtStore()) {
       ATH_MSG_DEBUG( " got store cleared event" );
       OrphanJetMomentMap::instance.clear();
       OrphanJetMomentMap::instance.reset();
       //JetMomentMapManager::s_instance.clear();
     }
   }
 }
 
