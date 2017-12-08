///////////////////////// -*- C++ -*- /////////////////////////////

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// Implementation file for class CutFlowSvc
// Authors: Joao Firmino da Costa <joao.costa@cern.ch> and David Cote <david.cote@cern.ch>
///////////////////////////////////////////////////////////////////

#include "CutFlowSvc.h"

// STL include
#include <algorithm>

// #include "FillEBCFromFlat.h"

#include "GaudiKernel/Incident.h"
#include "GaudiKernel/FileIncident.h"
#include "StoreGate/StoreClearedIncident.h"
#include "GaudiKernel/IIncidentSvc.h"
#include <TTree.h>
#include <TFile.h>

// EDM includes
#include "xAODCutFlow/CutBookkeeper.h"
#include "xAODCutFlow/CutBookkeeperContainer.h"
#include "xAODCutFlow/CutBookkeeperAuxContainer.h"
#include "EventBookkeeperMetaData/EventBookkeeperCollection.h"
#include "EventBookkeeperMetaData/EventBookkeeper.h"
#include "xAODEventInfo/EventInfo.h"


namespace {
  const std::string&
  getFileName(const Incident& inc)
  {
    static const std::string s_empty = "";
    const FileIncident* fileInc  = dynamic_cast<const FileIncident*>(&inc);
    return fileInc
      ? fileInc->fileName()
      : s_empty;
  }
}



CutFlowSvc::CutFlowSvc(const std::string& name,
                       ISvcLocator* pSvcLocator ) :
  AthService(name, pSvcLocator),
  m_outMetaDataStore("StoreGateSvc/MetaDataStore", name),
  m_inMetaDataStore("StoreGateSvc/InputMetaDataStore", name),
  m_eventStore("StoreGateSvc", name),
  m_completeCollName(""),
  m_incompleteCollName(""),
  m_skimmingCycle(0),
  m_inputStream(""),
  m_inputCompleteBookTmp(0),
  m_inputCompleteBookAuxTmp(0),
  m_fileCurrentlyOpened(false),
  m_ebkMap(),
  m_alreadyCopiedEventBookkeepers(false)
  //m_alreadyDeterminedCycleNumber(false)
{
  declareProperty("OutputCollName",           m_completeCollName="CutBookkeepers",
    "The default name of the xAOD::CutBookkeeperContainer for fully processed files");
  declareProperty("OutputIncompleteCollName", m_incompleteCollName = "IncompleteCutBookkeepers",
    "The default name of the xAOD::CutBookkeeperContainer for partially processed files");
  declareProperty("SkimmingCycle",            m_skimmingCycle = 0, "The processing step number");
  declareProperty("InputStream",              m_inputStream = "N/A", "The name of the input file stream");
  assert( pSvcLocator );
}



CutFlowSvc::~CutFlowSvc()
{
}



StatusCode
CutFlowSvc::initialize()
{
  ATH_MSG_DEBUG( "Initializing " << name() << " - package version " << PACKAGE_VERSION );

  //Get output MetaData StoreGate
  ATH_CHECK( m_outMetaDataStore.retrieve() );
  //Get input MetaData StoreGate
  ATH_CHECK( m_inMetaDataStore.retrieve() );
  //Get Event StoreGate
  ATH_CHECK( m_eventStore.retrieve() );


  //Get IncidentSvc
  ServiceHandle<IIncidentSvc> incSvc("IncidentSvc", this->name());
  ATH_CHECK( incSvc.retrieve() );
  incSvc->addListener(this, IncidentType::BeginInputFile, 60); // pri has to be < 100 to be after MetaDataSvc.
  incSvc->addListener(this, IncidentType::EndInputFile, 50); // pri has to be > 10 to be before MetaDataSvc.
  incSvc->addListener(this, IncidentType::EndRun, 50);
  incSvc->addListener(this, "StoreCleared", 50);


  // Set up the container structure that we will write to the output file
  xAOD::CutBookkeeperContainer* completeBook(NULL);
  completeBook = new xAOD::CutBookkeeperContainer();
  ATH_CHECK( recordCollection( completeBook, m_completeCollName) );

  xAOD::CutBookkeeperContainer* incompleteBook(NULL);
  incompleteBook = new xAOD::CutBookkeeperContainer();
  ATH_CHECK( recordCollection( incompleteBook, m_incompleteCollName) );

  delete m_inputCompleteBookTmp; m_inputCompleteBookTmp = NULL;
  delete m_inputCompleteBookAuxTmp; m_inputCompleteBookAuxTmp = NULL;
  //Note, do not sg->record inputCompleteTmp, this is internal
  m_inputCompleteBookTmp = new xAOD::CutBookkeeperContainer();
  // Take care of the peculiarities of the new xAOD EDM, i.e., create the needed AuxStore...
  m_inputCompleteBookAuxTmp =  new xAOD::CutBookkeeperAuxContainer();
  // ...and connect the two
  m_inputCompleteBookTmp->setStore( m_inputCompleteBookAuxTmp );

  m_fileCurrentlyOpened = false;


  // Determine the skimming cycle number that we should use now from the input file
  ATH_MSG_VERBOSE("Have currently the cycle number = " << m_skimmingCycle );
  ATH_CHECK( this->determineCycleNumberFromOldInput("EventBookkeepers") );
  ATH_CHECK( this->determineCycleNumberFromOldInput("IncompleteEventBookkeepers") );
  ATH_CHECK( this->determineCycleNumberFromInput(m_completeCollName) );
  ATH_CHECK( this->determineCycleNumberFromInput(m_incompleteCollName) );
  ATH_MSG_VERBOSE("Will use as currently cycle number = " << m_skimmingCycle );

  return StatusCode::SUCCESS;
}




