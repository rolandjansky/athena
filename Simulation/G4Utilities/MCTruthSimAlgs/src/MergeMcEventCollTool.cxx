/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#include "MergeMcEventCollTool.h"

#include "AthenaKernel/errorcheck.h" // Handy definitions for error checking
#include "GeneratorObjects/McEventCollection.h"
#include "PileUpTools/PileUpMergeSvc.h"
#include "TruthUtils/HepMCHelpers.h"

#include "GaudiKernel/PhysicalConstants.h"
using namespace Gaudi::Units;

//#include <boost/functional/hash.hpp>
#include <vector>
using std::vector;
#include <string>
using std::string;
#include <cmath>
using std::sqrt;
using std::abs;
// using std::cout;
// using std::endl;
typedef std::pair<int, int> IndexKey;
namespace {
  double charge( const int id ) {
    // anonymous namespace -> local to that .so
    /** 3*charge for basic pdgId codes -- used to parse unknown id's
        Fix from Frank for the charge of the MC Truth Particle */
    const int qcharge[100] =
      {+0, -1, +2, -1, +2, -1, +2, -1, +2, +0,  // 0-9
       +0, -3, +0, -3, +0, -3, +0, -3, +0, +0,  // 10-19
       +0, +0, +0, +3, +0, +0, +0, +0, +0, +0,  // 20-29
       +0, +0, +0, +3, +0, +0, +0, +3, +0, +0,  // 30-39
       +0, +0, +0, +0, +0, +0, +0, +0, +0, +0,
       +0, +0, +0, +0, +0, +0, +0, +0, +0, +0,
       +0, +0, +0, +0, +0, +0, +0, +0, +0, +0,
       +0, +0, +0, +0, +0, +0, +0, +0, +0, +0,
       +0, +0, +0, +0, +0, +0, +0, +0, +0, +0,
       +0, +0, +0, +0, +0, +0, +0, +0, +0, +0};


    /** Set charge using PDG convention:
        id = nnnnijkl
        i == 0, j == 0:   see qcharge[100]
        i == 0:           meson, j kbar quarks    l = 2*spin+1
        i != 0:           baryon, i j k quarks    l = 2*spin+1
        Default is 0; */

    const int sid = abs(id);
    double q = 0;
    if ( sid==11 || sid==13 || sid==15 || sid==17 ) q=1; // charged leptons
    else if (  sid==12 || sid==14 || sid==16 || sid==18 || sid==22 ) q=0; // neutral leptons and photon
    else if ( sid == 2212 || sid==24 ) q=1; //proton/anti-proton and W
    else if ( sid == 2112 || sid==23 ) q=0; //neutron/anti-neutron and Z
    else { // quarks, gluons, measons and other baryons
      int idmod = abs(id) % 10000;
      int q1 = (idmod/10) % 10;
      int q2 = (idmod/100) % 10;
      int q3 = (idmod/1000) % 10;
      if( idmod < 100 ) {
        q = qcharge[idmod]/3.;
      }
      else if( idmod < 1000 ) {
        q = (qcharge[q2]-qcharge[q1])/3.;
        if (qcharge[q2]==2) q=-q;
      }
      else if( idmod < 10000 ) {
        q = (qcharge[q3]+qcharge[q2]+qcharge[q1])/3.;
      }
    }

    q = (id < 0) ? -q : q;

    return q;
  }
  IndexKey makekey(int signal_process_id, int event_number, int separator_hack=0) {
    //std::size_t key(0);
    //Check for Separtor GenEvents
    if(signal_process_id==0 && event_number==-1) {
      //boost::hash_combine(key, separator_hack);
      return std::make_pair(separator_hack, event_number);
    }
    return std::make_pair(signal_process_id, event_number);
    //else { boost::hash_combine(key, signal_process_id); }
    //boost::hash_combine(key, event_number);
    //return key;
  }

