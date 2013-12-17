/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/


#include "ISF_FastCaloSimParametrization/LArIdentifierConverter.h"
#include "ISF_Geant4UserActions/FastCaloSimParamAction.h"


#include <stdexcept>
#include <map>
#include <string>
#include <stdlib.h>
#include <string.h>

#include "G4Step.hh"
#include "G4Event.hh"
#include "G4EventManager.hh"
#include "G4ThreeVector.hh"

// athena includes
#include "GaudiKernel/Bootstrap.h"
#include "GaudiKernel/ISvcLocator.h"
#include "StoreGate/StoreGate.h"
#include "StoreGate/StoreGateSvc.h"

// all the LAr calculators
#include "LArG4Code/LArVCalculator.h"
#include "LArG4Barrel/LArBarrelCalculator.h"
#include "LArG4EC/EnergyCalculator.h"
#include "LArG4FCAL/LArFCAL1Calculator.h"
#include "LArG4FCAL/LArFCAL2Calculator.h"
#include "LArG4FCAL/LArFCAL3Calculator.h"
//#include "LArG4HEC/LArHECCalculator.h"
#include "LArG4HEC/LArHECWheelCalculator.h"
//#include "LArG4HEC/LArHECLocalCalculator.h"
#include "LArG4EC/LArEndcapPresamplerCalculator.h"
#include "LArG4Barrel/LArBarrelPresamplerCalculator.h"

#include "LArG4HitManagement/LArHitMerger.h"

// TileCAL calculators
#include "TileGeoG4SD/TileGeoG4SDCalc.hh"

#include "ISF_Event/FCS_StepInfo.h"
#include "ISF_Event/FCS_StepInfoCollection.h"

// For MC Truth information:
#include "GeneratorObjects/McEventCollection.h"

static FastCaloSimParamAction ts1("FastCaloSimParamAction");

FastCaloSimParamAction::FastCaloSimParamAction(std::string s)
  : FADS::UserAction(s),
    m_current_calculator(0),
    m_current_calculator_Tile(0),
    m_current_solid(0),
    m_current_transform(0),
    m_calculator_EMECIW_pos(0),
    m_calculator_EMECIW_neg(0),  
    m_calculator_EMECOW_pos(0), 
    m_calculator_EMECOW_neg(0),
    //m_calculator_BIB(0),
    m_calculator_BOB(0),
    m_calculator_EMB(0),
    m_calculator_FCAL1(0),
    m_calculator_FCAL2(0),
    m_calculator_FCAL3(0),
    m_calculator_HEC(0),
    //m_calculator_HECLocal(0),
    m_calculator_EMBPS(0),
    m_calculator_EMEPS(0),
    m_calculator_Tile(0),
    m_lar_helper(0),
    m_storeGateSvc("StoreGateSvc", s), 
    m_detStore(0),
    m_eventSteps(0),
    m_ndetectors(0)
{
#ifdef _myDEBUG_
  G4cout << "############################################" << G4endl
	 << "##  FastCaloSimParamAction - Constructor  ##" << G4endl
	 << "############################################" << G4endl;
#endif

}

FastCaloSimParamAction::~FastCaloSimParamAction()
{
}

FastCaloSimParamAction::StoreGateSvc_t FastCaloSimParamAction::storeGateSvc() const
{
  //
  // Connect to StoreGate:
  //
  if ( !m_storeGateSvc ) {
    StatusCode sc = m_storeGateSvc.retrieve();
    if (sc.isFailure()) {
      throw std::runtime_error("FastCaloSimParamAction: cannot retrieve StoreGateSvc");
    }
  }

  return m_storeGateSvc;
}

StoreGateSvc* FastCaloSimParamAction::detStoreSvc() const
{
  if (!m_detStore) 
    {
      ISvcLocator* svcLocator = Gaudi::svcLocator();
      IService* pSvc;
      StatusCode status = svcLocator->service("DetectorStore",pSvc);
      if (status)
	{
	  StoreGateSvc* detStore = dynamic_cast<StoreGateSvc*>(pSvc);
	  if (!detStore) 
	    {
	      throw std::runtime_error("ZH: 0 pointer to the Detector Store!");
	    }
	  else return detStore;
	}
      else
      {
	throw std::runtime_error("FastCaloSimParamAction: cannot get DetectorStore");
      }
    }

  return m_detStore;
}

