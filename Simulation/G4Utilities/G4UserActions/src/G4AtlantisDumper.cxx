/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "G4UserActions/G4AtlantisDumper.h"
#include <iostream>
#include "GaudiKernel/IDataProviderSvc.h"
#include "GaudiKernel/INTupleSvc.h" 
#include "GaudiKernel/NTuple.h"
#include "GaudiKernel/SmartDataPtr.h"
#include "SimHelpers/ServiceAccessor.h"
#include "G4Step.hh"
#include "G4Run.hh"
#include "G4Event.hh"
#include "G4VProcess.hh"
#include "GaudiKernel/MsgStream.h"

#include "AthenaBaseComps/AthAlgorithm.h"
#include "GaudiKernel/ServiceHandle.h"
#include "GaudiKernel/Property.h"
#include "EventInfo/EventInfo.h"
#include "EventInfo/EventID.h"

static G4AtlantisDumper g4atlantisdumper("G4AtlantisDumper");


G4AtlantisDumper::G4AtlantisDumper(std::string s)
  : FADS::ActionsBase(s),
    FADS::UserAction(s),
    m_nsteps(0),
    m_file(NULL),
    m_tedep_cut(0),m_length_cut(0),m_p_cut(0), m_ke_cut(0){}
 
void G4AtlantisDumper::BeginOfEventAction(const G4Event* event)
{
  m_nsteps=0;
  ATH_MSG_INFO( "Hello from G4AtlantisDumper, event "<<event->GetEventID() );
  
  //parameters
  m_tedep_cut = 0.1;
  m_length_cut = 1.0;
  m_p_cut = 10.0;
  m_ke_cut = 10.0;
  
  if(theProperties.find("Edep")!=theProperties.end()){
    m_tedep_cut = atof(theProperties["Edep"].c_str());
    ATH_MSG_INFO("G4AtlantisDumper: Edep specified as "<<m_tedep_cut);
  }
  if(theProperties.find("Length")!=theProperties.end()){
    m_length_cut = atof(theProperties["Length"].c_str());
    ATH_MSG_INFO("G4AtlantisDumper: Length specified as "<<m_length_cut);
  }
  if(theProperties.find("P")!=theProperties.end()){
    m_p_cut = atof(theProperties["P"].c_str());
    ATH_MSG_INFO("G4AtlantisDumper: P specified as "<<m_p_cut);
  }
  if(theProperties.find("KE")!=theProperties.end()){
    m_ke_cut = atof(theProperties["KE"].c_str());
    ATH_MSG_INFO("G4AtlantisDumper: KE specified as "<<m_ke_cut);
  }
  
  ATH_MSG_INFO("G4AtlantisDumper:"
       <<"\nDeposited E cut = "<<m_tedep_cut
       <<"\nLength cut = "<<m_length_cut
       <<"\nMomentum cut = "<<m_p_cut
       <<"\nKinetic E cut = "<<m_ke_cut );
  //
  
  StoreGateSvc* evtStore=0;
  // Get all the Services
  std::list<IService*> svcs = Gaudi::svcLocator()->getServices();
  std::list<IService*>::const_iterator itrs;
  for (itrs=svcs.begin(); itrs!=svcs.end(); ++itrs) {
    //mLog << MSG::INFO << "-ACH Found service " <<(*itrs)->name() << endreq;
    Service* svc = dynamic_cast<Service*>(*itrs);
    if (svc == 0) {
      ATH_MSG_WARNING( "Service " << (*itrs)->name()<< " does not inherit from Service?" );
    } else {
      //mLog << MSG::INFO << "-ACH Found Service* " << svc->name()<< endreq;
      if (svc->name()=="StoreGateSvc") {
        ATH_MSG_INFO( "-ACH Found storage service" );
        evtStore = dynamic_cast<StoreGateSvc*>(svc);
      }
    }
  }
  
  if(!evtStore) {
    ATH_MSG_INFO("G4AtlantisDumper could find the StoreGateSvc");
    return;
  }

  const EventInfo* evt = 0;
  if (!evtStore->retrieve(evt, "McEventInfo").isSuccess() || 0==evt) {
    ATH_MSG_INFO("G4AtlantisDumper could not get event info!");
    return;
  }
  
  const EventID* eid = evt->event_ID();
  int athevent= eid->event_number();
  int athrun = eid->run_number();
  ATH_MSG_INFO("G4AtlantisDumper: Athena run event is "<<athrun<<" "<<athevent);
  char buf[1000]; 
  int length=snprintf(buf, 1000, "G4Atlantis_%d_%d.txt", athrun, athevent);
  if(999<length)
    {
      ATH_MSG_WARNING( "BeginOfEventAction:: m_filename string (" << buf << ") was truncated" );
    }
  ATH_MSG_INFO( "G4AtlantisDumper: Opening m_file "<<buf);
  m_file=new std::ofstream(); m_file->open(buf);
  m_filename = buf;
}