  class GenEventSorter {
  public:
    typedef std::map<IndexKey, int> PileUpBackgroundMap;
    GenEventSorter(const PileUpBackgroundMap& backgroundClassificationMap) : m_backgroundClassificationMap(backgroundClassificationMap) {}
    bool operator() (const HepMC::GenEvent *pGenEvent1, const HepMC::GenEvent *pGenEvent2) {
      const int signal_process_id1(pGenEvent1->signal_process_id()), event_number1(pGenEvent1->event_number()), separator_hack1(pGenEvent1->mpi());
      const int signal_process_id2(pGenEvent2->signal_process_id()), event_number2(pGenEvent2->event_number()), separator_hack2(pGenEvent2->mpi());
      const IndexKey key1(makekey(signal_process_id1, event_number1,separator_hack1));
      const IndexKey key2(makekey(signal_process_id2, event_number2,separator_hack2));
      const PileUpBackgroundMap::const_iterator event1=m_backgroundClassificationMap.find(key1);
      const PileUpBackgroundMap::const_iterator event2=m_backgroundClassificationMap.find(key2);
      if(event1==m_backgroundClassificationMap.end()) {throw 421;}
      if(event2==m_backgroundClassificationMap.end()) {throw 422;}
      //if(event1==event2) {throw 423;}
      //Both events have the same 'type'
      if(event1->second==event2->second) {
        if (is_separator(signal_process_id1, event_number1)) {return true;} //separator GenEvents should go at the start of each classification
        if (is_separator(signal_process_id2, event_number2)) {return false;}
        //Both events are from the same dataset
        if (signal_process_id1==signal_process_id2) {
          return (event_number1<event_number2);
        }
        return (signal_process_id1<signal_process_id2);
      }
      return (event1->second<event2->second);

    }
  private:
    inline bool is_separator(int signal_process_id, int event_number) const { return (0==signal_process_id && -1==event_number); }
    const PileUpBackgroundMap& m_backgroundClassificationMap;
  };
} //anonymous namespace

MergeMcEventCollTool::MergeMcEventCollTool(const std::string& type,
                                           const std::string& name,
                                           const IInterface *parent) :
  PileUpToolBase(type, name, parent)
{
}

StatusCode MergeMcEventCollTool::initialize() {
  ATH_MSG_DEBUG( "Initializing " << name() << " - package version " << PACKAGE_VERSION );
  if (m_doSlimming) {
    //set all m_saveTypes based on properties
    m_saveType[INTIME] = m_saveInTimePileup.value();
    m_saveType[OUTOFTIME] = m_saveOutOfTimePileup.value();
    m_saveType[RESTOFMB] = m_saveRestOfPileup.value();
    m_saveType[CAVERN] = m_saveCavernBackground.value();
  }
  else {
    m_keepUnstable        = true;
    m_saveType[INTIME]    = true;
    m_saveType[OUTOFTIME] = true;
    m_saveType[RESTOFMB]  = true;
    m_saveType[CAVERN]    = true;
  }
  m_r2Range = m_rRange*m_rRange;

  return StatusCode::SUCCESS;
}

StatusCode MergeMcEventCollTool::prepareEvent(const EventContext& /*ctx*/, unsigned int nInputEvents) {
  //clear the background classification map
  m_backgroundClassificationMap.clear();
  m_newevent=true;

  //Check we are getting at least one event
  m_nInputMcEventColls = nInputEvents;
  if (0 == m_nInputMcEventColls) {
    msg(MSG::ERROR)
      << "prepareEvent: TimedTruthList with key "
      << m_truthCollInputKey.value()
      << " is empty" << endmsg;
    return StatusCode::RECOVERABLE;
  }
  ATH_MSG_DEBUG( "prepareEvent: there are " << m_nInputMcEventColls << " subevents in this event.");

  return StatusCode::SUCCESS;
}

StatusCode MergeMcEventCollTool::processAllSubEvents(const EventContext& /*ctx*/) {
  ATH_MSG_VERBOSE ( "processAllSubEvents()" );
  if(!m_pMergeSvc) {
    if (!(m_pMergeSvc.retrieve()).isSuccess()) {
      ATH_MSG_FATAL ( "processAllSubEvents: Could not find PileUpMergeSvc" );
      return StatusCode::FAILURE;
    }
  }

  //PRECONDITIONS
  //first get the list of McEventCollections
  typedef PileUpMergeSvc::TimedList<McEventCollection>::type TimedTruthList;
  TimedTruthList truthList;
  if (!m_pMergeSvc->retrieveSubEvtsData(m_truthCollInputKey.value(), truthList).isSuccess() ) {
    msg(MSG::ERROR)
      << "execute: Can not find TimedTruthList with key "
      << m_truthCollInputKey.value() << endmsg;
    return StatusCode::RECOVERABLE;
  }

  //clear the background classification map
  m_backgroundClassificationMap.clear();

  //Check we are getting at least one event
  m_nInputMcEventColls=truthList.size();
  if (0 == m_nInputMcEventColls) {
    msg(MSG::ERROR)
      << "execute: TimedTruthList with key "
      << m_truthCollInputKey.value()
      << " is empty" << endmsg;
    return StatusCode::RECOVERABLE;
  }
  ATH_MSG_DEBUG( "execute: there are " << m_nInputMcEventColls << " subevents in this event.");
  TimedTruthList::iterator timedTruthListIter(truthList.begin()), endOfTimedTruthList(truthList.end());
  //loop over the McEventCollections (each one assumed to containing exactly one GenEvent) of the various input events
  while (timedTruthListIter != endOfTimedTruthList) {
    const PileUpTimeEventIndex& currentPileUpTimeEventIndex(timedTruthListIter->first);
    const McEventCollection *pBackgroundMcEvtColl(&*(timedTruthListIter->second));
    if (!processEvent(pBackgroundMcEvtColl, currentPileUpTimeEventIndex.time(), currentPileUpTimeEventIndex.index()).isSuccess()) {
      ATH_MSG_ERROR ("Failed to process McEventCollection." );
      return StatusCode::FAILURE;
    }
    ++timedTruthListIter;
  } //timed colls

  //Remove empty copies of GenEvents
  if(m_compressOutputCollection) CHECK( compressOutputMcEventCollection() );

  //Sort the GenEvents in the output McEventCollection according to background classification
  try {
    std::sort(m_pOvrlMcEvColl->begin(), m_pOvrlMcEvColl->end(), GenEventSorter(m_backgroundClassificationMap));
  }
  catch (int e) {
    ATH_MSG_ERROR("An exception occurred. Exception Nr. " << e);
    throw;
  }

  m_newevent=true;

  if(msgLvl(MSG::DEBUG)) {
    ATH_MSG_DEBUG("Sorted McEventCollection OK.");
    printDetailsOfMergedMcEventCollection();
  }

  if(m_pOvrlMcEvColl->at(0)->event_number()!=-2)
    {
      ATH_MSG_FATAL("Signal GenEvent is not first in the list! Something has gone wrong during the GenEvent sorting!");
      return StatusCode::FAILURE;
    }
  //Restore original event number
  m_pOvrlMcEvColl->at(0)->set_event_number(m_signal_event_number);

  return StatusCode::SUCCESS;
}

