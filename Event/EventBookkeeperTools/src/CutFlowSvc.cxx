/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

// Implementation file for class CutFlowSvc
// Authors:
//   Tadej Novak <tadej@cern.ch>
//   Joao Firmino da Costa <joao.costa@cern.ch> and David Cote <david.cote@cern.ch>

#include "CutFlowSvc.h"

#include "GaudiKernel/Incident.h"
#include "GaudiKernel/FileIncident.h"
#include "GaudiKernel/IIncidentSvc.h"

// EDM includes
#include "EventInfo/EventStreamInfo.h"


CutFlowSvc::CutFlowSvc(const std::string& name,
                       ISvcLocator* pSvcLocator ) :
  AthService(name, pSvcLocator)
{
  assert( pSvcLocator );
}


StatusCode
CutFlowSvc::initialize()
{
  ATH_MSG_DEBUG( "Initializing " << name() );

  //Get input MetaData StoreGate
  ATH_CHECK( m_inMetaDataStore.retrieve() );

  //Get IncidentSvc
  ServiceHandle<IIncidentSvc> incSvc("IncidentSvc", this->name());
  ATH_CHECK( incSvc.retrieve() );
  incSvc->addListener(this, IncidentType::BeginInputFile, 60); // pri has to be < 100 to be after MetaDataSvc.
  incSvc->addListener(this, "MetaDataStop", 30);

  // Create bookkeeper container for bookkeepers in _this_ processing
  ATH_CHECK(createContainer());

  // Determine the skimming cycle number that we should use now from the input file
  ATH_MSG_VERBOSE("Have currently the cycle number = " << m_skimmingCycle );
  ATH_CHECK( this->determineCycleNumberFromInput(m_completeCollName) );
  ATH_MSG_VERBOSE("Will use cycle number = " << m_skimmingCycle );

  return StatusCode::SUCCESS;
}



CutIdentifier CutFlowSvc::registerFilter( const std::string& name,
                                          const std::string& description )
{
  ATH_MSG_DEBUG("Registering filter with name '" << name << "' and description '" << description << "'");

  auto newCbk = std::make_unique<xAOD::CutBookkeeper>();
  newCbk->setName(name);
  newCbk->setDescription(description);
  newCbk->setInputStream(m_inputStream);
  newCbk->setCycle(m_skimmingCycle);
  CutIdentifier cutID = newCbk->uniqueIdentifier();

  // Let's see if an CutBookkeeper of this name already exists
  ATH_MSG_VERBOSE("Searching if this CutBookkeeper already exists");
  for (xAOD::CutBookkeeper* cbk : *m_container) {
    if (newCbk->isEqualTo(cbk)) {
      ATH_MSG_DEBUG("The CutBookkeeper with name '" << name << "' already exists"
                    << " and has cutID " << cbk->uniqueIdentifier() << "... Not adding!" );
      // Return the existing cut ID
      return cbk->uniqueIdentifier();
    }
  }

  // If it is a new CutBookkeeper, add it to the container
  ATH_MSG_DEBUG("Declaring a new filter with name '" << name << "' and cutID " << cutID );
  m_container->push_back(std::move(newCbk));

  return cutID;
}



// This method is probably only called by the DecionsSvc
CutIdentifier CutFlowSvc::registerTopFilter( const std::string& name,
                                             const std::string& description,
                                             unsigned int logic,
                                             const std::string& outputStream ) 
{
  ATH_MSG_DEBUG("Registering top filter with name '" << name << "' and description '" << description << "'"
                << ", logic=" << logic << ", outputStream=" << outputStream << ")");

  // Call the registerFilter method and get the correct CutBookkeeper
  // from the returned cutID
  CutIdentifier cutID = registerFilter(name, description);
  xAOD::CutBookkeeper* cbk = getCutBookkeeper(cutID);
  if (cbk == nullptr) {
    ATH_MSG_ERROR("registerTopFilter: Could not find CutBookkeeper with cutID " << cutID);
    return 0;
  }

  // Then set the logic and outputStream of the relevant CutBookkeeper
  cbk->setCutLogic(xAOD::CutBookkeeper::CutLogic(logic));
  cbk->setTopFilter(true);
  cbk->addOutputStreamForAllUsed(outputStream);

  return cutID;
}