StatusCode CutFlowSvc::determineCycleNumberFromInput( const std::string& collName )
{
  ATH_MSG_DEBUG("calling determineCycleNumberFromInput('" << collName
                  << "')... have currently cycle number = " << m_skimmingCycle );

  // Try to get CutBookkeepers from the input file
  if ( m_inMetaDataStore->contains<xAOD::CutBookkeeperContainer>(collName) ) {
    ATH_MSG_VERBOSE("Found xAOD::CutBookkeeperContainer in input MetaStore with name: " << collName);

    // There can always only be a single object in the input store. As the store
    // is connected to just a single input file.
    const xAOD::CutBookkeeperContainer* constColl = 0;
    ATH_CHECK( m_inMetaDataStore->retrieve( constColl, collName ) );
    xAOD::CutBookkeeperContainer* tmpColl = const_cast<xAOD::CutBookkeeperContainer*>(constColl);
    if ( !(tmpColl->hasStore()) ) {
      ATH_MSG_VERBOSE("Setting store of xAOD::CutBookkeeperContainer");
      // Get also the auxilliary store
      // const SG::IConstAuxStore* auxColl = 0;
      const xAOD::CutBookkeeperAuxContainer* auxColl = 0;
      ATH_CHECK( m_inMetaDataStore->retrieve(auxColl, collName+"Aux.") );
      tmpColl->setConstStore( auxColl );
    }
    // Now, iterate over all CutBookkeepers and search for the highest cycle number
    int maxCycle=0;
    for ( std::size_t i=0; i<tmpColl->size(); ++i ) {
      // Get the current old EBK
      const xAOD::CutBookkeeper* cbk = tmpColl->at(i);
      int inCycle = cbk->cycle();
      if (inCycle > maxCycle) maxCycle = inCycle;
    }
    m_skimmingCycle = std::max(m_skimmingCycle,maxCycle+1);
  }

  ATH_MSG_DEBUG("done calling determineCycleNumberFromInput('" << collName
                  << "')... have now cycle number = " << m_skimmingCycle );
  return StatusCode::SUCCESS;
}


StatusCode CutFlowSvc::determineCycleNumberFromOldInput( const std::string& collName )
{
  ATH_MSG_DEBUG("calling determineCycleNumberFromOldInput('" << collName
                  << "')... have currently cycle number = " << m_skimmingCycle );
  // Now check if there are old-style EventBookkeepers in the input file, using the old default name
  if ( m_inMetaDataStore->contains< ::EventBookkeeperCollection >(collName) ) {
    //get list of input metadata stores(!)
    std::list<SG::ObjectWithVersion< ::EventBookkeeperCollection > > allVersions;
    ATH_CHECK( m_inMetaDataStore->retrieveAllVersions(allVersions, collName) );

    //now get all corresponding lists...
    for ( std::list<SG::ObjectWithVersion< ::EventBookkeeperCollection> >::const_iterator
          iter = allVersions.begin();
          iter != allVersions.end();
          ++iter ) {
      //const DataHandle<xAOD::CutBookkeeperContainer> tmpColl = iter->dataObject;
      const ::EventBookkeeperCollection* tmpColl = iter->dataObject;
      // Now, iterate over all old-style EventBookkeepers and search for the highest cycle number
      for ( std::size_t i=0; i<tmpColl->size(); ++i ) {
        // Get the current old EBK
        const ::EventBookkeeper* oldEBK = tmpColl->at(i);
        int inCycle = oldEBK->getCycle();
        if ( inCycle > m_skimmingCycle ){ m_skimmingCycle = inCycle + 1; }
      }
    }
  }

  ATH_MSG_DEBUG("done calling determineCycleNumberFromOldInput('" << collName
                  << "')... have now cycle number = " << m_skimmingCycle );
  return StatusCode::SUCCESS;
}




CutIdentifier CutFlowSvc::registerFilter( const std::string& name,
                                          const std::string& description )
{
  ATH_MSG_DEBUG("calling registerFilter(" << name << ", " << description << ")" );

  xAOD::CutBookkeeper* tmpEBK = new xAOD::CutBookkeeper();
  tmpEBK->setName(name);
  tmpEBK->setDescription(description);
  tmpEBK->setInputStream(m_inputStream);
  tmpEBK->setCycle(m_skimmingCycle);
  CutIdentifier cutID = tmpEBK->uniqueIdentifier();

  // Get the complete bookkeeper collection of the meta-data store
  xAOD::CutBookkeeperContainer* completeBook(NULL);
  ATH_CHECK( m_outMetaDataStore->retrieve( completeBook, m_completeCollName) );

  // Let's see if an CutBookkeeper of this name already exists
  ATH_MSG_VERBOSE("in registerFilter(" << name << ", " << description << "): "
                  << "Going to search if this CutBookkeeper already exists" );
  bool existsAlready=false;
  for ( std::size_t i=0; i<completeBook->size(); ++i ) {
    xAOD::CutBookkeeper* ebk = completeBook->at(i);
    if( tmpEBK->isEqualTo( ebk) ) {
      ATH_MSG_DEBUG("The CutBookkeeper with name " << name
                    << " and cutID " << cutID << " already exists... not adding!" );
      existsAlready = true;
      cutID = ebk->uniqueIdentifier();
      break;
    }
  }

  ATH_MSG_VERBOSE("in registerFilter(" << name << ", " << description << "): "
                  << "Found it: " << existsAlready );

  // If this CutBookkeeper already exists, delete the temporary
  // and return the existing cutID
  if ( existsAlready ) {
    delete tmpEBK;
    return cutID;
  }

  // If it is a new CutBookkeeper, add it to the container
  ATH_MSG_DEBUG( "You are DECLARING a new filter of name " << name
                 << " and cutID " << cutID );
  completeBook->push_back(tmpEBK);

  // Fill the map
  m_ebkMap[cutID] = tmpEBK;

  ATH_MSG_VERBOSE("done calling registerFilter(" << name << ", " << description << ")" );
  return cutID;
}




