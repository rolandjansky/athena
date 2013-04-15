///////////////////////// -*- C++ -*- /////////////////////////////

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// Implementation file for class CutFlowSvc
// Authors: Joao Firmino da Costa <joao.costa@cern.ch> and David Cote <david.cote@cern.ch>
/////////////////////////////////////////////////////////////////// 

//For now, CutFlowSvc only deals with new transient objects while EventBookkeeperMetaDataTool deals with input persistent objects
//Eventually, EventBookkeeperMetaDataTool could be integrated in CutFlowSvc
//By convention, EventBookkeeperMetaDataTool runs before CutFlowSvc (when handling incidents)

#include "CutFlowSvc.h"

#include "GaudiKernel/FileIncident.h"
#include "StoreGate/StoreClearedIncident.h"
#include "GaudiKernel/IIncidentSvc.h"
#include <TTree.h>
#include <TFile.h>



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
  m_EventStore("StoreGateSvc", name),
  m_ebkMap()
{
  declareProperty("OutputCollName",m_completeCollName="EventBookkeepers");
  declareProperty("OutputIncompleteCollName",m_incompleteCollName = "IncompleteEventBookkeepers");
  declareProperty("SkimmingCycle",m_skimmingCycle = -1);
  declareProperty("InputStream",m_inputStream = "N/A");
  declareProperty("printStream",m_printStream = ""); //"" means Print() is not called at EndRun, 'any' will Print() all streams
  declareProperty("printVirtualFilters",m_printVirtual = "no"); //allowed values: yes/no 
  declareProperty("writeRootFileNamed",m_writeRootFileNamed = ""); //"" means no file is produced, anything else will trigger the file creation
  declareProperty("writeTxtFileNamed",m_writeTxtFileNamed = ""); // not yet implemented
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
  ATH_MSG_VERBOSE("== initialize ==");

  EventBookkeeperCollection* completeBook(NULL);
  completeBook = new EventBookkeeperCollection();
  ATH_CHECK( recordCollection( completeBook, m_completeCollName) );

  EventBookkeeperCollection* incompleteBook(NULL);
  incompleteBook = new EventBookkeeperCollection();
  ATH_CHECK( recordCollection( incompleteBook, m_incompleteCollName) );

  delete m_inputCompleteBookTmp; m_inputCompleteBookTmp = NULL;
  //Note, do not sg->record inputCompleteTmp, this is internal
  m_inputCompleteBookTmp = new EventBookkeeperCollection();

  //Get output MetaData StoreGate
  if (!m_outMetaDataStore.retrieve().isSuccess() || 
      NULL == m_outMetaDataStore) {
    ATH_MSG_ERROR("Could not find MetaDataStore");
    return StatusCode::FAILURE;
  }
  //Get input MetaData StoreGate
  if (!m_inMetaDataStore.retrieve().isSuccess() || 
      NULL == m_inMetaDataStore) {
    ATH_MSG_ERROR("Could not find InputMetaDataStore");
    return StatusCode::FAILURE;
  }
 
  
  //Get Event StoreGate
  if (!m_EventStore.retrieve().isSuccess() || 
      NULL == m_EventStore) {
    ATH_MSG_ERROR("Could not find EventStore");
    return StatusCode::FAILURE;
  }
  

  //Get IncidentSvc
  ServiceHandle<IIncidentSvc> incSvc("IncidentSvc", this->name());
  if (!incSvc.retrieve().isSuccess()) {
    ATH_MSG_ERROR("Unable to get the IncidentSvc");
    return StatusCode::FAILURE;
  }
  incSvc->addListener(this, "BeginInputFile", 60); // pri has to be < 100 to be after MetaDataSvc.
  incSvc->addListener(this, "EndInputFile", 50); // pri has to be > 10 to be before MetaDataSvc.
  incSvc->addListener(this, "EndRun", 50);
  incSvc->addListener(this, "StoreCleared", 50);

  m_fileCurrentlyOpened = false;

  return StatusCode::SUCCESS;
}





