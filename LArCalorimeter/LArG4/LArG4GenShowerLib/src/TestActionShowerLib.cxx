/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/


#include "LArG4GenShowerLib/TestActionShowerLib.h"

#include <stdexcept>
#include <map>
#include <string>
#include <stdlib.h>
#include <string.h>

#include "G4Step.hh"
#include "G4Event.hh"
#include "G4EventManager.hh"
#include "G4ThreeVector.hh"

// all the LAr calculators
#include "LArG4Code/LArVCalculator.h"
#include "LArG4Barrel/LArBarrelCalculator.h"
#include "LArG4EC/EnergyCalculator.h"
#include "LArG4FCAL/LArFCAL1Calculator.h"
#include "LArG4FCAL/LArFCAL2Calculator.h"
#include "LArG4FCAL/LArFCAL3Calculator.h"
#include "LArG4HEC/LArHECCalculator.h"
#include "LArG4HEC/LArHECLocalCalculator.h"

#include "LArG4GenShowerLib/StepInfo.h"
#include "LArG4GenShowerLib/StepInfoCollection.h"

// For MC Truth information:
#include "GeneratorObjects/McEventCollection.h"

TestActionShowerLib::TestActionShowerLib(const std::string& type, const std::string& name, const IInterface* parent):UserActionBase(type,name,parent),
														     m_current_calculator(0),
														     m_current_solid(0),
														     m_current_transform(0),
														     m_calculator_EMECIW(0),
														     m_calculator_EMECOW(0),
														     m_eventSteps(0)
{
#ifdef _myDEBUG_
  G4cout << "#########################################" << G4endl
	 << "##  TestActionShowerLib - Constructor  ##" << G4endl
	 << "#########################################" << G4endl;
#endif
}

TestActionShowerLib::~TestActionShowerLib()
{
}

void TestActionShowerLib::BeginOfEvent(const G4Event* )
{

  // init calculator
  if (m_calculator_EMECIW == 0)
    m_calculator_EMECIW = new LArG4::EC::EnergyCalculator(LArWheelCalculator::InnerAbsorberWheel);
  if (m_calculator_EMECOW == 0)
    m_calculator_EMECOW = new LArG4::EC::EnergyCalculator(LArWheelCalculator::OuterAbsorberWheel);
  if (m_current_transform == 0)
	m_current_transform = new G4AffineTransform ();

  m_eventSteps = new ShowerLib::StepInfoCollection();

#ifdef _myDEBUG_
  G4cout << "#########################################" << G4endl
	 << "##  TestActionShowerLib - BeginOfEvent ##" << G4endl
	 << "#########################################" << G4endl;
#endif

  return;
}

void TestActionShowerLib::EndOfEvent(const G4Event* )
{

#ifdef _myDEBUG_
  G4cout << "#########################################" << G4endl
	 << "##    TestActionShowerLib - EndOfEvent ##" << G4endl
	 << "##         MMM="<<m_count<<"     ###" << G4endl
	 << "#########################################" << G4endl;
#endif

  //
  // Zero order cleanup
  //   combine hits from the same spot (distance < 1 mm^2)
  //   only necessary for large number of spots (> 500)
  //
  const double dsame = 1.; // 1mm^2

#ifdef _myDEBUG_
  G4cout << "TestActionShowerLib::EndOfEventAction: Before initial cleanup, N=" << m_eventSteps->size() << G4endl;
#endif

  if (m_eventSteps->size()>500) {
    ShowerLib::StepInfoCollection::iterator i1 = m_eventSteps->begin();
    while (i1 != m_eventSteps->end()) {
      ShowerLib::StepInfoCollection::iterator i2 = i1 + 1;
      while (i2 != m_eventSteps->end()) {
    	  // if distance below cut off, combined and erase
    	  if ( (i1 != i2) && ((*i1)->diff2(**i2) < dsame)) {
    		  **i1 += **i2;
    		  i2 = m_eventSteps->erase(i2);
    	  } else {
    		  ++i2;
    	  }
      }
      ++i1;
    }
  }

#ifdef _myDEBUG_
  G4cout << "TestActionShowerLib::EndOfEventAction: After initial cleanup, N=" << m_eventSteps->size() << G4endl;
#endif

  //
  // Put eventSteps into event store
  //
  std::string location("EventSteps");
  StatusCode sc = evtStore()->record( m_eventSteps, location, true );
  if( sc.isFailure() ) {
    ATH_MSG_ERROR("Error: Couldn't store EventSteps object in event store at location: " << location);
  } else {
#ifdef _myDEBUG_
    G4cout << "TestActionShowerLib::EndOfEventAction: Stored EventSteps object (size: " << m_eventSteps->size() << ")"
	   << " in event store at location: " << location << G4endl;
#endif
  }

  return;
}