void
CutFlowSvc::setFilterDescription( CutIdentifier cutID,
                                  const std::string& descr )
{
  ATH_MSG_DEBUG("calling setFilterDescription(" << cutID << ", " << descr << ")" );

  xAOD::CutBookkeeper* cbk = getCutBookkeeper(cutID);
  cbk->setDescription(descr);
  return;
}



void
CutFlowSvc::addEvent( CutIdentifier cutID, double weight)
{
  ATH_MSG_VERBOSE("Adding event with weight "<< weight << "to cut " << cutID);

  std::lock_guard<std::recursive_mutex> lock(m_addEventMutex);

  xAOD::CutBookkeeper* cbk = getCutBookkeeper(cutID);
  if (cbk == nullptr) {
    ATH_MSG_DEBUG("Could not find CutBookkeeper, creating a new one");

    // Iterate over the complete bookkeepers and update the cutID-to-bookkeeper map
    ATH_MSG_DEBUG( "addEvent: Going to re-populate the map. Have "
                   << m_container->size() << " CutBookkeepers"
                   << " and skimming cycle " << m_skimmingCycle
                   << " and input Stream name " << m_inputStream );
    for (xAOD::CutBookkeeper* ebk : *m_container) {
      CutIdentifier cutIDCurrent = ebk->uniqueIdentifier();
      ATH_MSG_VERBOSE( "addEvent: Have CutBookkeeper with"
                       << " skimming cycle " << ebk->cycle()
                       << " and input Stream name " << ebk->inputStream()
                       << " and logic " << ebk->cutLogic()
                       << " isTopFilter " << ebk->isTopFilter()
                       << " and cutID " << cutIDCurrent
                       << " and name " << ebk->name() );
      if ( m_skimmingCycle == ebk->cycle() ) {
        if ( m_inputStream == ebk->inputStream() ) {
          CutIDMap_t::iterator mapIter = m_ebkMap.find(cutIDCurrent);
          ATH_MSG_DEBUG( "current cycle and input stream: Have CutBookkeeper with"
                         << " cutID " << cutIDCurrent
                         <<  " and name " << ebk->name() );
          if ( mapIter != m_ebkMap.end() ) { // we found this CutBookkeeper in the existing map
            (*mapIter).second = ebk;
          }
        }
      }
    } // End: Loop over all complete CutBookkeepers
    cbk = getCutBookkeeper(cutID);
  }
  if (cbk == nullptr) {
    ATH_MSG_ERROR ( "Got a zero pointer to an CutBookkeeper with cutID="
                    << cutID << "! This should never happen..." );
    return;
  }
  ATH_MSG_VERBOSE( "addEvent: have cutID " << cutID
                   << " and CutBookkeeper name " << cbk->name() );
  cbk->addNAcceptedEvents(1);
  cbk->addSumOfEventWeights(weight);
  cbk->addSumOfEventWeightsSquared(weight*weight);
  return;
}



//__________________________________________________________________________
void CutFlowSvc::handle( const Incident& inc )
{
  ATH_MSG_VERBOSE( "Start incident " << inc.type() );

  if ( inc.type() == IncidentType::BeginInputFile ) {
    // Check the stream name
    const EventStreamInfo* esi;
    if (m_inMetaDataStore->retrieve(esi).isFailure()) {
      ATH_MSG_WARNING("No EventStreamInfo taking stream from property InputStream");
    }
    else {
      std::string inputstream = *(esi->getProcessingTags().begin());
      if (m_inputStream.empty()) {m_inputStream=inputstream;}
      else if (m_inputStream!=inputstream) {
        const FileIncident* finc = dynamic_cast<const FileIncident*>(&inc);
        if (m_inputStream != "N/A" && m_inputStream != "unknownStream") {
          ATH_MSG_FATAL("File " << finc->fileName() 
                                << " stream " << inputstream 
                                << " does not match previous file " 
                                << m_inputStream);
          return;
        }
      }
    }
  }

  // Clear the file bookkeeper
  if (inc.type() == IncidentType::BeginInputFile || inc.type() == "MetaDataStop") {
    if (m_container != nullptr) {
      // Reset existing container
      for (xAOD::CutBookkeeper* cbk : *m_container) {
        cbk->setNAcceptedEvents(0);
        cbk->setSumOfEventWeights(0);
        cbk->setSumOfEventWeightsSquared(0);
      }
    }
  }

  ATH_MSG_DEBUG( "End incident " << inc.type() );
  return;
}