//EventBookkeeper* 
//CutFlowSvc::selfRegisterFilter(const std::string& name, const std::string& description)
CutIdentifier CutFlowSvc::selfRegisterFilter( const std::string& name,
                                              const std::string& description )
{
  CutIdentifier cutID = this->getCutIdentifier(name);
  EventBookkeeper *toReturn = NULL;

  bool notFound = true;
  EventBookkeeper tmp(name); //dummy eb just to use isEqualTo
  tmp.setCycle(m_skimmingCycle); //cycle is mandatory to use isEqualTo
  
  // Get the complete bookkeeper collection of the meta-data store
  EventBookkeeperCollection* completeBook(NULL);
  if( !(m_outMetaDataStore->retrieve( completeBook, m_completeCollName)).isSuccess() )
    {
      ATH_MSG_ERROR( "(file: " __FILE__ << ", line: " << __LINE__ << ") "
                     << "Couldn't find EventBookkeeperCollection with name " << m_completeCollName
                     << " in output meta-data store" );
      return 0;
    }


  for(unsigned int i = 0; (i<completeBook->size()) && notFound; i++){
    
    if(tmp.isEqualTo(completeBook->at(i))){
      toReturn = completeBook->at(i);	
      if( !(toReturn->m_declaredTopFilter) ){ ATH_MSG_WARNING("You are RE-DECLARING a filter of name "<<name); }
      else{ toReturn->setDescription(description); }
      notFound = false;
      ATH_MSG_DEBUG("You are DECLARING a filter of name " << name << " and cutID " << cutID );
    }
    else{
      std::vector<EventBookkeeper*>* tmpChildColl = completeBook->at(i)->getChildrenEventBookkeepers();
      for(unsigned int j = 0; (j<tmpChildColl->size()) && notFound; j++) {
        if(tmp.isEqualTo(tmpChildColl->at(j))){
          toReturn = tmpChildColl->at(j);
          if( !(toReturn->m_declaredChildFilter) ){ ATH_MSG_WARNING("You are RE-DECLARING a child filter of name "<<name); }
          else{ toReturn->setDescription(description); }
          notFound = false;	
          ATH_MSG_DEBUG("You are DECLARING a child-filter of name " << name << " and cutID " << cutID );
        }
      }
    }
  }
  //delete tmp;

  if(notFound){
    toReturn = new EventBookkeeper(name);
    toReturn->setDescription(description);	  
    toReturn->setCycle(m_skimmingCycle); 
    toReturn->setInputStream(m_inputStream);	  
    completeBook->push_back(toReturn);
    ATH_MSG_DEBUG("You are DECLARING a new filter of name " << name << " and cutID " << cutID );
  }

  // Fill the map
  m_ebkMap[cutID] = toReturn;
  
  return cutID;
}




void CutFlowSvc::declareTopFilter(const std::string& name, const std::string& logic, const std::string& outputStream ){
  //This method is probably only called by the DecionsSvc
  EventBookkeeper *toFind=0;

  bool notFound = true;
  EventBookkeeper tmp(name); //dummy eb just to use isEqualTo
  tmp.setCycle(m_skimmingCycle); //cycle is mandatory to use isEqualTo

  // Get the complete bookkeeper collection of the meta-data store
  EventBookkeeperCollection* completeBook(NULL);
  if( !(m_outMetaDataStore->retrieve( completeBook, m_completeCollName)).isSuccess() )
    {
      ATH_MSG_ERROR( "(file: " __FILE__ << ", line: " << __LINE__ << ") "
                     << "Couldn't find EventBookkeeperCollection with name " << m_completeCollName
                     << " in output meta-data store" );
      return;
    }

  for(unsigned int i=0; i<completeBook->size(); i++){ 
    if(completeBook->at(i)->isEqualTo(&tmp)) {
      toFind = completeBook->at(i);
      notFound = false;
      break;
    }
  }
  //delete tmp;

  if(notFound){ 
    CutIdentifier cutID = selfRegisterFilter(name,"Registered by CutFlowSvc::declareTopFilter.");
    toFind = this->getEventBookkeeper(cutID);
    if ( !toFind )
      {
        ATH_MSG_ERROR( "(file: " __FILE__ << ", line: " << __LINE__ << ") "
                       << "Couldn't find EventBookkeeper with cutID " << cutID );
        return;
      }
    //toFind->m_declaredTopFilter=true;
  }

  //Then set the logic and outputStream of the relevant EventBookkeeper
  toFind->setLogic(logic);
  toFind->setOutputStreamOfFamily(outputStream);
  toFind->m_declaredTopFilter=true;

  return;
}




