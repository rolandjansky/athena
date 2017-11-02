/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "StepNtuple.h"

#include "GaudiKernel/IDataProviderSvc.h"
#include "GaudiKernel/INTupleSvc.h"
#include "GaudiKernel/NTuple.h"
#include "GaudiKernel/SmartDataPtr.h"
#include "SimHelpers/ServiceAccessor.h"

#include "G4Step.hh"

#include <iostream>


#include "GaudiKernel/Bootstrap.h"
#include "GaudiKernel/ISvcLocator.h"
#include "GaudiKernel/IMessageSvc.h"

namespace G4UA{
  
  
  StepNtuple::StepNtuple():AthMessaging(Gaudi::svcLocator()->service< IMessageSvc >( "MessageSvc" ),"StepNtuple"){;
  }
  void StepNtuple::BeginOfEventAction(const G4Event*){

    eventSteps.clear();

  }
  
  void StepNtuple::EndOfEventAction(const G4Event*){
    
    //std::cout<<"start end of event, size is  "<<eventSteps.size()<<std::endl;
    
    m_nsteps=eventSteps.size();
    
    //std::cout<<"in end of event"<<std::endl;
    
    for(unsigned int k=0;k<eventSteps.size();k++){
      
      //std::cout<<"in loop end of event "<<k<<std::endl;
      
      m_pdgcode[k]=eventSteps[k].code;
      m_step_x[k]=eventSteps[k].x;
      m_step_y[k]=eventSteps[k].y;
      m_step_z[k]=eventSteps[k].z;
      m_time[k]=eventSteps[k].time;
      m_dep[k]=eventSteps[k].dep;
      
    }
    
    if(! ntupleSvc()->writeRecord("/NTUPLES/FILE1/StepNtuple/10").isSuccess()){
      
      ATH_MSG_ERROR( " failed to write record for this event" );
      
    }
    
  }
  
  void StepNtuple::UserSteppingAction(const G4Step* aStep){

    
    if(eventSteps.size()<49000){
      
      stepdata theInfo;
      
      theInfo.dep=aStep->GetTotalEnergyDeposit();
      theInfo.time=aStep->GetPreStepPoint()->GetGlobalTime();
      theInfo.code=aStep->GetTrack()->GetDefinition()->GetPDGEncoding();
      G4ThreeVector pos=aStep->GetPreStepPoint()->GetPosition();
      theInfo.x=pos.x();
      theInfo.y=pos.y();
      theInfo.z=pos.z();
      
      eventSteps.push_back(theInfo);
      //std::cout<<"stepping, size is "<<eventSteps.size()<<std::endl;
      
    }
    
    
  }
  
  void StepNtuple::BeginOfRunAction(const G4Run*){
    
    NTupleFilePtr file1(ntupleSvc(), "/NTUPLES/FILE1");
    
    SmartDataPtr<NTuple::Directory>
      ntdir(ntupleSvc(),"/NTUPLES/FILE1/StepNtuple");
    if ( !ntdir )
      {
	//    otherwise create the directory
	ntdir = ntupleSvc()->createDirectory(file1,"StepNtuple");
      }
    if ( ! ntdir )
      {
	ATH_MSG_ERROR( " failed to get ntuple directory" );
      }
    
    NTuplePtr nt(ntupleSvc(), "/NTUPLES/FILE1/StepNtuple/10");
    
    if ( !nt )    {    // Check if already booked
      
      nt = ntupleSvc()->book (ntdir.ptr(), 10,CLID_ColumnWiseTuple, "G4 Step Ntuple");
      if ( nt )    {
	
	
	ATH_MSG_DEBUG( "booked ntuple " );
	
	// WARNING!! Force limit to 50k tracks
	
	if(
	   nt->addItem ("NSteps", m_nsteps,0 ,50000).isFailure() ||
	   nt->addItem ("PCode", m_nsteps, m_pdgcode ).isFailure() ||
	   nt->addItem ("step_x", m_nsteps, m_step_x).isFailure() ||
	   nt->addItem ("step_y", m_nsteps, m_step_y).isFailure() ||
	   nt->addItem ("step_z", m_nsteps, m_step_z).isFailure() ||
	   nt->addItem ("dep", m_nsteps, m_dep).isFailure() ||
	   nt->addItem ("time", m_nsteps, m_time).isFailure()
	   )
	  
	  ATH_MSG_ERROR( "Could not book ntuple!! " );

	return;
      }
    }
    
    
  }
  
} // namespace G4UA 