void FastCaloSimParamAction::BeginOfEventAction(const G4Event* )
{
  //G4cout << "############################################" << G4endl
  //     << "##  FastCaloSimParamAction - BeginOfEvent ##" << G4endl
  //     << "############################################" << G4endl;

  // init calculator
  if (m_calculator_EMECIW_pos == 0)
    m_calculator_EMECIW_pos = new LArG4::EC::EnergyCalculator(LArWheelCalculator::InnerAbsorberWheel,LArG4::EC::EnergyCalculator::EMEC_ECOR_ROPT, 1);
  if (m_calculator_EMECOW_pos == 0)
    m_calculator_EMECOW_pos = new LArG4::EC::EnergyCalculator(LArWheelCalculator::OuterAbsorberWheel,LArG4::EC::EnergyCalculator::EMEC_ECOR_ROPT, 1);

  if (m_calculator_EMECIW_neg == 0)
    m_calculator_EMECIW_neg = new LArG4::EC::EnergyCalculator(LArWheelCalculator::InnerAbsorberWheel,LArG4::EC::EnergyCalculator::EMEC_ECOR_ROPT, -1);
  if (m_calculator_EMECOW_neg == 0)
    m_calculator_EMECOW_neg = new LArG4::EC::EnergyCalculator(LArWheelCalculator::OuterAbsorberWheel,LArG4::EC::EnergyCalculator::EMEC_ECOR_ROPT, -11);
  //if (m_calculator_BIB == 0)
  //  m_calculator_BIB = new LArG4::EC::EnergyCalculator(LArWheelCalculator::BackInnerBarretteWheel);
  if (m_calculator_BOB == 0)
    m_calculator_BOB = new LArG4::EC::EnergyCalculator(LArWheelCalculator::BackOuterBarretteWheel);
  if (m_calculator_FCAL1 == 0)
    m_calculator_FCAL1 = LArFCAL1Calculator::GetInstance();
  if (m_calculator_FCAL2 == 0)
    m_calculator_FCAL2 = LArFCAL2Calculator::GetInstance();
  if (m_calculator_FCAL3 == 0)
    m_calculator_FCAL3 = LArFCAL3Calculator::GetInstance();
  if (m_calculator_HEC == 0)
    m_calculator_HEC = LArHECWheelCalculator::GetCalculator();
  //if (m_calculator_HECLocal == 0)
  //  m_calculator_HECLocal = LArHECLocalCalculator::GetCalculator();
  if (m_calculator_EMB == 0)
    m_calculator_EMB = LArBarrelCalculator::GetCalculator();
  if (m_calculator_EMBPS == 0)
    m_calculator_EMBPS = LArBarrelPresamplerCalculator::GetCalculator();
  if (m_calculator_EMEPS == 0)
    m_calculator_EMEPS = LArEndcapPresamplerCalculator::GetCalculator();
  if (m_calculator_Tile == 0)
    m_calculator_Tile = TileGeoG4SDCalc::instance();

  if (m_current_transform == 0)
	m_current_transform = new G4AffineTransform ();


  m_eventSteps = new ISF_FCS_Parametrization::FCS_StepInfoCollection();
  //G4cout << "############################################" << G4endl
  //	 << "## FastCaloSimParamAction - BeginOfEvent2 ##" << G4endl
  //	 << "############################################" << G4endl;
 
  return;
}