// This method is probably only called by the DecionsSvc
CutIdentifier CutFlowSvc::registerTopFilter( const std::string& name,
                                             const std::string& description,
                                             unsigned int logic,
                                             const std::string& outputStream ) {
  ATH_MSG_DEBUG("calling registerTopFilter(" << name << ", " << description
                << ", " << logic << ", " << outputStream << ")" );
  // Call the registerFilter method and get the correct CutBookkeeper
  // from the returned cutID
  CutIdentifier cutID = this->registerFilter( name, description );
  xAOD::CutBookkeeper* ebk = this->getCutBookkeeper(cutID);
  if ( !ebk ) {
    ATH_MSG_ERROR( "(file: " __FILE__ << ", line: " << __LINE__ << ") "
                   << "Couldn't find CutBookkeeper with cutID " << cutID );
    return 0;
  }

  //Then set the logic and outputStream of the relevant CutBookkeeper
  // toFind->setDescription(description);
  ebk->setCutLogic(xAOD::CutBookkeeper::CutLogic(logic));
  ebk->setTopFilter(true);
  ebk->addOutputStreamForAllUsed(outputStream);

  return cutID;
}




CutIdentifier CutFlowSvc::registerCut( const std::string& name,
                                       const std::string& description,
                                       CutIdentifier originCutID )
{
  ATH_MSG_DEBUG("calling registerCut(" << name << ", " << description
                << ", " << originCutID << ")" );

  // Get the CutBookkeeper of the origin Filter Algorithm/Tool
  xAOD::CutBookkeeper *originEBK = this->getCutBookkeeper(originCutID);
  if ( !originEBK ) {
    ATH_MSG_ERROR( "(file: " __FILE__ << ", line: " << __LINE__ << ") "
                   << "Couldn't find CutBookkeeper with cutID " << originCutID );
    return 0;
  }

  // Call the registerFilter method and get the correct CutBookkeeper
  // from the returned cutID
  CutIdentifier cutID = this->registerFilter( name, description );
  xAOD::CutBookkeeper* ebk = this->getCutBookkeeper(cutID);
  if ( !ebk ) {
    ATH_MSG_ERROR( "(file: " __FILE__ << ", line: " << __LINE__ << ") "
                   << "Couldn't find CutBookkeeper with cutID " << cutID );
    return 0;
  }
  originEBK->addUsedOther( ebk );

  return cutID;
}




CutIdentifier CutFlowSvc::declareUsedOtherFilter( const std::string& name,
                                                  CutIdentifier originCutID )
{
  ATH_MSG_DEBUG("calling declareUsedOtherFilter(" << name << ", " << originCutID << ")" );

  // Get the CutBookkeeper of the origin cut
  xAOD::CutBookkeeper *originEBK = this->getCutBookkeeper(originCutID);
  if ( !originEBK ) {
    ATH_MSG_ERROR( "(file: " << __FILE__ << ", line: " << __LINE__ << ") "
                   << "Couldn't find CutBookkeeper with cutID " << originCutID );
    return 0;
  }

  // Create a temporary CutBookkeeper object
  xAOD::CutBookkeeper* tmpEBK = new xAOD::CutBookkeeper();
  // tmpEBK->makePrivateStore();
  tmpEBK->setName(name);
  tmpEBK->setInputStream(m_inputStream);
  tmpEBK->setCycle(m_skimmingCycle);
  CutIdentifier cutID = tmpEBK->uniqueIdentifier();

  // Get the complete bookkeeper collection of the meta-data store
  xAOD::CutBookkeeperContainer* completeBook(NULL);
  ATH_CHECK( m_outMetaDataStore->retrieve( completeBook, m_completeCollName) );

  // See if the CutBookkeeper already exists or not
  bool existsAlready = false;
  for ( std::size_t i=0; i<completeBook->size(); ++i ) {
    xAOD::CutBookkeeper* ebk = completeBook->at(i);
    if( tmpEBK->isEqualTo( ebk ) ) {
      originEBK->addUsedOther( ebk );
      cutID = ebk->uniqueIdentifier();
      existsAlready = true;
    }
  }

  // If this CutBookkeeper already exists, delete the temporary
  // and return the existing cutID
  if ( existsAlready ) {
    delete tmpEBK;
    return cutID;
  }

  // Otherwise, add the new one to the collection
  tmpEBK->setDescription( "Registered by origin filter" );
  completeBook->push_back( tmpEBK );
  originEBK->addUsedOther( tmpEBK );

  // Fill the map
  m_ebkMap[cutID] = tmpEBK;

  return cutID;
}




