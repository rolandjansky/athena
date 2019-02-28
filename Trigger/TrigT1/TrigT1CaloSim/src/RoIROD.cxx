/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
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

#include "TrigT1Interfaces/TrigT1Interfaces_ClassDEF.h"

#include <algorithm>


LVL1::RoIROD::RoIROD
  ( const std::string& name, ISvcLocator* pSvcLocator ) 
    : AthReentrantAlgorithm( name, pSvcLocator ),
    m_emTauSlinkLocation(TrigT1CaloDefs::EmTauSlinkLocation),
    m_jepSlinkLocation(TrigT1CaloDefs::jepSlinkLocation)
{

  declareProperty(  "EmTauSlinkLocation",   m_emTauSlinkLocation );
  declareProperty(  "JEPSlinkLocation",     m_jepSlinkLocation );
}

// Destructor
LVL1::RoIROD::~RoIROD() {
} 


//---------------------------------
// initialise() 
//---------------------------------

StatusCode LVL1::RoIROD::initialize()
{
  ATH_CHECK( m_emTauRoILocation.initialize() );
  ATH_CHECK( m_JetRoILocation.initialize() );
  ATH_CHECK( m_energyRoILocation.initialize() );

  m_jepSlinkKeys.clear();
  m_jepSlinkKeys.push_back (m_jepSlinkLocation + "0");
  m_jepSlinkKeys.push_back (m_jepSlinkLocation + "1");
  ATH_CHECK( m_jepSlinkKeys.initialize() );

  m_emTauSlinkKeys.clear();
  m_emTauSlinkKeys.push_back (m_emTauSlinkLocation + "0");
  m_emTauSlinkKeys.push_back (m_emTauSlinkLocation + "1");
  m_emTauSlinkKeys.push_back (m_emTauSlinkLocation + "2");
  m_emTauSlinkKeys.push_back (m_emTauSlinkLocation + "3");
  ATH_CHECK( m_emTauSlinkKeys.initialize() );

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

StatusCode LVL1::RoIROD::execute (const EventContext& ctx) const
{
  ATH_MSG_DEBUG("Executing" );

  /** there are 4 CP RoI RODs which have a Slink cable connected to the RoIB. This array holds pointers to 4
  DataVectors containing the Slink words*/
  t_SlinkPtr CPRoIROD[TrigT1CaloDefs::numOfCPRoIRODs];

  /** there are 2 Jet RoI RODs which have a Slink cable connected to the RoIB. This array holds pointers to 2
  DataVectors containing the Slink words*/
  t_SlinkPtr jepRoIROD[TrigT1CaloDefs::numOfJEPRoIRODs];
  assignVectors (CPRoIROD, jepRoIROD);    // create new storage vectors 
  formSlinkObjects (CPRoIROD, jepRoIROD, ctx); // Fill storage vectors
  ATH_CHECK( saveSlinkObjects (CPRoIROD, jepRoIROD, ctx) ); // save storage vectors

  return StatusCode::SUCCESS ;
}

/** get ROIwords and form Slink words from them, adding header and tail. */
void LVL1::RoIROD::formSlinkObjects (t_SlinkPtr CPRoIROD[TrigT1CaloDefs::numOfCPRoIRODs],
                                     t_SlinkPtr jepRoIROD[TrigT1CaloDefs::numOfJEPRoIRODs],
                                     const EventContext& ctx) const
{
  ATH_MSG_DEBUG("formSlinkObjects" );

  /** Set SLink headers */
  for (unsigned int i = 0; i<TrigT1CaloDefs::numOfCPRoIRODs;i++){
    addHeader(*CPRoIROD[i],0x72,i, ctx);//set modID to crate #
  }
  for (unsigned int i = 0; i<TrigT1CaloDefs::numOfJEPRoIRODs;i++){
    addHeader(*jepRoIROD[i],0x73,i, ctx);//set modID to crate #
  }
  
  
  /** Retrieve CPMTobRoIs and copy data to SLink vectors */
  ATH_MSG_DEBUG("Load and process CPM TOBs");
  SG::ReadHandle<t_cpmTobRoIContainer> EMTaus (m_emTauRoILocation, ctx);
  /// Push TOBs to ROD vectors
  /// This version makes no attempt to order them.
  for (const CPMTobRoI* tau : *EMTaus) {
    int crate = tau->crate();
    CPRoIROD[crate]->push_back( getWord(tau->roiWord()) );
  }
  
  
  /** Retrieve JEMTobRoIs and copy data to SLink vectors */
  ATH_MSG_DEBUG("Load and process Jet TOBs");
  SG::ReadHandle<t_jemTobRoIContainer> Jets (m_JetRoILocation, ctx);
  /// Push TOBs to ROD vectors
  /// This version makes no attempt to order them.
  for (const JEMTobRoI* jet : *Jets) {
    int crate = jet->crate();
    jepRoIROD[crate]->push_back( getWord(jet->roiWord()) );
  }
  
  
  /** Retrieve EnergySum RoI(s) and add to SLink record */
  SG::ReadHandle<t_EnergyRoIContainer> energy (m_energyRoILocation, ctx);
  /** Add 6 ET RoIWords to crate 1 ROD data. */
  jepRoIROD[1]->push_back(getWord(energy->exWord(LVL1::CMXRoI::NORMAL)));
  jepRoIROD[1]->push_back(getWord(energy->eyWord(LVL1::CMXRoI::NORMAL)));
  jepRoIROD[1]->push_back(getWord(energy->etWord(LVL1::CMXRoI::NORMAL)));
  jepRoIROD[1]->push_back(getWord(energy->exWord(LVL1::CMXRoI::MASKED)));
  jepRoIROD[1]->push_back(getWord(energy->eyWord(LVL1::CMXRoI::MASKED)));
  jepRoIROD[1]->push_back(getWord(energy->etWord(LVL1::CMXRoI::MASKED)));
  
  
  /** Set SLink tails */
  const unsigned int headerWords=8+1;//8 in Slink head + control word
  
  for (unsigned int i = 0; i<TrigT1CaloDefs::numOfCPRoIRODs;i++){
    addTail(*CPRoIROD[i], (CPRoIROD[i]->size()-headerWords) );
  }
  for (unsigned int i = 0; i<TrigT1CaloDefs::numOfJEPRoIRODs;i++){
    addTail(*jepRoIROD[i], (jepRoIROD[i]->size()-headerWords) );
  }

  return;
}

/** adds slink header as defined in DAQ-98-129*/
void LVL1::RoIROD::addHeader (t_SlinkContainer& slink,
                              unsigned int subDetID,
                              unsigned int moduleId,
                              const EventContext& ctx) const
{
  slink.push_back(getWord(0xb0f00000));      // beginning of fragment word (control word)
  slink.push_back(getWord(0xee1234ee));       // start of header marker
  slink.push_back(getWord(8));                       // header length
  slink.push_back(getWord(1));                      // format version number
  unsigned int moduleType=0x00;  // ROD
  slink.push_back(getWord( (moduleType<<16)+(subDetID<<8)+moduleId ));  // 1st 8b but source id: CP system
  slink.push_back(getWord(ctx.eventID().event_number()));       // 8b ECR_count (set to 0) | 24b L1 id = evt number
  slink.push_back( getWord(1 ));                           // 8b BCID
  slink.push_back( getWord(1 ));                           // 8b L1 trig type
  slink.push_back( getWord(0 ));                           // detector evt type}
}

/** add Slink tail */
void LVL1::RoIROD::addTail (t_SlinkContainer& slink,
                            unsigned int numberOfDataWords) const
{
  slink.push_back(getWord(0 ));                           // write status word 1
  slink.push_back(getWord(0 ));                           // write status word 2
//^^^ not simulating any errors.
  slink.push_back(getWord(2 ));                           // status block length
  slink.push_back(getWord(numberOfDataWords));            // calculated data block length
  slink.push_back(getWord(1 ));                           // status block position
// ^^^ indicates that data precedes status block
  slink.push_back(getWord(0xe0f00000 ));                  // end of fragment word
}

/** save Slink Objects to SG */
StatusCode LVL1::RoIROD::saveSlinkObjects (t_SlinkPtr CPRoIROD[TrigT1CaloDefs::numOfCPRoIRODs],
                                           t_SlinkPtr jepRoIROD[TrigT1CaloDefs::numOfJEPRoIRODs],
                                           const EventContext& ctx) const
{
  ATH_MSG_DEBUG("saveSlinkObjects");

  for (unsigned int i = 0; i<TrigT1CaloDefs::numOfCPRoIRODs;++i){
    ATH_CHECK( SG::makeHandle (m_emTauSlinkKeys[i], ctx).record (std::move(CPRoIROD[i])) );
  }

  for (unsigned int i = 0; i<TrigT1CaloDefs::numOfJEPRoIRODs;++i){
    ATH_CHECK( SG::makeHandle (m_jepSlinkKeys[i], ctx).record (std::move(jepRoIROD[i])) );
  }
  
  return StatusCode::SUCCESS;
}


/** Create the object vectors to be stored in SG and clear mult vectors*/
void LVL1::RoIROD::assignVectors (t_SlinkPtr CPRoIROD[TrigT1CaloDefs::numOfCPRoIRODs],
                                  t_SlinkPtr jepRoIROD[TrigT1CaloDefs::numOfJEPRoIRODs]) const
{
  ATH_MSG_DEBUG("assignVectors" );

  for (unsigned int i = 0; i<TrigT1CaloDefs::numOfCPRoIRODs;i++){
    CPRoIROD[i] = std::make_unique<t_SlinkContainer>();
  }
  for (unsigned int i = 0; i<TrigT1CaloDefs::numOfJEPRoIRODs;i++){
    jepRoIROD[i] = std::make_unique<t_SlinkContainer>();
  }

}

std::unique_ptr<LVL1CTP::SlinkWord>
LVL1::RoIROD::getWord(unsigned int tword) const
{
  return std::make_unique<LVL1CTP::SlinkWord>(tword);
}


/** prints out the Slink info. */
void LVL1::RoIROD::dumpSlinks (t_SlinkPtr CPRoIROD[TrigT1CaloDefs::numOfCPRoIRODs]) const
{
  for (unsigned int slink=0; slink<(TrigT1CaloDefs::numOfCPRoIRODs);slink++){
    ATH_MSG_INFO("Slink cable  "<<slink
        <<" has "<<(CPRoIROD[slink]->size())<<" words");
    for (DataVector<LVL1CTP::SlinkWord>::iterator i=CPRoIROD[slink]->begin();
                                      i!=CPRoIROD[slink]->end();++i){
      ATH_MSG_INFO("Word :"<<MSG::hex<<(*i)->word()<<MSG::dec);
    }
  }
}


