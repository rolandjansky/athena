/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/** @file EventSelectorCounterTool.cxx
 *  @brief This file contains the implementation for the EventSelectorCounterTool class.
 *  @author Peter van Gemmeren <gemmeren@anl.gov>
 *  $Id: EventSelectorCounterTool.cxx 663679 2015-04-29 08:31:54Z krasznaa $
 **/

// David Cote: this tool is broken since the event weight has been moved to EventInfo which is not yet loaded
//             at the moment Pre/PostNext() is called. The reason for using these functions was to support TAG filtering
//             but it is not doable anymore in this fashion. Hence this tool is now replaced by a simple Alg: EventCounterAlg.

#include "EventSelectorCounterTool.h"

#include "GaudiKernel/MsgStream.h"

#include "StoreGate/StoreGateSvc.h"


//___________________________________________________________________________
EventSelectorCounterTool::EventSelectorCounterTool(const std::string& type,
                                                   const std::string& name,
                                                   const IInterface* parent)
  : AthAlgTool(type, name, parent),
    m_metaDataStore("StoreGateSvc/MetaDataStore", name),
    m_cutflowSvc("CutFlowSvc/CutFlowSvc",name)
{
  declareInterface<IAthenaSelectorTool>(this);
  m_notInitialized=true;
}



//___________________________________________________________________________
EventSelectorCounterTool::~EventSelectorCounterTool() {
}



//___________________________________________________________________________
StatusCode EventSelectorCounterTool::initialize() {
   // locate the DetectorStore and initialize our local ptr
   ATH_CHECK( m_metaDataStore.retrieve() );
   return(StatusCode::SUCCESS);
}



//___________________________________________________________________________
StatusCode EventSelectorCounterTool::postInitialize() {
   return(StatusCode::SUCCESS);
}



//__________________________________________________________________________
StatusCode EventSelectorCounterTool::preNext() {
  if(m_notInitialized){
    m_notInitialized=false;
    //Get ICutFlowSvc pointer and register the two evt counters of this tool
    //Do it passed initialize to ensure that it happens after the CutFlowSvc initialization
    ATH_CHECK( m_cutflowSvc.retrieve() );

    m_allEvtsCutID = m_cutflowSvc->registerFilter(this->name()+"AllEvents","All events processed before any offline cut.");
    m_allEvtsPostNextCutID = m_cutflowSvc->registerFilter(this->name()+"AllEventsPostTAG","Nb of events after TAG selection.");
  }

  m_cutflowSvc->addEvent( m_allEvtsCutID );
  return(StatusCode::SUCCESS);
}



//__________________________________________________________________________
StatusCode EventSelectorCounterTool::postNext() {
  m_cutflowSvc->addEvent( m_allEvtsPostNextCutID );
  return(StatusCode::SUCCESS);
}



//__________________________________________________________________________
StatusCode EventSelectorCounterTool::preFinalize() {
  return(StatusCode::SUCCESS);
}


//__________________________________________________________________________
StatusCode EventSelectorCounterTool::finalize() {
  return(StatusCode::SUCCESS);
}