StatusCode CutFlowSvc::determineCycleNumberFromInput( const std::string& collName )
{
  ATH_MSG_DEBUG("calling determineCycleNumberFromInput('" << collName
                  << "')... have currently cycle number = " << m_skimmingCycle );

  // Try to get CutBookkeepers from the input file
  if ( m_inMetaDataStore->contains<xAOD::CutBookkeeperContainer>(collName) ) {
    ATH_MSG_VERBOSE("Found xAOD::CutBookkeeperContainer in input MetaStore with name: " << collName);

    const xAOD::CutBookkeeperContainer* inputContainer{};
    ATH_CHECK( m_inMetaDataStore->retrieve( inputContainer, collName ) );
    // Now, iterate over all CutBookkeepers and search for the highest cycle number
    int maxCycle{};
    for (const xAOD::CutBookkeeper* cbk : *inputContainer) {
      int inCycle = cbk->cycle();
      if (inCycle > maxCycle) maxCycle = inCycle;
    }
    m_skimmingCycle = std::max(m_skimmingCycle, maxCycle + 1);
  }

  ATH_MSG_DEBUG("done calling determineCycleNumberFromInput('" << collName
                  << "')... have now cycle number = " << m_skimmingCycle );
  return StatusCode::SUCCESS;
}



StatusCode CutFlowSvc::createContainer()
{
  ATH_MSG_VERBOSE("createContainer()");

  // Expect that this should only be called once for now
  if (m_container != nullptr) {
    ATH_MSG_ERROR("xAOD::CutBookkeeperContainer already exists. This is unexpected.");
    return StatusCode::FAILURE;
  }

  // Create the containers
  m_container = std::make_unique<xAOD::CutBookkeeperContainer>();
  m_containerAux = std::make_unique<xAOD::CutBookkeeperAuxContainer>();
  m_container->setStore(m_containerAux.get());

  return StatusCode::SUCCESS;
}



const xAOD::CutBookkeeperContainer* CutFlowSvc::getCutBookkeepers() const
{
  return m_container.get();
}



xAOD::CutBookkeeper* CutFlowSvc::getCutBookkeeper(const CutIdentifier cutID) const
{
  for (xAOD::CutBookkeeper* cbk : *m_container) {
    if (cbk->uniqueIdentifier() == cutID) {
      return cbk;
    }
  }
  return nullptr;
}



uint64_t CutFlowSvc::getNAcceptedEvents(const CutIdentifier cutID) const
{
  const xAOD::CutBookkeeper* cbk = getCutBookkeeper(cutID);
  if (cbk == nullptr) {
    return std::numeric_limits<uint64_t>::max();
  }
  return cbk->nAcceptedEvents();
}


StatusCode
CutFlowSvc::queryInterface( const InterfaceID& riid, void** ppvi )
{
  // valid placeholder?
  if ( 0 == ppvi ) { return StatusCode::FAILURE ; }  // RETURN
  if ( ICutFlowSvc::interfaceID() == riid ) {
    *ppvi = static_cast<ICutFlowSvc*>(this);
    addRef(); // NB! : inrement the reference count!
    return StatusCode::SUCCESS;                     // RETURN
  } else if ( IIncidentListener::interfaceID() == riid ) {
    *ppvi = static_cast<IIncidentListener*>(this);
    addRef(); // NB! : inrement the reference count!
    return StatusCode::SUCCESS;                     // RETURN
  }
  // Interface is not directly availible: try out a base class
  return AthService::queryInterface( riid, ppvi );
}