StatusCode MergeMcEventCollTool::processBunchXing(int bunchXing,
                                                SubEventIterator bSubEvents,
                                                SubEventIterator eSubEvents)
{
  ATH_MSG_VERBOSE ( "processBunchXing()" );
  SubEventIterator iEvt(bSubEvents);
  //loop over the McEventCollections (each one assumed to containing exactly one GenEvent) of the various input events
  while (iEvt != eSubEvents) {
    const McEventCollection *pMEC(NULL);
    if (!m_pMergeSvc->retrieveSingleSubEvtData(m_truthCollInputKey.value(), pMEC,
					       bunchXing, iEvt).isSuccess()){
      ATH_MSG_ERROR("McEventCollection not found for event key " << m_truthCollInputKey.value());
      return StatusCode::FAILURE;
    }

    if (!processEvent(pMEC,iEvt->time(),iEvt->index()).isSuccess()) {
      ATH_MSG_ERROR ("processBunchXing: Failed to process McEventCollection." );
      return StatusCode::FAILURE;
    }
    ++iEvt;
  }
  return StatusCode::SUCCESS;
}

StatusCode MergeMcEventCollTool::mergeEvent(const EventContext& /*ctx*/) {
  ATH_MSG_DEBUG( "mergeEvent" );
  if(m_nBkgEventsReadSoFar+1<m_nInputMcEventColls) {
    ATH_MSG_WARNING( "mergeEvent: Expected " << m_nInputMcEventColls << " subevents, but only saw " << m_nBkgEventsReadSoFar+1 << "! The job will probably crash now..." );
    return StatusCode::FAILURE;
  }

  //Remove empty copies of GenEvents
  if(m_compressOutputCollection) CHECK( compressOutputMcEventCollection() );

  if(m_backgroundClassificationMap.size()!=m_pOvrlMcEvColl->size()) {
    ATH_MSG_WARNING( "There are " << m_pOvrlMcEvColl->size() << " GenEvents in the McEventCollection, but " << m_backgroundClassificationMap.size() << " entries in the classification map." );
  }
  //Sort the GenEvents in the output McEventCollection according to background classification
  try {
    std::sort(m_pOvrlMcEvColl->begin(), m_pOvrlMcEvColl->end(), GenEventSorter(m_backgroundClassificationMap));
  }
  catch (int e) {
    ATH_MSG_ERROR("An exception occurred. Exception Nr. " << e);
    throw;
  }
  ATH_MSG_DEBUG("Sorted McEventCollection OK.");
  if(msgLvl(MSG::DEBUG)) { printDetailsOfMergedMcEventCollection(); }

  if(m_pOvrlMcEvColl->at(0)->event_number()!=-2)
    {
      ATH_MSG_FATAL("Signal GenEvent is not first in the list! Something has gone wrong during the GenEvent sorting!");
      return StatusCode::FAILURE;
    }
  //Restore original event number
  m_pOvrlMcEvColl->at(0)->set_event_number(m_signal_event_number);

  return StatusCode::SUCCESS;
}
void MergeMcEventCollTool::printDetailsOfMergedMcEventCollection() const {
  if (! m_pOvrlMcEvColl->empty()) {
    DataVector<HepMC::GenEvent>::const_iterator outputEventItr(m_pOvrlMcEvColl->begin());
    const DataVector<HepMC::GenEvent>::const_iterator endOfEvents(m_pOvrlMcEvColl->end());
    ATH_MSG_INFO ( "$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$" );
    ATH_MSG_INFO ( "INTIME("<<int(INTIME)<<"), OUTOFTIME("<<int(OUTOFTIME)<<"), RESTOFMB("<<int(RESTOFMB)<<"), CAVERN("<<int(CAVERN)<<"), NOPUTYPE("<<int(NOPUTYPE)<<")" );
    ATH_MSG_INFO ( "Current OUTPUT GenEvent: " );
    while(outputEventItr!=endOfEvents) {
      const int signal_process_id((*outputEventItr)->signal_process_id()), event_number((*outputEventItr)->event_number()), separator_hack((*outputEventItr)->mpi());
      const IndexKey key(makekey(signal_process_id,event_number,separator_hack));
      const PileUpBackgroundMap::const_iterator event(m_backgroundClassificationMap.find(key));
      ATH_MSG_INFO ( "GenEvent #"<<event_number<<", signal_process_id="<<signal_process_id<<", category="<<event->second<<", number of Vertices="<<(*outputEventItr)->vertices_size() );
      ++outputEventItr;
    }
    ATH_MSG_INFO ( "$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$" );
  }
  return;
}

