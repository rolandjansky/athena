/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/

#include "NewMergeMcEventCollTool.h"
#include "GeneratorObjects/McEventCollectionHelper.h"
#include "EventInfo/EventType.h"
#include "EventInfo/PileUpTimeEventIndex.h"
#include "GeneratorObjects/McEventCollection.h"
#include "PileUpTools/PileUpMergeSvc.h"
#include "StoreGate/StoreGateSvc.h"
#include <fstream>

NewMergeMcEventCollTool::NewMergeMcEventCollTool(const std::string& type,
                                                   const std::string& name,
                                                   const IInterface *parent) :
  PileUpToolBase(type, name, parent),
  m_pMergeSvc("", name),
  m_expectLowPtMinBiasBackgroundCollection(0),
  m_expectHighPtMinBiasBackgroundCollection(0),
  m_truthCollKey("TruthEvent"),
  m_newevent(true),
  m_nInputMcEventColls(0),
  m_nBkgEventsReadSoFar(0),
  m_pileUpType(-1) // initialise to PileUpTimeEventIndex::PileUpType::Unknown
{
  declareInterface<IPileUpTool>(this);
  declareProperty("TruthCollKey",   m_truthCollKey);
  declareProperty("PileUpMergeSvc", m_pMergeSvc);
  declareProperty("ExpectLowPtMinBiasBackgroundCollection", m_expectLowPtMinBiasBackgroundCollection);
  declareProperty("ExpectHighPtMinBiasBackgroundCollection", m_expectHighPtMinBiasBackgroundCollection);
  declareProperty("PileUpType", m_pileUpType);
}

StatusCode NewMergeMcEventCollTool::initialize()
{
  if(!m_pMergeSvc.empty())
    {
      ATH_CHECK(m_pMergeSvc.retrieve());
    }

  ATH_CHECK( m_wrhMcEventCollection.initialize() );
  return StatusCode::SUCCESS;
}

/// PileUpTools Approach
StatusCode NewMergeMcEventCollTool::prepareEvent(unsigned int nInputEvents)
{
  ATH_MSG_VERBOSE( this->name()<<"::prepareEvent()" );
  m_newevent=true;
  m_nBkgEventsReadSoFar=0;

  //Check we are getting at least one event
  m_nInputMcEventColls = nInputEvents;
  if (0 == m_nInputMcEventColls)
    {
      ATH_MSG_ERROR("prepareEvent: TimedTruthList with key "
                    << m_truthCollKey.value() << " is empty");
      return StatusCode::RECOVERABLE;
    }
  ATH_MSG_VERBOSE(  this->name()<<"::prepareEvent: there are " << m_nInputMcEventColls << " subevents in this event.");

  ATH_MSG_VERBOSE ( this->name()<<"::prepareEvent() clearing m_outputMcEventCollectionMap" );
  m_outputMcEventCollectionMap.clear();

  return StatusCode::SUCCESS;
}

StatusCode NewMergeMcEventCollTool::processBunchXing(int /*bunchXing*/,
                                                      SubEventIterator bSubEvents,
                                                      SubEventIterator eSubEvents)
{
  //If the job has to deal with MinBias events, making sure that the collections exist in StoreGate for every event (otherwise the out-streaming will complain)
  if (m_expectLowPtMinBiasBackgroundCollection) {
    ATH_MSG_VERBOSE( this->name()<<"::processBunchXing: creating low-pT MinBias collection to be used in this or following events" );
    ATH_CHECK( prepareOutputMcEventCollections( PileUpTimeEventIndex::MinimumBias ) );
  }
  if (m_expectHighPtMinBiasBackgroundCollection) {
    ATH_MSG_VERBOSE( this->name()<<"::processBunchXing: creating high-pT MinBias collection to be used in this or following events" );
    ATH_CHECK( prepareOutputMcEventCollections( PileUpTimeEventIndex::HighPtMinimumBias ) );
  }
  SubEventIterator iEvt(bSubEvents);
  //loop over the McEventCollections (each one assumed to containing exactly one GenEvent) of the various input events
  while (iEvt != eSubEvents)
    {
      StoreGateSvc& seStore(*iEvt->ptr()->evtStore());
      ATH_CHECK( prepareOutputMcEventCollections(iEvt->type()) );
      const McEventCollection *pMEC(nullptr);
      ATH_CHECK(seStore.retrieve(pMEC, m_truthCollKey.value()));
      ATH_MSG_VERBOSE( this->name()<<"::processBunchXing: SubEvt McEventCollection from StoreGate " << seStore.name() << " of PileUpType " << iEvt->type() );
      ATH_CHECK(this->processEvent(pMEC, iEvt->type(), iEvt->time()));
      ++iEvt;
    }
  return StatusCode::SUCCESS;
}