void
CutFlowSvc::addEvent( CutIdentifier cutID )
{
  ATH_MSG_VERBOSE("calling addEvent(" << cutID << ")" );

  double evtWeight=1.0;

  const xAOD::EventInfo* evtInfo = 0;
  StatusCode sc = m_eventStore->retrieve(evtInfo);
  if ( sc.isFailure() || NULL == evtInfo ) {
    ATH_MSG_WARNING("Could not retrieve EventInfo from StoreGate  ");
    evtWeight=-1000.;
  } else {
    // Only try to access the mcEventWeight is we are running on Monte Carlo, duhhh!
    if ( evtInfo->eventType(xAOD::EventInfo::IS_SIMULATION) ) {
      evtWeight = evtInfo->mcEventWeight();
    }
  }

  addEvent(cutID,evtWeight);

  return;
}



void
CutFlowSvc::addEvent( CutIdentifier cutID, double weight)
{
  ATH_MSG_VERBOSE("calling addEvent(" << cutID << ", " << weight << ")" );

  xAOD::CutBookkeeper* eb = this->getCutBookkeeper(cutID);
  if ( !eb ) {
    // Get the complete bookkeeper collection of the output meta-data store
    xAOD::CutBookkeeperContainer* completeBook(NULL);
    if( !(m_outMetaDataStore->retrieve( completeBook, m_completeCollName)).isSuccess() ) {
      ATH_MSG_ERROR( "Could not retrieve complete CutBookkeepers from output MetaDataStore." );
      return;
    }


    // Iterate over the complete bookkeepers and update the cutID-to-bookkeeper map
    ATH_MSG_DEBUG( "addEvent: Going to re-populate the map. Have "
                   << completeBook->size() << " CutBookkeepers"
                   << " and skimming cycle " << m_skimmingCycle
                   << " and input Stream name " << m_inputStream.value() );
    xAOD::CutBookkeeperContainer::iterator iter    = completeBook->begin();
    xAOD::CutBookkeeperContainer::iterator iterEnd = completeBook->end();
    for ( ; iter != iterEnd; ++iter ) {
      xAOD::CutBookkeeper* ebk = *iter;
      CutIdentifier cutID2 = ebk->uniqueIdentifier();
      ATH_MSG_VERBOSE( "addEvent: Have CutBookkeeper with"
                       << " skimming cycle " << ebk->cycle()
                       << " and input Stream name " << ebk->inputStream()
                       << " and logic " << ebk->cutLogic()
                       << " isTopFilter " << ebk->isTopFilter()
                       << " and cutID " << cutID2
                       << " and name " << ebk->name() );
      if ( m_skimmingCycle == ebk->cycle() ) {
        if ( m_inputStream.value() == ebk->inputStream() ) {
          CutIDMap_t::iterator mapIter = m_ebkMap.find(cutID2);
          ATH_MSG_DEBUG( "BeginInputFile: Have CutBookkeeper with"
                         << " cutID " << cutID2
                         <<  " and name " << ebk->name() );
          if ( mapIter != m_ebkMap.end() ) { // we found this CutBookkeeper in the existing map
            (*mapIter).second = ebk;
          }
        }
      }
    } // End: Loop over all complete CutBookkeepers
    eb = this->getCutBookkeeper(cutID);
  }
  if ( !eb ) {
    ATH_MSG_ERROR ( "Got a zero pointer to an CutBookkeeper with cutID="
                    << cutID << "! This should never happen..." );
    return;
  }
  ATH_MSG_VERBOSE( "addEvent: have cutID " << cutID
                   << " and CutBookkeeper name " << eb->name() );
  eb->addNAcceptedEvents(1);
  eb->addSumOfEventWeights(weight);
  eb->addSumOfEventWeightsSquared(weight*weight);
  return;
}