StatusCode MergeMcEventCollTool::processFirstSubEvent(const McEventCollection *pMcEvtColl) {
  StatusCode sc(StatusCode::FAILURE);
  //Assumes the first sub-event is the signal event
  m_pOvrlMcEvColl = new McEventCollection(*pMcEvtColl);
  m_signal_event_number = m_pOvrlMcEvColl->at(0)->event_number();
  m_pOvrlMcEvColl->at(0)->set_event_number(-2); //Set this to zero for the purposes of sorting. (restore after sorting).

  updateClassificationMap(m_pOvrlMcEvColl->at(0)->signal_process_id(), m_pOvrlMcEvColl->at(0)->event_number(), 0,- 1, true);
  m_newevent=false; //Now the McEventCollection and classification map are not empty this should be set to false.
  ATH_MSG_DEBUG( "execute: copied original event McEventCollection" );
  const unsigned int nBackgroundMcEventCollections(m_nInputMcEventColls-1); // -1 for original event
  // -> reserve enough space
  unsigned int outCollSize(m_pOvrlMcEvColl->size());
  if(!m_onlySaveSignalTruth) { outCollSize += nBackgroundMcEventCollections; }
  outCollSize += NOPUTYPE-1; // Adding room for the spacers
  m_pOvrlMcEvColl->resize(outCollSize);
  if(!m_onlySaveSignalTruth) { ATH_MSG_DEBUG ( "Number of input SubEvents = " << m_nInputMcEventColls ); }
  ATH_MSG_DEBUG ( "Total Size of Output McEventCollection = " << outCollSize );

  //now place the "separator" GenEvents. This is a hack used by clients to find/double-check where the GenEvents
  //of a given type start in the overlay McEventCollection.

  // For example if intime and cavern saving is enabled i.e.
  // Configurable          Variable in code         Example Value
  // SaveInTimeMinBias     m_saveType[INTIME]       true
  // SaveOutOfTimeMinBias  m_saveType[OUTOFTIME]    false
  // SaveRestOfMinBias     m_saveType[RESTOFMB]     false
  // SaveCavernBackground  m_saveType[CAVERN]       true
  //If we have two input GenEvents the output collection would look like
  //SIGNAL, INTIME0, INTIME1, SEPARATOR, SEPARATOR, SEPARATOR, CAVERN0, CAVERN1
  unsigned int currentMcEventCollectionIndex(1);
  ATH_MSG_DEBUG ( "Event Type: SIGNAL Starts at Position 0" );
  for (int type(OUTOFTIME); type<NOPUTYPE; ++type) {
    //if a type is enabled leave room to insert the events of that type, otherwise place separator immediately after
    currentMcEventCollectionIndex += 1;
    m_pOvrlMcEvColl->at(currentMcEventCollectionIndex-1) = new HepMC::GenEvent(0, -1); //pid 0 & event_number -1 flags this GenEvent as SEPARATOR
    m_pOvrlMcEvColl->at(currentMcEventCollectionIndex-1)->set_mpi(type);
    updateClassificationMap(0, -1, type, type, true);
    ATH_MSG_DEBUG ( "Placing Separator for Type: "<<type<<" at Posistion: " << currentMcEventCollectionIndex-1 );
  }
  m_startingIndexForBackground=currentMcEventCollectionIndex;
  m_nBkgEventsReadSoFar=0; //number of input events/eventcolls read so far. Used to build index of output GenEvent in ovrl McEvColl
  sc = evtStore()->record(m_pOvrlMcEvColl, m_truthCollOutputKey);//**BIG TEST**//sc=StatusCode::SUCCESS;
  ATH_MSG_DEBUG( "Starting loop on Background events ... " );

  return sc;
}