void FastCaloSimParamAction::EndOfEventAction(const G4Event* )
{

  //G4cout << "############################################" << G4endl
  //	 << "##    FastCaloSimParamAction - EndOfEvent ##" << G4endl
  // << "##                                        ##" << G4endl
  // << "############################################" << G4endl;

  //
  // Zero order cleanup
  //   combine hits from the same spot (distance < 1 mm^2)
  //   only necessary for large number of spots (> 500)
  //
  //const double dsame = 1.; // 1mm^2
  //don't need it at the moment
#ifdef _myDEBUG_
  G4cout << "FastCaloSimParamAction::EndOfEventAction: Before initial cleanup, N=" << m_eventSteps->size() << G4endl;
#endif
  /*
  if (m_eventSteps->size()>2000) { //can have more.. ZH
    std::cout <<"ZH too many hits: "<<m_eventSteps->size()<<" will merge"<<std::endl;
   
    ISF_FCS_Parametrization::FCS_StepInfoCollection::iterator i1 = m_eventSteps->begin();
    while (i1 != m_eventSteps->end()) {
      ISF_FCS_Parametrization::FCS_StepInfoCollection::iterator i2 = i1 + 1;
      while (i2 != m_eventSteps->end()) {
    	  // if distance below cut off, combined and erase && make sure they're in the same detector ZH && time!!!??
	if ( (i1 != i2) && ((*i1)->diff2(**i2) < dsame) && (*i1)->cell_ID() == (*i2)->cell_ID()  ) {
    		  **i1 += **i2;
    		  i2 = m_eventSteps->erase(i2);
    	  } else {
    		  ++i2;
    	  }
      }
      ++i1;
    }
  }
  */
  //  G4cout << "FastCaloSimParamAction::EndOfEventAction: After initial cleanup, N=" << m_eventSteps->size() << G4endl;
  if (m_eventSteps->size()==0) return; //don't need to play with it
  G4cout << "FastCaloSimParamAction::EndOfEventAction: After initial cleanup, N=" << m_eventSteps->size() << G4endl;

  //
  // Put eventSteps into event store
  //
  std::string location("ZHEventSteps");
  ISF_FCS_Parametrization::FCS_StepInfoCollection* test;
  // std::cout <<"Check if already in StoreGate:"<<std::endl;
  if (storeGateSvc()->contains<ISF_FCS_Parametrization::FCS_StepInfoCollection>(location))
    {
      StatusCode check = storeGateSvc()->retrieve(test,location);
      if (check.isSuccess())
	{
	  //std::cout <<"ZH, Already have in StoreGate : "<<test->size()<<std::endl;
	  //want to merge and overwrite!
	  for (ISF_FCS_Parametrization::FCS_StepInfoCollection::iterator iter = m_eventSteps->begin();iter != m_eventSteps->end();iter++)
	    {
	      test->push_back((*iter));
	    }
	  //std::cout <<"Now have: "<<test->size()<<std::endl;
	  //      StatusCode sc = storeGateSvc()->remove(                                     
	  //check
	  check = storeGateSvc()->retrieve(test,location);
	  if (check.isSuccess())
	    { 
	      std::cout <<"ZH, check in StoreGate : "<<test->size()<<std::endl;
	    }
	  /*	  
      StatusCode sc = storeGateSvc()->record( test, location); //want to overwrite? but current release doesn't have this method???
      if( sc.isFailure() ) {
      	G4cout << "Error: Couldn't store EventSteps object in event store at location: " << location << G4endl;//
            } else {

	  G4cout << "Info: Stored EventSteps object (size: " << test->size() << ")"
		 << " in event store at location: " << location << G4endl;
	      }
	  */
	}
      else
	{
	  std::cout <<"ZH WTF ??"<<std::endl;
	}
    }
  else
    {
      StatusCode sc = storeGateSvc()->record( m_eventSteps, location, true );
      if( sc.isFailure() ) {
	G4cout << "Error: Couldn't store EventSteps object in event store at location: " << location << G4endl;
      } else {
	G4cout << "Info: Stored EventSteps object (size: " << m_eventSteps->size() << ")"
	       << " in event store at location: " << location << G4endl;
      }
    }
  return;
}

void FastCaloSimParamAction::BeginOfRunAction(const G4Run* )
{
  //G4cout << "############################################" << G4endl
  // << "##    FastCaloSimParamAction - BeginOfRun ##" << G4endl
  // << "############################################" << G4endl;

  // ?? Ok, where do I need this??
  // init calculator
  if (m_calculator_EMECIW_pos == 0)
    m_calculator_EMECIW_pos = new LArG4::EC::EnergyCalculator(LArWheelCalculator::InnerAbsorberWheel,LArG4::EC::EnergyCalculator::EMEC_ECOR_ROPT, 1);
  if (m_calculator_EMECOW_pos == 0)
    m_calculator_EMECOW_pos = new LArG4::EC::EnergyCalculator(LArWheelCalculator::OuterAbsorberWheel,LArG4::EC::EnergyCalculator::EMEC_ECOR_ROPT, 1);
  if (m_calculator_EMECIW_neg == 0)
    m_calculator_EMECIW_neg = new LArG4::EC::EnergyCalculator(LArWheelCalculator::InnerAbsorberWheel,LArG4::EC::EnergyCalculator::EMEC_ECOR_ROPT, -1);
  if (m_calculator_EMECOW_neg == 0)
    m_calculator_EMECOW_neg = new LArG4::EC::EnergyCalculator(LArWheelCalculator::OuterAbsorberWheel,LArG4::EC::EnergyCalculator::EMEC_ECOR_ROPT, -11);
  //if (m_calculator_BIB == 0)
  //  m_calculator_BIB = new LArG4::EC::EnergyCalculator(LArWheelCalculator::BackInnerBarretteWheel);
  if (m_calculator_BOB == 0)
    m_calculator_BOB = new LArG4::EC::EnergyCalculator(LArWheelCalculator::BackOuterBarretteWheel);
  if (m_calculator_FCAL1 == 0)
    m_calculator_FCAL1 = LArFCAL1Calculator::GetInstance();
  if (m_calculator_FCAL2 == 0)
    m_calculator_FCAL2 = LArFCAL2Calculator::GetInstance();
  if (m_calculator_FCAL3 == 0)
    m_calculator_FCAL3 = LArFCAL3Calculator::GetInstance();
  if (m_calculator_HEC == 0)
    m_calculator_HEC = LArHECWheelCalculator::GetCalculator();
  //if (m_calculator_HECLocal == 0)
  //  m_calculator_HECLocal = LArHECLocalCalculator::GetCalculator();
  if (m_calculator_EMB ==0)
    m_calculator_EMB = LArBarrelCalculator::GetCalculator();
  if (m_calculator_EMBPS == 0)
    m_calculator_EMBPS = LArBarrelPresamplerCalculator::GetCalculator();
  if (m_calculator_EMEPS == 0)
    m_calculator_EMEPS = LArEndcapPresamplerCalculator::GetCalculator();
  if (m_calculator_Tile ==0)
    m_calculator_Tile = TileGeoG4SDCalc::instance();

  if (m_current_transform == 0)
	m_current_transform = new G4AffineTransform ();
  m_lar_helper = new LArIdentifierConverter(detStoreSvc());


  return;
}