StatusCode NewMergeMcEventCollTool::mergeEvent()
{
  ATH_MSG_VERBOSE(  this->name()<<"::mergeEvent()" );
  if(m_nBkgEventsReadSoFar+1<m_nInputMcEventColls)
    {
      ATH_MSG_WARNING( "mergeEvent: Expected " << m_nInputMcEventColls << " subevents, but only saw " << m_nBkgEventsReadSoFar+1 << "! The job will probably crash now..." );
      return StatusCode::FAILURE;
    }
  if(msgLvl(MSG::VERBOSE)) { this->printDetailsOfMergedMcEventCollection(); }
  return StatusCode::SUCCESS;
}

/// Algorithm Approach
StatusCode NewMergeMcEventCollTool::processAllSubEvents()
{
  ATH_MSG_VERBOSE ( this->name()<<"::processAllSubEvents()" );

  ATH_MSG_VERBOSE ( this->name()<<"::processAllSubEvents() clearing m_outputMcEventCollectionMap" );
  m_outputMcEventCollectionMap.clear();

  //first get the list of McEventCollections
  typedef PileUpMergeSvc::TimedList<McEventCollection>::type TimedTruthList;
  TimedTruthList truthList;
  ATH_CHECK(m_pMergeSvc->retrieveSubEvtsData(m_truthCollKey.value(), truthList));

  m_newevent=true;
  m_nBkgEventsReadSoFar=0;

  //Check we are getting at least one event
  m_nInputMcEventColls=truthList.size();
  if (0 == m_nInputMcEventColls)
    {
      ATH_MSG_ERROR("TimedTruthList with key " << m_truthCollKey.value() << " is empty.");
      return StatusCode::RECOVERABLE;
    }

  ATH_MSG_DEBUG( "execute: there are " << m_nInputMcEventColls << " subevents in this event.");
  //TODO can we make this into an auto for loop?
  TimedTruthList::iterator timedTruthListIter(truthList.begin()), endOfTimedTruthList(truthList.end());
  //loop over the McEventCollections (each one assumed to containing exactly one GenEvent) of the various input events
  while (timedTruthListIter != endOfTimedTruthList)
    {
      const McEventCollection *pBackgroundMcEvtColl(&*(timedTruthListIter->second));
      ATH_CHECK(this->processEvent(pBackgroundMcEvtColl,PileUpTimeEventIndex::Signal));
      ++timedTruthListIter;
    } //timed colls

  if(msgLvl(MSG::VERBOSE)) { this->printDetailsOfMergedMcEventCollection(); }
  return StatusCode::SUCCESS;
}

/// Common methods