//__________________________________________________________________________
void CutFlowSvc::handle( const Incident& inc )
{
  ATH_MSG_VERBOSE( "Start incident " << inc.type() << ", fileOpened " << m_fileCurrentlyOpened );

  //CLEARING METADATA STORE
  //Things to do:
  // 1) transer tmp complete CutBookkeepers from input to output, and clear the tmp list
  if ( inc.type() == "StoreCleared" ) {
    ATH_MSG_VERBOSE( "Start incident processing " << inc.type() << ", fileOpened " << m_fileCurrentlyOpened );
    const StoreClearedIncident* sinc = dynamic_cast<const StoreClearedIncident*>(&inc);
    if ( sinc != 0 && sinc->store() == &*m_outMetaDataStore ) {
      ATH_MSG_DEBUG( "StoreCleared: Going to re-initialize the map" );
      // Set all CutBookkeeper pointers in the internal map to zero
      for ( CutIDMap_t::iterator mapIter = m_ebkMap.begin(),
            mapIterEnd = m_ebkMap.end();
            mapIter != mapIterEnd;
            ++mapIter ) {
        (*mapIter).second = 0;
      }
    }
    return;
  }


  //OPENING NEW INPUT FILE
  //Things to do:
  // 1) note that a file is currently opened
  // 2) Load CutBookkeepers from input file
  //    2a) if incomplete from input, directly propagate to output
  //    2b) if complete from input, wait for EndInputFile to decide what to do in output

  // Get the incomplete bookkeeper collection of the meta-data store
  xAOD::CutBookkeeperContainer* incompleteBook(NULL);
  if( !(m_outMetaDataStore->retrieve( incompleteBook, m_incompleteCollName)).isSuccess() ) {
    ATH_MSG_ERROR( "Could not retrieve incomplete CutBookkeepers from output MetaDataStore" );
    return;
  }

  if ( inc.type() == IncidentType::BeginInputFile ) {
    m_fileCurrentlyOpened = true;

    // Copy the existing cut bookkeepers from the input file to the output
    m_alreadyCopiedEventBookkeepers = false;
    ATH_MSG_DEBUG( "Start incident processing " << inc.type() << ", fileOpened " << m_fileCurrentlyOpened );
    if( !(updateCollFromInputStore(m_inputCompleteBookTmp, m_completeCollName) ).isSuccess() ) {
      ATH_MSG_ERROR( "Could not update complete CutBookkeepers from input" );
      return;
    }
    if( !(updateCollFromInputStore(incompleteBook,m_incompleteCollName) ).isSuccess() ) {
      ATH_MSG_ERROR( "Could not update incomplete CutBookkeepers from input" );
      return;
    }
  }


  // Get the complete bookkeeper collection of the output meta-data store
  xAOD::CutBookkeeperContainer* completeBook(NULL);
  if( !(m_outMetaDataStore->retrieve( completeBook, m_completeCollName) ).isSuccess() ) {
    ATH_MSG_ERROR( "Could not get complete CutBookkeepers from output MetaDataStore" );
    return;
  }

  //CLOSING INPUT FILE
  //Things to do:
  // 1) note that no file is currently opened
  if ( inc.type() == IncidentType::EndInputFile ) {
    m_fileCurrentlyOpened = false;
    ATH_MSG_DEBUG( "Start incident processing " << inc.type() << ", fileOpened " << m_fileCurrentlyOpened );
    ATH_MSG_DEBUG("In EndInputFile: updating container with completeBook of size=" << completeBook->size()
                  << ", and m_inputCompleteBookTmp with size=" << m_inputCompleteBookTmp->size() );
    if( !(this->updateContainer( completeBook, m_inputCompleteBookTmp )).isSuccess() ) {
      ATH_MSG_ERROR( "Could not update complete CutBookkeepers from input at EndInputFile" );
      return;
    }
    ATH_MSG_DEBUG("Now clearing the inputCompleteBookTmp... currently, it has size=" << m_inputCompleteBookTmp->size() );
    m_inputCompleteBookTmp->clear();
  }

  //TERMINATING THE JOB (EndRun)
  //Things to do:
  // 1) Create new incomplete CutBookkeepers if relevant
  // 2) Print cut flow summary
  // 3) Write root file if requested
  if ( inc.type() == IncidentType::EndRun ) {
    ATH_MSG_DEBUG( "Start incident processing " << inc.type() << ", fileOpened " << m_fileCurrentlyOpened );
    if ( m_fileCurrentlyOpened ) {
      //Document which file caused the incompletion
      std::string fileName = ::getFileName(inc);
      std::string postfix = " (incomplete from " + fileName + ")";
      ATH_MSG_DEBUG("In EndRun with open file: " << fileName);
      ATH_MSG_VERBOSE("Have inputCompleteBookTmp container with size=" << m_inputCompleteBookTmp->size() );
      for ( std::size_t i=0; i<m_inputCompleteBookTmp->size(); ++i ) {
        std::string newDescrip = m_inputCompleteBookTmp->at(i)->description()
                                 + postfix;
        m_inputCompleteBookTmp->at(i)->setDescription( newDescrip );
      }
      //Then update incompleteBook
      if( !(this->updateContainer( incompleteBook, m_inputCompleteBookTmp ) ).isSuccess() ) {
        ATH_MSG_ERROR( "Could not update incomplete CutBookkeepers from input at EndRun" );
        return;
      }
    }

    // Clear the internal map
    m_ebkMap.clear();
  }
  ATH_MSG_DEBUG( "End incident " << inc.type() << " fileOpened " << m_fileCurrentlyOpened );
  return;
}




void
CutFlowSvc::print()
{
  return;
}