CutIdentifier CutFlowSvc::registerCut( const std::string& name, const std::string& description, CutIdentifier parentCutID )
{
  ATH_MSG_VERBOSE ( "registerCut: with name " << name << " and parent cutID " << parentCutID );

  // Get the EventBookkeeper of the parent cut
  EventBookkeeper *parentEBK = this->getEventBookkeeper(parentCutID);
  if ( !parentEBK )
    {
      ATH_MSG_ERROR( "(file: " __FILE__ << ", line: " << __LINE__ << ") "
                     << "Couldn't find EventBookkeeper with cutID " << parentCutID );
      return 0;
    }

  // Generate the cutID for this cut
  CutIdentifier cutID = this->getCutIdentifier(name);

  EventBookkeeper *toReturn=0;
  bool notFound = true;
  EventBookkeeper *tmp = new EventBookkeeper(name); //dummy eb just to use isEqualTo
  tmp->setCycle(parentEBK->getCycle()); //cycle is mandatory to use isEqualTo
  
  std::vector<EventBookkeeper*>* tmpChildColl = parentEBK->getChildrenEventBookkeepers();
  for(unsigned int j = 0; (j<tmpChildColl->size()) && notFound; j++) {
    if(tmp->isEqualTo(tmpChildColl->at(j))){
      toReturn = tmpChildColl->at(j);
      if( !(toReturn->m_declaredChildFilter) ){ ATH_MSG_WARNING("You are RE-DECLARING a filter of name "<<name); }
      else{ toReturn->setDescription(description); }
      notFound = false;	
    }
  }
  delete tmp;

  if(notFound){ toReturn = parentEBK->AddNewChild(name,description); }

  // Fill the map
  m_ebkMap[cutID] = toReturn;

  return cutID;
}




void CutFlowSvc::declareChildFilter( const std::string& name, CutIdentifier parentCutID )
{
  // Get the EventBookkeeper of the parent cut
  EventBookkeeper *parentEBK = this->getEventBookkeeper(parentCutID);
  if ( !parentEBK )
    {
      ATH_MSG_ERROR( "(file: " __FILE__ << ", line: " << __LINE__ << ") "
                     << "Couldn't find EventBookkeeper with cutID " << parentCutID );
      return;
    }

  CutIdentifier cutID = this->getCutIdentifier(name);
  bool notFound = true;
  EventBookkeeper *tmp = new EventBookkeeper(name); //dummy eb just to use isEqualTo
  tmp->setCycle(parentEBK->getCycle()); //cycle is mandatory to use isEqualTo

  // Get the complete bookkeeper collection of the meta-data store
  EventBookkeeperCollection* completeBook(NULL);
  if( !(m_outMetaDataStore->retrieve( completeBook, m_completeCollName)).isSuccess() )
    {
      ATH_MSG_ERROR( "(file: " __FILE__ << ", line: " << __LINE__ << ") "
                     << "Couldn't find EventBookkeeperCollection with name " << m_completeCollName
                     << " in output meta-data store" );
      return;
    }

  for(unsigned int i=0; (i<completeBook->size()) && notFound; i++){ 
    if(completeBook->at(i)->isEqualTo(tmp)) {
       
      EventBookkeeper* tmp2=0;
      completeBook->swapElement(i,0,tmp2);
      parentEBK->AddChild(tmp2); 
      completeBook->erase(completeBook->begin()+i);
      notFound=false;      
      // Fill the map, if needed
      if ( m_ebkMap[cutID] == 0 ){ m_ebkMap[cutID] = tmp2;}
    }
  }
  delete tmp;
 
  if (notFound){
    EventBookkeeper* tmp2 = parentEBK->AddNewChild(name,"Registered by parent filter");
    tmp2->m_declaredChildFilter=true;
    // Fill the map, if needed
    if ( m_ebkMap[cutID] == 0 ){ m_ebkMap[cutID] = tmp2;}
  }

  return;
}




void
CutFlowSvc::addEvent( CutIdentifier cutID )
{
  double evtWeight=1.0;

  const EventInfo* evtInfo = NULL;
  StatusCode sc = m_EventStore->retrieve(evtInfo);
  if (sc.isFailure() || NULL == evtInfo) {
    ATH_MSG_WARNING("Could not retrieve EventInfo from StoreGate  ");
    evtWeight=-1000.;
  } else {
    evtWeight = evtInfo->event_type()->mc_event_weight();
  }    

  addEvent(cutID,evtWeight);

  return;
}