bool MergeMcEventCollTool::isTruthFiltertedMcEventCollection(const McEventCollection *pMcEvtColl) const {
  const HepMC::GenEvent& currentBackgroundEvent(**(pMcEvtColl->begin()));
  if (currentBackgroundEvent.particles_size()>1) {return false;}
  //ATH_MSG_VERBOSE ( "isTruthFiltertedMcEventCollection: GenEvent.particles_size() = " << currentBackgroundEvent.particles_size() );
  HepMC::GenEvent::particle_const_iterator currentGenParticle(currentBackgroundEvent.particle_range().begin());
  const HepMC::GenEvent::particle_const_iterator endOfParticles(currentBackgroundEvent.particle_range().end());
  while (currentGenParticle!=endOfParticles) {
    //Geantino PDGID = 999.
    if((*currentGenParticle)->pdg_id()==999) { return true; }
    ++currentGenParticle;
  }
  return false;
}

StatusCode MergeMcEventCollTool::processEvent(const McEventCollection *pMcEvtColl, const double currentEventTime, const int currentBkgEventIndex) {
  ATH_MSG_VERBOSE ( "processEvent()" );
  if(m_newevent) return processFirstSubEvent(pMcEvtColl);

  if (pMcEvtColl->empty() || m_onlySaveSignalTruth) {
    ++m_nBkgEventsReadSoFar;
    return StatusCode::SUCCESS;
  }
  //GenEvt is there

  //Examine the properties of the GenEvent - if it looks like a TruthFiltered background event (contains a Geantino?!) then save the whole event as below, otherwise do the usual classification.
  if(isTruthFiltertedMcEventCollection(pMcEvtColl)) {
    if ( processTruthFilteredEvent(pMcEvtColl,  currentEventTime, currentBkgEventIndex).isSuccess() ) {
      ++m_nBkgEventsReadSoFar;
      return StatusCode::SUCCESS;
    }
    else {
      ATH_MSG_ERROR ("Failed to process a Truth Filtered GenEvent.");
    }
  }
  else {
    if ( processUnfilteredEvent(pMcEvtColl,  currentEventTime, currentBkgEventIndex).isSuccess() ) {
      ++m_nBkgEventsReadSoFar;
      return StatusCode::SUCCESS;
    }
    else {
      ATH_MSG_ERROR ("Failed to process an Unfiltered GenEvent.");
    }
  }
  //If we make it here something has gone wrong.
  ++m_nBkgEventsReadSoFar;
  return StatusCode::FAILURE;
}

StatusCode MergeMcEventCollTool::saveHeavyIonInfo(const McEventCollection *pMcEvtColl)
{
  if (m_pOvrlMcEvColl->at(0)->heavy_ion()) return StatusCode::SUCCESS;
  if (pMcEvtColl->at(0)->heavy_ion())
    {
      m_pOvrlMcEvColl->at(0)->set_heavy_ion(*(pMcEvtColl->at(0)->heavy_ion()));
    }
  return StatusCode::SUCCESS;
}

StatusCode MergeMcEventCollTool::processTruthFilteredEvent(const McEventCollection *pMcEvtColl, const double currentEventTime, const int currentBkgEventIndex) {
  //insert the GenEvent into the overlay McEventCollection.
  ATH_MSG_VERBOSE ( "processTruthFilteredEvent()" );
  ATH_CHECK(this->saveHeavyIonInfo(pMcEvtColl));
  m_pOvrlMcEvColl->at(m_startingIndexForBackground+m_nBkgEventsReadSoFar)=new HepMC::GenEvent(**(pMcEvtColl->begin()));
  HepMC::GenEvent& currentBackgroundEvent(*(m_pOvrlMcEvColl->at(m_startingIndexForBackground+m_nBkgEventsReadSoFar)));
  currentBackgroundEvent.set_event_number(currentBkgEventIndex);
  puType currentGenEventClassification(RESTOFMB);
  if ( fabs(currentEventTime)<51.0 ) {
    currentGenEventClassification = ( fabs(currentEventTime)<1.0 ) ? INTIME : OUTOFTIME;
  }
  updateClassificationMap(currentBackgroundEvent.signal_process_id(),
                          currentBackgroundEvent.event_number(),
                          0, currentGenEventClassification, true);
  return StatusCode::SUCCESS;
}