StatusCode
CutFlowSvc::updateCollFromInputStore(xAOD::CutBookkeeperContainer* coll,
                                     const std::string &collName)
{
  ATH_MSG_DEBUG("calling updateCollFromInputStore(" << collName << ") with coll-size=" << coll->size() );

  if ( m_inMetaDataStore->contains<xAOD::CutBookkeeperContainer>(collName) ) {
    ATH_MSG_VERBOSE("Input MetaData contains type xAOD::CutBookkeeperContainer with name" << collName);

    // There can always only be a single object in the input store. As the store
    // is connected to just a single input file.
    const xAOD::CutBookkeeperContainer* tmpColl = 0;
    ATH_CHECK( m_inMetaDataStore->retrieve( tmpColl, collName ) );

    // Check that we succeeded:
    if( ! tmpColl->hasStore() ) {
       ATH_MSG_FATAL( "Object doesn't have an auxiliary store" );
       return StatusCode::FAILURE;
    }

    //...and update coll with each list.
    ATH_CHECK( this->updateContainer(coll, tmpColl) );
  }
  // Now check if there are old-style EventBookkeepers in the input file
  else if ( m_inMetaDataStore->contains< ::EventBookkeeperCollection >(collName) ) {
    ATH_MSG_VERBOSE("Input MetaData contains type EventBookkeeperCollection with name" << collName);
    //get list of input metadata stores(!)
    std::list<SG::ObjectWithVersion< ::EventBookkeeperCollection > > allVersions;
    ATH_CHECK( m_inMetaDataStore->retrieveAllVersions(allVersions, collName) );

    //now get all corresponding lists...
    for ( std::list<SG::ObjectWithVersion< ::EventBookkeeperCollection> >::const_iterator
            iter = allVersions.begin();
          iter != allVersions.end();
          ++iter ) {
      //const DataHandle<xAOD::CutBookkeeperContainer> tmpColl = iter->dataObject;
      const ::EventBookkeeperCollection* tmpColl = iter->dataObject;
      //...and update coll with each list.
      ATH_CHECK( this->updateContainerFromOldEDM(coll, tmpColl) );
    }
  }
  // Now check if there are old-style EventBookkeepers in the input file, using the old default name
  else if ( m_inMetaDataStore->contains< ::EventBookkeeperCollection >("EventBookkeepers") ) {
    if (!m_alreadyCopiedEventBookkeepers) {
      ATH_MSG_VERBOSE("Input MetaData contains type EventBookkeeperCollection with name EventBookkeepers");
      m_alreadyCopiedEventBookkeepers = true;
      //get list of input metadata stores(!)
      std::list<SG::ObjectWithVersion< ::EventBookkeeperCollection > > allVersions;
      ATH_CHECK( m_inMetaDataStore->retrieveAllVersions(allVersions, "EventBookkeepers") );

      //now get all corresponding lists...
      for ( std::list<SG::ObjectWithVersion< ::EventBookkeeperCollection> >::const_iterator
            iter = allVersions.begin();
            iter != allVersions.end();
            ++iter ) {
        //const DataHandle<xAOD::CutBookkeeperContainer> tmpColl = iter->dataObject;
        const ::EventBookkeeperCollection* tmpColl = iter->dataObject;
        //...and update coll with each list.
        ATH_CHECK( this->updateContainerFromOldEDM(coll, tmpColl) );
      }
    }
    else {
      ATH_MSG_VERBOSE("Already copied EventBookkeeperCollection with name EventBookkeepers for this input file");
    }
  }
  else {
    ATH_MSG_INFO( "Cannot find xAOD::CutBookkeeperContainer "
                  << "or an old-style EventBookkeeperCollection "
                  << "with name " << collName << " in the input file.");
  }
  return StatusCode::SUCCESS;
}


namespace {


xAOD::CutBookkeeper*
resolveLink (const xAOD::CutBookkeeper* old,
             xAOD::CutBookkeeperContainer& contToUpdate,
             const xAOD::CutBookkeeperContainer& otherCont,
             const std::vector<size_t>& otherIndices)
{
  {
    xAOD::CutBookkeeperContainer::iterator matchIter = 
      std::find( contToUpdate.begin(),
                 contToUpdate.end(),
                 old );
    if (matchIter != contToUpdate.end())
      return *matchIter;
  }

  {
    xAOD::CutBookkeeperContainer::const_iterator matchIter = 
      std::find( otherCont.begin(),
                 otherCont.end(),
                 old );
    if (matchIter != contToUpdate.end()) {
      size_t pos = matchIter - otherCont.begin();
      return contToUpdate[otherIndices[pos]];
    }
  }

  // If we didn't find it, we need to add it
  xAOD::CutBookkeeper* newEBK = new xAOD::CutBookkeeper();
  if ( newEBK->usingPrivateStore() ) { newEBK->releasePrivateStore(); }
  newEBK->makePrivateStore(old);
  contToUpdate.push_back( newEBK );
  return newEBK;
}


} // anonymous namespace