void TestActionShowerLib::BeginOfRun(const G4Run* )
{
#ifdef _myDEBUG_
  G4cout << "#########################################" << G4endl
	 << "##    TestActionShowerLib - BeginOfRun ##" << G4endl
	 << "#########################################" << G4endl;
#endif

  // init calculator
  if (m_calculator_EMECIW == 0)
    m_calculator_EMECIW = new LArG4::EC::EnergyCalculator(LArWheelCalculator::InnerAbsorberWheel);
  if (m_calculator_EMECOW == 0)
    m_calculator_EMECOW = new LArG4::EC::EnergyCalculator(LArWheelCalculator::OuterAbsorberWheel);
  if (m_current_transform == 0)
	m_current_transform = new G4AffineTransform ();

  return;
}

void TestActionShowerLib::EndOfRun(const G4Run* )
{

  if (m_calculator_EMECIW != 0) {
    delete m_calculator_EMECIW;
    m_calculator_EMECIW = 0;
  }

  if (m_calculator_EMECOW != 0) {
    delete m_calculator_EMECOW;
    m_calculator_EMECOW = 0;
  }

#ifdef _myDEBUG_
  G4cout << "#########################################" << G4endl
	 << "##    TestActionShowerLib - EndOfRun   ##" << G4endl
	 << "#########################################" << G4endl;
#endif

  return;
}

