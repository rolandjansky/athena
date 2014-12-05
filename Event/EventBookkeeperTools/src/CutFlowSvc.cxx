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
  m_ebkMap()
{
  declareProperty("OutputCollName",           m_completeCollName="CutBookkeepers",
    "The default name of the xAOD::CutBookkeeperContainer for fully processed files");
  declareProperty("OutputIncompleteCollName", m_incompleteCollName = "IncompleteCutBookkeepers",
    "The default name of the xAOD::CutBookkeeperContainer for partially processed files");
  declareProperty("SkimmingCycle",            m_skimmingCycle = -1, "The processing step number");
  declareProperty("InputStream",              m_inputStream = "N/A", "The name of the input file stream");
  declareProperty("printStream",              m_printStream = "",
    "Empty string means Print() is not called at EndRun, 'any' will Print() all streams" );
  declareProperty("printVirtualFilters",      m_printVirtual = "no", "allowed values: yes/no" );
  declareProperty("writeRootFileNamed",       m_writeRootFileNamed = "",
    "Empty string means no file is produced, anything else will trigger the file creation" );
  declareProperty("writeTxtFileNamed",        m_writeTxtFileNamed = ""); // not yet implemented
  assert( pSvcLocator );

  m_inputCompleteBookTmp = NULL;
  m_fileCurrentlyOpened  = false;
}



CutFlowSvc::~CutFlowSvc()
{
}