void FastCaloSimParamAction::EndOfRunAction(const G4Run* )
{
  G4cout << "############################################" << G4endl                                                                      
	 << "##    FastCaloSimParamAction - EndOfRun   ##" << G4endl                                                                      
         << "##          deleting calculators          ##" << G4endl
         << "############################################" << G4endl;         

  if (m_calculator_EMECIW_pos != 0) {
    delete m_calculator_EMECIW_pos;
    m_calculator_EMECIW_pos = 0;
  }
  if (m_calculator_EMECIW_neg != 0) {
    delete m_calculator_EMECIW_neg;
    m_calculator_EMECIW_neg = 0;
  }
  if (m_calculator_EMECOW_pos != 0) {
    delete m_calculator_EMECOW_pos;
    m_calculator_EMECOW_pos = 0;
  }
  if (m_calculator_EMECOW_neg != 0) {
    delete m_calculator_EMECOW_neg;
    m_calculator_EMECOW_neg = 0;
  }
  //if (m_calculator_BIB != 0) {
  //  delete m_calculator_BIB;
  //  m_calculator_BIB=0;
  //}
  if (m_calculator_BOB != 0) {
    delete m_calculator_BOB;
    m_calculator_BOB=0;
  }
  if (m_calculator_EMB !=0) {
    delete m_calculator_EMB;
    m_calculator_EMB = 0;
  }

  if (m_calculator_FCAL1 !=0) {
    delete m_calculator_FCAL1;
    m_calculator_FCAL1 = 0;
  }

  if (m_calculator_FCAL2 !=0) {
    delete m_calculator_FCAL2;
    m_calculator_FCAL2 = 0;
  }

  if (m_calculator_FCAL3 !=0) {
    delete m_calculator_FCAL3;
    m_calculator_FCAL3 = 0;
  }

  if (m_calculator_HEC !=0) {
    delete m_calculator_HEC;
    m_calculator_HEC = 0;
  }
  std::cout <<"ZH EM PS calculators: "<<std::endl;
  
  if (m_calculator_EMBPS !=0) {
    delete m_calculator_EMBPS;
    m_calculator_EMBPS = 0;
  }
  /*
  if (m_calculator_EMEPS !=0) {
    delete m_calculator_EMEPS;
    m_calculator_EMEPS = 0;
  }
  */
  //if (m_calculator_HECLocal !=0) {
  //  delete m_calculator_HECLocal;
  //  m_calculator_HECLocal = 0;
  //}

  if (m_calculator_Tile !=0) {
    delete m_calculator_Tile;
    m_calculator_Tile = 0;

  }

  std::cout <<"ZH good detector map: "<<std::endl;
  for (std::map<std::string, int>::iterator it = m_detectormap.begin(); it!= m_detectormap.end(); ++it)
    {
      std::cout <<it->second<<"  "<<it->first<<std::endl;
    }
  std::cout <<"-----------------------------------------------"<<std::endl;
  std::cout <<"List of unused parts :"<<std::endl;
  for (std::set<std::string>::iterator it = m_unuseddetector.begin(); it!=m_unuseddetector.end(); it++)
    {
      std::cout <<(*it)<<"   ";
    }
  std::cout <<std::endl;


  G4cout << "############################################" << G4endl
	 << "##    FastCaloSimParamAction - EndOfRun   ##" << G4endl
	 << "############################################" << G4endl;

  return;
}