void
CutFlowSvc::addEvent( CutIdentifier cutID, double weight)
{
  EventBookkeeper* eb = this->getEventBookkeeper(cutID);
  if ( !eb ) {
    // Get the complete bookkeeper collection of the output meta-data store
    EventBookkeeperCollection* completeBook(NULL);
    if( !(m_outMetaDataStore->retrieve( completeBook, m_completeCollName)).isSuccess() )
    {
      ATH_MSG_ERROR( "(file: " __FILE__ << ", line: " << __LINE__ << ") "
                     << "Couldn't find EventBookkeeperCollection with name " << m_completeCollName
                     << " in output meta-data store" );
      return;
    }
    // Iterate over the complete bookkeepers and update the cutID-to-bookkeeper map
    ATH_MSG_DEBUG( "addEvent: Going to re-populate the map. Have "
                   << completeBook->size() << " EventBookkeepers" 
                   << " and skimming cycle " << m_skimmingCycle 
                   << " and input Stream name " << m_inputStream.value() );
    EventBookkeeperCollection::iterator iter    = completeBook->begin();
    EventBookkeeperCollection::iterator iterEnd = completeBook->end();
    for ( ; iter != iterEnd; ++iter ) {
      EventBookkeeper* ebk = *iter;
      CutIdentifier cutID = this->getCutIdentifier( ebk->getName() );
      ATH_MSG_VERBOSE( "addEvent: Have EventBookkeeper with"
                       << " skimming cycle " << ebk->getCycle()
                       << " and input Stream name " << ebk->getInputStream()
                       << " and output Stream name " << ebk->getOutputStream()
                       << " and logic " << ebk->getLogic()
                       << " isTF " << ebk->m_declaredTopFilter << " isCF " << ebk->m_declaredChildFilter
                       << " and cutID " << cutID
                       << " and name " << ebk->getName() );
      if ( m_skimmingCycle == ebk->getCycle() ) {
        if ( m_inputStream.value() == ebk->getInputStream() ) {
          //CutIdentifier cutID = this->getCutIdentifier( ebk->getName() );
          CutIDMap_t::iterator mapIter = m_ebkMap.find(cutID);
          ATH_MSG_DEBUG( "BeginInputFile: Have EventBookkeeper with"
                         << " cutID " << cutID
                         <<  " and name " << ebk->getName() );
          if ( mapIter != m_ebkMap.end() ) { // we found this EventBookkeeper in the existing map
            (*mapIter).second = ebk;
          }
          // Now, also look at all children
          std::vector<EventBookkeeper*>* childVecPtr = ebk->getChildrenEventBookkeepers();
          this->fillMapWithChildren( childVecPtr );
        }
      }
    }
    eb = this->getEventBookkeeper(cutID);
  }
  if ( !eb ) {
    ATH_MSG_ERROR ( "Got a zero pointer to an EventBookkeeper with cutID=" << cutID << "! This should never happen..." );
    return;
  }
  ATH_MSG_VERBOSE( "addEvent: have cutID " << cutID << " and EventBookkeeper name " << eb->getName() );
  eb->addNAcceptedEvents(1);
  eb->addNWeightedAcceptedEvents(weight);
  return;
}



