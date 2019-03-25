/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/

#include "SimpleMergeMcEventCollTool.h"
#include "GeneratorObjects/McEventCollection.h"
#include "PileUpTools/PileUpMergeSvc.h"
#include "StoreGate/StoreGateSvc.h"
#include <fstream>

SimpleMergeMcEventCollTool::SimpleMergeMcEventCollTool(const std::string& type,
                                                       const std::string& name,
                                                       const IInterface *parent) :
  PileUpToolBase(type, name, parent)
{
  declareProperty("TruthCollInputKey",   m_truthCollInputKey);
}

StatusCode SimpleMergeMcEventCollTool::initialize()
{
  if(!m_pMergeSvc.empty())
    {
       ATH_CHECK(m_pMergeSvc.retrieve());
    }
  return StatusCode::SUCCESS;
}

/// PileUpTools Approach
StatusCode SimpleMergeMcEventCollTool::prepareEvent(unsigned int nInputEvents)
{
  ATH_MSG_VERBOSE ( "prepareEvent()" );
  m_nBkgEventsReadSoFar=0;

  //Check we are getting at least one event
  m_nInputMcEventColls = nInputEvents;
  if (0 == m_nInputMcEventColls)
     {
      ATH_MSG_ERROR("prepareEvent: TimedTruthList with key "
                    << m_truthCollInputKey.value() << " is empty");
      return StatusCode::RECOVERABLE;
    }
  ATH_MSG_DEBUG( "prepareEvent: there are " << m_nInputMcEventColls << " subevents in this event.");

  if (!m_outputMcEventCollection.isValid()) {
    // Would be nice to avoid having the WriteHandle as a member
    // variable, but this is the only way to allow multiple function
    // calls to add information to the version of the
    // McEventCollection in the output StoreGate
    m_outputMcEventCollection = SG::makeHandle(m_truthCollOutputKey);
    ATH_CHECK(m_outputMcEventCollection.record(std::make_unique<McEventCollection>()));
  }
  else {
    ATH_MSG_ERROR("WriteHandle already valid??");
    return StatusCode::FAILURE;
  }

  return StatusCode::SUCCESS;
}

StatusCode SimpleMergeMcEventCollTool::processBunchXing(int /*bunchXing*/,
                                                        SubEventIterator bSubEvents,
                                                        SubEventIterator eSubEvents)
{
  SubEventIterator iEvt(bSubEvents);
  //loop over the McEventCollections (each one assumed to containing exactly one GenEvent) of the various input events
  while (iEvt != eSubEvents)
    {
      StoreGateSvc& seStore(*iEvt->ptr()->evtStore());
      const McEventCollection *pMEC(nullptr);
      ATH_CHECK(seStore.retrieve(pMEC, m_truthCollInputKey.value()));
      ATH_MSG_DEBUG ("processBunchXing: SubEvt McEventCollection from StoreGate " << seStore.name() );
      ATH_CHECK(this->processEvent(pMEC, m_outputMcEventCollection.ptr()));
      ++iEvt;
    }
  return StatusCode::SUCCESS;
}

StatusCode SimpleMergeMcEventCollTool::mergeEvent()
{
  ATH_MSG_DEBUG( "mergeEvent()" );
  if(m_nBkgEventsReadSoFar+1<m_nInputMcEventColls)
    {
      ATH_MSG_WARNING( "mergeEvent: Expected " << m_nInputMcEventColls << " subevents, but only saw " << m_nBkgEventsReadSoFar+1 << "! The job will probably crash now..." );
      return StatusCode::FAILURE;
    }
  if(msgLvl(MSG::VERBOSE)) { this->printDetailsOfMergedMcEventCollection(m_outputMcEventCollection.ptr()); }
  return StatusCode::SUCCESS;
}