StatusCode
CutFlowSvc::updateContainer( xAOD::CutBookkeeperContainer* contToUpdate,
                             const xAOD::CutBookkeeperContainer* otherCont ) {
  ATH_MSG_DEBUG("calling updateContainer(...)" );
  ATH_MSG_VERBOSE("Have container to update with size=" << contToUpdate->size()
                  << ", and other container with size=" << otherCont->size() );

  size_t origSize = contToUpdate->size();

  // Vector of indices in contToUpdate of elements in otherCont.
  std::vector< std::size_t > otherIndices (otherCont->size());
  // Loop through otherCont.
  // If element already in contToUpdate, update event counts, otherwise create new element
  for ( std::size_t i=0; i<otherCont->size(); ++i ) {
    const xAOD::CutBookkeeper* otherEBK = otherCont->at(i);
    ATH_MSG_VERBOSE("Looping through otherCont at index " << i);
    ATH_MSG_VERBOSE("Have otherEBK with: name=" << otherEBK->name()
                    << ", cycle=" << otherEBK->cycle()
                    << ", nAcceptedEvents=" << otherEBK->nAcceptedEvents()
                    << ", inputStream=" << otherEBK->inputStream() );


    // Loop through the container to be updated (contToUpdate) and see if we find a match
    bool foundEBKToUpdate(false);
    for ( std::size_t j=0; j<contToUpdate->size(); ++j ) {
      xAOD::CutBookkeeper* ebkToUpdate = contToUpdate->at(j);
      // Check if they are identical, if so, update; else add otherEBK
      if ( otherEBK->isEqualTo(ebkToUpdate) ) {
        ebkToUpdate->setPayload( ebkToUpdate->payload() + otherEBK->payload() );
        otherIndices[i] = j;
        foundEBKToUpdate = true;
        break;
      }
    } // End: Inner loop over contToUpdate
    if (!foundEBKToUpdate) {
      xAOD::CutBookkeeper* newEBK = new xAOD::CutBookkeeper();
      if ( newEBK->usingPrivateStore() ) { newEBK->releasePrivateStore(); }
      newEBK->makePrivateStore(otherEBK);
      contToUpdate->push_back( newEBK );
      std::size_t ebIdx = newEBK->index();
      otherIndices[i] = ebIdx;
    }
  } // End: Outer loop over contToUpdate

  // Now, we still need to fix the cross-referencing of the newly added CutBookkkeepers
  for ( std::size_t i=origSize; i<contToUpdate->size(); ++i ) {
    xAOD::CutBookkeeper* ebkToModify = contToUpdate->at(i);

    // Parent check
    if ( ebkToModify->hasParent() ) {
      const xAOD::CutBookkeeper* oldParent = ebkToModify->parent();
      const xAOD::CutBookkeeper* newParent = resolveLink (oldParent,
                                                          *contToUpdate,
                                                          *otherCont,
                                                          otherIndices);
      ebkToModify->setParent (newParent);
    } // Done fixing parent

    // Children check
    std::vector< xAOD::CutBookkeeper* > newChildren;
    for ( std::size_t oldIdx=0; oldIdx<ebkToModify->nChildren(); ++oldIdx ) {
      const xAOD::CutBookkeeper* oldEBK = ebkToModify->child(oldIdx);
      newChildren.push_back (resolveLink (oldEBK,
                                          *contToUpdate,
                                          *otherCont,
                                          otherIndices));
    } // Done fixing children
    ebkToModify->setChildren (newChildren);

    // Used others check
    std::vector< xAOD::CutBookkeeper* > newOthers;
    for ( std::size_t oldIdx=0; oldIdx<ebkToModify->nUsedOthers(); ++oldIdx ) {
      const xAOD::CutBookkeeper* oldEBK = ebkToModify->usedOther(oldIdx);
      newOthers.push_back (resolveLink (oldEBK,
                                        *contToUpdate,
                                        *otherCont,
                                        otherIndices));
    } // Done fixing used others
    ebkToModify->setUsedOthers (newOthers);

    // Siblings check
    std::vector< xAOD::CutBookkeeper* > newSiblings;
    for ( std::size_t oldIdx=0; oldIdx<ebkToModify->nSiblings(); ++oldIdx ) {
      const xAOD::CutBookkeeper* oldEBK = ebkToModify->sibling(oldIdx);
      newSiblings.push_back (resolveLink (oldEBK,
                                          *contToUpdate,
                                          *otherCont,
                                          otherIndices));
    } // Done fixing siblings
    ebkToModify->setSiblings (newSiblings);
  } // Done fixing all cross references
  return StatusCode::SUCCESS;
}




StatusCode CutFlowSvc::updateContainerFromOldEDM( xAOD::CutBookkeeperContainer* contToUpdate,
                                                  const EventBookkeeperCollection* otherContOldEDM ) {
  // Helper class to update a container with information from another one from the old EDM
  ATH_MSG_DEBUG("calling updateContainerFromOldEDM(contToUpdate, otherContOldEDM)"
                << " with sizes=(" << contToUpdate->size() << "," << otherContOldEDM->size() << ")");

  // Create a new CutBookkeeperContainer to hold the transferred objects
  // Note, do not sg->record, this is internal
  xAOD::CutBookkeeperContainer otherCont;
  // Take care of the peculiarities of the new xAOD EDM, i.e., create the needed AuxStore...
  xAOD::CutBookkeeperAuxContainer otherAuxCont;
  // ...and connect both
  otherCont.setStore( &otherAuxCont );

  // Now, iterate over all old-style EventBookkeepers and create new ones
  for ( std::size_t i=0; i<otherContOldEDM->size(); ++i ) {
    // Get the current old EBK
    const ::EventBookkeeper* oldEBK = otherContOldEDM->at(i);

    // Create a new CutBookkeeper
    xAOD::CutBookkeeper* newEBK = new xAOD::CutBookkeeper();
    otherCont.push_back( newEBK );

    // Update the container
    ATH_CHECK( this->updateContainerFromOldEDM( &otherCont, newEBK, oldEBK ) );
  }

  // Now, we can pass this one on to the standard merging method
  ATH_CHECK( this->updateContainer( contToUpdate, &otherCont ) );
  ATH_MSG_VERBOSE("Resulting size of contToUpdate=" << contToUpdate->size() << ", and of otherCont=" << otherCont.size() );

  return StatusCode::SUCCESS;
}




