/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/***************************************************************************
                          ROD.cxx  -  description
                             -------------------
    begin                : 26/02/2002
    copyright            : (C) 2002 by moyse
    email                : e.moyse@qmul.ac.uk
 ***************************************************************************/



//#include <fstream>
#include <string>

#ifndef  TRIGGERSPACE

// running in Athena
#include "TrigT1Interfaces/CPRoIDecoder.h"
#include "TrigT1Interfaces/JEPRoIDecoder.h"
#include "TrigT1CaloUtils/CoordToHardware.h"
#include "GaudiKernel/MsgStream.h"
#include "EventInfo/EventInfo.h"
#include "TrigT1CaloSim/ROD.h"
#include "EventInfo/EventID.h"
//#include "TrigT1Calo/Parity.h"
#include "TrigConfL1Data/L1DataDef.h"
#include "TrigConfL1Data/ThresholdConfig.h"
#include "TrigT1Interfaces/TrigT1Interfaces_ClassDEF.h"
#include "TrigT1CaloEvent/EmTauROI_ClassDEF.h"
#include "TrigT1CaloEvent/JetROI_ClassDEF.h"
#else

// running in TRIGGERSPACE
#include "CPRoIDecoder.h"

#endif

#include <algorithm>


LVL1::ROD::ROD
  ( const std::string& name, ISvcLocator* pSvcLocator ) 
    : Algorithm( name, pSvcLocator ),
    m_storeGate("StoreGateSvc", name), 
    m_configSvc("TrigConf::LVL1ConfigSvc/LVL1ConfigSvc", name), 
    m_eventNumber(0),
    m_emTauRoILocation(TrigT1CaloDefs::EmTauROILocation), m_emTauCTPLocation(TrigT1CaloDefs::EmTauCTPLocation),
    m_emTauSlinkLocation(TrigT1CaloDefs::EmTauSlinkLocation),m_jepSlinkLocation(TrigT1CaloDefs::jepSlinkLocation),
    //m_energySlinkLocation(TrigT1CaloDefs::energySlinkLocation),
    //m_jetEtSlinkLocation(TrigT1CaloDefs::jetEtSlinkLocation),
    m_JetRoILocation(TrigT1CaloDefs::JetROILocation),
    m_JetCTPLocation(TrigT1CaloDefs::JetCTPLocation),
    m_JetEtRoILocation(TrigT1CaloDefs::JetEtROILocation),
    m_energyRoILocation(TrigT1CaloDefs::EnergyROILocation),
    m_jetEtRoI(0),
    m_emTauCTP(0),
    m_JetCTP(0),
    m_jetEtThreshMap(0)
{
  m_emTauOverflow = new std::vector<unsigned int>;
  m_jetOverflow = new std::vector<unsigned int>;

  declareProperty("EventStore",m_storeGate,"StoreGate Service");
  declareProperty( "LVL1ConfigSvc", m_configSvc, "LVL1 Config Service");
  declareProperty(  "EmTauRoILocation",     m_emTauRoILocation );
  declareProperty(  "EmTauCTPLocation",     m_emTauCTPLocation );
  declareProperty(  "EmTauSlinkLocation",   m_emTauSlinkLocation );
  declareProperty(  "JEPSlinkLocation",     m_jepSlinkLocation );
  declareProperty(  "JetRoILocation",       m_JetRoILocation );
  declareProperty(  "JetCTPLocation",       m_JetCTPLocation );
  declareProperty(  "JetEtRoILocation",     m_JetEtRoILocation );
  declareProperty(  "EnergyRoILocation",    m_energyRoILocation );

  for (unsigned int i = 0; i<TrigT1CaloDefs::numOfCPRoIRODs;i++){
    m_CPRoIROD[i]=0;
  }
  for (unsigned int i = 0; i<TrigT1CaloDefs::numOfJEPRoIRODs;i++){
    m_jepRoIROD[i]=0;
  }
  for (unsigned int i = 0; i<TrigT1CaloDefs::numOfCPThresholds;i++){
    m_emTauMult[i]=0;
  }
  for (unsigned int i = 0; i<TrigT1CaloDefs::numOfJetThresholds;i++){
    m_jetMult[i]=0;
    m_factor[i]=0;
  }
  for (unsigned int i = 0; i<TrigT1CaloDefs::numOfFwdJetThresholds;i++){
    m_leftFwdJetMult[i]=0;
    m_rightFwdJetMult[i]=0;
    m_fwdfactor[i]=0;
  }
}