/// Algorithm Approach
StatusCode SimpleMergeMcEventCollTool::processAllSubEvents()
{
  ATH_MSG_VERBOSE ( "processAllSubEvents()" );
  SG::WriteHandle<McEventCollection> outputMcEventCollection(m_truthCollOutputKey);
  ATH_CHECK(outputMcEventCollection.record(std::make_unique<McEventCollection>()));

  //first get the list of McEventCollections
  typedef PileUpMergeSvc::TimedList<McEventCollection>::type TimedTruthList;
  TimedTruthList truthList;
  ATH_CHECK(m_pMergeSvc->retrieveSubEvtsData(m_truthCollInputKey.value(), truthList));

  m_nBkgEventsReadSoFar=0;

  //Check we are getting at least one event
  m_nInputMcEventColls=truthList.size();
  if (0 == m_nInputMcEventColls)
    {
      ATH_MSG_ERROR("TimedTruthList with key " << m_truthCollInputKey.value() << " is empty.");
      return StatusCode::RECOVERABLE;
    }

  ATH_MSG_DEBUG( "execute: there are " << m_nInputMcEventColls << " subevents in this event.");
  //TODO can we make this into an auto for loop?
  TimedTruthList::iterator timedTruthListIter(truthList.begin()), endOfTimedTruthList(truthList.end());
  //loop over the McEventCollections (each one assumed to containing exactly one GenEvent) of the various input events
  while (timedTruthListIter != endOfTimedTruthList)
    {
      const McEventCollection *pBackgroundMcEvtColl(&*(timedTruthListIter->second));
      ATH_CHECK(this->processEvent(pBackgroundMcEvtColl,outputMcEventCollection.ptr()));
      ++timedTruthListIter;
    } //timed colls

  if(msgLvl(MSG::VERBOSE)) { this->printDetailsOfMergedMcEventCollection(outputMcEventCollection.ptr()); }
  return StatusCode::SUCCESS;
}

/// Common methods

StatusCode SimpleMergeMcEventCollTool::processEvent(const McEventCollection *pMcEvtColl, McEventCollection *outputMcEventCollection)
{
  ATH_MSG_VERBOSE ( "processEvent()" );
  if (!outputMcEventCollection) {
    ATH_MSG_ERROR( this->name()<<"::processEvent() was passed an null output McEventCollection pointer." );
    return StatusCode::FAILURE;
  }
  if (!pMcEvtColl) {
    ATH_MSG_ERROR( this->name()<<"::processEvent() was passed an null input McEventCollection pointer." );
    return StatusCode::FAILURE;
  }

  if (pMcEvtColl->empty())
    {
      ++m_nBkgEventsReadSoFar;
      return StatusCode::SUCCESS;
    }
  //GenEvt is there

  const HepMC::GenEvent& currentBackgroundEvent(**(pMcEvtColl->begin()));
  // FIXME no protection against multiple GenEvents having the same event number
  outputMcEventCollection->push_back(new HepMC::GenEvent(currentBackgroundEvent));
  ++m_nBkgEventsReadSoFar;
  return StatusCode::SUCCESS;
}

void SimpleMergeMcEventCollTool::printDetailsOfMergedMcEventCollection(McEventCollection *outputMcEventCollection) const
{
  if (outputMcEventCollection->empty()) { return; }
  DataVector<HepMC::GenEvent>::const_iterator outputEventItr(outputMcEventCollection->begin());
  const DataVector<HepMC::GenEvent>::const_iterator endOfEvents(outputMcEventCollection->end());
  ATH_MSG_INFO ( "$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$" );
  ATH_MSG_INFO ( "Current OUTPUT GenEvent: " );
  while(outputEventItr!=endOfEvents)
    {
      const int signal_process_id((*outputEventItr)->signal_process_id());
      const int event_number((*outputEventItr)->event_number());
      ATH_MSG_INFO ( "GenEvent #"<<event_number<<", signal_process_id="<<signal_process_id<</*", category="<<event->second<<*/", number of Vertices="<<(*outputEventItr)->vertices_size() );
      char fname[80];
      sprintf(fname,"%s.event%d.txt",m_truthCollInputKey.value().c_str(),event_number);
      std::ofstream of(fname);
      (*outputEventItr)->print(of); // verbose output
      of.close();
      //      HepMC::GenEvent& currentSignalEvent(**(outputEventItr));
      //      currentSignalEvent.print();
      //      ATH_MSG_INFO ( "Current signal_process_vertex: " );
      //      if(0!=currentSignalEvent.signal_process_vertex()) currentSignalEvent.signal_process_vertex()->print();
      //      else ATH_MSG_INFO ( "signal_process_vertex is NULL" );
      ++outputEventItr;
    }
  ATH_MSG_INFO ( "$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$" );
  return;
}