StatusCode MergeMcEventCollTool::processUnfilteredEvent(const McEventCollection *pMcEvtColl, const double currentEventTime, const int currentBkgEventIndex) {
  ATH_MSG_VERBOSE ( "processUnfilteredEvent()" );
  ATH_CHECK(this->saveHeavyIonInfo(pMcEvtColl));
  const HepMC::GenEvent& currentBackgroundEvent(**(pMcEvtColl->begin()));         //background event
  //handle the slimming case
  //ATH_MSG_VERBOSE( "The MB Event Number is " << currentBkgEventIndex << ". m_nBkgEventsReadSoFar = " << m_nBkgEventsReadSoFar );
  HepMC::GenVertexPtr  pCopyOfGenVertex(NULL);
  if ( currentBackgroundEvent.signal_process_vertex() ) pCopyOfGenVertex = new HepMC::GenVertex ( *currentBackgroundEvent.signal_process_vertex() );
  //insert the GenEvent into the overlay McEventCollection.
  m_pOvrlMcEvColl->at(m_startingIndexForBackground+m_nBkgEventsReadSoFar) = new HepMC::GenEvent(currentBackgroundEvent.signal_process_id(), currentBkgEventIndex, pCopyOfGenVertex );
  updateClassificationMap(currentBackgroundEvent.signal_process_id(), currentBkgEventIndex, 0, RESTOFMB, true);

  unsigned int nCollisionVerticesFound(0);
  //loop over vertices in Background GenEvent
  HepMC::GenEvent::vertex_const_iterator currentVertexIter(currentBackgroundEvent.vertices_begin());
  const HepMC::GenEvent::vertex_const_iterator endOfCurrentListOfVertices(currentBackgroundEvent.vertices_end());
  ATH_MSG_VERBOSE( "Starting a vertex loop ... " );
  //cout << "Starting a vertex loop ... " <<endl;
  for (; currentVertexIter != endOfCurrentListOfVertices; ++currentVertexIter) {
    const HepMC::GenVertexPtr  pCurrentVertex(*currentVertexIter);
    HepMC::GenVertexPtr  pCopyOfVertexForClassification[NOPUTYPE];
    for (int type(INTIME); type<NOPUTYPE; ++type) pCopyOfVertexForClassification[type]=(HepMC::GenVertexPtr )0;

    //check for collision vertices for in-time events
    bool isCollisionVertex(false);
    if(m_addBackgroundCollisionVertices && nCollisionVerticesFound<2 && currentEventTime==0.0) {
      isCollisionVertex=isInitialCollisionVertex(pCurrentVertex);
      if(isCollisionVertex) ++nCollisionVerticesFound;
    }

    //loop over outgoing particles in the current GenVertex keeping those not classified as NOPUTYPE
    ATH_MSG_VERBOSE( "Starting an outgoing particle loop ... " );
    //cout << "Starting an outgoing particle loop ... " <<endl;
    HepMC::GenVertex::particles_out_const_iterator currentVertexParticleIter(pCurrentVertex->particles_out_const_begin());
    const HepMC::GenVertex::particles_out_const_iterator endOfListOfParticlesFromCurrentVertex(pCurrentVertex->particles_out_const_end());
    for (; currentVertexParticleIter != endOfListOfParticlesFromCurrentVertex; ++currentVertexParticleIter) {
      ATH_MSG_VERBOSE( "Found a particle at location " << std::hex << *currentVertexParticleIter << std::dec  << " with PDG ID = " << (*currentVertexParticleIter)->pdg_id() );
      const HepMC::GenParticlePtr  pCurrentVertexParticle(*currentVertexParticleIter);
      const HepMC::GenVertexPtr  pCurrentParticleProductionVertex(pCurrentVertexParticle->production_vertex());
      puType particleClassification(classifyVertex(pCurrentVertexParticle, pCurrentParticleProductionVertex,currentEventTime));
      //hack to keep the complete vertex information for the interaction vertices of in-time background events
      if(isCollisionVertex && NOPUTYPE==particleClassification) {
        particleClassification=INTIME;
      }
      //add particle to appropriate vertex
      //  cout << "Parttype is " << particleClassification << endl;
      if (particleClassification<NOPUTYPE && m_saveType[particleClassification]) {
        if (!pCopyOfVertexForClassification[particleClassification]) {
          pCopyOfVertexForClassification[particleClassification] = new HepMC::GenVertex(pCurrentVertex->position());
          ATH_MSG_VERBOSE( "Added bkg vertex " << pCopyOfVertexForClassification[particleClassification]
                           //  << " at position " << pCurrentVertex->position()
                           << " at position " << HepMC::GenVertex(pCurrentVertex->position())
                           << " for pu type = " << particleClassification );
        }
        pCopyOfVertexForClassification[particleClassification]->add_particle_out( new HepMC::GenParticle(*pCurrentVertexParticle) );
        ATH_MSG_VERBOSE( "Added bkg particle at location " << std::hex
                         << *currentVertexParticleIter << std::dec
                         << " with PDG ID = " << (*currentVertexParticleIter)->pdg_id() );
      }
    } //particle loop
    /** add the in-coming particles to the in-time minbias vertex only */
    if (m_saveType[INTIME] && pCopyOfVertexForClassification[INTIME]) {
      //        cout << "saving incoming parts for INTIME vertex " <<  pCopyOfVertexForClassification[INTIME] << endl;
      HepMC::GenVertex::particles_in_const_iterator currentVertexParticleIter(pCurrentVertex->particles_in_const_begin());
      const HepMC::GenVertex::particles_in_const_iterator endOfListOfParticlesFromCurrentVertex(pCurrentVertex->particles_in_const_end());
      for (; currentVertexParticleIter != endOfListOfParticlesFromCurrentVertex; ++currentVertexParticleIter) {
        pCopyOfVertexForClassification[INTIME]->add_particle_in ( new HepMC::GenParticle(**currentVertexParticleIter) );
      }
    }
    //keep vertices with outgoing particles
    for (int type(INTIME); type<NOPUTYPE; ++type) {
      if (m_saveType[type] && pCopyOfVertexForClassification[type] && (pCopyOfVertexForClassification[type]->particles_out_size() > 0) ) {
        m_pOvrlMcEvColl->at(m_startingIndexForBackground+m_nBkgEventsReadSoFar)->add_vertex( pCopyOfVertexForClassification[type]);
        updateClassificationMap(currentBackgroundEvent.signal_process_id(), currentBkgEventIndex, 0, type, false);
      }
    }
  } //vertex loop
  //       if(m_doSlimming) ATH_MSG_VERBOSE( "execute: copied skimmed Background GenEvent into Output McEventCollection." );
  //       else ATH_MSG_VERBOSE( "execute: copied full Background GenEvent into Output McEventCollection." );
  return StatusCode::SUCCESS;
}