// Destructor
LVL1::ROD::~ROD() {
  delete m_emTauOverflow;
  delete m_jetOverflow;
} 


//---------------------------------
// initialise() 
//---------------------------------

StatusCode LVL1::ROD::initialize()
{

  // We must here instantiate items which can only be made after
  // any job options have been set
  MsgStream log( messageService(), name() ) ;

  //
  // Connect to the LVL1ConfigSvc for the trigger configuration:
  //
  StatusCode sc = m_configSvc.retrieve();
  if ( sc.isFailure() ) {
    log << MSG::ERROR << "Couldn't connect to " << m_configSvc.typeAndName() 
        << endreq;
    return sc;
  } else {
    log << MSG::DEBUG << "Connected to " << m_configSvc.typeAndName() << endreq;
  }

  //
  // Connect to StoreGate:
  //
  sc = m_storeGate.retrieve();
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


//-------------------------------------------------
// re-initialise JetEt weights at start of each run 
//-------------------------------------------------

StatusCode LVL1::ROD::beginRun()
{
  jetEtFactors();
  return StatusCode::SUCCESS ;
}


//---------------------------------
// finalise() 
//---------------------------------

StatusCode LVL1::ROD::finalize()
{
   MsgStream log( messageService(), name() ) ;
   log << MSG::INFO << "Finalizing" << endreq;
   return StatusCode::SUCCESS ;
}


//----------------------------------------------
// execute() method called once per event
//----------------------------------------------
//


StatusCode LVL1::ROD::execute( )
{
  MsgStream log( messageService(), name() ) ;
  log << MSG::DEBUG << "Executing" << endreq;

  const EventInfo* evt;
  if (StatusCode::SUCCESS == m_storeGate->retrieve(evt)){
    m_eventNumber = evt->event_ID()->event_number();
  }else{
    log << MSG::ERROR<< " Unable to retrieve EventInfo from StoreGate "<< endreq;
  }

  assignVectors();    // create new storage vectors and clear mult vectors
  formSlinkObjects(); // add headers to DataVectors
  loadRoIs();         // load RoIs and call processRoIs() on them
  jetEtTrigger();
  formCTPObjects();   // form CTP objects (using mults extracted by processRoIs() )
  finaliseSlinkObjects();
  saveSlinkObjects(); // save
  saveCTPObjects();

  printOverflows();
  return StatusCode::SUCCESS ;
}

/** get ROIwords and form Slink words from them, adding header and tail. */
void LVL1::ROD::formSlinkObjects(){
  MsgStream log( messageService(), name() ) ;
  log << MSG::DEBUG << "formSlinkObjects" << endreq;

  for (unsigned int i = 0; i<TrigT1CaloDefs::numOfCPRoIRODs;i++){
    addHeader(m_CPRoIROD[i],0x72,i);//set modID to crate #
  }
  for (unsigned int i = 0; i<TrigT1CaloDefs::numOfJEPRoIRODs;i++){
    addHeader(m_jepRoIROD[i],0x73,i);//set modID to crate #
  }
  //m_energyRoIROD=new DataVector<LVL1CTP::SlinkWord>;
  //addHeader(m_energyRoIROD,0x73,0);//set modID to 0
  
  //m_jetEtRoIROD=new DataVector<LVL1CTP::SlinkWord>;
  //addHeader(m_jetEtRoIROD,0x73,1);//set modID to 1  *** CHECK THIS ***
}

/** adds slink header as defined in DAQ-98-129*/
void LVL1::ROD::addHeader( DataVector<LVL1CTP::SlinkWord>* slink, unsigned int subDetID, unsigned int moduleId){
	//slink->push_back(getWord(0));    // zero data at start of event
	slink->push_back(getWord(0xb0f00000));      // beginning of fragment word (control word)
	slink->push_back(getWord(0xee1234ee));       // start of header marker
	slink->push_back(getWord(8));                       // header length
	slink->push_back(getWord(1));                      // format version number
  unsigned int moduleType=0x00;  // ROD
//  unsigned int subDetID  =0x72;   // Cluster Processor
//  unsigned int moduleId  =crate; // Serial # of ROD - not formally defined yet, but this seems sensible 
	slink->push_back(getWord( (moduleType<<16)+(subDetID<<8)+moduleId ));  // 1st 8b but source id: CP system
	slink->push_back(getWord(m_eventNumber));       // 8b ECR_count (set to 0) | 24b L1 id = evt number
  slink->push_back( getWord(1 ));                           // 8b BCID
  slink->push_back( getWord(1 ));                           // 8b L1 trig type
  slink->push_back( getWord(0 ));                           // detector evt type}
}

/** add Slink tail */
void LVL1::ROD::addTail(DataVector<LVL1CTP::SlinkWord>* slink, unsigned int numberOfDataWords){
 	slink->push_back(getWord(0 ));                           // write status word 1
  slink->push_back(getWord(0 ));                           // write status word 2
//^^^ not simulating any errors.

  slink->push_back(getWord(2 ));                           // status block length
  slink->push_back(getWord(numberOfDataWords));            // calculated data block length
  slink->push_back(getWord(1 ));                           // status block position
// ^^^ indicates that data precedes status block
  slink->push_back(getWord(0xe0f00000 ));                  // end of fragment word
//  slink->push_back(getWord(0 ));                         // zero data at end of event
}

/** save Slink Objects to SG */
void LVL1::ROD::saveSlinkObjects(){
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

/** save CTP Objects to SG */
void LVL1::ROD::saveCTPObjects(){
  MsgStream log( messageService(), name() ) ;
  log << MSG::DEBUG<< "saveCTPObjects"<<endreq;

  StatusCode sc = m_storeGate->overwrite(m_emTauCTP,m_emTauCTPLocation,true,false,false);

  if (sc.isSuccess() ){
    log << MSG::DEBUG << "Stored EmTau CTP object at "<< m_emTauCTPLocation << endreq;
  } else {
    log << MSG::ERROR << "Failed to write EmTau CTP object!"<< endreq;
  } // endif

  sc = m_storeGate->overwrite(m_JetCTP, m_JetCTPLocation,true,false,false);

  if (sc.isSuccess() ){
    log << MSG::DEBUG << "Stored Jet CTP object, which has cableword 0 = 0x"
        <<hex<<(m_JetCTP->cableWord0() )<<" cableword 1 = 0x"
        <<(m_JetCTP->cableWord1() )<<dec<< endreq;
  } else {
    log << MSG::ERROR << "Failed to write jet CTP object!"<< endreq;
  } // endif
  return;
}

/** Get Jet, Energy and EmTau RoI objects. */
void LVL1::ROD::loadRoIs(){
  MsgStream log( messageService(), name() ) ;
  log << MSG::DEBUG<< "loadRoIs"<<endreq;
  const t_emTauRoIContainer* EMs ;
  StatusCode sc1 = m_storeGate->retrieve(EMs, m_emTauRoILocation);

  if( sc1==StatusCode::FAILURE ) {
    log << MSG::DEBUG<< "No EmTauRoIs found. "<< endreq ;
  }else{
    processRoIs(EMs);
  }

  const t_jetRoIContainer* Jets   ;
  sc1 = m_storeGate->retrieve(Jets, m_JetRoILocation);

  if( sc1==StatusCode::FAILURE ) {
    log << MSG::DEBUG << "No JetRoIs found."<< endreq ;
  }else{
    processRoIs(Jets);
  }

  const t_EnergyRoIContainer* energy   ;
  sc1 = m_storeGate->retrieve(energy, m_energyRoILocation);

  if( sc1==StatusCode::FAILURE ) {
    log << MSG::DEBUG << "No Energy RoI found."<< endreq ;
  }else{
    processRoIs(energy);
  }
  
  return;
}

/** process Em/Tau RoIs extracting RoI words and counting multiplicities */
void LVL1::ROD::processRoIs(const t_emTauRoIContainer* EMs){
  MsgStream log( messageService(), name() ) ;
  log << MSG::DEBUG<< "processing EmTau RoIs"<<endreq;
  int crate = 0;
  unsigned int RoIWord=0;
  unsigned int count=0;
  unsigned int maxMult = (1<<m_bitsPerThresh)-1;  
  for (t_emTauRoIContainer::const_iterator it=EMs->begin(); it!=EMs->end(); it++){
    RoIWord=(*it)->roiWord();
    log << MSG::DEBUG<< "examining EmTau RoI : "<<++count
        << " which has RoIword : "<<hex<<RoIWord<<dec
        << " with coords ("<<(*it)->phi()<<","<<(*it)->eta()<<")"<<endreq;
    CPRoIDecoder conv;
    crate=conv.crate(RoIWord);
    log << MSG::DEBUG<< "Adding RoI to crate : "<<crate<<endreq;
    m_CPRoIROD[crate]->push_back( getWord(RoIWord) );
    for (unsigned int thresh=0; thresh< TrigT1CaloDefs::numOfCPThresholds; thresh++){
      if ((*it)->thresholdPassed(thresh+1)&&m_emTauMult[thresh]<maxMult) (m_emTauMult[thresh])++;
    }
    CoordinateRange roi=conv.coordinate(RoIWord);
    Coordinate centre((*it)->phi(), (*it)->eta());
    if ( (roi.contains(centre) )!=true) log << MSG::ERROR << "Error in processRoIs with roiword :"
        <<hex<<RoIWord<<dec<<endl
        << "roi centre ("<<centre.phi()<<", "<<centre.eta()<<") isn't in range phi: "
        <<roi.phiRange().min()<<" to "<<roi.phiRange().max()<<", eta:"
        <<roi.etaRange().min()<<" to "<<roi.etaRange().max()
        <<endreq;

  }
  return;
}

/** process jet RoIs extracting RoI words and counting multiplicities */
void LVL1::ROD::processRoIs(const t_jetRoIContainer* jets){
  MsgStream log( messageService(), name() ) ;
  log << MSG::DEBUG<< "processing Jet RoIs"<<endreq;
  int crate = 0;
  unsigned int RoIWord=0;
  unsigned int count=0;
  unsigned int maxMult = (1<<m_bitsPerThresh)-1;
  unsigned int maxFwdMult = (1<<m_bitsPerFwdThresh)-1;
  for (t_jetRoIContainer::const_iterator it=jets->begin(); it!=jets->end(); it++){
    RoIWord=(*it)->roiWord();
    log << MSG::DEBUG<< "examining Jet RoI : "<<++count
        << " which has RoIword : "<<hex<<RoIWord<<dec
        << " with coords ("<<(*it)->phi()<<","<<(*it)->eta()<<")"<<endreq;
    JEPRoIDecoder conv;
    crate=conv.crate(RoIWord);
    log << MSG::DEBUG<< "Adding RoI to crate : "<<crate<<endreq;
    m_jepRoIROD[crate]->push_back( getWord(RoIWord) );
    if ((*it)->isForward()) {
      log << MSG::VERBOSE<< "JetRoI with "<<RoIWord<<" is forward jet. "<<crate<<endreq;

      for (unsigned int thresh=0; thresh< TrigT1CaloDefs::numOfFwdJetThresholds; ++thresh){
        if ((*it)->fwdThresholdPassed(thresh+1)) {
          if ((*it)->eta()<0.0&&m_leftFwdJetMult[thresh]<maxFwdMult) (m_leftFwdJetMult[thresh])++;
          if ((*it)->eta()>0.0&&m_rightFwdJetMult[thresh]<maxFwdMult) (m_rightFwdJetMult[thresh])++;
        }
      }//endfor thresh
    }else{
      // not forward jet.
      for (unsigned int thresh2=0; thresh2< TrigT1CaloDefs::numOfJetThresholds; ++thresh2){
        if ((*it)->thresholdPassed(thresh2+1)&&m_jetMult[thresh2]<maxMult) (m_jetMult[thresh2])++;
      }//endfor thresh2
    }
  }//endfor it
  return;
}

/** process energy RoI extracting RoI words and counting multiplicities */
void LVL1::ROD::processRoIs(const t_EnergyRoIContainer* energy){
  MsgStream log( messageService(), name() ) ;
  log << MSG::DEBUG<< "processing energy RoI"<<endreq;

    log << MSG::DEBUG<< "examining energy RoI : "
        << " which has RoIword 0: "<<hex<<(energy->roiWord0())<<dec
        << " which has RoIword 1: "<<hex<<(energy->roiWord1())<<dec
        << " which has RoIword 2: "<<hex<<(energy->roiWord2())<<dec
        <<endreq;

  m_jepRoIROD[1]->push_back(getWord(energy->roiWord0()));
  m_jepRoIROD[1]->push_back(getWord(energy->roiWord1()));
  m_jepRoIROD[1]->push_back(getWord(energy->roiWord2()));

  return;
}

/** Create the object vectors to be stored in SG and clear mult vectors*/
void LVL1::ROD::assignVectors(){
  MsgStream log( messageService(), name() ) ;
  log << MSG::DEBUG << "assignVectors" << endreq;

  for (unsigned int i = 0; i<TrigT1CaloDefs::numOfCPRoIRODs;i++){
    m_CPRoIROD[i]=new DataVector<LVL1CTP::SlinkWord>;
  }
  for (unsigned int i = 0; i<TrigT1CaloDefs::numOfJEPRoIRODs;i++){
    m_jepRoIROD[i]=new DataVector<LVL1CTP::SlinkWord>;
  }

  //clear multiplicity arrays
  for (unsigned int i = 0; i<TrigT1CaloDefs::numOfCPThresholds;i++){
    m_emTauMult[i]=0;
  }

    //clear multiplicity arrays
  for (unsigned int i = 0; i<TrigT1CaloDefs::numOfJetThresholds;i++){
    m_jetMult[i]=0;
  }
  //clear multiplicity arrays
  for (unsigned int i = 0; i<TrigT1CaloDefs::numOfFwdJetThresholds;i++){
    m_leftFwdJetMult[i]=0;
    m_rightFwdJetMult[i]=0;
  }

  // clear overflow vector
  m_emTauOverflow->clear();
  m_jetOverflow->clear();
}

LVL1CTP::SlinkWord* LVL1::ROD::getWord(unsigned int tword){
  return new LVL1CTP::SlinkWord(tword);
}


/** create objects to be passed to CTP. */
void LVL1::ROD::formCTPObjects(){
  MsgStream log( messageService(), name() ) ;
  log << MSG::DEBUG<< "formCTPObjects"<<endreq;
  formEmTauCTPObjects();
  formJetCTPObjects();
  // Energy CTP objects formed in EnergyTrigger.
}

/** returns a mult. data word with 3 bits per threshold, starting at thresh first and ending at thresh last.
<pre> 2*0 | 2b (reserved) | 1b Parity | 4b JetEtMap | Th8 | Th7 | Th6 | Th5 | Th4| Th3 | Th2 | Th1 | 0</pre>*/
unsigned int LVL1::ROD::jetCTPWord0(){

  const unsigned int first=1;
  const unsigned int last =TrigT1CaloDefs::numOfJetThresholds;

  MsgStream log( messageService(), name() ) ;
  log << MSG::DEBUG<< "Forming Jet CTP threshold word 0 between "<<first<<" and "<<last<<endreq;
  unsigned int temp=0;
  unsigned int mult=0;
  unsigned int multMask=0;
  const unsigned int maxMult=(1<<m_bitsPerThresh)-1;
  if ( (last<=(TrigT1CaloDefs::numOfJetThresholds))&&(first>0) ){
    //okay, in allowed bounds so now step through, check that mult isn't too large
    for (unsigned int count=first-1; count<last; count++){
      mult=m_jetMult[count];
      if (mult>maxMult) {m_jetOverflow->push_back(count);mult=maxMult;}
      multMask=(mult<<(m_bitsPerThresh*(count-first+1)));
      temp=(temp|multMask);
      log << MSG::DEBUG<< "Threshold "<<(count+1)<<" has multiplicity of "<<mult<<" : (temp = "<<temp
          <<") from multMask :"<<multMask<<endreq;
    }
  }else{
    log << MSG::ERROR<< "LVL1::jetCTP::word - threshold # outside bounds. Asked for "
        <<first<<" to "<<last<<endreq;
  }
  // bits 0-23 are the 8 thresholds.
  // bits 24-27 are the JetEtMap
  // bit 32 is parity (but can't do that in a 32 bit word!)
  temp += (m_jetEtThreshMap<<24);
  //Parity genParity;
  //temp += (genParity.odd(temp)<<31);
  return temp;
}

/** returns a mult. data word with 3 bits per threshold
<pre> 14*0 | 2b (reserved) | 1b Parity | R4 | R3 | R2 | R1 | L4 | L3 | L2 | L1 | 0</pre>*/
unsigned int LVL1::ROD::jetCTPWord1(){

  const unsigned int first=1;
  const unsigned int last =TrigT1CaloDefs::numOfFwdJetThresholds;

  MsgStream log( messageService(), name() ) ;
  log << MSG::DEBUG<< "Forming Jet CTP threshold word 1 between "<<first<<" and "<<last<<endreq;
  unsigned int temp=0;
  unsigned int mult=0;
  unsigned int multMask=0;
  const unsigned int maxMult=(1<<m_bitsPerFwdThresh)-1;
  if ( (last<=(TrigT1CaloDefs::numOfFwdJetThresholds))&&(first>0) ){
    //okay, in allowed bounds so now step through, check that mult isn't too large
    for (unsigned int count=first-1; count<last; count++){
      mult=m_leftFwdJetMult[count];
      if (mult>maxMult) {m_jetOverflow->push_back(count);mult=maxMult;}
      multMask=(mult<<(m_bitsPerFwdThresh*(count-first+1)));
      temp=(temp|multMask);
      log << MSG::DEBUG<< "Fwd Threshold "<<(count+1)<<" has multiplicity of "<<mult<<" for L and ";

      mult=m_rightFwdJetMult[count];
      log << MSG::DEBUG << "Right threshold " << count << " has mult = " << mult << endreq;
      if (mult>maxMult) {m_jetOverflow->push_back(count);mult=maxMult;}
      multMask=(mult<<(m_bitsPerFwdThresh*(count-first+1)+8));
      //shifted left by 8 to leave space for L thr.
      temp=(temp|multMask);
      log << MSG::DEBUG<<mult<<" for R"<<endreq;

    }
  }else{
    log << MSG::ERROR<< "LVL1::jetCTP::word 1 - threshold # outside bounds. Asked for "
        <<first<<" to "<<last<<endreq;
  }
  // bits 0-15 are the forward jet thresholds.
  // bit 32 is parity - but we can't do that in a 32 bit word!
  //Parity genParity;
  //temp += (genParity.odd(temp)<<31);
  return temp;
}

/** returns a mult. data word with 3 bits per threshold, starting at thresh first and ending at thresh last.
<pre> 6*0 | 2b (reserved) | 1b Parity | Th8 | Th7 | Th6 | Th5 | Th4| Th3 | Th2 | Th1 | 0</pre>*/
unsigned int LVL1::ROD::EmTauCTPWord(unsigned int first, unsigned int last){
  MsgStream log( messageService(), name() ) ;
  log << MSG::DEBUG<< "Forming EmTau CTP threshold word between "<<first<<" and "<<last<<endreq;
  unsigned int temp=0;
  unsigned int mult=0;
  unsigned int multMask=0;
  const unsigned int maxMult=(1<<m_bitsPerThresh)-1;
  if ( (last<=(TrigT1CaloDefs::numOfCPThresholds))&&(first>0) ){
    //okay, in allowed bounds so now step through, check that mult isn't too large
    for (unsigned int count=first-1; count<last; count++){
      mult=m_emTauMult[count];
      if (mult>maxMult) {m_emTauOverflow->push_back(count);mult=maxMult;}
      multMask=(mult<<(m_bitsPerThresh*(count-first+1)));
      temp=(temp|multMask);
      log << MSG::DEBUG<< "Threshold "<<(count+1)<<" has multiplicity of "<<mult<<" : (temp = "<<temp
          <<") from multMask :"<<multMask<<endreq;
    }
  }else{
    log << MSG::ERROR<< "LVL1::EmTauCTP::word - threshold # outside bounds. Asked for "
        <<first<<" to "<<last<<endreq;
  }
  // Parity should be in bit 32, but can't do that in 32 bit word.
  //Parity genParity;
  //temp += (genParity.odd(temp)<<31);
  return temp;
}

/** form EmTauCTP objects */
void LVL1::ROD::formEmTauCTPObjects(){
  /**\todo fix hard coding */
  unsigned int cableword0=EmTauCTPWord(1,8);
  unsigned int cableword1=EmTauCTPWord(9,16);
  m_emTauCTP = new EmTauCTP( cableword0, cableword1 );
  return;

}

/** form JetCTP objects */
void LVL1::ROD::formJetCTPObjects(){
  /**\todo fix hard coding */
  unsigned int cableword0=jetCTPWord0();
  unsigned int cableword1=jetCTPWord1();
  m_JetCTP = new JetCTP( cableword0, cableword1 );
  return;
}
/** dump details of any threshold overflows (i.e. more RoIs pass threshold(s) than can be indicated to CTP) */
void LVL1::ROD::printOverflows() const{
  MsgStream log( messageService(), name() ) ;

  for (std::vector<unsigned int>::iterator it=m_emTauOverflow->begin();
                                 it!=m_emTauOverflow->end(); it++){
    log << MSG::DEBUG << "EM/TAU: Overflow with threshold : "<<*it<<endreq;
  }

  for (std::vector<unsigned int>::iterator it=m_jetOverflow->begin();
                                 it!=m_jetOverflow->end(); it++){
    log << MSG::DEBUG << "JET: Overflow with threshold : "<<*it<<endreq;
  }
  return;
}
/** add tails to Slink objects. */
void LVL1::ROD::finaliseSlinkObjects(){
  MsgStream log( messageService(), name() ) ;
  log << MSG::DEBUG << "finalising Slink Objects: "<<endreq;
  /**\todo get rid of hard coded number-of-header-words*/
  const unsigned int headerWords=8+1;//8 in Slink head + control word

  for (unsigned int i = 0; i<TrigT1CaloDefs::numOfCPRoIRODs;i++){
    addTail(m_CPRoIROD[i], (m_CPRoIROD[i]->size()-headerWords) );
  }
  for (unsigned int i = 0; i<TrigT1CaloDefs::numOfJEPRoIRODs;i++){
    addTail(m_jepRoIROD[i],(m_jepRoIROD[i]->size()-headerWords) );
  }
  
  return;
}

/** prints out the Slink info. */
void LVL1::ROD::dumpSlinks() const{
  MsgStream log( messageService(), name() ) ;
  for (unsigned int slink=0; slink<(TrigT1CaloDefs::numOfCPRoIRODs);slink++){
    log << MSG::INFO<< "Slink cable  "<<slink
        <<" has "<<(m_CPRoIROD[slink]->size())<<" words"<<endreq;
    for (DataVector<LVL1CTP::SlinkWord>::iterator i=m_CPRoIROD[slink]->begin();
                                      i!=m_CPRoIROD[slink]->end();++i){
      log <<MSG::INFO<<"Word :"<<hex<<(*i)->word()<<dec<<endreq;
    }
  }
}

/** returns JetEt.
    Calculation is simple: JetET = sum(multiplicity*factor).
    You just have to calculate appropriate factors first */
unsigned int LVL1::ROD::getJetEt() const{
  MsgStream log( messageService(), name() ) ;

  unsigned int jetEt = 0;
  
  // Once you've calculated factors, you do not need a sorted threshold list
  for (unsigned int j=0; j<TrigT1CaloDefs::numOfJetThresholds; j++) {
    jetEt += m_jetMult[j] * m_factor[j];
    log << MSG::DEBUG << "Jet Threshold " << j << 
           " has multiplicity " << m_jetMult[j] <<
           " giving jetEt = " << jetEt << endreq; 
  }
  for (unsigned int j=0; j<TrigT1CaloDefs::numOfFwdJetThresholds; j++) {
    jetEt += (m_leftFwdJetMult[j]+m_rightFwdJetMult[j]) * m_fwdfactor[j];
    log << MSG::DEBUG << "Forward Jet Threshold " << j << 
           " has multiplicity " << 
	     m_leftFwdJetMult[j]+m_rightFwdJetMult[j] <<
           " giving jetEt = " << jetEt << endreq; 
  }
  
  return jetEt;
}

/** Obtains factors for JetET trigger */
void::LVL1::ROD::jetEtFactors() {
  MsgStream log( messageService(), name() ) ;
  
 // Get factors from configSvc
 const std::vector<int> factors = m_configSvc->thresholdConfig()->caloInfo().jetWeights();
 log << MSG::INFO << "Jet Weights from configSvc: " << endreq;
 for (std::vector<int>::const_iterator it = factors.begin(); it != factors.end(); ++it) {
   log << MSG::INFO << "   " << (*it) << endreq;
 }

  // First 8 factors are for central jets, last 4 for forward
  for (unsigned int thresh = 0; thresh < TrigT1CaloDefs::numOfJetThresholds; thresh++) {
    if (thresh < factors.size()) {
      m_factor[thresh] = factors[thresh];
     }
     else {
      m_factor[thresh] = 0;
     }
  }
  for (unsigned int thresh = 0; thresh < TrigT1CaloDefs::numOfFwdJetThresholds; thresh++) {
    if (thresh+8 < factors.size()) {
      m_fwdfactor[thresh] = factors[thresh+8];
     }
     else {
      m_fwdfactor[thresh] = 0;
    }
  }
    
}

/** estimates Jet ET and does trigger. */
void LVL1::ROD::jetEtTrigger(){
  MsgStream log( messageService(), name() ) ;
  
  L1DataDef def;
  
  // Get JetET value and initialise hit vector
  unsigned int jetEt=getJetEt();
  m_jetEtThreshMap=0;
 
  log << MSG::DEBUG<<"starting jetEtTrigger() with jetEt="<<jetEt<<endreq;
  
  std::vector<TriggerThreshold*> thresholds = m_configSvc->ctpConfig()->menu().thresholdVector();
  std::vector<TriggerThreshold*>::const_iterator it;
  for (it = thresholds.begin(); it != thresholds.end(); ++it) {
    if ( (*it)->type() == def.jeType() ) {
      TriggerThresholdValue* tv = (*it)->triggerThresholdValue(0,0); // ET trigger thresholds can only have one global value
      unsigned int threshNum = (*it)->thresholdNumber();
      unsigned int thresholdValue = static_cast<unsigned>((*tv).ptcut());
      // set bit if relevant threshold is passed
      if (jetEt>thresholdValue) m_jetEtThreshMap=m_jetEtThreshMap|(1<<(threshNum));
      log << MSG::DEBUG<<"Passed threshold "<<(threshNum+1)
          << " (" << thresholdValue << " GeV)"<<endreq;
    }
  }
  
  // create RoIWord
  unsigned int roiWord = (TrigT1CaloDefs::jetRoIType<<30) + (TrigT1CaloDefs::jetEtRoI<<29);
  roiWord += m_jetEtThreshMap;
  log << MSG::DEBUG << "JetEt RoIWord = " << hex << roiWord << dec << endreq;
  
  // create RoI
  m_jetEtRoI = new JetEtRoI(roiWord, jetEt);
  
  // add to Slink data stream for crate 1
  // This should be suppressed if no thresholds are passed:
  if (m_jetEtThreshMap > 0) m_jepRoIROD[1]->push_back(getWord( m_jetEtRoI->roiWord() ));
  
  // and add to StoreGate (for later persistency)
  StatusCode sc = m_storeGate->overwrite(m_jetEtRoI, m_JetEtRoILocation,true,false,false);
  if (sc != StatusCode::SUCCESS) log << MSG::ERROR << "Failed to record jetEtRoI" << endreq; 
 
  return;  
}