void TestActionShowerLib::Step(const G4Step* aStep)
{
	bool emptydet = (m_eventSteps->detector[0] == '\0'); //empty string. man, i hate pure C!
	if (emptydet) { //give name to the detector, set calculator, transformation and G4Solid for the whole shower
    	G4ThreeVector pos = aStep->GetPostStepPoint()->GetPosition();
    	G4TouchableHistory* theTouchable = (G4TouchableHistory*)(aStep->GetPostStepPoint()->GetTouchable());
    	int depth = theTouchable->GetHistoryDepth();
    	bool correct_volume = false;
    	if (depth < 2) { //this is obviously the wrong volume
    		correct_volume = true; // this may appear misleading, but actually it means: "no need to look deeper"
    	}
    	G4VPhysicalVolume* rootVol = theTouchable->GetVolume(depth - 1);
    	int curdepth = 0;
		m_current_transform->SetNetRotation(*(theTouchable->GetRotation(curdepth)));
		m_current_transform->SetNetTranslation(theTouchable->GetTranslation(curdepth));
		m_current_transform->Invert();
    	G4VPhysicalVolume* cur_volume = theTouchable->GetVolume(curdepth);
    	G4LogicalVolume* cur_log_volume = cur_volume->GetLogicalVolume();

    	do {
    		if ((cur_log_volume->GetName() == "LArMgr::LAr::FCAL::Module1::Absorber") ||
       			(cur_log_volume->GetName() == "LArMgr::LAr::FCAL::Module2::Absorber") ||
       			(cur_log_volume->GetName() == "LArMgr::LAr::EMEC::Pos::OuterWheel") ||
       			(cur_log_volume->GetName() == "LArMgr::LAr::EMEC::Neg::OuterWheel") ||
       			(cur_log_volume->GetName() == "LArMgr::LAr::EMEC::Pos::InnerWheel") ||
       			(cur_log_volume->GetName() == "LArMgr::LAr::EMEC::Neg::InnerWheel") ||
       			(cur_log_volume->GetName() == "LArMgr::LAr::EMB::STAC")) {
    			correct_volume = true;
    		}
    		if (!correct_volume) { //go one level up
    			curdepth++;
    			m_current_transform->SetNetRotation(*(theTouchable->GetRotation(curdepth)));
    			m_current_transform->SetNetTranslation(theTouchable->GetTranslation(curdepth));
    			cur_volume = theTouchable->GetVolume(curdepth);
    			cur_log_volume = cur_volume->GetLogicalVolume();
    		}
    	} while ((!correct_volume) && (cur_volume != rootVol) && (curdepth < (depth-1)));

    	m_current_solid = cur_log_volume->GetSolid();

    	if        (cur_log_volume->GetName() == "LArMgr::LAr::FCAL::Module1::Absorber") {
    		// shower is inside FCAL1
    		m_current_calculator = LArFCAL1Calculator::GetInstance();
    		strcpy(m_eventSteps->detector,"FCAL1");
    	} else if (cur_log_volume->GetName() == "LArMgr::LAr::FCAL::Module2::Absorber") {
    		// shower is inside FCAL2
    		m_current_calculator = LArFCAL2Calculator::GetInstance();
    		strcpy(m_eventSteps->detector,"FCAL2");
    	} else if ((cur_log_volume->GetName() == "LArMgr::LAr::EMEC::Pos::InnerWheel") ||
                   (cur_log_volume->GetName() == "LArMgr::LAr::EMEC::Neg::InnerWheel")) {
    		// shower is inside inner EMEC
    		m_current_calculator = m_calculator_EMECIW;
    		strcpy(m_eventSteps->detector,"EMEC");
    	} else if ((cur_log_volume->GetName() == "LArMgr::LAr::EMEC::Pos::OuterWheel") ||
                   (cur_log_volume->GetName() == "LArMgr::LAr::EMEC::Neg::OuterWheel")) {
    		// shower is inside outer EMEC positive
    		m_current_calculator = m_calculator_EMECOW;
    		strcpy(m_eventSteps->detector,"EMEC");
    	} else if (cur_log_volume->GetName() == "LArMgr::LAr::EMB::STAC") {
    		// shower is inside EMB positive
    		m_current_calculator = LArBarrelCalculator::GetCalculator();
    		strcpy(m_eventSteps->detector,"EMB");
    	} else {
    		// outside.
    		m_current_calculator = NULL;
    	}
	}

  if (aStep->GetTotalEnergyDeposit()>0) {
	  //first, let's see if the shower is valid
	  if (m_current_calculator == NULL) {
		  m_eventSteps->invalid_energy += aStep->GetTotalEnergyDeposit();
		  return;
	  }
	  //then, let's see if we travel outside the detector volume

	  // Initial position:
	  G4ThreeVector pos1=aStep->GetPreStepPoint()->GetPosition();
	  G4ThreeVector pos2=aStep->GetPostStepPoint()->GetPosition();

	  // Average position:
	  G4ThreeVector pos = 0.5*(pos1+pos2);

	  G4ThreeVector prepos = pos1;
	  m_current_transform->ApplyPointTransform(prepos);
	  if (m_current_solid->Inside(prepos) == kOutside) {
		  m_eventSteps->invalid_energy += aStep->GetTotalEnergyDeposit();
		  return;
	  }

	G4VPhysicalVolume* pCurrentVolume = aStep->GetPreStepPoint()->GetPhysicalVolume();
    if (!pCurrentVolume->GetLogicalVolume()->GetSensitiveDetector()){
      return;
    }

    double et = 0; // Total collected charge

    if (m_current_calculator->Process(aStep)) {
		int nlarh = m_current_calculator->getNumHits();
		for (int i=0; i<nlarh; ++i) {
			et += (m_current_calculator->energy(i));
		}
    } else {
      G4cout << "Error: Hit not processed by calculator!" << G4endl;
      return;
    }

    // drop hits with zero deposited energy (could still happen with negative corrections from calculator)
    if (et <= 0.) {
      return;
    }

    ShowerLib::StepInfo* theInfo = new ShowerLib::StepInfo();

    theInfo->setE(et);

#ifdef _myDEBUG_
    G4cout<<" TAGINFO: "<< et <<" "<<aStep->GetTotalEnergyDeposit()<< G4endl;
#endif

    theInfo->setTime(aStep->GetPreStepPoint()->GetGlobalTime());

    theInfo->setP(pos);

    m_eventSteps->push_back(theInfo);

  } else {
#ifdef _myDEBUG_
    G4cout << "DEBUG: step " << aStep->GetTotalEnergyDeposit() << G4endl;
#endif
  }

  return;
}


StatusCode TestActionShowerLib::queryInterface(const InterfaceID& riid, void** ppvInterface)
{
  if ( IUserAction::interfaceID().versionMatch(riid) ) {
    *ppvInterface = dynamic_cast<IUserAction*>(this);
    addRef();
  } else {
    // Interface is not directly available : try out a base class
    return UserActionBase::queryInterface(riid, ppvInterface);
  }
  return StatusCode::SUCCESS;
}