//__________________________________________________________________________
void CutFlowSvc::handle(const Incident& inc) 
{
  ATH_MSG_DEBUG( "Incident " << inc.type() << " fileOpened " << m_fileCurrentlyOpened );

  //CLEARING METADATA STORE
  //Things to do:
  // 1) transer tmp complete EventBookkeepers from input to output, and clear the tmp list
  if (inc.type() == "StoreCleared") {
    const StoreClearedIncident* sinc = dynamic_cast<const StoreClearedIncident*>(&inc);
    if (sinc != 0 && sinc->store() == &*m_outMetaDataStore) {
      ATH_MSG_DEBUG( "StoreCleared: Going to re-initialize the map" );
      // Set all EventBookkeeper pointer in the internal map to zero
      for (CutIDMap_t::iterator mapIter = m_ebkMap.begin(), mapIterEnd = m_ebkMap.end(); mapIter != mapIterEnd; ++mapIter ) {
        (*mapIter).second = 0;
      }
    }
    return;
  }
  //OPENING NEW INPUT FILE
  //Things to do:
  // 1) note that a file is currently opened
  // 2) Load EventBookkeepers from input file
  //    2a) if incomplete from input, directly propagate to output
  //    2b) if complete from input, wait for EndInputFile to decide what to do in output

  // Get the incomplete bookkeeper collection of the meta-data store
  EventBookkeeperCollection* incompleteBook(NULL);
  if( !(m_outMetaDataStore->retrieve( incompleteBook, m_incompleteCollName)).isSuccess() )
    {
      ATH_MSG_ERROR( "(file: " __FILE__ << ", line: " << __LINE__ << ") "
                     << "Couldn't find EventBookkeeperCollection with name " << m_incompleteCollName
                     << " in output meta-data store" );
      return;
    }

  if (inc.type() == "BeginInputFile") {
    m_fileCurrentlyOpened=true;

    updateCollFromInputStore(incompleteBook,m_incompleteCollName);
    updateCollFromInputStore(m_inputCompleteBookTmp, m_completeCollName);
  }


  // Get the complete bookkeeper collection of the output meta-data store
  EventBookkeeperCollection* completeBook(NULL);
  if( !(m_outMetaDataStore->retrieve( completeBook, m_completeCollName)).isSuccess() )
    {
      ATH_MSG_ERROR( "(file: " __FILE__ << ", line: " << __LINE__ << ") "
                     << "Couldn't find EventBookkeeperCollection with name " << m_completeCollName
                     << " in output meta-data store" );
      return;
    }

  //CLOSING INPUT FILE
  //Things to do:
  // 1) note that no file is currently opened
  if (inc.type() == "EndInputFile") {
    m_fileCurrentlyOpened=false;
    completeBook->UpdateFromColl( m_inputCompleteBookTmp );
    m_inputCompleteBookTmp->clear();
  }

  //TERMINATING THE JOB (EndRun)
  //Things to do:
  // 1) Create new incomplete EventBookkeepers if relevant
  // 2) Print cut flow summary
  // 3) Write root file if requested
  if (inc.type() == "EndRun") {
    if(m_fileCurrentlyOpened){ 	
      //Document which file caused the incompletion
      for(unsigned int i=0; i<m_inputCompleteBookTmp->size(); i++){
        std::string fileName= ::getFileName(inc);
        std::string newDescrip = m_inputCompleteBookTmp->at(i)->getDescription() + " (incomplete from "+fileName+")";
        m_inputCompleteBookTmp->at(i)->setDescription( newDescrip );
      }
      //Then update incompleteBook
      incompleteBook->UpdateFromColl( m_inputCompleteBookTmp );
    }

    if(m_writeRootFileNamed.value().length()>0){  
      TFile *F = new TFile(m_writeRootFileNamed.value().c_str(),"recreate");
      TTree *t = dumpCutFlowToTTree("CutFlow");
      t->Print();
      F->Write();
      delete F;      
    }
    m_ebkMap.clear();

    if(m_printStream.value().length()>0){ print(); }
  }
  ATH_MSG_DEBUG("Incident "<<inc.type()<<" fileOpened "<<m_fileCurrentlyOpened);
  return;
}




