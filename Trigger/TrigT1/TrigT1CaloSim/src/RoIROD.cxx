/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/
/***************************************************************************
                          RoIROD.cxx  -  description
                             -------------------
    begin                : Mon Jul 28 2014
    email                : Alan.Watson@CERN.CH
 ***************************************************************************/



//#include <fstream>
#include <string>

// running in Athena
#include "TrigT1CaloSim/RoIROD.h"
#include "xAODEventInfo/EventInfo.h"
#include "TrigT1Interfaces/TrigT1Interfaces_ClassDEF.h"

#include <algorithm>


LVL1::RoIROD::RoIROD
  ( const std::string& name, ISvcLocator* pSvcLocator ) 
    : AthAlgorithm( name, pSvcLocator ),
    m_eventNumber(0),
    m_emTauRoILocation(TrigT1CaloDefs::CPMTobRoILocation), 
    m_emTauSlinkLocation(TrigT1CaloDefs::EmTauSlinkLocation),
    m_JetRoILocation(TrigT1CaloDefs::JEMTobRoILocation),
    m_energyRoILocation(TrigT1CaloDefs::CMXRoILocation),
    m_jepSlinkLocation(TrigT1CaloDefs::jepSlinkLocation)
{

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
  return StatusCode::SUCCESS ;
}


//---------------------------------
// finalise() 
//---------------------------------

StatusCode LVL1::RoIROD::finalize()
{
   ATH_MSG_INFO("Finalizing" );
   return StatusCode::SUCCESS ;
}


//----------------------------------------------
// execute() method called once per event
//----------------------------------------------
//

StatusCode LVL1::RoIROD::execute( )
{
  ATH_MSG_DEBUG("Executing" );

  const xAOD::EventInfo* evt;
  if (StatusCode::SUCCESS == evtStore()->retrieve(evt)){
    m_eventNumber = evt->eventNumber();
  }else{
    ATH_MSG_ERROR( " Unable to retrieve EventInfo from StoreGate ");
  }

  assignVectors();    // create new storage vectors 
  formSlinkObjects(); // Fill storage vectors
  saveSlinkObjects(); // save storage vectors

  return StatusCode::SUCCESS ;
}

/** get ROIwords and form Slink words from them, adding header and tail. */
void LVL1::RoIROD::formSlinkObjects(){
  ATH_MSG_DEBUG("formSlinkObjects" );

  /** Set SLink headers */
  for (unsigned int i = 0; i<TrigT1CaloDefs::numOfCPRoIRODs;i++){
    addHeader(m_CPRoIROD[i],0x72,i);//set modID to crate #
  }
  for (unsigned int i = 0; i<TrigT1CaloDefs::numOfJEPRoIRODs;i++){
    addHeader(m_jepRoIROD[i],0x73,i);//set modID to crate #
  }
  
  
  /** Retrieve CPMTobRoIs and copy data to SLink vectors */
  ATH_MSG_DEBUG("Load and process CPM TOBs");
  const t_cpmTobRoIContainer* EMTaus ;
  StatusCode sc1 = evtStore()->retrieve(EMTaus, m_emTauRoILocation);

  if ( sc1==StatusCode::FAILURE ) {
    ATH_MSG_DEBUG("No CPMTobRoIs found. ");
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
  ATH_MSG_DEBUG("Load and process Jet TOBs");
  const t_jemTobRoIContainer* Jets ;
  StatusCode sc2 = evtStore()->retrieve(Jets, m_JetRoILocation);

  if ( sc2==StatusCode::FAILURE ) {
    ATH_MSG_DEBUG("No JEMTobRoIs found. ");
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
  StatusCode sc3 = evtStore()->retrieve(energy, m_energyRoILocation);

  if  ( sc3==StatusCode::FAILURE ) {
    ATH_MSG_DEBUG("No EnergySum RoIs found.");
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
  ATH_MSG_DEBUG("saveSlinkObjects");

  /**\todo There must be a better way of doing this, but CERN doesn't seem to have sstream.h*/
  std::string emTauSlinkLocation[4];
  emTauSlinkLocation[0]= m_emTauSlinkLocation+"0";
  emTauSlinkLocation[1]= m_emTauSlinkLocation+"1";
  emTauSlinkLocation[2]= m_emTauSlinkLocation+"2";
  emTauSlinkLocation[3]= m_emTauSlinkLocation+"3";

  for (unsigned int i = 0; i<TrigT1CaloDefs::numOfCPRoIRODs;++i){

    StatusCode sc = evtStore()->overwrite(m_CPRoIROD[i],emTauSlinkLocation[i], true);

    if (sc.isSuccess() ){
      ATH_MSG_DEBUG("Stored EmTau Slink object at "<< emTauSlinkLocation[i] <<" with "
          <<(m_CPRoIROD[i]->size())<<" words");
    } else {
      ATH_MSG_ERROR("Failed to write EmTau Slink object!");
    } // endif
  }

  std::string jepSlinkLocation[2];
  jepSlinkLocation[0]= m_jepSlinkLocation+"0";
  jepSlinkLocation[1]= m_jepSlinkLocation+"1";
  for (unsigned int i = 0; i<TrigT1CaloDefs::numOfJEPRoIRODs;++i){
    StatusCode sc = evtStore()->overwrite(m_jepRoIROD[i],jepSlinkLocation[i], true);
    if (sc.isSuccess() ){
      ATH_MSG_DEBUG("Stored JetEnergy Slink object at "<< jepSlinkLocation[i] <<" with "
          <<(m_jepRoIROD[i]->size())<<" words");
    } else {
      ATH_MSG_ERROR("Failed to write JetEnergy Slink object!");
    } // endif
  }
  
  return;
}


/** Create the object vectors to be stored in SG and clear mult vectors*/
void LVL1::RoIROD::assignVectors(){
  ATH_MSG_DEBUG("assignVectors" );

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
  for (unsigned int slink=0; slink<(TrigT1CaloDefs::numOfCPRoIRODs);slink++){
    ATH_MSG_INFO("Slink cable  "<<slink
        <<" has "<<(m_CPRoIROD[slink]->size())<<" words");
    for (DataVector<LVL1CTP::SlinkWord>::iterator i=m_CPRoIROD[slink]->begin();
                                      i!=m_CPRoIROD[slink]->end();++i){
      ATH_MSG_INFO("Word :"<<MSG::hex<<(*i)->word()<<MSG::dec);
    }
  }
}


