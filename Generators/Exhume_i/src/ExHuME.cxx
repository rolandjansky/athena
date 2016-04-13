/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "Exhume_i/ExHuME.h"
#include "GaudiKernel/MsgStream.h"

#include "Exhume_i/Higgs.h"
#include "Exhume_i/GG.h"
//#include "Exhume_i/GluinoGluino.h"
#include "Exhume_i/QQ.h"
#include "Exhume_i/DiPhoton.h"

#include "StoreGate/StoreGateSvc.h"

#include "Exhume_i/Event.h"
#include "Exhume_i/RandomNumber.h"

#include "HepMC/HEPEVT_Wrapper.h"
#include "HepMC/IO_HEPEVT.h"

#include "TruthUtils/GeneratorName.h"


/////////////////////////////////////////////////////////////////////////////

IAtRndmGenSvc* Exhume::RandomNumber::p_AtRndmGenSvc;

Atlas_HEPEVT*  ExHuME::atlas_HEPEVT = new Atlas_HEPEVT();

std::string Exhume::RandomNumber::Exhume_stream;
std::string Exhume::RandomNumber::Pythia_stream;

extern "C" double atl_pyr_( int* /*idummy*/ )
{
  CLHEP::HepRandomEngine* engine = 
    Exhume::RandomNumber::p_AtRndmGenSvc->GetEngine(Exhume::RandomNumber::Pythia_stream);
  //std::cout<<"Called ATL_PYR" << std::endl;
  return CLHEP::RandFlat::shoot(engine);
}

ExHuME::ExHuME(const std::string& name, ISvcLocator* pSvcLocator) :
GenModule(name, pSvcLocator)
{



// Part 2: Properties go here
  //Set up process List
  //std::map<std::string,int>::iterator itr=ProcessList.begin();
  ProcessList.insert(std::pair<std::string,int> ("Higgs",0));
  ProcessList.insert(std::pair<std::string,int> ("QQ",1));
  ProcessList.insert(std::pair<std::string,int> ("GG",2));
  ProcessList.insert(std::pair<std::string,int> ("DiPhoton",3));

  MassMax=500.0;
  MassMin=100.0;
  ETMin=0.0;//for 2->2processes only
  CosThetaMax=0.975;//for 2->2 processes only
  HiggsDecayChannel=-1;//set to all
  dirty_xs=true;
  ProcessName="Higgs";
  RandomSeed=1111;


  //allow input from joboption
  declareProperty("Process",ProcessName);
  declareProperty("ExhumeRunControl",ExhumeRunControl);
  declareProperty("MassMin",MassMin);
  declareProperty("MassMax",MassMax);
  //2->2 options
  declareProperty("CosThetaMax",CosThetaMax);
  declareProperty("ETMin",ETMin);
  declareProperty("HiggsDecay",HiggsDecayChannel);

  declareProperty("UseTauola",m_TauolaLinked=false);
  declareProperty("UsePhotos",m_PhotosLinked=false);

  ProcessID = 0;   		
  m_efficiency_et_cut = 0;
  m_events = 0;

  myPythiaInterface = 0;
  myEvent = 0;
  myCrossSection = 0;
  myHiggs = 0;
  myTwoSpace = 0;
}

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *

StatusCode ExHuME::genInitialize(){

// Part 1: Get the messaging service, print where you are
  

  //MsgStream log(msgSvc(), name());
  if(msgLvl(MSG::INFO)){
    msg(MSG::INFO) << " ExHuME Initialising  \n" << endreq;  
  }
  SetProcess();
  SetExhumeParameters();
  static const bool CREATEIFNOTTHERE(true);
  StatusCode RndmStatus = service("AtRndmGenSvc",
				  myEvent->Random.p_AtRndmGenSvc,
				  CREATEIFNOTTHERE);
  if(!RndmStatus.isSuccess() || myEvent->Random.p_AtRndmGenSvc==0){
    
    msg(MSG::ERROR) << "Couldn't initialise Random Number Service" << endreq;
    return( RndmStatus);
  }
  
  Exhume::RandomNumber::Pythia_stream = "PYTHIA";

  HepMC::HEPEVT_Wrapper::set_sizeof_int(4);
  HepMC::HEPEVT_Wrapper::set_sizeof_real(8);
  HepMC::HEPEVT_Wrapper::set_max_number_entries(10000);

  return StatusCode::SUCCESS;
}

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *

