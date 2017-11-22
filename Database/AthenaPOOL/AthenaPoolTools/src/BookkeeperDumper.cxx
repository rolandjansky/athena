/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/**
 * @file BookkeeperDumper.cxx
 * @brief Implementation of class BookkeeperDumper
 */
 
#include "BookkeeperDumper.h"

#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/AlgFactory.h"

#include "PersistentDataModel/Token.h"
#include "PersistentDataModel/DataHeader.h"
#include "GaudiKernel/IIncidentSvc.h"
#include "GaudiKernel/FileIncident.h"

#include "StoreGate/StoreGateSvc.h"
#include "AthenaKernel/IClassIDSvc.h"

#include "xAODCutFlow/CutBookkeeperContainer.h"


//___________________________________________________________________________
BookkeeperDumper::BookkeeperDumper(const std::string& name, ISvcLocator* pSvcLocator) : 
   AthAlgorithm(name, pSvcLocator) 
{
}

BookkeeperDumper::~BookkeeperDumper()
{}

StatusCode BookkeeperDumper::initialize() 
{
   // clear containers
   ATH_MSG_INFO ( "in initialize()" );

   ServiceHandle<IIncidentSvc> incSvc("IncidentSvc", this->name());
   ATH_CHECK( incSvc.retrieve() );
   incSvc->addListener(this, "BeginInputFile", 100); 

   return StatusCode::SUCCESS;
}

void BookkeeperDumper::handle(const Incident& inc)
{
  
  ATH_MSG_INFO("handle() " << inc.type());

  // Need to get input file name for event comparison
  if (inc.type()=="BeginInputFile") {
    ServiceHandle<StoreGateSvc> mdstore("StoreGateSvc/InputMetaDataStore", name());
    if (mdstore.retrieve().isSuccess()) {
      const DataHandle<xAOD::CutBookkeeperContainer> compBook(NULL);
      if (mdstore->retrieve(compBook, "CutBookkeepers").isSuccess()) {
        ATH_MSG_INFO("CBK size = " << compBook->size());
        for (auto it = compBook->begin(); it != compBook->end(); ++it) {
          ATH_MSG_INFO("CBK name= " << (*it)->name() << " cycle=" << (*it)->cycle() << " stream=" << (*it)->inputStream() << " N=" << (*it)->nAcceptedEvents() << " W=" << (*it)->sumOfEventWeights() << " nc=" << (*it)->nChildren());
        }
      } else {
        ATH_MSG_INFO("CBK No CutBookkeepers " << mdstore->dump());
      }
      const DataHandle<xAOD::CutBookkeeperContainer> incompBook(NULL);
      if (mdstore->retrieve(incompBook, "IncompleteCutBookkeepers").isSuccess()) {
        ATH_MSG_INFO("ICBK size = " << incompBook->size());
        for (auto it = incompBook->begin(); it != incompBook->end(); ++it) {
          ATH_MSG_INFO("ICBK name= " << (*it)->name() << " cycle=" << (*it)->cycle() << " stream=" << (*it)->inputStream() << " N=" << (*it)->nAcceptedEvents() << " W=" << (*it)->sumOfEventWeights() << " nc=" << (*it)->nChildren());
        }
      } else {
        ATH_MSG_INFO("ICBK No CutBookkeepers " << mdstore->dump());
      }
    }
  }
}

StatusCode BookkeeperDumper::execute() 
{
   ATH_MSG_DEBUG ( "in execute()" );

   return(StatusCode::SUCCESS);
}

StatusCode BookkeeperDumper::finalize() 
{
   ATH_MSG_DEBUG ( "in m_finalize()" );
   
   return(StatusCode::SUCCESS);
}