StatusCode CutFlowSvc::updateContainerFromOldEDM( xAOD::CutBookkeeperContainer* contToUpdate,
                                                  xAOD::CutBookkeeper* newEBK,
                                                  const EventBookkeeper* oldEBK,
                                                  const xAOD::CutBookkeeper* parentEBK ) {
  // Helper class to update a container with information from another one from the old EDM
  ATH_MSG_DEBUG("calling updateContainerFromOldEDM(contToUpdate, newEBK, oldEBK, parentEBK)" );
  ATH_MSG_VERBOSE("Old EBK has: name=" << oldEBK->getName()
                  << ", cycle=" << oldEBK->getCycle()
                  << ", nAcceptedEvents=" << oldEBK->getNAcceptedEvents()
                  << ", inputStream=" << oldEBK->getInputStream()
                  << ", outputStream=" << oldEBK->getOutputStream() );

  // Set all the properties of this new CutBookkeeper
  newEBK->setName( oldEBK->getName() );
  newEBK->setDescription( oldEBK->getDescription() );
  newEBK->setInputStream( oldEBK->getInputStream() );
  newEBK->addOutputStream( oldEBK->getOutputStream() );
  newEBK->setCycle( oldEBK->getCycle() );
  newEBK->setNAcceptedEvents( oldEBK->getNAcceptedEvents() );
  newEBK->setSumOfEventWeights( oldEBK->getNWeightedAcceptedEvents() );
  if ( parentEBK ) { newEBK->setParent( parentEBK ); }

  // Deal with the translation of the logic
  const std::string& oldLogic = oldEBK->getLogic();
  if ( oldLogic == "Accept" ) { newEBK->setCutLogic( xAOD::CutBookkeeper::CutLogic::ACCEPT ); }
  else if ( oldLogic == "Require" ) { newEBK->setCutLogic( xAOD::CutBookkeeper::CutLogic::REQUIRE ); }
  else if ( oldLogic == "Veto" ) { newEBK->setCutLogic( xAOD::CutBookkeeper::CutLogic::VETO ); }
  else if ( oldLogic == "Other" ) { newEBK->setCutLogic( xAOD::CutBookkeeper::CutLogic::OTHER ); }
  else if ( oldLogic == "AllEvents" ) { newEBK->setCutLogic( xAOD::CutBookkeeper::CutLogic::ALLEVENTSPROCESSED ); }
  else { newEBK->setCutLogic( xAOD::CutBookkeeper::CutLogic::UNKNOWN ); }

  // Deal with the translation of the old children to the new used others
  const std::vector<EventBookkeeper*>* oldChildren = oldEBK->getChildrenEventBookkeepers();
  for ( std::size_t i=0; i<oldChildren->size(); ++i ) {
    ATH_MSG_VERBOSE("Updating child number " << i);
    // Get the current old child
    const ::EventBookkeeper* oldChild = oldChildren->at(i);

    // Create a new CutBookkeeper
    xAOD::CutBookkeeper* newOther = new xAOD::CutBookkeeper();
    contToUpdate->push_back( newOther );

    // Add this new other to the existing CutBookkeeper
    newEBK->addUsedOther( newOther );

    // Do this iteratively
    ATH_CHECK( this->updateContainerFromOldEDM( contToUpdate, newOther, oldChild, newEBK ) );
  }
  ATH_MSG_DEBUG("Done with calling updateContainerFromOldEDM(contToUpdate, newEBK, oldEBK, parentEBK)" );
  return StatusCode::SUCCESS;
}


StatusCode
CutFlowSvc::stop()
{
  const DataHandle<xAOD::CutBookkeeperContainer> coll;
  std::string name = "CutBookkeepers";
  if (m_outMetaDataStore->retrieve(coll,name).isSuccess()) {
    for(xAOD::CutBookkeeperContainer::const_iterator it = coll->begin();
                                          it != coll->end(); it++) {
      ATH_MSG_INFO("CutBookkeeper " << name << " " << (*it)->inputStream() << " " << (*it)->name() << " Cyc=" << (*it)->cycle() << " N=" << (*it)->nAcceptedEvents() << " weight^2=" << (*it)->sumOfEventWeightsSquared());
    }
  }
  name = "IncompleteCutBookkeepers";
  if (m_outMetaDataStore->retrieve(coll,name).isSuccess()) {
    for(xAOD::CutBookkeeperContainer::const_iterator it = coll->begin();
                                          it != coll->end(); it++) {
      ATH_MSG_INFO("CutBookkeeper " << name << " " << (*it)->inputStream() << " " << (*it)->name() << " Cyc=" << (*it)->cycle() << " N=" << (*it)->nAcceptedEvents() << " weight^2=" << (*it)->sumOfEventWeightsSquared());
    }
  }
  return StatusCode::SUCCESS;
}

StatusCode
CutFlowSvc::finalize()
{
  ATH_MSG_DEBUG( "Finalizing " << name() << " - package version " << PACKAGE_VERSION );
  return StatusCode::SUCCESS;
}



StatusCode
CutFlowSvc::recordCollection( xAOD::CutBookkeeperContainer * coll,
                              const std::string &collName) {
  ATH_MSG_VERBOSE("calling recordCollection(..., " << collName << ")" );

  //Expected that CutFlowSvc is the only one allowed to record *coll
  if( m_outMetaDataStore->contains<xAOD::CutBookkeeperContainer>(collName) ) {
    ATH_MSG_ERROR( "xAOD::CutBookkeeperContainer " << collName
      << " already exists in output MetaData store. This is unexpected." );
    return StatusCode::FAILURE;
  }

  // Record the container
  ATH_CHECK( m_outMetaDataStore->record(coll, collName) );
  ATH_MSG_VERBOSE("Recorded xAOD::CutBookkeeperContainer " << collName);


  // Take care of the peculiarities of the new xAOD EDM, i.e., create the needed AuxStore
  xAOD::CutBookkeeperAuxContainer* auxCont = new xAOD::CutBookkeeperAuxContainer;
  coll->setStore( auxCont ); //gives it a new associated aux container

  // Record the aux container
  ATH_CHECK( m_outMetaDataStore->record( auxCont, collName+"Aux." ) );
  ATH_MSG_VERBOSE( "Recorded xAOD::CutBookkeeperContainer " << collName << "Aux." );

  return StatusCode::SUCCESS;
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
  }
  // Interface is not directly availible: try out a base class
  return AthService::queryInterface( riid, ppvi );
}



void
CutFlowSvc::setFilterDescription( CutIdentifier cutID,
                                  const std::string& descr )
{
  ATH_MSG_DEBUG("calling setFilterDescription(" << cutID << ", " << descr << ")" );

  xAOD::CutBookkeeper* ebk = this->getCutBookkeeper(cutID);
  ebk->setDescription(descr);
  return;
}