StatusCode ExHuME::callGenerator() {

  if(msgLvl(MSG::INFO)){
    msg(MSG::INFO) << "Generating Exclusive "<<ProcessName.c_str()
		   << " Event..." << endreq;
  }
  bool passEvent=false;
  if(ProcessID==0) passEvent=true;
  do{  
    
    myEvent->Generate();
    std::vector<Exhume::Particle> partons = myCrossSection->GetPartons();
    for(std::vector<Exhume::Particle>::iterator itr=partons.begin();
	itr!=partons.end();itr++){
      if(itr->p.et()> ETMin) passEvent=true;
      
    }
    m_efficiency_et_cut++;
    
  }while(!passEvent);
  
  m_events++;
  
  myPythiaInterface->Hadronise();
  
  HepMC::HEPEVT_Wrapper::set_event_number(m_events);
  
  return StatusCode::SUCCESS;
}

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *

StatusCode ExHuME::genFinalize() {
  
  if(msgLvl(MSG::INFO)){
    msg(MSG::INFO) << "Ending ExHuME " << endreq;
  }
  double XS=myEvent->CrossSectionCalculation(); 
  double Eff=myEvent->GetEfficiency();

  if(msgLvl(MSG::INFO)){   
    msg(MSG::INFO)<<"The efficiency of event generation was "<<Eff
		  <<"%"<<endreq;
    msg(MSG::INFO)<<"The initial cross section for process "<<ProcessName
		  <<" was "<<XS<<" fb"<<endreq;
    msg(MSG::INFO)<<"The efficiency of ET CUT was "<<100.0*double(m_events)/m_efficiency_et_cut<<"%"<<endreq;
    msg(MSG::INFO)<<"The final cross section for process "<<ProcessName
		  <<" is "<<XS*double(m_events)/m_efficiency_et_cut<<
      " fb"<<endreq;
  }
  delete myCrossSection;
  delete myPythiaInterface;
  delete myEvent;
  
  myEvent=0;
  myPythiaInterface=0;
  myCrossSection=0;

  
return StatusCode::SUCCESS;
}

StatusCode ExHuME::fillEvt(HepMC::GenEvent* evt){
 
  if(msgLvl(MSG::INFO)){   
    msg(MSG::INFO) << "Filing Event...." << endreq;
  }
  if(msgLvl(MSG::DEBUG)){   
    msg(MSG::DEBUG) << "Atlas_HEPEVT Filling.  \n"  << endreq;
  }
  store_Atlas_HEPEVT();
  
  HepMC::IO_HEPEVT hepio; //this has been found to be not defined
  int pr_id = PYTHIA;
  evt->set_signal_process_id(pr_id);
  
  hepio.fill_next_event(evt);
 
  GeVToMeV(evt);

  return StatusCode::SUCCESS;
}

void ExHuME::SetProcess(){
  
  //  std::cout<<ProcessName<<std::endl;
  std::map<std::string,int>::iterator itr=ProcessList.find(ProcessName);
  //std::cout<<itr->first<<std::endl;
  
  if(itr!=ProcessList.end()){
    ProcessID=itr->second;
    
  }
  else{
    MsgStream log(msgSvc(), name());
    log << MSG::INFO <<"Cannot find Process Name, setting to Higgs"<<endreq;
    ProcessName="Higgs";
    ProcessID=0;
  }
  
  return;
}