void FastCaloSimParamAction::SteppingAction(const G4Step* aStep)
{
  //G4cout <<"FastCaloSimParamAction - SteppingAction"<<G4endl;
  m_current_calculator = NULL;
  m_current_calculator_Tile = NULL;

  G4ThreeVector position1 = aStep->GetPreStepPoint()->GetPosition(); //pre step is the position i'm interested in
  G4ThreeVector position2 = aStep->GetPostStepPoint()->GetPosition();
  G4ThreeVector pos = 0.5*(position1+position2);

  G4TouchableHistory* theTouchable = (G4TouchableHistory*)(aStep->GetPreStepPoint()->GetTouchable()); //this has all the history depth
  if (!theTouchable)
    {
      //This shouldn't happen anyway
      //std::cout <<"ZH Missing touchable??"<<std::endl;
      return;
    }
  int depth = theTouchable->GetHistoryDepth();
  G4VPhysicalVolume *pCurrentVolume = aStep->GetPreStepPoint()->GetPhysicalVolume();
  if (!pCurrentVolume)
    {
      //std::cout <<"ZH Missing current volume??"<<std::endl;
      return;
    }

  G4LogicalVolume* pCurrentLogicalVolume = pCurrentVolume->GetLogicalVolume();
  if (!pCurrentLogicalVolume)
    {
      //std::cout <<"ZH Missing current logical volume"<<std::endl;
      return;
    }

  if (!pCurrentLogicalVolume->GetSensitiveDetector())
    {
      //std::cout <<"No SD"<<std::endl;
      //this volume doesn't have a sensitive detector -> not interested
      return;
    }
  std::string CurrentLogicalVolumeName = pCurrentLogicalVolume->GetName();
  std::string OrigLogicalVolumeName = CurrentLogicalVolumeName;
  //  std::cout <<"ZH Step length: "<<aStep->GetStepLength() /CLHEP::mm <<" in "<<OrigLogicalVolumeName<<std::endl;
  //double StepLength = aStep->GetStepLength();
  //std::cout <<"ZH Step: "<<StepLength<<std::endl;
  //std::cout <<"Current : "<<CurrentLogicalVolumeName<<std::endl;

  std::string tilestring("Tile");

  for (int idepth = 0; idepth<depth; idepth++)
    {
      pCurrentVolume = theTouchable->GetVolume(idepth);
      pCurrentLogicalVolume = pCurrentVolume->GetLogicalVolume();
      CurrentLogicalVolumeName = pCurrentLogicalVolume->GetName();
      //std::cout <<CurrentLogicalVolumeName<<" depth: "<<idepth<<std::endl;
      if(CurrentLogicalVolumeName == "LArMgr::LAr::FCAL::Module1::Absorber") 
	{
	  // shower is inside FCAL1
	  m_current_calculator = m_calculator_FCAL1;
	  //m_current_calculator = NULL;
	  break;
	} 
      else if (CurrentLogicalVolumeName == "LArMgr::LAr::FCAL::Module2::Absorber")
	{
	  // shower is inside FCAL2
	  m_current_calculator = m_calculator_FCAL2;
	  //m_current_calculator = NULL; //try disable..
	  break;
	} 
      else if (CurrentLogicalVolumeName == "LArMgr::LAr::FCAL::Module3::Absorber")
        {
          // shower is inside FCAL3                                                                            
          m_current_calculator = m_calculator_FCAL3;                                                                              
          break;
        }
      else if (CurrentLogicalVolumeName == "LArMgr::LAr::EMEC::Pos::InnerWheel")
	{
	  m_current_calculator = m_calculator_EMECIW_pos;
	  break;
	}
      else if (CurrentLogicalVolumeName == "LArMgr::LAr::EMEC::Neg::InnerWheel")
	{
	  // shower is inside inner EMEC
	  m_current_calculator = m_calculator_EMECIW_neg;
	  break;
	} 
      else if (CurrentLogicalVolumeName == "LArMgr::LAr::EMEC::Pos::OuterWheel")
	{
	  m_current_calculator = m_calculator_EMECOW_pos;
	  break;
	}
      else if (CurrentLogicalVolumeName == "LArMgr::LAr::EMEC::Neg::OuterWheel")
	{
	  // shower is inside outer EMEC positive
	  m_current_calculator = m_calculator_EMECOW_neg;
	  break;
	} 
      else if (CurrentLogicalVolumeName == "LArMgr::LAr::EMB::STAC")
	{
	  // shower is inside EMB positive
	  m_current_calculator = m_calculator_EMB;
	  break;
	} 
      //else if (CurrentLogicalVolumeName == "LArMgr::LAr::EMEC::FrontInnerBarrette::Module::Phidiv")
      //	{
      //	  m_current_calculator = m_calculator_BIB;
      //	  break;
      //	}
      else if (CurrentLogicalVolumeName == "LArMgr::LAr::EMEC::BackOuterBarrette::Module::Phidiv")
	{
	  m_current_calculator = m_calculator_BOB;
	  break;
	}
      //else if (CurrentLogicalVolumeName == "LArMgr::LAr::HEC::Module::Depth::Slice::Local") 
      //{
      //  m_current_calculator = m_calculator_HECLocal;
      //  break;
      //} //doesn't exist anymore 
      else if (CurrentLogicalVolumeName == "LArMgr::LAr::HEC::Module::Depth::Slice") 
	{
	  m_current_calculator = m_calculator_HEC;
	  break;
	} 
      else if (CurrentLogicalVolumeName == "LArMgr::LAr::Barrel::Presampler::Module")
	{
	  m_current_calculator = m_calculator_EMBPS;
	}
      else if (CurrentLogicalVolumeName == "LArMgr::LAr::Endcap::Presampler::LiquidArgon")
	{
	  m_current_calculator = m_calculator_EMEPS;
	}
      else if (CurrentLogicalVolumeName.find(tilestring)!= std::string::npos)
	{
	  m_current_calculator_Tile = m_calculator_Tile;
	  break;
	}
      
    }
  //std::cout <<"--- END SEARCHING ---"<<std::endl;

  if (m_current_calculator || m_current_calculator_Tile)
    {
      //Good detector
      if (m_detectormap.find(OrigLogicalVolumeName) == m_detectormap.end())
	{
	  m_detectormap.insert(std::pair<std::string,int>(OrigLogicalVolumeName,m_ndetectors));
	  m_ndetectors++;
	  std::cout <<"ZH I have added new detector: "<<OrigLogicalVolumeName<<" because I know a calculator for: "<<CurrentLogicalVolumeName<<std::endl;
	}
    }
  else
    {
      //Unused
      if ( m_unuseddetector.insert(OrigLogicalVolumeName).second)
	{
	  std::cout <<"ZH I don't have a calculator for: "<<OrigLogicalVolumeName<<" and I'm here for the first time"<<std::endl;
	}
      //std::cout <<"End here"<<std::endl;
      return;
    }
  //std::cout <<"Continue"<<std::endl;
  
  ////////////////////////////////////////////////////////////
  // Calculation and storage of LAr/Tile Hit information
  ////////////////////////////////////////////////////////////

  if (aStep->GetTotalEnergyDeposit()>0) {
      //std::cout <<"ZH Step length: "<<aStep->GetStepLength() /CLHEP::mm <<" in "<<OrigLogicalVolumeName<<std::endl;
      /*
      G4VPhysicalVolume *pNextVolume = aStep->GetPostStepPoint()->GetPhysicalVolume();
      if (!pNextVolume)
	{
	  //std::cout <<"ZH Missing next volume??"<<std::endl;
	  //return;
	}

      G4LogicalVolume* pNextLogicalVolume = pNextVolume->GetLogicalVolume();
      if (!pNextLogicalVolume)
	{
	  //std::cout <<"ZH Missing next logical volume"<<std::endl;
	  //return;
	}

      if (!pNextLogicalVolume->GetSensitiveDetector())
	{
	  //std::cout <<"ZH Missing next SD"<<std::endl;                  
	  //return;
	}
      if (pNextLogicalVolume)
	{
	  //std::cout <<"ZH Missing volume: "<<pNextLogicalVolume->GetName()<<std::endl;
	}
      */
      //std::cout <<"ZH calculator should do something"<<OrigLogicalVolumeName<<" "<<m_current_calculator<<std::endl;
      
      /////////////////////
      //Do LAr Stuff
      /////////////////////
      if (m_current_calculator) {
	
	double et = 0; // Total collected charge
	//G4cout << m_current_calculator << " " << position << G4endl;
		
	if (m_current_calculator->Process(aStep)) 
	  {
	    //this was to test if presampler calculators work
	    /*
	    if (test_presamplere)
	      {
		G4cout <<"ZH presampler calculator works: "<<m_current_calculator->getNumHits()<<" "<<test_presamplere<<G4endl;
		int nlarh = m_current_calculator->getNumHits();
		for (int i=0; i<nlarh; ++i)
		  {
		    std::cout <<"ZH Hit: "<<i<<" E: "<<m_current_calculator->energy(i)<<" in: "<<(std::string) m_current_calculator->identifier(i)<<std::endl;            
		  }
	      }
	    */
	    /*
	    if (m_current_calculator == m_calculator_BIB)
	      {
		std::cout <<"ZH BIB test: "<<std::endl;
		int nlarh = m_current_calculator->getNumHits();
		for (int i=0; i<nlarh; ++i)
		  {
		    std::cout <<"ZH BIB Hit: "<<i<<" E: "<<m_current_calculator->energy(i)<<" in: "<<(std::string) m_current_calculator->identifier(i)<<std::endl;
	          }
	      }  
	    */
	    /*
	    if (m_current_calculator == m_calculator_BOB)
              { 
		std::cout <<"ZH BOB test: "<<std::endl;
                int nlarh = m_current_calculator->getNumHits();
                for (int i=0; i<nlarh; ++i)
                  { 
		    std::cout <<"ZH BOB Hit: "<< pCurrentVolume->GetName()<<i<<" E: "<<m_current_calculator->energy(i)<<" in: "<<(std::string) m_current_calculator->identifier(i)<<std::endl;  
                  }
              }
	    */       
	    /*
	    //Ok, this seems to be giving same E,t, but different identifiers -> keep pos/neg separated
	    if (m_current_calculator == m_calculator_EMECIW_neg)
	      {
		std::cout <<"ZH emeciw pos/neg test: "<<std::endl;
                int nlarh = m_current_calculator->getNumHits();
                for (int i=0; i<nlarh; ++i)
                  {
		    std::cout <<"ZH Hit neg: "<<i<<" E: "<<m_current_calculator->energy(i)<<" t: "<<m_current_calculator->time(i)<<" in: "<<(std::string) m_current_calculator->identifier(i)<<std::endl;
                  }
		m_calculator_EMECIW_pos->Process(aStep);
		int nlarh2 = m_calculator_EMECIW_pos->getNumHits();
		for (int i=0; i<nlarh2; ++i)
                  {
		    std::cout <<"ZH Hit pos: "<<i<<" E: "<<m_calculator_EMECIW_pos->energy(i)<<" t: "<<m_calculator_EMECIW_pos->time(i)<<" in: "<<(std::string) m_calculator_EMECIW_pos->identifier(i)<<std::endl;
                  }
		
	      }
	    */

	    //G4cout <<"ZH process step ok: "<<m_current_calculator->getNumHits()<<G4endl;
	    int nlarh = m_current_calculator->getNumHits();
	    for (int i=0; i<nlarh; ++i) 
	      {
		et += (m_current_calculator->energy(i));
		//std::cout <<"ZH Hit: "<<i<<" E: "<<m_current_calculator->energy(i)<<" in: "<<(std::string) m_current_calculator->identifier(i)<<std::endl;
	      }
	    //if (pCurrentVolume->GetName().find("HEC") != std::string::npos)
	    //  {
	    //    std::cout <<"ZH HEC good: "<<m_current_calculator->getNumHits()<<std::endl;
	    //  }
	    
	  } 
	else 
	  {
	    //Maybe 0 hits or something like that...
	    //G4cout << "ZH: Error: Hit not processed by calculator! But why am I here?" << G4endl;
	    //G4cout << "ZH: calculator not processed: Volume: "<< pCurrentVolume->GetName()<<" "<<m_current_calculator<< G4endl;
	    //if (pCurrentVolume->GetName().find("HEC") != std::string::npos)
	    //  {
	    //    std::cout <<"ZH HEC bad??"<<std::endl;
	    //  }
	    return;
	  }
	
	// drop hits with zero deposited energy (could still happen with negative corrections from calculator)
	if (et <= 0.)
	  {
	    return;
	  }
	
	int nlarh = m_current_calculator->getNumHits();
	for (int i=0; i<nlarh; ++i)
	  {
	    LArG4Identifier ident = m_current_calculator->identifier(i);
	    Identifier id = m_lar_helper->convertID(ident);
	    
	    Identifier inv_id;
	    if (id == inv_id)
	      {
		std::cout <<"Something wrong with identifier (LAr): "<<(std::string) ident;
		std::cout <<" "<<id<<" "<<id.getString()<<" "<<CurrentLogicalVolumeName  <<" nhit: "<<nlarh<<std::endl;
		std::cout <<inv_id<<std::endl;
	      }
	    //std::cout <<"LArIdent "<<(std::string) ident<<std::endl;
	    //std::cout <<"Ident "<<id.getString()<<std::endl;
	    //std::cout <<"ZH Step: LAr: "<<StepLength<<std::endl;
	    ISF_FCS_Parametrization::FCS_StepInfo* theInfo = new ISF_FCS_Parametrization::FCS_StepInfo(pos, id, (double) m_current_calculator->energy(i), (double) m_current_calculator->time(i), true, nlarh); //store nlarh as info 
	    //This one stores also StepLength, but it is not yet in SVN...
	    //	    ISF_FCS_Parametrization::FCS_StepInfo* theInfo = new ISF_FCS_Parametrization::FCS_StepInfo(pos, id, (double) m_current_calculator->energy(i), (double) m_current_calculator->time(i), true, nlarh, StepLength); //store nlarh as info 
	    m_eventSteps->push_back(theInfo);
	  }
	//std::cout <<"----"<<std::endl;
      }
      
      ////////////////////////
      //Do TileCal Stuff
      ////////////////////////
       
      else if (m_current_calculator_Tile) 
	{
	  //std::cout<<"GG: Hello" << std::endl;
          
	  //calculation of MicroHit with aStep
	  TileMicroHit micHit = m_calculator_Tile->TileGeoG4SDCalc::GetTileMicroHit(aStep);  
	  Identifier m_invalid_id;
          
	  //Check if MicroHit is not in scintillator
	  if ((micHit.pmt_up == m_invalid_id) && (micHit.pmt_down == m_invalid_id)) {
	    std::cout <<"Invalid hit in Tile??"<<std::endl;
	    return;}
	  else {
	    // Store TileHits Information
	    //std::cout <<"ZH Step: Tile: "<<StepLength<<std::endl;
	    if ((micHit.pmt_up == m_invalid_id) ||  (micHit.pmt_down == m_invalid_id))
	      {
		//std::cout <<"Something wrong in identifier: One tile pmt: "<<micHit.pmt_up<<" "<<micHit.pmt_down<<std::endl;
		//std::cout <<"E up: "<<micHit.e_up<<" E down: "<<micHit.e_down<<" T up: "<<micHit.time_up<<" T down: "<<micHit.time_down<<std::endl;
	      }
	    ISF_FCS_Parametrization::FCS_StepInfo* theInfo_Tile_up = new ISF_FCS_Parametrization::FCS_StepInfo(pos, micHit.pmt_up, micHit.e_up, micHit.time_up, true,1);	    
	    //Commented out version needs ISF_Event which is not yet in SVN..
	    //	    ISF_FCS_Parametrization::FCS_StepInfo* theInfo_Tile_up = new ISF_FCS_Parametrization::FCS_StepInfo(pos, micHit.pmt_up, micHit.e_up, micHit.time_up, true,1,StepLength);
	    m_eventSteps->push_back(theInfo_Tile_up);
                
	    ISF_FCS_Parametrization::FCS_StepInfo* theInfo_Tile_down = new ISF_FCS_Parametrization::FCS_StepInfo(pos, micHit.pmt_down, micHit.e_down,micHit.time_down , true,1);
	    //ISF_FCS_Parametrization::FCS_StepInfo* theInfo_Tile_down = new ISF_FCS_Parametrization::FCS_StepInfo(pos, micHit.pmt_down, micHit.e_down,micHit.time_down , true,1,StepLength);
	    m_eventSteps->push_back(theInfo_Tile_down);
                
	    //std::cout << "GG: GetTileMicroHit: pmtID_up,pmtID_down,edep_up,edep_down,scin_Time_up,scin_Time_down:\t" << micHit.pmt_up <<";\t"<< micHit.pmt_down <<";\t"<< micHit.e_up <<";\t"<< micHit.e_down <<";\t" << micHit.time_up <<";\t"<< micHit.time_down << std::endl;
	    
	  }
	  

	}
      else
	{
	  //Shouldn't be here anyway..
	  G4cout <<"ZH ???"<<aStep->GetTotalEnergyDeposit() << G4endl;
	}
      //std::cout <<"End of step"<<std::endl;
      return;
  }
  return;
}