StatusCode
CutFlowSvc::initialize()
{
  // ATH_MSG_VERBOSE("== initialize ==");
  ATH_MSG_DEBUG( "Initializing " << name() << " - package version " << PACKAGE_VERSION );

  xAOD::CutBookkeeperContainer* completeBook(NULL);
  completeBook = new xAOD::CutBookkeeperContainer();
  ATH_CHECK( recordCollection( completeBook, m_completeCollName) );

  xAOD::CutBookkeeperContainer* incompleteBook(NULL);
  incompleteBook = new xAOD::CutBookkeeperContainer();
  ATH_CHECK( recordCollection( incompleteBook, m_incompleteCollName) );

  delete m_inputCompleteBookTmp; m_inputCompleteBookTmp = NULL;
  //Note, do not sg->record inputCompleteTmp, this is internal
  m_inputCompleteBookTmp = new xAOD::CutBookkeeperContainer();
  // Take care of the peculiarities of the new xAOD EDM, i.e., create the needed AuxStore
  m_inputCompleteBookTmp->setStore( new xAOD::CutBookkeeperAuxContainer );

  //Get output MetaData StoreGate
  ATH_CHECK( m_outMetaDataStore.retrieve() );

  //Get input MetaData StoreGate
  ATH_CHECK( m_inMetaDataStore.retrieve() );

  //Get Event StoreGate
  ATH_CHECK( m_eventStore.retrieve() );

  //Get IncidentSvc
  ServiceHandle<IIncidentSvc> incSvc("IncidentSvc", this->name());
  ATH_CHECK( incSvc.retrieve() );
  incSvc->addListener(this, "BeginInputFile", 60); // pri has to be < 100 to be after MetaDataSvc.
  incSvc->addListener(this, "EndInputFile", 50); // pri has to be > 10 to be before MetaDataSvc.
  incSvc->addListener(this, "EndRun", 50);
  incSvc->addListener(this, "StoreCleared", 50);

  m_fileCurrentlyOpened = false;

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
  originEBK->addUsedOther( tmpEBK );
  completeBook->push_back( tmpEBK );

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
      CutIdentifier cutID = ebk->uniqueIdentifier();
      ATH_MSG_VERBOSE( "addEvent: Have CutBookkeeper with"
                       << " skimming cycle " << ebk->cycle()
                       << " and input Stream name " << ebk->inputStream()
                       << " and logic " << ebk->cutLogic()
                       << " isTopFilter " << ebk->isTopFilter()
                       << " and cutID " << cutID
                       << " and name " << ebk->name() );
      if ( m_skimmingCycle == ebk->cycle() ) {
        if ( m_inputStream.value() == ebk->inputStream() ) {
          CutIDMap_t::iterator mapIter = m_ebkMap.find(cutID);
          ATH_MSG_DEBUG( "BeginInputFile: Have CutBookkeeper with"
                         << " cutID " << cutID
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

  if ( inc.type() == "BeginInputFile" ) {
    m_fileCurrentlyOpened = true;

    if( !(updateCollFromInputStore(incompleteBook,m_incompleteCollName) ).isSuccess() ) {
      ATH_MSG_ERROR( "Could not update incomplete CutBookkeepers from input" );
      return;
    }
    if( !(updateCollFromInputStore(m_inputCompleteBookTmp, m_completeCollName) ).isSuccess() ) {
      ATH_MSG_ERROR( "Could not update complete CutBookkeepers from input" );
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
  if ( inc.type() == "EndInputFile" ) {
    m_fileCurrentlyOpened = false;
    if( !(this->updateContainer( completeBook, m_inputCompleteBookTmp )).isSuccess() ) {
      ATH_MSG_ERROR( "Could not update complete CutBookkeepers from input at EndInputFile" );
      return;
    }
    m_inputCompleteBookTmp->clear();
  }

  //TERMINATING THE JOB (EndRun)
  //Things to do:
  // 1) Create new incomplete CutBookkeepers if relevant
  // 2) Print cut flow summary
  // 3) Write root file if requested
  if ( inc.type() == "EndRun" ) {
    if ( m_fileCurrentlyOpened ) {
      //Document which file caused the incompletion
      std::string fileName = ::getFileName(inc);
      std::string postfix = " (incomplete from " + fileName + ")";
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

    // // Write a flat ROOT file
    // if(m_writeRootFileNamed.value().length()>0){
    //   TFile *F = new TFile(m_writeRootFileNamed.value().c_str(),"recreate");
    //   TTree *t = dumpCutFlowToTTree("CutFlow");
    //   t->Print();
    //   F->Write();
    //   delete F;
    // }

    // Clear the internal map
    m_ebkMap.clear();

    // // Print some info to the log file
    // if(m_printStream.value().length()>0){ print(); }
  }
  ATH_MSG_DEBUG( "End incident " << inc.type() << " fileOpened " << m_fileCurrentlyOpened );
  return;
}




void
CutFlowSvc::print()
{
  // std::cout
  //   << "\n\n"
  //   << "              * * * * CutFlowSvc SUMMARY * * * *\n"
  //   << "\n"
  //   << "-------------------------------------------------------------------------\n"
  //   << "Cut-flow information printed as: \n"
  //   << "NAME  INPUTSTREAM  OUTPUTSTREAM  LOGIC  CYCLE  (#CHILDREN) : "
  //   << "DESCRIPTION ==> #AccEvents  wAccEvents\n"
  //   << "...where:\n"
  //   << "* NAME == Filter name;\n"
  //   << "* INPUTSTREAM == Name of the stream to which this filter is applied;\n"
  //   << "* OUTPUTSTREAM == Name of the stream produced at the end of the current"
  //   << " skimming cycle;\n"
  //   << "* LOGIC == Type of Filter:  Top, Child, Accept, Veto, Reject, N/A;\n"
  //   << "* CYCLE == Skimming cycle of the filter;\n"
  //   << "* CHILDREN == Number of children cuts/filters. These are the cuts/filters"
  //   << " that together make up the current filter;\n"
  //   << "* DECRIPTION == Small description about the filter, which may be equal"
  //   << " to the association of its child filters;\n"
  //   << "* AccEvents == Number of accepted events;\n"
  //   << "* wAccEvents ==  Weighted number of accepted events.\n"
  //   << "-------------------------------------------------------------------------"
  //   << std::endl;
  //
  // // Get the complete bookkeeper collection of the meta-data store
  // xAOD::CutBookkeeperContainer* completeBook(NULL);
  // ATH_CHECK( m_outMetaDataStore->retrieve( completeBook, m_completeCollName) );
  //
  // // Get the incomplete bookkeeper collection of the meta-data store
  // xAOD::CutBookkeeperContainer* incompleteBook(NULL);
  // ATH_CHECK( m_outMetaDataStore->retrieve( incompleteBook, m_incompleteCollName) );
  //
  // //Sanity checks
  // if (!incompleteBook->empty()) {
  //   ATH_MSG_WARNING("There are INCOMPLETE CutBookkeepers. "
  //                   << "The cut flow of these EB cannot be trusted." << endreq
  //                   << "To avoid this problem, make sure to process ALL "
  //                   << "the events from the input file(s) in the same job.");
  //   for (std::size_t i=0; i<incompleteBook->size(); i++) {
  //     incompleteBook->at(i)->PrintFamily("incomplete: ");
  //   }
  // } else{
  //   std::cout << "No incomplete CutBookkeepers: perfect! :-)" << std::endl;
  // }
  // if (completeBook->empty()) {
  //   ATH_MSG_WARNING("There are NO complete CutBookkeepers in this job: unexpected.");
  //   return;
  // }
  // std::cout
  // << "-------------------------------------------------------------------------"
  // << std::endl;
  //
  // //Print options
  // bool printVirtual=false;
  // if(m_printVirtual.value()=="yes"){ printVirtual=true; }
  // else if(m_printVirtual.value()=="no"){ printVirtual=false; }
  // else {
  //   ATH_MSG_WARNING("Unexpected value of printVirtualFilters: "
  //                   <<m_printVirtual.value()<<" (please use 'yes' or 'no').");
  // }
  // std::cout << "Print options: \n"
  //           << " *printStream: " << m_printStream.value()
  //           << "\n"
  //           << " *print virtual filters: " << m_printVirtual.value()
  //           << std::endl;
  // if (m_skimmingCycle>-1) {
  //   std::cout << " *SkimmingCycle: " << m_skimmingCycle
  //             << std::endl;
  // } else {
  //   std::cout << " *SkimmingCycle: any" << std::endl;
  // }
  //
  // if (!m_writeRootFileNamed.value().empty()) {
  //   std::cout << " *writeRootFile:" << m_writeRootFileNamed.value()
  //             << std::endl;
  // } else {
  //   std::cout << " *writeRootFile: no" << std::endl;
  // }
  // std::cout << "-------------------------------------------------------------------------"
  //           << std::endl;
  //
  // //Find total number of processed events
  // uint64_t totalEntries=0;
  // double totalWeights=0;
  // static const std::string s_ALLEXECUTEDEVTS = "AllExecutedEvents";
  // for (std::size_t i=0; i<completeBook->size(); i++) {
  //   CutBookkeeper* tmpBook = completeBook->at(i);
  //   if (tmpBook->getName()==s_ALLEXECUTEDEVTS &&
  //       tmpBook->getCycle()==0) {
  //     totalEntries=tmpBook->getNAcceptedEvents();
  //     totalWeights=tmpBook->getNWeightedAcceptedEvents();
  //     break;
  //   }
  // }
  // if(totalEntries>0){
  //   std::cout << "Total number of processed events: "<< totalEntries << "\n"
  //             << "   |-->sum of weights:            " << totalWeights << "\n"
  //             << std::endl;
  // }
  //
  // //Now print filters as specified by options
  // std::cout << "Filter(s) specified by options:" << std::endl;
  // for (std::size_t i=0; i<completeBook->size(); i++) {
  //   CutBookkeeper* tmpBook = completeBook->at(i);
  //   bool cycleOK=(m_skimmingCycle<0 || tmpBook->getCycle()==m_skimmingCycle);
  //   bool virtualOK=(printVirtual || tmpBook->getInputStream()!="Virtual");
  //   bool streamOK=(m_printStream.value()=="any" ||
  //                  tmpBook->getOutputStream()==m_printStream.value());
  //   if(cycleOK && virtualOK && streamOK){
  //     tmpBook->PrintFamily();
  //     if(!tmpBook->getChildrenCutBookkeepers()->empty()){
  //       std::cout<< std::endl;
  //     }
  //   }
  // }
  // std::cout << std::endl;
  return;
}




// TTree*
// CutFlowSvc::dumpCutFlowToTTree( const char* treeName )
// {
//   // Get the complete bookkeeper collection of the meta-data store
//   xAOD::CutBookkeeperContainer* completeBook(NULL);
//   if( !(m_outMetaDataStore->retrieve( completeBook, m_completeCollName)).isSuccess() )
//     {
//       ATH_MSG_ERROR( "(file: " __FILE__ << ", line: " << __LINE__ << ") "
//                      << "Couldn't find xAOD::CutBookkeeperContainer with name " << m_completeCollName
//                      << " in output meta-data store" );
//       return 0;
//     }
//
//   // Get the incomplete bookkeeper collection of the meta-data store
//   xAOD::CutBookkeeperContainer* incompleteBook(NULL);
//   if( !(m_outMetaDataStore->retrieve( incompleteBook, m_incompleteCollName)).isSuccess() )
//     {
//       ATH_MSG_ERROR( "(file: " __FILE__ << ", line: " << __LINE__ << ") "
//                      << "Couldn't find xAOD::CutBookkeeperContainer with name " << m_incompleteCollName
//                      << " in output meta-data store" );
//       return 0;
//     }
//
//   //Get a copy of completeBook with flat strucutre (instead of tree structure)
//   xAOD::CutBookkeeperContainer* collFLAT = completeBook->GetCopyWithFlatStructure();
//   xAOD::CutBookkeeperContainer* collFLAT_i = incompleteBook->GetCopyWithFlatStructure();
//
//   //Now map collFLAT to ntuple TTree
//   int CutFlow_Nb=collFLAT->size();
//   const std::size_t ressz = collFLAT->size() + collFLAT_i->size();
//   std::vector<std::string> cf_name; cf_name.reserve(ressz);
//   std::vector<std::string> cf_inputstream; cf_inputstream.reserve(ressz);
//   std::vector<std::string> cf_outputstream; cf_outputstream.reserve(ressz);
//   std::vector<std::string> cf_description; cf_description.reserve(ressz);
//   std::vector<std::string> cf_logic; cf_logic.reserve(ressz);
//   std::vector<ULong_t> cf_nAcceptedEvents; cf_nAcceptedEvents.reserve(ressz);
//   std::vector<Double_t> cf_nWeightedAcceptedEvents; cf_nWeightedAcceptedEvents.reserve(ressz);
//   std::vector<Int_t> cf_isComplete; cf_isComplete.reserve(ressz);
//   std::vector<Int_t> cf_cycle; cf_cycle.reserve(ressz);
//   std::vector<Int_t> cf_parentIndex; cf_parentIndex.reserve(ressz);
//   std::vector<Int_t> cf_nbChildren; cf_nbChildren.reserve(ressz);
//   std::vector< std::vector<UInt_t> > cf_childrenIndices; cf_childrenIndices.reserve(ressz);
//
//   //Loop over all complete EBs and tag them as such
//   for(unsigned int i=0; i<collFLAT->size(); i++){
//     CutBookkeeper* tmp = collFLAT->at(i);
//     ATH_MSG_DEBUG("Complete EB from collFLAT: "<<tmp->getName());
//     cf_name.push_back(tmp->getName());
//     cf_inputstream.push_back(tmp->getInputStream());
//     cf_outputstream.push_back(tmp->getOutputStream());
//     cf_description.push_back(tmp->getDescription());
//     cf_logic.push_back(tmp->getLogic());
//     cf_nAcceptedEvents.push_back(tmp->getNAcceptedEvents());
//     cf_nWeightedAcceptedEvents.push_back(tmp->getNWeightedAcceptedEvents());
//     cf_cycle.push_back(tmp->getCycle());
//     cf_isComplete.push_back(1);
//     cf_parentIndex.push_back(tmp->m_parentIndex);
//     cf_nbChildren.push_back(tmp->m_childrenIndices->size());
//     cf_childrenIndices.push_back
//       (std::vector<UInt_t>(tmp->m_childrenIndices->begin(),
//                            tmp->m_childrenIndices->end()));
//   }
//
//   //Loop over all incomplete EBs and tag them as such
//   for(unsigned int i=0; i<collFLAT_i->size(); i++){
//     CutBookkeeper* tmp = collFLAT_i->at(i);
//     ATH_MSG_DEBUG("Incomplete EB from collFLAT: "<<tmp->getName());
//     cf_name.push_back(tmp->getName());
//     cf_inputstream.push_back(tmp->getInputStream());
//     cf_outputstream.push_back(tmp->getOutputStream());
//     cf_description.push_back(tmp->getDescription());
//     cf_logic.push_back(tmp->getLogic());
//     cf_nAcceptedEvents.push_back(tmp->getNAcceptedEvents());
//     cf_nWeightedAcceptedEvents.push_back(tmp->getNWeightedAcceptedEvents());
//     cf_cycle.push_back(tmp->getCycle());
//     cf_isComplete.push_back(0);
//     cf_parentIndex.push_back(tmp->m_parentIndex);
//     cf_nbChildren.push_back(tmp->m_childrenIndices->size());
//     cf_childrenIndices.push_back
//       (std::vector<UInt_t>(tmp->m_childrenIndices->begin(),
//                            tmp->m_childrenIndices->end()));
//   }
//
//   //Fill output TTree
//   TTree* t = new TTree(treeName,treeName);
//   t->Branch("CutFlow_Nb", &CutFlow_Nb,"CutFlow_Nb/I");
//   t->Branch("name", &cf_name);
//   t->Branch("inputstream", &cf_inputstream);
//   t->Branch("outputstream", &cf_outputstream);
//   t->Branch("description", &cf_description);
//   t->Branch("logic", &cf_logic);
//   t->Branch("nAcceptedEvents", &cf_nAcceptedEvents);
//   t->Branch("nWeightedAcceptedEvents", &cf_nWeightedAcceptedEvents);
//   t->Branch("cycle", &cf_cycle);
//   t->Branch("isComplete", &cf_isComplete);
//   t->Branch("parentIndex", &cf_parentIndex);
//   t->Branch("nbChildren", &cf_nbChildren);
//   t->Branch("childrenIndices", &cf_childrenIndices);
//   t->Fill();
//
//   //delete local collFLAT
//   delete collFLAT;
//   delete collFLAT_i;
//
//   //The caller now owns the TTree
//   return t;
// }




// void
// CutFlowSvc::loadCutFlowFromTTree( TTree* )
// {
//   //This function remains to be filled...
//   //It should load the content of it into completeBook
//
//   //The caller now owns coll
//   return;
// }




StatusCode
CutFlowSvc::updateCollFromInputStore(xAOD::CutBookkeeperContainer* coll,
                                     const std::string &collName)
{
  ATH_MSG_DEBUG("calling updateCollFromInputStore(" << collName << ")" );

  if ( m_inMetaDataStore->contains<xAOD::CutBookkeeperContainer>(collName) ) {

    //get list of input metadata stores(!)
    std::list<SG::ObjectWithVersion<xAOD::CutBookkeeperContainer> > allVersions;
    ATH_CHECK( m_inMetaDataStore->retrieveAllVersions(allVersions, collName) );

    //now get all corresponding lists...
    for ( std::list<SG::ObjectWithVersion<xAOD::CutBookkeeperContainer> >::const_iterator
            iter = allVersions.begin();
          iter != allVersions.end();
          ++iter ) {
      //const DataHandle<xAOD::CutBookkeeperContainer> tmpColl = iter->dataObject;
      const xAOD::CutBookkeeperContainer* tmpColl = iter->dataObject;
      //...and update coll with each list.
      ATH_CHECK( this->updateContainer(coll, tmpColl) );
    }
  }
  // Now check if there are old-style EventBookkeepers in the input file
  else if ( m_inMetaDataStore->contains< ::EventBookkeeperCollection >(collName) ) {
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
      // } else if (inputStoreHasFlatTTree()) {
  //   ATH_MSG_INFO("Flat TTree CutFlow history detected! Parsing for " << collName << "...");
  //   if (collName == std::string(m_incompleteCollName)) {
  //     fillIncompleteCollectionFromInputStore(coll);
  //   } else if (collName == std::string(m_completeCollName)) {
  //     fillCompleteCollectionFromInputStore(coll);
  //   } else {
  //     ATH_MSG_ERROR("Bad collection name: " << collName);
  //     return;
  //   }
  // }
  else {
    ATH_MSG_INFO( "Cannot find xAOD::CutBookkeeperContainer "
                  << "or an old-style EventBookkeeperCollection "
                  << "with name " << collName << " in the input file.");
  }
  return StatusCode::SUCCESS;
}

// bool CutFlowSvc::inputStoreHasFlatTTree() const
// {
//   return (m_inMetaDataStore->contains<int>("CutFlowTree/CutFlow_Nb"));
// }

// void CutFlowSvc::fillIncompleteCollectionFromInputStore(xAOD::CutBookkeeperContainer *coll)
// {
//   fillCollectionFromInputStore(coll, 0);
// }

// void CutFlowSvc::fillCompleteCollectionFromInputStore(xAOD::CutBookkeeperContainer *coll)
// {
//   fillCollectionFromInputStore(coll, 1);
// }

// void CutFlowSvc::fillCollectionFromInputStore(xAOD::CutBookkeeperContainer *coll, int wantIsComplete)
// {
//   xAOD::CutBookkeeperContainer *tmpColl = new xAOD::CutBookkeeperContainer();
//
//   FillEBCFromFlat filler(m_inMetaDataStore, tmpColl, wantIsComplete);
//   filler.fill();
//
//   coll->UpdateFromColl(tmpColl);
//   delete tmpColl;
//   ATH_MSG_INFO("fillCollectionFromInputStore: final coll->size() = " << coll->size());
// }




StatusCode
CutFlowSvc::updateContainer( xAOD::CutBookkeeperContainer* contToUpdate,
                             const xAOD::CutBookkeeperContainer* otherCont ) {
  ATH_MSG_DEBUG("calling updateContainer(...)" );

  // Create an vector of indices of all the newly transferred CutBookkeepers
  std::vector< std::size_t > newEBKIndices;
  // Loop through otherCont.
  // If element already in contToUpdate, update event counts, otherwise create new element
  for ( std::size_t i=0; i<otherCont->size(); ++i ) {
    const xAOD::CutBookkeeper* otherEBK = otherCont->at(i);

    // Loop through the container to be updated (contToUpdate) and see if we find a match
    for ( std::size_t j=0; j<contToUpdate->size(); ++j ) {
      xAOD::CutBookkeeper* ebkToUpdate = contToUpdate->at(j);
      // Check if they are identical, if so, update; else add otherEBK
      if ( otherEBK->isEqualTo(ebkToUpdate) ) {
        ebkToUpdate->setPayload( ebkToUpdate->payload() + otherEBK->payload() );
      }
      else {
        xAOD::CutBookkeeper* newEBK = new xAOD::CutBookkeeper();
        if ( newEBK->usingPrivateStore() ) { newEBK->releasePrivateStore(); }
        newEBK->makePrivateStore(otherEBK);
        contToUpdate->push_back( newEBK );
        std::size_t ebIdx = newEBK->index();
        newEBKIndices.push_back(ebIdx);
      }
    } // End: Inner loop over contToUpdate
  } // End: Outer loop over contToUpdate

  // Now, we still need to fix the cross-referencing of the newly added CutBookkkeepers
  for ( std::size_t i=0; i<newEBKIndices.size(); ++i ) {
    std::size_t otherIndex = newEBKIndices.at(i);
    xAOD::CutBookkeeper* ebkToModify = contToUpdate->at(otherIndex);

    // Parent check
    if ( ebkToModify->hasParent() ) {
      const xAOD::CutBookkeeper* oldParent = ebkToModify->parent();
      xAOD::CutBookkeeperContainer::iterator matchIter =  std::find( contToUpdate->begin(),
                                                                       contToUpdate->end(),
                                                                       oldParent );
      // If we found it, we can modify
      if ( matchIter != contToUpdate->end() ) {
        ATH_MSG_VERBOSE("Updating ElementLink to parent");
        ebkToModify->setParent( *matchIter );
      }
      // If we didn't find it, we need to add it
      else {
        ATH_MSG_VERBOSE("Adding new parent");
        xAOD::CutBookkeeper* newEBK = new xAOD::CutBookkeeper();
        if ( newEBK->usingPrivateStore() ) { newEBK->releasePrivateStore(); }
        newEBK->makePrivateStore(oldParent);
        contToUpdate->push_back( newEBK );
        ebkToModify->setParent( newEBK );
      }
    } // Done fixing parent

    // Children check
    for ( std::size_t oldIdx=0; oldIdx<ebkToModify->nChildren(); ++oldIdx ) {
      const xAOD::CutBookkeeper* oldEBK = ebkToModify->child(oldIdx);
      xAOD::CutBookkeeperContainer::iterator matchIter =  std::find( contToUpdate->begin(),
                                                                       contToUpdate->end(),
                                                                       oldEBK );
      // If we found it, we can modify
      if ( matchIter != contToUpdate->end() ) {
        ATH_MSG_VERBOSE("Updating ElementLink to child");
        ebkToModify->addChild( *matchIter );
      }
      // If we didn't find it, we need to add it
      else {
        ATH_MSG_VERBOSE("Adding new child");
        xAOD::CutBookkeeper* newEBK = new xAOD::CutBookkeeper();
        if ( newEBK->usingPrivateStore() ) { newEBK->releasePrivateStore(); }
        newEBK->makePrivateStore(oldEBK);
        contToUpdate->push_back( newEBK );
        ebkToModify->addChild( newEBK );
      }
    } // Done fixing children

    // Used others check
    for ( std::size_t oldIdx=0; oldIdx<ebkToModify->nUsedOthers(); ++oldIdx ) {
      const xAOD::CutBookkeeper* oldEBK = ebkToModify->usedOther(oldIdx);
      xAOD::CutBookkeeperContainer::iterator matchIter =  std::find( contToUpdate->begin(),
                                                                       contToUpdate->end(),
                                                                       oldEBK );
      // If we found it, we can modify
      if ( matchIter != contToUpdate->end() ) {
        ATH_MSG_VERBOSE("Updating ElementLink to usedOther");
        ebkToModify->addUsedOther( *matchIter );
      }
      // If we didn't find it, we need to add it
      else {
        ATH_MSG_VERBOSE("Adding new usedOther");
        xAOD::CutBookkeeper* newEBK = new xAOD::CutBookkeeper();
        if ( newEBK->usingPrivateStore() ) { newEBK->releasePrivateStore(); }
        newEBK->makePrivateStore(oldEBK);
        contToUpdate->push_back( newEBK );
        ebkToModify->addUsedOther( newEBK );
      }
    } // Done fixing used others

    // Siblings check
    for ( std::size_t oldIdx=0; oldIdx<ebkToModify->nSiblings(); ++oldIdx ) {
      const xAOD::CutBookkeeper* oldEBK = ebkToModify->sibling(oldIdx);
      xAOD::CutBookkeeperContainer::iterator matchIter =  std::find( contToUpdate->begin(),
                                                                       contToUpdate->end(),
                                                                       oldEBK );
      // If we found it, we can modify
      if ( matchIter != contToUpdate->end() ) {
        ATH_MSG_VERBOSE("Updating ElementLink to sibling");
        ebkToModify->addSibling( *matchIter );
      }
      // If we didn't find it, we need to add it
      else {
        ATH_MSG_VERBOSE("Adding new sibling");
        xAOD::CutBookkeeper* newEBK = new xAOD::CutBookkeeper();
        if ( newEBK->usingPrivateStore() ) { newEBK->releasePrivateStore(); }
        newEBK->makePrivateStore(oldEBK);
        contToUpdate->push_back( newEBK );
        ebkToModify->addSibling( newEBK );
      }
    } // Done fixing siblings
  } // Done fixing all cross references
  return StatusCode::SUCCESS;
}




StatusCode CutFlowSvc::updateContainerFromOldEDM( xAOD::CutBookkeeperContainer* contToUpdate,
                                                  const EventBookkeeperCollection* otherContOldEDM ) {
  // Helper class to update a container with information from another one from the old EDM
  ATH_MSG_DEBUG("calling updateContainerFromOldEDM(...)" );

  // Create a new CutBookkeeperContainer to hold the transferred objects
  // Note, do not sg->record, this is internal
  xAOD::CutBookkeeperContainer* otherCont = new xAOD::CutBookkeeperContainer();
  // Take care of the peculiarities of the new xAOD EDM, i.e., create the needed AuxStore
  otherCont->setStore( new xAOD::CutBookkeeperAuxContainer );

  // Now, iterate over all old-style EventBookkeepers and create new ones
  for ( std::size_t i=0; i<otherContOldEDM->size(); ++i ) {
    // Get the current old EBK
    const ::EventBookkeeper* oldEBK = otherContOldEDM->at(i);

    // Create a new CutBookkeeper
    xAOD::CutBookkeeper* newEBK = new xAOD::CutBookkeeper();
    otherCont->push_back( newEBK );

    // Update the container
    ATH_CHECK( this->updateContainerFromOldEDM( otherCont, newEBK, oldEBK ) );
  }

  // Now, we can pass this one on to the standard merging method
  ATH_CHECK( this->updateContainer( contToUpdate, otherCont ) );

  // Clean up
  if ( otherCont ) { delete otherCont; }

  return StatusCode::SUCCESS;
}




StatusCode CutFlowSvc::updateContainerFromOldEDM( xAOD::CutBookkeeperContainer* contToUpdate,
                                                  xAOD::CutBookkeeper* newEBK,
                                                  const EventBookkeeper* oldEBK,
                                                  const xAOD::CutBookkeeper* parentEBK ) {
  // Helper class to update a container with information from another one from the old EDM
  ATH_MSG_DEBUG("calling updateContainerFromOldEDM(xAOD::CutBookkeeperContainer* contToUpdate,"
                                               << "xAOD::CutBookkeeper* newEBK,"
                                               << "const EventBookkeeper* oldEBK,"
                                               << "const xAOD::CutBookkeeper* parentEBK)" );

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
  return StatusCode::SUCCESS;
}



StatusCode
CutFlowSvc::finalize()
{
  ATH_MSG_DEBUG("calling finalize()" );
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
