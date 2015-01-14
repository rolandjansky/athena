/***************************************************************************
                          RoIROD.cxx  -  description
                             -------------------
    begin                : Mon Jul 28 2014
    copyright            : (C) 2014 by Alan Watson
    email                : Alan.Watson@CERN.CH
 ***************************************************************************/



//#include <fstream>
#include <string>

// running in Athena
#include "TrigT1CaloSim/RoIROD.h"

#include "GaudiKernel/MsgStream.h"
#include "EventInfo/EventInfo.h"
#include "EventInfo/EventID.h"

#include "TrigT1Interfaces/TrigT1Interfaces_ClassDEF.h"

#include <algorithm>


LVL1::RoIROD::RoIROD
  ( const std::string& name, ISvcLocator* pSvcLocator ) 
    : Algorithm( name, pSvcLocator ),
    m_storeGate("StoreGateSvc", name), 
    m_eventNumber(0),
    m_emTauRoILocation(TrigT1CaloDefs::CPMTobRoILocation), 
    m_emTauSlinkLocation(TrigT1CaloDefs::EmTauSlinkLocation),
    m_JetRoILocation(TrigT1CaloDefs::JEMTobRoILocation),
    m_energyRoILocation(TrigT1CaloDefs::CMXRoILocation),
    m_jepSlinkLocation(TrigT1CaloDefs::jepSlinkLocation)
{

  declareProperty("EventStore",m_storeGate,"StoreGate Service");
  declareProperty(  "EmTauRoILocation",     m_emTauRoILocation );
  declareProperty(  "EmTauSlinkLocation",   m_emTauSlinkLocation );
  declareProperty(  "JEPSlinkLocation",     m_jepSlinkLocation );
  declareProperty(  "JetRoILocation",       m_JetRoILocation );
  declareProperty(  "EnergyRoILocation",    m_energyRoILocation );

  for (unsigned int i = 0; i<TrigT1CaloDefs::numOfCPRoIRODs;i++){
    m_CPRoIROD[i]=0;
  }
  for (unsigned int i = 0; i<TrigT1CaloDefs::numOfJEPRoIRODs;i++){
    m_jepRoIROD[i]=0;
  }
}

// Destructor
LVL1::RoIROD::~RoIROD() {
} 


//---------------------------------
// initialise() 
//---------------------------------

StatusCode LVL1::RoIROD::initialize()
{

  // We must here instantiate items which can only be made after
  // any job options have been set
  MsgStream log( messageService(), name() ) ;

  //
  // Connect to StoreGate:
  //
  StatusCode sc = m_storeGate.retrieve();
  if ( sc.isFailure() ) {
    log << MSG::ERROR << "Couldn't connect to " << m_storeGate.typeAndName() 
        << endreq;
    return sc;
  } else {
    log << MSG::DEBUG << "Connected to " << m_storeGate.typeAndName() 
        << endreq;
  }

  return StatusCode::SUCCESS ;
}


//---------------------------------
// finalise() 
//---------------------------------

StatusCode LVL1::RoIROD::finalize()
{
   MsgStream log( messageService(), name() ) ;
   log << MSG::INFO << "Finalizing" << endreq;
   return StatusCode::SUCCESS ;
}


//----------------------------------------------
// execute() method called once per event
//----------------------------------------------
//

StatusCode LVL1::RoIROD::execute( )
{
  MsgStream log( messageService(), name() ) ;
  log << MSG::DEBUG << "Executing" << endreq;

  const EventInfo* evt;
  if (StatusCode::SUCCESS == m_storeGate->retrieve(evt)){
    m_eventNumber = evt->event_ID()->event_number();
  }else{
    log << MSG::ERROR<< " Unable to retrieve EventInfo from StoreGate "<< endreq;
  }

  assignVectors();    // create new storage vectors 
  formSlinkObjects(); // Fill storage vectors
  saveSlinkObjects(); // save storage vectors

  return StatusCode::SUCCESS ;
}

