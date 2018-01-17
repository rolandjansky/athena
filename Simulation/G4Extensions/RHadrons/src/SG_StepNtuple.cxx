/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "SG_StepNtuple.h"
#include <iostream>
#include "GaudiKernel/IDataProviderSvc.h"
#include "GaudiKernel/INTupleSvc.h"
#include "GaudiKernel/NTuple.h"
#include "GaudiKernel/SmartDataPtr.h"
#include "SimHelpers/ServiceAccessor.h"
#include "G4Step.hh"
#include "G4Event.hh"

#include "GaudiKernel/Bootstrap.h"
#include "GaudiKernel/ISvcLocator.h"
#include "GaudiKernel/IMessageSvc.h"

namespace G4UA{


SG_StepNtuple::SG_StepNtuple():AthMessaging(Gaudi::svcLocator()->service< IMessageSvc >( "MessageSvc" ),"SG_StepNtuple"){;
}

  void SG_StepNtuple::BeginOfRunAction(const G4Run*){
    
    
    NTupleFilePtr file1(ntupleSvc(), "/NTUPLES/FILE1");
    
    SmartDataPtr<NTuple::Directory>
      ntdir(ntupleSvc(),"/NTUPLES/FILE1/StepNtuple");
    
    if ( !ntdir ) ntdir = ntupleSvc()->createDirectory(file1,"StepNtuple");
    //if ( !ntdir ) log << MSG::ERROR << " failed to get ntuple directory" << endmsg;
    NTuplePtr nt(ntupleSvc(), "/NTUPLES/FILE1/StepNtuple/10");
    if ( !nt ) {    // Check if already booked
      nt = ntupleSvc()->book (ntdir.ptr(), 10,CLID_ColumnWiseTuple, "GEANT4 Step NTuple");
      if ( nt ) {

	ATH_MSG_INFO("booked step ntuple ");

	if( nt->addItem ("nstep", m_nsteps,0 ,50000)!=StatusCode::SUCCESS // WARNING!! Force limit to 50k tracks
	    || nt->addItem ("pdg", m_nsteps, m_pdg)!=StatusCode::SUCCESS
	    || nt->addItem ("charge", m_nsteps, m_charge)!=StatusCode::SUCCESS
	    || nt->addItem ("mass", m_nsteps, m_mass)!=StatusCode::SUCCESS
	    || nt->addItem ("baryon", m_nsteps, m_baryon)!=StatusCode::SUCCESS
	    || nt->addItem ("x1", m_nsteps, m_x1)!=StatusCode::SUCCESS
	    || nt->addItem ("y1", m_nsteps, m_y1)!=StatusCode::SUCCESS
	    || nt->addItem ("z1", m_nsteps, m_z1)!=StatusCode::SUCCESS
	    || nt->addItem ("t1", m_nsteps, m_t1)!=StatusCode::SUCCESS
	    || nt->addItem ("x2", m_nsteps, m_x2)!=StatusCode::SUCCESS
	    || nt->addItem ("y2", m_nsteps, m_y2)!=StatusCode::SUCCESS
	    || nt->addItem ("z2", m_nsteps, m_z2)!=StatusCode::SUCCESS
	    || nt->addItem ("t2", m_nsteps, m_t2)!=StatusCode::SUCCESS
	    || nt->addItem ("dep", m_nsteps, m_dep)!=StatusCode::SUCCESS
	    || nt->addItem ("ke1", m_nsteps, m_ke1)!=StatusCode::SUCCESS
	    || nt->addItem ("ke2", m_nsteps, m_ke2)!=StatusCode::SUCCESS
	    || nt->addItem ("rh", m_nsteps, m_rh)!=StatusCode::SUCCESS
	    || nt->addItem ("rhid", m_nsteps, m_rhid)!=StatusCode::SUCCESS
	    || nt->addItem ("step", m_nsteps, m_step)!=StatusCode::SUCCESS
	    || nt->addItem ("pt1", m_nsteps, m_pt1)!=StatusCode::SUCCESS
	    || nt->addItem ("pt2", m_nsteps, m_pt2)!=StatusCode::SUCCESS
	    || nt->addItem ("minA",m_nsteps, m_minA)!=StatusCode::SUCCESS
	    || nt->addItem ("v2",m_nsteps, m_v2)!=StatusCode::SUCCESS
	    || nt->addItem ("vthresh",m_nsteps, m_vthresh)!=StatusCode::SUCCESS
	    || nt->addItem ("vbelowthresh",m_nsteps, m_vbelowthresh)!=StatusCode::SUCCESS
	    || nt->addItem ("evtid", m_evtid)!=StatusCode::SUCCESS)

	  ATH_MSG_ERROR("Could not configure branches ");
	
      }
      
      else ATH_MSG_ERROR("Could not book step ntuple!! ");
    }
    
    //set initial values
    nevents=0;
    
    //These are the RHadron pdg_id
    
    //newest codes for intermediate states
    rhs.insert(1000991);
    rhs.insert(1009211);
    rhs.insert(1009111);
    rhs.insert(1009311);
    rhs.insert(1009321);
    rhs.insert(1093122);
    rhs.insert(1092212);
    rhs.insert(1092112);
    //All other Rhadron codes
    rhs.insert(1000993);
    rhs.insert(1009213);
    rhs.insert(1009313);
    rhs.insert(1009323);
    rhs.insert(1009113);
    rhs.insert(1009223);
    rhs.insert(1009333);
    rhs.insert(1091114);
    rhs.insert(1092114);
    rhs.insert(1092214);
    rhs.insert(1092224);
    rhs.insert(1093114);
    rhs.insert(1093214);
    rhs.insert(1093224);
    rhs.insert(1093314);
    rhs.insert(1093324);
    rhs.insert(1093334);
    //adding to stop rhadron values by pdg_id
    rhs.insert(1000612);
    rhs.insert(1000622);
    rhs.insert(1000632);
    rhs.insert(1000642);
    rhs.insert(1000652);
    rhs.insert(1006113);
    rhs.insert(1006211);
    rhs.insert(1006213);
    rhs.insert(1006223);
    rhs.insert(1006311);
    rhs.insert(1006313);
    rhs.insert(1006321);
    rhs.insert(1006323);
    rhs.insert(1006333);
    //adding to sbottom rhadron values by pdg_id
    rhs.insert(1000512);
    rhs.insert(1000522);
    rhs.insert(1005211);
    //adding the stau by pdg_id
    rhs.insert(1000015);
    assert(rhs.size()==43);
    
  }
  