void
CutFlowSvc::print()
{
  std::cout
    << "\n\n"
    << "              * * * * CutFlowSvc SUMMARY * * * *\n"
    << "\n"
    << "-------------------------------------------------------------------------\n"
    << "Cut-flow information printed as: \n"
    << "NAME  INPUTSTREAM  OUTPUTSTREAM  LOGIC  CYCLE  (#CHILDREN) : DESCRIPTION ==> #AccEvents  wAccEvents\n"
    << "...where:\n"
    << "* NAME == Filter name;\n"
    << "* INPUTSTREAM == Name of the stream to which this filter is applied;\n"
    << "* OUTPUTSTREAM == Name of the stream produced at the end of the current skimming cycle;\n"
    << "* LOGIC == Type of Filter:  Top, Child, Accept, Veto, Reject, N/A;\n"
    << "* CYCLE == Skimming cycle of the filter;\n"
    << "* CHILDREN == Number of children cuts/filters. These are the cuts/filters that together make up the current filter;\n"
    << "* DECRIPTION == Small description about the filter, which may be equal to the association of its child filters;\n"
    << "* AccEvents == Number of accepted events;\n"
    << "* wAccEvents ==  Weighted number of accepted events.\n"
    << "-------------------------------------------------------------------------"
    << std::endl;

  // Get the complete bookkeeper collection of the meta-data store
  EventBookkeeperCollection* completeBook(NULL);
  if( !(m_outMetaDataStore->retrieve( completeBook, m_completeCollName)).isSuccess() )
    {
      ATH_MSG_ERROR( "(file: " __FILE__ << ", line: " << __LINE__ << ") "
                     << "Couldn't find EventBookkeeperCollection with name " << m_completeCollName
                     << " in output meta-data store" );
      return;
    }

  // Get the incomplete bookkeeper collection of the meta-data store
  EventBookkeeperCollection* incompleteBook(NULL);
  if( !(m_outMetaDataStore->retrieve( incompleteBook, m_incompleteCollName)).isSuccess() )
    {
      ATH_MSG_ERROR( "(file: " __FILE__ << ", line: " << __LINE__ << ") "
                     << "Couldn't find EventBookkeeperCollection with name " << m_incompleteCollName
                     << " in output meta-data store" );
      return;
    }

  //Sanity checks
  if (!incompleteBook->empty()) {
    ATH_MSG_WARNING("There are INCOMPLETE EventBookkeepers. The cut flow of these EB cannot be trusted."<<endreq
                    <<"To avoid this problem, make sure to process ALL the events from the input file(s) in the same job.");
    for (std::size_t i=0; i<incompleteBook->size(); i++) { 
      incompleteBook->at(i)->PrintFamily("incomplete: "); 
    }
  } else{
    std::cout << "No incomplete EventBookkeepers: perfect! :-)" << std::endl;
  }
  if (completeBook->empty()) {
    ATH_MSG_WARNING("There are NO complete EventBookkeepers in this job: unexpected.");
    return;
  }
  std::cout << "-------------------------------------------------------------------------" << std::endl;    

  //Print options
  bool printVirtual=false;
  if(m_printVirtual.value()=="yes"){ printVirtual=true; }
  else if(m_printVirtual.value()=="no"){ printVirtual=false; }
  else{ ATH_MSG_WARNING("Unexpected value of printVirtualFilters: "<<m_printVirtual.value()<<" (please use 'yes' or 'no')."); }
  std::cout << "Print options: \n"
            << " *printStream: " << m_printStream.value()
            << "\n"
            << " *print virtual filters: " << m_printVirtual.value()
            << std::endl;
  if (m_skimmingCycle>-1) { 
    std::cout << " *SkimmingCycle: " << m_skimmingCycle
              << std::endl; 
  } else { 
    std::cout << " *SkimmingCycle: any" << std::endl; 
  }
  
  if (!m_writeRootFileNamed.value().empty()) { 
    std::cout << " *writeRootFile:" << m_writeRootFileNamed.value()
              << std::endl;
  } else { 
    std::cout << " *writeRootFile: no" << std::endl;
  }
  std::cout << "-------------------------------------------------------------------------"
            << std::endl;    

  //Find total number of processed events
  uint64_t totalEntries=0;
  double totalWeights=0;
  static const std::string s_ALLEXECUTEDEVTS = "AllExecutedEvents";
  for (std::size_t i=0; i<completeBook->size(); i++) {
    EventBookkeeper* tmpBook = completeBook->at(i);
    if (tmpBook->getName()==s_ALLEXECUTEDEVTS && 
        tmpBook->getCycle()==0) {
      totalEntries=tmpBook->getNAcceptedEvents();
      totalWeights=tmpBook->getNWeightedAcceptedEvents();
      break;
    }
  }
  if(totalEntries>0){
    std::cout << "Total number of processed events: "<< totalEntries << "\n"
              << "   |-->sum of weights:            " << totalWeights << "\n"
              << std::endl;
  }

  //Now print filters as specified by options
  std::cout << "Filter(s) specified by options:" << std::endl;
  for (std::size_t i=0; i<completeBook->size(); i++) {
    EventBookkeeper* tmpBook = completeBook->at(i);
    bool cycleOK=(m_skimmingCycle<0 || tmpBook->getCycle()==m_skimmingCycle);
    bool virtualOK=(printVirtual || tmpBook->getInputStream()!="Virtual");
    bool streamOK=(m_printStream.value()=="any" || 
                   tmpBook->getOutputStream()==m_printStream.value());
    if(cycleOK && virtualOK && streamOK){
      tmpBook->PrintFamily();    
      if(!tmpBook->getChildrenEventBookkeepers()->empty()){ 
        std::cout<< std::endl; 
      }
    }
  }
  std::cout << std::endl;
  return;
}