void G4AtlantisDumper::EndOfEventAction(const G4Event* event)
{
  ATH_MSG_INFO( "Goodbye from G4AtlantisDumper, event "<<event->GetEventID());
  m_file->close();
  delete m_file;

  ATH_MSG_INFO("G4AtlantisDumper: zipping "<<m_filename);
  system(("zip -m "+m_filename+".zip "+m_filename).c_str());
  
}

void G4AtlantisDumper::BeginOfRunAction(const G4Run* run)
{
  ATH_MSG_INFO( "Hello from G4AtlantisDumper, run "<<run->GetRunID());
}
void G4AtlantisDumper::EndOfRunAction(const G4Run* run)
{
  ATH_MSG_INFO( "Goodbye from G4AtlantisDumper, run "<<run->GetRunID());
}

void G4AtlantisDumper::SteppingAction(const G4Step* aStep)
{
    const G4Track *t=aStep->GetTrack();
    //if(m_nsteps>40) return;
    if (!t) return;
    
    int id=t->GetTrackID();
    int pid=t->GetParentID();

    int fs=aStep->IsFirstStepInVolume();
    int ls=aStep->IsLastStepInVolume();
    int stepn=t->GetCurrentStepNumber();
    
    const G4TrackVector *sec=aStep->GetSecondary();
    int nsec=0; if (sec) nsec=sec->size();    

    double tedep=aStep->GetTotalEnergyDeposit();
    double tnedep=aStep->GetNonIonizingEnergyDeposit();
    double length=aStep->GetStepLength();
    int pdg=t->GetDefinition()->GetPDGEncoding();
  
    bool keep=false;
    if (fs||ls) keep=true;
    if (tedep>m_tedep_cut) keep=true;//default = 0.1 MeV
    if (length>m_length_cut) keep=true;//default = 1.0 mm
    if (aStep->GetPreStepPoint()->GetMomentum().mag()<m_p_cut) keep=false;//default = 10 MeV
    if (aStep->GetPreStepPoint()->GetKineticEnergy()<m_ke_cut) keep=false;//default = 10 MeV
    if (!keep) return;

    //const G4VPhysicalVolume *v =t->GetVolume();
    //const G4Material *m =t->GetMaterial();
    
    const G4VProcess *p =t->GetCreatorProcess();
    G4String pn="none"; if(p) pn=p->GetProcessName();
  
    G4ThreeVector pos1=aStep->GetPreStepPoint()->GetPosition();
    double x1=pos1.x();    double y1=pos1.y();    double z1=pos1.z();
    double t1=aStep->GetPreStepPoint()->GetGlobalTime();
    G4ThreeVector pos2=aStep->GetPostStepPoint()->GetPosition();
    double x2=pos2.x();    double y2=pos2.y();    double z2=pos2.z();
    double t2=aStep->GetPostStepPoint()->GetGlobalTime();
    G4ThreeVector p1=aStep->GetPreStepPoint()->GetMomentum();
    double px1=p1.x();    double py1=p1.y();    double pz1=p1.z();
    G4ThreeVector p2=aStep->GetPostStepPoint()->GetMomentum();
    double px2=p2.x();    double py2=p2.y();    double pz2=p2.z();
    double ke1=aStep->GetPreStepPoint()->GetKineticEnergy();
    double ke2=aStep->GetPostStepPoint()->GetKineticEnergy();
    //GetVelocity() //GetBeta() //GetGamma()
    //GetSensitiveDetector() //GetPolarization()
    double mass=aStep->GetPreStepPoint()->GetMass();
    double charge=aStep->GetPreStepPoint()->GetCharge();
    //GetMagneticMoment()

    (*m_file)<<"ACH_G4Step";
    (*m_file)<<" "<<m_nsteps;    (*m_file)<<" "<<id;    (*m_file)<<" "<<pid;    (*m_file)<<" "<<(int)fs;    (*m_file)<<" "<<(int)ls;    (*m_file)<<" "<<stepn;
    (*m_file)<<" "<<pn;    (*m_file)<<" "<<nsec;
    (*m_file)<<" "<<tedep;    (*m_file)<<" "<<tnedep;    (*m_file)<<" "<<length;    (*m_file)<<" "<<pdg;    (*m_file)<<" "<<mass;    (*m_file)<<" "<<charge;
    (*m_file)<<" "<<x1;    (*m_file)<<" "<<y1;    (*m_file)<<" "<<z1;    (*m_file)<<" "<<t1;
    (*m_file)<<" "<<px1;    (*m_file)<<" "<<py1;    (*m_file)<<" "<<pz1;    (*m_file)<<" "<<ke1;
    (*m_file)<<" "<<x2;    (*m_file)<<" "<<y2;    (*m_file)<<" "<<z2;    (*m_file)<<" "<<t2;
    (*m_file)<<" "<<px2;    (*m_file)<<" "<<py2;    (*m_file)<<" "<<pz2;    (*m_file)<<" "<<ke2;
    (*m_file)<<std::endl;

    ++m_nsteps;
}