/** get ROIwords and form Slink words from them, adding header and tail. */
void LVL1::RoIROD::formSlinkObjects(){
  MsgStream log( messageService(), name() ) ;
  log << MSG::DEBUG << "formSlinkObjects" << endreq;

  /** Set SLink headers */
  for (unsigned int i = 0; i<TrigT1CaloDefs::numOfCPRoIRODs;i++){
    addHeader(m_CPRoIROD[i],0x72,i);//set modID to crate #
  }
  for (unsigned int i = 0; i<TrigT1CaloDefs::numOfJEPRoIRODs;i++){
    addHeader(m_jepRoIROD[i],0x73,i);//set modID to crate #
  }
  
  
  /** Retrieve CPMTobRoIs and copy data to SLink vectors */
  log << MSG::DEBUG<< "Load and process CPM TOBs"<<endreq;
  const t_cpmTobRoIContainer* EMTaus ;
  StatusCode sc1 = m_storeGate->retrieve(EMTaus, m_emTauRoILocation);

  if ( sc1==StatusCode::FAILURE ) {
    log << MSG::DEBUG<< "No CPMTobRoIs found. "<< endreq ;
  }
  else{
    /// Push TOBs to ROD vectors
    /// This version makes no attempt to order them.
    DataVector<CPMTobRoI>::const_iterator it = EMTaus->begin(); 
    for ( ; it != EMTaus->end(); ++it) {
      int crate = (*it)->crate();
      m_CPRoIROD[crate]->push_back( getWord((*it)->roiWord()) );
    }
  }
  
  
  /** Retrieve JEMTobRoIs and copy data to SLink vectors */
  log << MSG::DEBUG<< "Load and process Jet TOBs"<<endreq;
  const t_jemTobRoIContainer* Jets ;
  StatusCode sc2 = m_storeGate->retrieve(Jets, m_JetRoILocation);

  if ( sc2==StatusCode::FAILURE ) {
    log << MSG::DEBUG<< "No JEMTobRoIs found. "<< endreq ;
  }
  else{
    /// Push TOBs to ROD vectors
    /// This version makes no attempt to order them.
    DataVector<JEMTobRoI>::const_iterator it = Jets->begin(); 
    for ( ; it != Jets->end(); ++it) {
      int crate = (*it)->crate();
      m_jepRoIROD[crate]->push_back( getWord((*it)->roiWord()) );
    }
  }
  
  
  /** Retrieve EnergySum RoI(s) and add to SLink record */
  const t_EnergyRoIContainer* energy   ;
  StatusCode sc3 = m_storeGate->retrieve(energy, m_energyRoILocation);

  if  ( sc3==StatusCode::FAILURE ) {
    log << MSG::DEBUG << "No EnergySum RoIs found."<< endreq ;
  }
  else{
    /** Add 6 ET RoIWords to crate 1 ROD data. */
    m_jepRoIROD[1]->push_back(getWord(energy->exWord(LVL1::CMXRoI::NORMAL)));
    m_jepRoIROD[1]->push_back(getWord(energy->eyWord(LVL1::CMXRoI::NORMAL)));
    m_jepRoIROD[1]->push_back(getWord(energy->etWord(LVL1::CMXRoI::NORMAL)));
    m_jepRoIROD[1]->push_back(getWord(energy->exWord(LVL1::CMXRoI::MASKED)));
    m_jepRoIROD[1]->push_back(getWord(energy->eyWord(LVL1::CMXRoI::MASKED)));
    m_jepRoIROD[1]->push_back(getWord(energy->etWord(LVL1::CMXRoI::MASKED)));
  }
  
  
  /** Set SLink tails */
  const unsigned int headerWords=8+1;//8 in Slink head + control word
  
  for (unsigned int i = 0; i<TrigT1CaloDefs::numOfCPRoIRODs;i++){
    addTail(m_CPRoIROD[i], (m_CPRoIROD[i]->size()-headerWords) );
  }
  for (unsigned int i = 0; i<TrigT1CaloDefs::numOfJEPRoIRODs;i++){
    addTail(m_jepRoIROD[i],(m_jepRoIROD[i]->size()-headerWords) );
  }

  return;
}

/** adds slink header as defined in DAQ-98-129*/
void LVL1::RoIROD::addHeader( DataVector<LVL1CTP::SlinkWord>* slink, unsigned int subDetID, unsigned int moduleId){
  slink->push_back(getWord(0xb0f00000));      // beginning of fragment word (control word)
  slink->push_back(getWord(0xee1234ee));       // start of header marker
  slink->push_back(getWord(8));                       // header length
  slink->push_back(getWord(1));                      // format version number
  unsigned int moduleType=0x00;  // ROD
  slink->push_back(getWord( (moduleType<<16)+(subDetID<<8)+moduleId ));  // 1st 8b but source id: CP system
  slink->push_back(getWord(m_eventNumber));       // 8b ECR_count (set to 0) | 24b L1 id = evt number
  slink->push_back( getWord(1 ));                           // 8b BCID
  slink->push_back( getWord(1 ));                           // 8b L1 trig type
  slink->push_back( getWord(0 ));                           // detector evt type}
}