bool MergeMcEventCollTool::isInitialCollisionVertex(const HepMC::GenVertexPtr  pCurrentVertex) const {
  HepMC::GenVertex::particles_in_const_iterator currentVertexParticleIter(pCurrentVertex->particles_in_const_begin());
  const HepMC::GenVertex::particles_in_const_iterator endOfListOfParticlesFromCurrentVertex(pCurrentVertex->particles_in_const_end());
  while(currentVertexParticleIter != endOfListOfParticlesFromCurrentVertex) {
    const HepMC::GenParticlePtr  pCurrentVertexParticle(*currentVertexParticleIter);
    // FIXME: Nasty kludge will only work for Pythia minbias currently
    // Eventually just look for beam particles with status 4, but this
    // requires an update to the HepMC version used by ATLAS.
    if ( (4==pCurrentVertexParticle->status()) ||
         (2212==pCurrentVertexParticle->pdg_id()
          && (1==pCurrentVertexParticle->barcode() || 2==pCurrentVertexParticle->barcode()) ) ) {
      return true;
    }
    ++currentVertexParticleIter;
  }
  return false;
}

MergeMcEventCollTool::puType MergeMcEventCollTool::classifyVertex(const HepMC::GenParticlePtr  pCurrentVertexParticle, const HepMC::GenVertexPtr  pCurrentParticleProductionVertex, double currentEventTime) {
  //=======================================================================
  //handle the slimming case
  //=======================================================================
  puType particleClassification(NOPUTYPE);
  if ( pCurrentParticleProductionVertex ) {
    //throw away unstable particles and particles outside eta cut
    if ( m_keepUnstable || MC::isSimStable(pCurrentVertexParticle) ) {
      /** cut to select between minbias and cavern background pileup events */
      if ( fabs(pCurrentParticleProductionVertex->position().z()) < m_zRange ) {
        const float xi((pCurrentParticleProductionVertex->position()).x());
        const float yi((pCurrentParticleProductionVertex->position()).y());
        if ( !m_doSlimming || ( (xi*xi + yi*yi < m_r2Range) && (pCurrentVertexParticle->momentum().perp() > m_ptMin) ) ) {
          const double eta(pCurrentVertexParticle->momentum().pseudoRapidity());
          if ( !m_doSlimming || (fabs(eta) <= m_absEtaMax) ) {
            const bool currentEventIsInTime((m_lowTimeToKeep<=currentEventTime) && (currentEventTime<=m_highTimeToKeep));
            if ( currentEventIsInTime ) {
              // cout << "the Time is " << currentEventTime << std::endl;
              if ( 0.0==currentEventTime ) {
                particleClassification=INTIME;
              }
              else if (fabs(eta) <= m_absEtaMax_outOfTime) {
                particleClassification=OUTOFTIME;
                //cout << "out of time MB, -2BC, +2BC and |eta<3|" << endl;
              }
              else {
                particleClassification=RESTOFMB;
              }
            }
            else {
              particleClassification=RESTOFMB;
            }
          } // r2 && eta cut
        } // ptmin cut
      } // zrange cut
      else {
        // select cavern events
        double mass(pCurrentVertexParticle->momentum().m());
        if ( mass < 0.0 ) { mass = 0.0; }
        const double kineticEnergy(pCurrentVertexParticle->momentum().e() - mass);
        if ( !m_doSlimming || ( (kineticEnergy > m_minKinE) && (charge(pCurrentVertexParticle->pdg_id()) != 0) ) ) {
          particleClassification=CAVERN;
          // cout << "Cavern particle selected: " << "id= " << pCurrentVertexParticle->pdg_id() << " charge = " << charge(pCurrentVertexParticle->pdg_id()) << " pt= " << pCurrentVertexParticle->momentum().perp()) ;
        }
      } // zrange cut
    } // keep unstable
  } // pCurrentParticleProductionVertex is not NULL
  return particleClassification;
}