  void SG_StepNtuple::BeginOfEventAction(const G4Event*){
    m_nsteps=0;
    rhid=0;//the rhadron index (either the first or second rhadon, usually)
    nevents++; m_evtid=nevents;//since it gets cleared out after every fill...
    
  }
  
  void SG_StepNtuple::EndOfEventAction(const G4Event*){

    if(! ntupleSvc()->writeRecord("/NTUPLES/FILE1/StepNtuple/10").isSuccess())
      ATH_MSG_ERROR( " failed to write record for this event" );
    
    //this also seems to zero out all the arrays... so beware!
  }
  
  void SG_StepNtuple::UserSteppingAction(const G4Step* aStep){
    if(m_nsteps<50000){
      int pdg = aStep->GetTrack()->GetDefinition()->GetPDGEncoding();
      bool rhad=false;
      if (std::find(rhs.begin(),rhs.end(),std::abs(pdg))!=rhs.end()) {
	rhad=true;
      }
      
      //
      if (!rhad && isSUSYParticle(std::abs(pdg))){
	std::cout<<"ACH139: SG_StepNtuple: other code thinks "<<pdg<<" is an Rhadron!"<<std::endl;
	rhad=true;
      }
      //
      
      if (rhad){
	
	//
	G4Material * mat = aStep->GetTrack()->GetMaterial();
	double minA=1500000.;
	for (unsigned int i=0;i<mat->GetNumberOfElements();++i){
	  if (mat->GetElement(i) && minA>mat->GetElement(i)->GetN()){
	    minA=mat->GetElement(i)->GetN();
	  }
	}
	//
	
	bool firstslow = aStep->GetPostStepPoint()->GetVelocity()<0.15*std::pow(minA,-2./3.)*CLHEP::c_light;
	//just save the first slow step for the rhadron
	for (int i=0; i<m_nsteps; ++i){
	  if (m_rhid[i]==rhid && m_vbelowthresh[i]>0) firstslow=false;
	}
	if (firstslow || aStep->GetTrack()->GetCurrentStepNumber()<=1 || aStep->GetPostStepPoint()->GetKineticEnergy()==0.){
	  
	  //
	  //int id = aStep->GetTrack()->GetDynamicParticle()->GetDefinition()->GetPDGEncoding();
	  int id = std::abs(aStep->GetTrack()->GetDynamicParticle()->GetDefinition()->GetPDGEncoding());
	  if (id>=1000000 && id<=1100000 && isSUSYParticle(id)){
	    m_rh[m_nsteps] = 1;//other code agrees it's an Rhadron
	  }
	  else{
	    m_rh[m_nsteps] = 0;//other code doesn't agree it's an Rhadron
	  }
	  //
	  
	  if (aStep->GetPreStepPoint()->GetGlobalTime()==0) rhid++;
	  m_rhid[m_nsteps]=rhid;
	  
	  m_pdg[m_nsteps]=aStep->GetTrack()->GetDefinition()->GetPDGEncoding();
	  m_charge[m_nsteps]=aStep->GetTrack()->GetDefinition()->GetPDGCharge();
	  m_dep[m_nsteps]=aStep->GetTotalEnergyDeposit();
	  m_mass[m_nsteps]=aStep->GetTrack()->GetDefinition()->GetPDGMass();
	  m_baryon[m_nsteps]=aStep->GetTrack()->GetDefinition()->GetBaryonNumber();
	  m_t1[m_nsteps]=aStep->GetPreStepPoint()->GetGlobalTime();
	  m_t2[m_nsteps]=aStep->GetPostStepPoint()->GetGlobalTime();
	  G4ThreeVector pos1=aStep->GetPreStepPoint()->GetPosition();
	  m_x1[m_nsteps]=pos1.x();
	  m_y1[m_nsteps]=pos1.y();
	  m_z1[m_nsteps]=pos1.z();
	  G4ThreeVector pos2=aStep->GetPostStepPoint()->GetPosition();
	  m_x2[m_nsteps]=pos2.x();
	  m_y2[m_nsteps]=pos2.y();
	  m_z2[m_nsteps]=pos2.z();
	  m_ke1[m_nsteps]=aStep->GetPreStepPoint()->GetKineticEnergy();
	  m_ke2[m_nsteps]=aStep->GetPostStepPoint()->GetKineticEnergy();
	  m_pt1[m_nsteps]=aStep->GetPreStepPoint()->GetMomentum().perp();
	  m_pt2[m_nsteps]=aStep->GetPostStepPoint()->GetMomentum().perp();
	  
	  //
	  m_minA[m_nsteps]=minA;
	  m_v2[m_nsteps]=aStep->GetPostStepPoint()->GetVelocity();
	  m_vthresh[m_nsteps]=0.15*std::pow(minA,-2./3.)*CLHEP::c_light;
	  m_vbelowthresh[m_nsteps]=(firstslow?1:0);
	  //
	  
	  m_step[m_nsteps]=m_nsteps;
	  ++m_nsteps;
	  //std::cout<<"stepping, size is "<<m_nsteps<<std::endl;
	} // writing the step because it stopped or is the start of the "track"
      } //rhad true
      else {
	
	//KILL the particles here, so we don't waste time in GEANT tracking what happens to it!
	if (std::abs(pdg)>1000000 && std::abs(pdg)<10000000){
	  std::cout<<"ACH129: SG_StepNtuple: Killing non-rh track with pdg "<<pdg<<std::endl;
	}
	aStep->GetTrack()->SetTrackStatus(fKillTrackAndSecondaries);
	const G4TrackVector *tv = aStep->GetSecondary();
	//if ((*tv).size()>0) std::cout<<" ... and its "<<(*tv).size()<<" secondaries"<<std::endl;
	for (unsigned int i=0;i<tv->size();i++){
	  G4Track *t = (*tv)[i];
	  t->SetTrackStatus(fKillTrackAndSecondaries);
	}
	
      } // not an rhad
      
    } //m_nsteps<50000
    
    
  }
  

  bool SG_StepNtuple::isSUSYParticle(const int id) const
  {
    if (id==1000021 || id==1000005 || id==1000006 || id==1000512 || id==1000522 || id==1000991 || id==1000993 ||
	id==1000612 || id==1000622 || id==1000632 || id==1000642 || id==1000652 || id==1005211 ||
	id==1006113 || id==1006211 || id==1006213 || id==1006223 || id==1006311 ||
	id==1006313 || id==1006321 || id==1006323 || id==1006333 ||
	id==1009111 || id==1009113 || id==1009211 || id==1009213 || id==1009311 ||
	id==1009313 || id==1009321 || id==1009323 || id==1009223 || id==1009333 ||
	id==1092112 || id==1091114 || id==1092114 || id==1092212 || id==1092214 || id==1092224 ||
	id==1093114 || id==1093122 || id==1093214 || id==1093224 || id==1093314 || id==1093324 || id==1093334)
      return true;
    return false;
  }
  
  
} // namespace G4UA 