/** add Slink tail */
void LVL1::RoIROD::addTail(DataVector<LVL1CTP::SlinkWord>* slink, unsigned int numberOfDataWords){
  slink->push_back(getWord(0 ));                           // write status word 1
  slink->push_back(getWord(0 ));                           // write status word 2
//^^^ not simulating any errors.
  slink->push_back(getWord(2 ));                           // status block length
  slink->push_back(getWord(numberOfDataWords));            // calculated data block length
  slink->push_back(getWord(1 ));                           // status block position
// ^^^ indicates that data precedes status block
  slink->push_back(getWord(0xe0f00000 ));                  // end of fragment word
}

/** save Slink Objects to SG */
void LVL1::RoIROD::saveSlinkObjects(){
  MsgStream log( messageService(), name() ) ;
  log << MSG::DEBUG<< "saveSlinkObjects"<<endreq;

  /**\todo There must be a better way of doing this, but CERN doesn't seem to have sstream.h*/
  std::string emTauSlinkLocation[4];
  emTauSlinkLocation[0]= m_emTauSlinkLocation+"0";
  emTauSlinkLocation[1]= m_emTauSlinkLocation+"1";
  emTauSlinkLocation[2]= m_emTauSlinkLocation+"2";
  emTauSlinkLocation[3]= m_emTauSlinkLocation+"3";

  for (unsigned int i = 0; i<TrigT1CaloDefs::numOfCPRoIRODs;++i){

    StatusCode sc = m_storeGate->overwrite(m_CPRoIROD[i],emTauSlinkLocation[i],true,false,false);

    if (sc.isSuccess() ){
      log << MSG::DEBUG << "Stored EmTau Slink object at "<< emTauSlinkLocation[i] <<" with "
          <<(m_CPRoIROD[i]->size())<<" words"<< endreq;
    } else {
      log << MSG::ERROR << "Failed to write EmTau Slink object!"<< endreq;
    } // endif
  }

  std::string jepSlinkLocation[2];
  jepSlinkLocation[0]= m_jepSlinkLocation+"0";
  jepSlinkLocation[1]= m_jepSlinkLocation+"1";
  for (unsigned int i = 0; i<TrigT1CaloDefs::numOfJEPRoIRODs;++i){
    StatusCode sc = m_storeGate->overwrite(m_jepRoIROD[i],jepSlinkLocation[i],true,false,false);
    if (sc.isSuccess() ){
      log << MSG::DEBUG << "Stored JetEnergy Slink object at "<< jepSlinkLocation[i] <<" with "
          <<(m_jepRoIROD[i]->size())<<" words"<< endreq;
    } else {
      log << MSG::ERROR << "Failed to write JetEnergy Slink object!"<< endreq;
    } // endif
  }
  
  return;
}


/** Create the object vectors to be stored in SG and clear mult vectors*/
void LVL1::RoIROD::assignVectors(){
  MsgStream log( messageService(), name() ) ;
  log << MSG::DEBUG << "assignVectors" << endreq;

  for (unsigned int i = 0; i<TrigT1CaloDefs::numOfCPRoIRODs;i++){
    m_CPRoIROD[i]=new DataVector<LVL1CTP::SlinkWord>;
  }
  for (unsigned int i = 0; i<TrigT1CaloDefs::numOfJEPRoIRODs;i++){
    m_jepRoIROD[i]=new DataVector<LVL1CTP::SlinkWord>;
  }

}

LVL1CTP::SlinkWord* LVL1::RoIROD::getWord(unsigned int tword){
  return new LVL1CTP::SlinkWord(tword);
}


/** prints out the Slink info. */
void LVL1::RoIROD::dumpSlinks() const{
  MsgStream log( messageService(), name() ) ;
  for (unsigned int slink=0; slink<(TrigT1CaloDefs::numOfCPRoIRODs);slink++){
    log << MSG::INFO<< "Slink cable  "<<slink
        <<" has "<<(m_CPRoIROD[slink]->size())<<" words"<<endreq;
    for (DataVector<LVL1CTP::SlinkWord>::iterator i=m_CPRoIROD[slink]->begin();
                                      i!=m_CPRoIROD[slink]->end();++i){
      log <<MSG::INFO<<"Word :"<<MSG::hex<<(*i)->word()<<MSG::dec<<endreq;
    }
  }
}