TTree* 
CutFlowSvc::dumpCutFlowToTTree( const char* treeName )
{
  // Get the complete bookkeeper collection of the meta-data store
  EventBookkeeperCollection* completeBook(NULL);
  if( !(m_outMetaDataStore->retrieve( completeBook, m_completeCollName)).isSuccess() )
    {
      ATH_MSG_ERROR( "(file: " __FILE__ << ", line: " << __LINE__ << ") "
                     << "Couldn't find EventBookkeeperCollection with name " << m_completeCollName
                     << " in output meta-data store" );
      return 0;
    }

  // Get the incomplete bookkeeper collection of the meta-data store
  EventBookkeeperCollection* incompleteBook(NULL);
  if( !(m_outMetaDataStore->retrieve( incompleteBook, m_incompleteCollName)).isSuccess() )
    {
      ATH_MSG_ERROR( "(file: " __FILE__ << ", line: " << __LINE__ << ") "
                     << "Couldn't find EventBookkeeperCollection with name " << m_incompleteCollName
                     << " in output meta-data store" );
      return 0;
    }

  //Get a copy of completeBook with flat strucutre (instead of tree structure)
  EventBookkeeperCollection* collFLAT = completeBook->GetCopyWithFlatStructure();
  EventBookkeeperCollection* collFLAT_i = incompleteBook->GetCopyWithFlatStructure();

  //Now map collFLAT to ntuple TTree
  int CutFlow_Nb=collFLAT->size();
  const std::size_t ressz = collFLAT->size() + collFLAT_i->size();
  std::vector<std::string> cf_name; cf_name.reserve(ressz);
  std::vector<std::string> cf_inputstream; cf_inputstream.reserve(ressz);
  std::vector<std::string> cf_outputstream; cf_outputstream.reserve(ressz);
  std::vector<std::string> cf_description; cf_description.reserve(ressz);
  std::vector<std::string> cf_logic; cf_logic.reserve(ressz);
  std::vector<ULong_t> cf_nAcceptedEvents; cf_nAcceptedEvents.reserve(ressz);
  std::vector<Double_t> cf_nWeightedAcceptedEvents; cf_nWeightedAcceptedEvents.reserve(ressz);
  std::vector<Int_t> cf_isComplete; cf_isComplete.reserve(ressz);
  std::vector<Int_t> cf_cycle; cf_cycle.reserve(ressz);
  std::vector<Int_t> cf_parentIndex; cf_parentIndex.reserve(ressz);
  std::vector<Int_t> cf_nbChildren; cf_nbChildren.reserve(ressz);
  std::vector< std::vector<UInt_t> > cf_childrenIndices; cf_childrenIndices.reserve(ressz);

  //Loop over all complete EBs and tag them as such
  for(unsigned int i=0; i<collFLAT->size(); i++){
    EventBookkeeper* tmp = collFLAT->at(i);
    ATH_MSG_DEBUG("Complete EB from collFLAT: "<<tmp->getName());
    cf_name.push_back(tmp->getName());
    cf_inputstream.push_back(tmp->getInputStream());
    cf_outputstream.push_back(tmp->getOutputStream());
    cf_description.push_back(tmp->getDescription());
    cf_logic.push_back(tmp->getLogic());
    cf_nAcceptedEvents.push_back(tmp->getNAcceptedEvents());
    cf_nWeightedAcceptedEvents.push_back(tmp->getNWeightedAcceptedEvents());
    cf_cycle.push_back(tmp->getCycle());
    cf_isComplete.push_back(1);
    cf_parentIndex.push_back(tmp->m_parentIndex);
    cf_nbChildren.push_back(tmp->m_childrenIndices->size());
    cf_childrenIndices.push_back
      (std::vector<UInt_t>(tmp->m_childrenIndices->begin(),
                           tmp->m_childrenIndices->end()));
  }

  //Loop over all incomplete EBs and tag them as such
  for(unsigned int i=0; i<collFLAT_i->size(); i++){
    EventBookkeeper* tmp = collFLAT_i->at(i);
    ATH_MSG_DEBUG("Incomplete EB from collFLAT: "<<tmp->getName());
    cf_name.push_back(tmp->getName());
    cf_inputstream.push_back(tmp->getInputStream());
    cf_outputstream.push_back(tmp->getOutputStream());
    cf_description.push_back(tmp->getDescription());
    cf_logic.push_back(tmp->getLogic());
    cf_nAcceptedEvents.push_back(tmp->getNAcceptedEvents());
    cf_nWeightedAcceptedEvents.push_back(tmp->getNWeightedAcceptedEvents());
    cf_cycle.push_back(tmp->getCycle());
    cf_isComplete.push_back(0);
    cf_parentIndex.push_back(tmp->m_parentIndex);
    cf_nbChildren.push_back(tmp->m_childrenIndices->size());
    cf_childrenIndices.push_back
      (std::vector<UInt_t>(tmp->m_childrenIndices->begin(),
                           tmp->m_childrenIndices->end()));
  }

  //Fill output TTree
  TTree* t = new TTree(treeName,treeName);
  t->Branch("CutFlow_Nb", &CutFlow_Nb,"CutFlow_Nb/I");
  t->Branch("name", &cf_name);
  t->Branch("inputstream", &cf_inputstream);
  t->Branch("outputstream", &cf_outputstream);
  t->Branch("description", &cf_description);
  t->Branch("logic", &cf_logic);
  t->Branch("nAcceptedEvents", &cf_nAcceptedEvents);
  t->Branch("nWeightedAcceptedEvents", &cf_nWeightedAcceptedEvents);
  t->Branch("cycle", &cf_cycle);
  t->Branch("isComplete", &cf_isComplete);
  t->Branch("parentIndex", &cf_parentIndex);
  t->Branch("nbChildren", &cf_nbChildren);
  t->Branch("childrenIndices", &cf_childrenIndices);
  t->Fill();  

  //delete local collFLAT
  delete collFLAT;
  delete collFLAT_i;

  //The caller now owns the TTree
  return t;
}