StatusCode MergeMcEventCollTool::compressOutputMcEventCollection() {
  int currentClassification=int(INTIME);
  if (! m_pOvrlMcEvColl->empty()) {
    DataVector<HepMC::GenEvent>::iterator outputEventItr(m_pOvrlMcEvColl->begin());
    while(outputEventItr!=m_pOvrlMcEvColl->end()) { //as end may change
      const int signal_process_id((*outputEventItr)->signal_process_id()),event_number((*outputEventItr)->event_number());
      //Check for separators
      if(signal_process_id==0 && event_number==-1) {
        ++outputEventItr;
        ++currentClassification;
        continue;
      }
      if((*outputEventItr)->vertices_empty()) {
        //Delete empty GenEvent
        outputEventItr=m_pOvrlMcEvColl->erase(outputEventItr);
        ATH_MSG_VERBOSE( "compressOutputMcEventCollection() Removed Empty GenEvent #" << event_number << ", signal_process_id(" << signal_process_id << "), category = " << currentClassification);
        continue;
      }
      ++outputEventItr;
    }
  }
  return StatusCode::SUCCESS;
}

void MergeMcEventCollTool::updateClassificationMap(int signal_process_id, int event_number, int separator_hack, int classification, bool firstUpdateForThisEvent=false)
{
  if(m_newevent && !m_backgroundClassificationMap.empty()) {
    ATH_MSG_ERROR( "updateClassidificationMap: GenEvent #" << event_number << ", signal_process_id(" << signal_process_id << "), category = " << classification );
    ATH_MSG_ERROR ("Failed to clear background classification map! Size = "<< m_backgroundClassificationMap.size());
    m_backgroundClassificationMap.clear();
    if (!m_backgroundClassificationMap.empty()) {
      throw 43;
    }
  }
  if(-1==classification && !m_newevent) {
    ATH_MSG_ERROR( "updateClassidificationMap: GenEvent #" << event_number << ", signal_process_id(" << signal_process_id << "), category = " << classification );
    ATH_MSG_FATAL ("Should only ever be one signal event in the background classification map! Bailing out.");
    throw 44;
  }
  // Check for separators
  IndexKey key(makekey(signal_process_id,event_number));
  //Check for Separator GenEvents
  if(signal_process_id==0 && event_number==-1) {
    key=makekey(separator_hack,event_number);
  }
  ATH_MSG_VERBOSE( "updateClassidificationMap: GenEvent #" << event_number << ", signal_process_id(" << signal_process_id << "), category = " << classification << ", key = " << key);
  const PileUpBackgroundMap::iterator event(m_backgroundClassificationMap.find(key));
  if(event!=m_backgroundClassificationMap.end()) {
    if(firstUpdateForThisEvent) {
      ATH_MSG_ERROR( "updateClassidificationMap: Repeated KEY! "<< key <<". Previous category = " << event->second );
    }
    else {
      ATH_MSG_DEBUG( "updateClassidificationMap: Updating categorty for existing key "<< key <<". Previous category = " << event->second << ", new catagory = " << classification );
    }
    if(int(RESTOFMB)!=event->second) {
      if(event->second<=classification) return;
    }
  }
  m_backgroundClassificationMap[key]=classification;
  return;
}