StatusCode NewMergeMcEventCollTool::prepareOutputMcEventCollections(const int pileUpType)
{
  ATH_MSG_VERBOSE(  this->name()<<"::prepareOutputMcEventCollections(" << pileUpType << ")" );

  if (m_outputMcEventCollectionMap.find(pileUpType)==m_outputMcEventCollectionMap.end()) {
    const std::string outputCollectionName = McEventCollectionHelper::getMcEventCollectionNameFromPileUpType(pileUpType);
    m_outputMcEventCollectionMap.emplace(pileUpType, new McEventCollection());
    ATH_CHECK(evtStore()->record(m_outputMcEventCollectionMap.at(pileUpType), outputCollectionName));
    ATH_MSG_VERBOSE(  this->name()<<"::prepareOutputMcEventCollections() recording " << outputCollectionName << " associated to pileUpType " << pileUpType );
  }
  return StatusCode::SUCCESS;
}

StatusCode NewMergeMcEventCollTool::processEvent(const McEventCollection *pMcEvtColl, const int& pileUpType, PileUpTimeEventIndex::time_type timeOffset)
{
  ATH_MSG_VERBOSE(  this->name()<<"::processEvent()" );
  if (!pMcEvtColl->empty())
    {

	McEventCollection * newMCcoll = new McEventCollection();
	for (unsigned int iEv=0; iEv<pMcEvtColl->size(); iEv++) {
		const HepMC::GenEvent& c_evt(*((*pMcEvtColl)[iEv]));
		HepMC::GenEvent * evt = new HepMC::GenEvent(c_evt);
		for (HepMC::GenEvent::vertex_iterator itVer=evt->vertices_begin(); itVer!=evt->vertices_end(); ++itVer) {
			HepMC::FourVector newPos((*itVer)->position().x(),(*itVer)->position().y(),(*itVer)->position().z(),(*itVer)->position().t()+timeOffset);
			(*itVer)->set_position(newPos);
		}
		newMCcoll->push_back(evt);
	}

      auto outputMcEventCollection = m_outputMcEventCollectionMap.find(pileUpType);
      if(outputMcEventCollection!= m_outputMcEventCollectionMap.end())
        {
          //GenEvt is there
          HepMC::GenEvent& currentBackgroundEvent(**(newMCcoll->begin()));
          m_outputMcEventCollectionMap.at(pileUpType)->push_back(new HepMC::GenEvent(currentBackgroundEvent));
          ATH_MSG_VERBOSE("NewMergeMcEventCollTool::processEvent - added GenEvent to McEventCollection for pileUpType = "<<pileUpType);
        }
      else
        {
          m_outputMcEventCollectionMap.emplace(pileUpType, new McEventCollection(*newMCcoll));
          ATH_MSG_WARNING("NewMergeMcEventCollTool::processEvent - added McEventCollection for unexpected pileUpType("<<pileUpType<<")!!");
        }
    }
  if(!m_newevent) ++m_nBkgEventsReadSoFar;
  else m_newevent=false;
  return StatusCode::SUCCESS;
}

void NewMergeMcEventCollTool::printDetailsOfMergedMcEventCollection() const
{
  if (m_outputMcEventCollectionMap.empty()) { return; }
  for(const auto& outputMapEntry : m_outputMcEventCollectionMap)
    {
      DataVector<HepMC::GenEvent>::const_iterator outputEventItr(outputMapEntry.second->begin());
      const DataVector<HepMC::GenEvent>::const_iterator endOfEvents(outputMapEntry.second->end());
      ATH_MSG_INFO ( "$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$" );
      ATH_MSG_INFO ( "pileUpType: " << outputMapEntry.first);
      ATH_MSG_INFO ( "$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$" );
      ATH_MSG_INFO ( "Current OUTPUT GenEvent: " );
      while(outputEventItr!=endOfEvents)
        {
          const int signal_process_id((*outputEventItr)->signal_process_id());
          const int event_number((*outputEventItr)->event_number());
          ATH_MSG_INFO ( "GenEvent #"<<event_number<<", signal_process_id="<<signal_process_id<</*", category="<<event->second<<*/", number of Vertices="<<(*outputEventItr)->vertices_size() );
          char fname[80];
          sprintf(fname,"%s.event%d.txt",m_truthCollKey.value().c_str(),event_number);
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
    }
  return;
}