void 
CutFlowSvc::loadCutFlowFromTTree( TTree* )
{
  //This function remains to be filled... 
  //It should load the content of it into completeBook
  
  //The caller now owns coll
  return;
}




void 
CutFlowSvc::updateCollFromInputStore(EventBookkeeperCollection* coll, 
                                     const std::string &collName)
{
  if(m_inMetaDataStore->contains<EventBookkeeperCollection>(collName)) {
    
    //get list of input metadata stores(!)
    std::list<SG::ObjectWithVersion<EventBookkeeperCollection> > allVersions;
    StatusCode sc = m_inMetaDataStore->retrieveAllVersions(allVersions, collName);
    if (!sc.isSuccess()){
      ATH_MSG_ERROR("Could not find "<<collName<<" in input metadata store");
      return;
    }
    
    //now get all corresponding lists...
    for (std::list<SG::ObjectWithVersion<EventBookkeeperCollection> >::const_iterator iter = allVersions.begin(); iter != allVersions.end(); iter++) {
      //const DataHandle<EventBookkeeperCollection> tmpColl = iter->dataObject;
      const EventBookkeeperCollection* tmpColl = iter->dataObject;
      //...and update coll with each list.
      coll->UpdateFromColl(tmpColl);
    }
  }
  else {
    ATH_MSG_INFO("Cannot find EventBookkeeperCollection "<<collName<<" in input file.");
  }
  return;
}



StatusCode
CutFlowSvc::finalize()
{
  return StatusCode::SUCCESS;
}



StatusCode 
CutFlowSvc::recordCollection(EventBookkeeperCollection * coll, 
                             const std::string &collName)
{
  //Expected that CutFlowSvc is the only one allowed to record *coll
  if( m_outMetaDataStore->contains<EventBookkeeperCollection>(collName) ) {
    ATH_MSG_ERROR("EventBookkeeperCollection "<<collName<<" already exists in output MetaData store. This is unexpected.");
    return StatusCode::FAILURE;
  }

  StatusCode sc = m_outMetaDataStore->record(coll, collName);
  if (!sc.isSuccess() || 0 == coll) {
    ATH_MSG_ERROR("Could not record EventBookkeeperCollection "<<collName);
    return sc;
  }  
  ATH_MSG_VERBOSE("Recorded EventBookkeeperCollection " << collName);

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
  EventBookkeeper* ebk = this->getEventBookkeeper(cutID);
  ebk->setDescription(descr);
  return;
}


void
CutFlowSvc::fillMapWithChildren( const std::vector<EventBookkeeper*>* childVecPtr )
{
  if ( !childVecPtr )
    {
      ATH_MSG_VERBOSE( "Got a zero pointer to the children of EventBookkeepers." );
    }

  std::size_t nChildren = childVecPtr->size();
  for ( std::size_t i=0; i<nChildren; ++i )
    {
      // Get current child
      EventBookkeeper* child = childVecPtr->at(i);
      if ( !child )
        {
          ATH_MSG_ERROR( "Got a zero pointer to a child of an EventBookkeepers." );
          continue;
        }
      CutIdentifier cutID = this->getCutIdentifier( child->getName() );
      CutIDMap_t::iterator mapIter = m_ebkMap.find(cutID);
      if ( mapIter != m_ebkMap.end() ) { // we found this EventBookkeeper in the existing map
        (*mapIter).second = child;
      }
      // Now, also look at all children of this child
      std::vector<EventBookkeeper*>* childChildVecPtr = child->getChildrenEventBookkeepers();
      this->fillMapWithChildren( childChildVecPtr );
    }
  return;
}
