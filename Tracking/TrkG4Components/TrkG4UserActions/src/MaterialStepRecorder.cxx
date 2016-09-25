/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// MaterialStepRecorder.cxx, (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////

#include "TrkG4UserActions/MaterialStepRecorder.h"
#include "TrkGeometry/MaterialStep.h"
#include <iostream>
#include "GaudiKernel/IDataProviderSvc.h"
#include "GaudiKernel/INTupleSvc.h" 
#include "GaudiKernel/NTuple.h"
#include "GaudiKernel/SmartDataPtr.h"
#include "GaudiKernel/MsgStream.h"
#include "SimHelpers/ServiceAccessor.h"
#include "StoreGate/StoreGateSvc.h"
#include "G4Step.hh"
#include "G4Material.hh"
#include "G4Element.hh"
#include "G4StepPoint.hh"
#include "G4TouchableHistory.hh"
#include "G4LogicalVolume.hh"


#include "GaudiKernel/Bootstrap.h"
#include "GaudiKernel/ISvcLocator.h"
#include "GaudiKernel/IMessageSvc.h"

namespace G4UA{
  
  
  MaterialStepRecorder::MaterialStepRecorder():
    AthMessaging(Gaudi::svcLocator()->service< IMessageSvc >( "MessageSvc" ),"MaterialStepRecorder"),
    m_evtStore("StoreGateSvc/StoreGateSvc","MaterialStepRecorder"),
    m_detStore("StoreGateSvc/DetectorStore","MaterialStepRecorder"),
    m_matStepCollection(0),
    m_matStepCollectionName("MaterialStepRecords"),
    m_totalNbOfAtoms(0),
    m_totalSteps(0),
    m_eventID(0)
   
  {
  }

  void MaterialStepRecorder::beginOfEvent(const G4Event*){

    
    ATH_MSG_DEBUG(" BeginOfEventAction");
    
    // create a new Collection
    m_matStepCollection = new Trk::MaterialStepCollection;
    
    //    m_eventStepLength = 0;
    
  }
  
  void MaterialStepRecorder::endOfEvent(const G4Event*){

    ATH_MSG_DEBUG(" EndOfEventAction");
    
    ++m_eventID;
    // write the Collection to StoreGate
    if(m_evtStore->record(m_matStepCollection, m_matStepCollectionName, false).isFailure())
      ATH_MSG_ERROR("cannot record step collection to StoreGate");
    
  }
  
  void MaterialStepRecorder::beginOfRun(const G4Run*){

    ATH_MSG_DEBUG(" BeginOfRunAction");
    
    // initialize
    m_totalSteps = 0;
    m_eventID = 0;
    
  }
  
  void MaterialStepRecorder::processStep(const G4Step* aStep){;
    
    // std::cout << "[MaterialStepRecorder] SteppingAction" << std::endl;
    
    // kill secondaries
    if (aStep->GetTrack()->GetParentID()) {
      aStep->GetTrack()->SetTrackStatus(fStopAndKill);
      return;
    }
    
    // the material information
    G4TouchableHistory* touchHist = (G4TouchableHistory*)aStep->GetPreStepPoint()->GetTouchable();
    // G4LogicalVolume
    G4LogicalVolume *lv= touchHist ? touchHist->GetVolume()->GetLogicalVolume() : 0;
    G4Material *mat    = lv ? lv->GetMaterial() : 0;
    
    // log the information // cut off air
    if (mat && mat->GetRadlen() < 200000) {
      
      ++m_totalSteps;
      
      // the step information
      double steplength     = aStep->GetStepLength();
      //        m_eventStepLength    += steplength;
      // the position information
      G4ThreeVector pos     = aStep->GetPreStepPoint()->GetPosition();
      
      double X0             = mat->GetRadlen();
      double L0             = mat->GetNuclearInterLength();
      double A              = 0.;
      double Z              = 0.;
      //        double totAtoms       = 0.;
      
      double rho            = mat->GetDensity()*CLHEP::mm3/CLHEP::gram;
      
      // get the number of Elements
      size_t                 elNumber = mat->GetNumberOfElements();
      const G4ElementVector* elVector = mat->GetElementVector();
      double                 totAtoms = mat->GetTotNbOfAtomsPerVolume();
      
      if (1 == elNumber) {
	
	A   = mat->GetA()/CLHEP::gram;
	Z   = mat->GetZ();
	
      } else {
	
	const G4double*   atVector = mat->GetVecNbOfAtomsPerVolume();
	
	for (size_t iel = 0; iel<elNumber; ++iel) {
	  
	  G4Element*  currentEl  = (*elVector)[iel];
	  double      currentNum = atVector ? atVector[iel] : 1.;
	  double      relNbAtoms = currentNum/totAtoms;
	  
	  A += relNbAtoms*currentEl->GetA()/CLHEP::gram;
	  Z += relNbAtoms*currentEl->GetZ();
	  
	}
	
	//            A /= totAtoms;
	//            Z /= totAtoms;
	
      }
      
      // is it a Geantino?
      if (aStep->GetTrack()->GetParticleDefinition()->GetPDGEncoding() == 0) {
	
	m_matStepCollection->push_back(new Trk::MaterialStep(pos.x(), pos.y(), pos.z(), steplength, X0, L0, A, Z, rho));
	
      }
      
    }
    
  }
  
  
} // namespace G4UA 