void ExHuME::SetExhumeParameters(){

  //reset min mass if user has been ambiguous
  bool ETMinSet=false;
  if(ProcessID==0) ETMin=0.0;
  if(ETMin>1.0) ETMinSet=true;
  if(ETMinSet){
    MassMin=2*ETMin;
  }
  if(MassMax<=MassMin){
    MassMax=10*MassMin;
  }

  switch(ProcessID){
  case 0:
    if(msgLvl(MSG::INFO)){   
      msg(MSG::INFO) << "Higgs Production Chosen" << endreq;
    }
    myHiggs = new Exhume::Higgs(ExhumeRunControl);
    myCrossSection = myHiggs;
    myHiggs=0;
    // ETMin=0.0;//etmin undefined for resonance.
    break;
  case 1:
    if(msgLvl(MSG::INFO)){   
      msg(MSG::INFO) << "QQ Production Chosen" << endreq;
    }  
    myTwoSpace = new Exhume::QQ(ExhumeRunControl);
    myTwoSpace->SetThetaMin(acos(CosThetaMax));
    myTwoSpace->SetWeightMass(MassMin);
    myCrossSection=myTwoSpace;
    myTwoSpace=0;
    break;
  case 2:
    if(msgLvl(MSG::INFO)){   
      msg(MSG::INFO) << "GG Production Chosen" << endreq;
    }
    myTwoSpace = new Exhume::GG(ExhumeRunControl);
    myTwoSpace->SetThetaMin(acos(CosThetaMax));
    myCrossSection=myTwoSpace;
    myTwoSpace=0;
    break;
  case 3:
    if(msgLvl(MSG::INFO)){   
      msg(MSG::INFO) << "DiPhoton Production Chosen" << endreq;
    }
    myTwoSpace = new Exhume::DiPhoton(ExhumeRunControl);
    myTwoSpace->SetThetaMin(acos(CosThetaMax));
    myCrossSection=myTwoSpace;
    myTwoSpace=0;
    break;
  default:
    if(msgLvl(MSG::INFO)){   
      msg(MSG::INFO) << "Higgs Production Chosen" << endreq;
    }
    myHiggs = new Exhume::Higgs(ExhumeRunControl);
    myCrossSection=myHiggs;
    myHiggs=0;
    break;    
  }
  //Name = myCrossSection->GetName();
  m_events=0;
  m_efficiency_et_cut=0;
  myEvent = new Exhume::Event(*myCrossSection,RandomSeed);
  //all pythia interface stuff removed as its causing visaje_() errors??
  myPythiaInterface= new Exhume::PythiaInterface(*myCrossSection);
  myPythiaInterface->SetHiggsDecay(HiggsDecayChannel);
  if(m_TauolaLinked){
    if(msgLvl(MSG::INFO)){   
      msg(MSG::INFO) << "Setting Tau Stable in PYTHIA to allow TAUOLA decays " 
		     << endreq;
    }
    myPythiaInterface->SetTauDecay(false);
  }
  if(m_PhotosLinked){
    if(msgLvl(MSG::INFO)){ 
      msg(MSG::INFO) 
	<< "Setting NO QED radiation from leptons in PYTHIA to allow PHOTOS " 
	<< endreq;
    }
    myPythiaInterface->SetQEDRadiationFromLeptons(20000.0);
  }	
  
  myEvent->SetMassRange(MassMin,MassMax);
  myEvent->SetParameterSpace();
  dirty_xs=false;
  return;
  
}

void ExHuME::store_Atlas_HEPEVT(void){
  
  if(msgLvl(MSG::DEBUG)){ 
    msg(MSG::DEBUG) << "atlas_HEPEVT------" << atlas_HEPEVT->nhep()  
		    << endreq;
    msg(MSG::DEBUG) << "atlas_HEPEVT------" << atlas_HEPEVT->isthep(10)  
		    << endreq;
    msg(MSG::DEBUG) << "atlas_HEPEVT------" << atlas_HEPEVT->idhep(10)  
		    << endreq;
    msg(MSG::DEBUG) << "atlas_HEPEVT------" << atlas_HEPEVT->jmohep(1,10)  
		    << endreq;
    msg(MSG::DEBUG) << "atlas_HEPEVT------" << atlas_HEPEVT->jdahep(2,10)  
		    << endreq;
  }
  atlas_HEPEVT->fill();
    
  Atlas_HEPEVT* Ahep = new Atlas_HEPEVT();
  *(Ahep)=*(atlas_HEPEVT);
  static const std::string keyid = "Pythia";
  StatusCode sc = evtStore()->record(Ahep, keyid);
  if (!sc.isSuccess()) {
    if(msgLvl(MSG::WARNING)){ 
      msg(MSG::WARNING) << " Could not record Atlas_HEPEVT" << endreq;
    }
  }
}


  



