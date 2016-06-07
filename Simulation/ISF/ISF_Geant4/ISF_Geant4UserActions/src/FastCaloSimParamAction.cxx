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
#include <cmath>

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
#include "LArG4HEC/LArHECWheelCalculator.h"
#include "LArG4EC/LArEndcapPresamplerCalculator.h"
#include "LArG4Barrel/LArBarrelPresamplerCalculator.h"

#include "LArG4HitManagement/LArHitMerger.h"

// TileCAL calculators
#include "TileGeoG4SD/TileGeoG4SDCalc.hh"

#include "ISF_FastCaloSimEvent/FCS_StepInfo.h"
#include "ISF_FastCaloSimEvent/FCS_StepInfoCollection.h"

//Geometry
#include "CaloDetDescr/CaloDetDescrElement.h"
#include "CaloDetDescr/CaloDetDescrManager.h"
#include "CaloIdentifier/CaloIdManager.h"

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
    m_calculator_BOB(0),
    m_calculator_EMB(0),
    m_calculator_FCAL1(0),
    m_calculator_FCAL2(0),
    m_calculator_FCAL3(0),
    m_calculator_HEC(0),
    m_calculator_EMBPS(0),
    m_calculator_EMEPS(0),
    m_calculator_Tile(0),
    m_lar_helper(0),
    m_lar_emID(0),
    m_calo_dd_man(0),
    m_storeGateSvc("StoreGateSvc", s), 
    m_detStore(0),
    m_eventSteps(0),
    m_maxRadius(25.),
    m_maxRadiusLAr(25.),
    m_maxRadiusHEC(100.),
    m_maxRadiusFCAL(100.),
    m_maxRadiusTile(100.),
    m_maxTime(25.),
    m_maxTimeLAr(25.),
    m_maxTimeHEC(25.),
    m_maxTimeFCAL(25.),
    m_maxTimeTile(25.),
    m_ndetectors(0),
    m_shift_lar_subhit(true),
    m_shorten_lar_step(true),
    m_isinit(false)
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
              throw std::runtime_error("Null pointer to the Detector Store!");
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
  if (!m_isinit) ParseProperties();
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
    m_calculator_EMECOW_neg = new LArG4::EC::EnergyCalculator(LArWheelCalculator::OuterAbsorberWheel,LArG4::EC::EnergyCalculator::EMEC_ECOR_ROPT, -1);
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
  //         << "## FastCaloSimParamAction - BeginOfEvent2 ##" << G4endl
  //         << "############################################" << G4endl;
 
  return;
}

void FastCaloSimParamAction::EndOfEventAction(const G4Event* )
{

  //G4cout << "############################################" << G4endl
  //       << "##    FastCaloSimParamAction - EndOfEvent ##" << G4endl
  //       << "##                                        ##" << G4endl
  //       << "############################################" << G4endl;

#ifdef _myDEBUG_
  G4cout << "FastCaloSimParamAction::EndOfEventAction: Before initial cleanup, N=" << m_eventSteps->size() << G4endl;
#endif

  // Unpack map into vector
  for (auto it : m_hit_map){
    for (auto a_s : * it.second){
      // Giving away ownership of the objects!
      m_eventSteps->push_back( a_s );
    }
    it.second->clear();
    delete it.second;
  } // Vector of IDs in the map
  m_hit_map.clear();

  //  G4cout << "FastCaloSimParamAction::EndOfEventAction: After initial cleanup, N=" << m_eventSteps->size() << G4endl;
  if (m_eventSteps->size()==0) return; //don't need to play with it
  G4cout << "FastCaloSimParamAction::EndOfEventAction: After initial cleanup, N=" << m_eventSteps->size() << G4endl;

  //
  // Put eventSteps into event store
  //
  std::string location("ZHMergedEventSteps");
  ISF_FCS_Parametrization::FCS_StepInfoCollection* test;
  // std::cout <<"Check if already in StoreGate:"<<std::endl;
  if (storeGateSvc()->contains<ISF_FCS_Parametrization::FCS_StepInfoCollection>(location))
    {
      StatusCode check = storeGateSvc()->retrieve(test,location);
      if (check.isSuccess())
        {
          //want to merge and overwrite!
          for (ISF_FCS_Parametrization::FCS_StepInfoCollection::iterator iter = m_eventSteps->begin();iter != m_eventSteps->end();iter++)
            {
              test->push_back((*iter));
            }
          check = storeGateSvc()->retrieve(test,location);
          if (check.isSuccess())
            { 
              std::cout <<"ZH, check in StoreGate : "<<test->size()<<std::endl;
            }
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
  if (!m_isinit) ParseProperties();
  // ?? Ok, where do I need this??
  // init calculator
  if (m_calculator_EMECIW_pos == 0)
    m_calculator_EMECIW_pos = new LArG4::EC::EnergyCalculator(LArWheelCalculator::InnerAbsorberWheel,LArG4::EC::EnergyCalculator::EMEC_ECOR_ROPT, 1);
  if (m_calculator_EMECOW_pos == 0)
    m_calculator_EMECOW_pos = new LArG4::EC::EnergyCalculator(LArWheelCalculator::OuterAbsorberWheel,LArG4::EC::EnergyCalculator::EMEC_ECOR_ROPT, 1);
  if (m_calculator_EMECIW_neg == 0)
    m_calculator_EMECIW_neg = new LArG4::EC::EnergyCalculator(LArWheelCalculator::InnerAbsorberWheel,LArG4::EC::EnergyCalculator::EMEC_ECOR_ROPT, -1);
  if (m_calculator_EMECOW_neg == 0)
    m_calculator_EMECOW_neg = new LArG4::EC::EnergyCalculator(LArWheelCalculator::OuterAbsorberWheel,LArG4::EC::EnergyCalculator::EMEC_ECOR_ROPT, -1);
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

  //get also lar em helper
  m_calo_dd_man  = CaloDetDescrManager::instance();

  const DataHandle<CaloIdManager> caloIdManager;
  StatusCode sc=detStoreSvc()->retrieve(caloIdManager);
  if(sc.isSuccess())
    std::cout<<"CaloIDManager retrieved."<<std::endl;
   else
     throw std::runtime_error("ISF_HitAnalysis: Unable to retrieve CaloIDManeger");
  
  m_lar_emID=caloIdManager->getEM_ID();
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

  if (m_calculator_Tile !=0) {
    delete m_calculator_Tile;
    m_calculator_Tile = 0;

  }

  G4cout << "############################################" << G4endl
         << "##    FastCaloSimParamAction - EndOfRun   ##" << G4endl
         << "############################################" << G4endl;

  return;
}

void FastCaloSimParamAction::SteppingAction(const G4Step* aStep)
{
  //G4cout <<"FastCaloSimParamAction - SteppingAction"<<G4endl;
  // No energy deposit, then return right away
  if (aStep->GetTotalEnergyDeposit()<=0) return;

  m_current_calculator = NULL;
  m_current_calculator_Tile = NULL;

  G4ThreeVector position1 = aStep->GetPreStepPoint()->GetPosition(); //pre step is the position i'm interested in
  G4ThreeVector position2 = aStep->GetPostStepPoint()->GetPosition();
  G4ThreeVector pos = 0.5*(position1+position2);

  G4TouchableHistory* theTouchable = (G4TouchableHistory*)(aStep->GetPreStepPoint()->GetTouchable()); //this has all the history depth
  if (!theTouchable){
    //This shouldn't happen anyway
    return;
  }

  int depth = theTouchable->GetHistoryDepth();
  G4VPhysicalVolume *pCurrentVolume = aStep->GetPreStepPoint()->GetPhysicalVolume();
  if (!pCurrentVolume){
    // Missing current volume?
    return;
  }

  G4LogicalVolume* pCurrentLogicalVolume = pCurrentVolume->GetLogicalVolume();
  if (!pCurrentLogicalVolume){
    // Missing current logical volume
    return;
  }

  if (!pCurrentLogicalVolume->GetSensitiveDetector()){
    //this volume doesn't have a sensitive detector -> not interested
    return;
  }
  std::string CurrentLogicalVolumeName = pCurrentLogicalVolume->GetName();
  std::string OrigLogicalVolumeName = CurrentLogicalVolumeName;
  double StepLength = aStep->GetStepLength()/ CLHEP::mm;;

  std::string tilestring("Tile");

  for (int idepth = 0; idepth<depth; idepth++){
      pCurrentVolume = theTouchable->GetVolume(idepth);
      pCurrentLogicalVolume = pCurrentVolume->GetLogicalVolume();
      CurrentLogicalVolumeName = pCurrentLogicalVolume->GetName();
      if(CurrentLogicalVolumeName == "LArMgr::LAr::FCAL::Module1::Absorber"){
          // shower is inside FCAL1
          m_current_calculator = m_calculator_FCAL1;
          break;
      } else if (CurrentLogicalVolumeName == "LArMgr::LAr::FCAL::Module2::Absorber"){
          // shower is inside FCAL2
          m_current_calculator = m_calculator_FCAL2;
          break;
      } else if (CurrentLogicalVolumeName == "LArMgr::LAr::FCAL::Module3::Absorber"){
          // shower is inside FCAL3                                                                            
          m_current_calculator = m_calculator_FCAL3;                                                                              
          break;
      } else if (CurrentLogicalVolumeName == "LArMgr::LAr::EMEC::Pos::InnerWheel"){
          m_current_calculator = m_calculator_EMECIW_pos;
          break;
      } else if (CurrentLogicalVolumeName == "LArMgr::LAr::EMEC::Neg::InnerWheel"){
          // shower is inside inner EMEC
          m_current_calculator = m_calculator_EMECIW_neg;
          break;
      } else if (CurrentLogicalVolumeName == "LArMgr::LAr::EMEC::Pos::OuterWheel"){
          m_current_calculator = m_calculator_EMECOW_pos;
          break;
      } else if (CurrentLogicalVolumeName == "LArMgr::LAr::EMEC::Neg::OuterWheel"){
          // shower is inside outer EMEC positive
          m_current_calculator = m_calculator_EMECOW_neg;
          break;
      } else if (CurrentLogicalVolumeName == "LArMgr::LAr::EMB::STAC"){
          // shower is inside EMB positive
          m_current_calculator = m_calculator_EMB;
          break;
      } else if (CurrentLogicalVolumeName == "LArMgr::LAr::EMEC::BackOuterBarrette::Module::Phidiv"){
          m_current_calculator = m_calculator_BOB;
          break;
      } else if (CurrentLogicalVolumeName == "LArMgr::LAr::HEC::Module::Depth::Slice"){
          m_current_calculator = m_calculator_HEC;
          break;
      } else if (CurrentLogicalVolumeName == "LArMgr::LAr::Barrel::Presampler::Module"){
          m_current_calculator = m_calculator_EMBPS;
      } else if (CurrentLogicalVolumeName == "LArMgr::LAr::Endcap::Presampler::LiquidArgon"){
          m_current_calculator = m_calculator_EMEPS;
      } else if (CurrentLogicalVolumeName.find(tilestring)!= std::string::npos){
          m_current_calculator_Tile = m_calculator_Tile;
          break;
      }
  } // Loop over depth to find an interesting volume

  if (!m_current_calculator && !m_current_calculator_Tile){
      //Unused
      return;
  }

  ////////////////////////////////////////////////////////////
  // Calculation and storage of LAr/Tile Hit information
  ////////////////////////////////////////////////////////////

  /////////////////////
  //Do LAr Stuff
  /////////////////////
  if (m_current_calculator) {
    
    double et = 0; // Total collected charge

    std::vector<const G4Step*> steps;
    bool shortstep = false;
    if (m_shorten_lar_step && StepLength>0.2)
      {                
        //create smaller substeps instead
        G4int nsub_step=(int) (StepLength/0.2) + 1;
        G4double delta=1./((double) nsub_step);
        //std::cout <<"Orig prestep: "<<aStep->GetPreStepPoint()->GetPosition()<<std::endl;
        for (G4int i=0;i<nsub_step;i++) {
          // G4double fraction=(((G4double) i)+0.5)*delta;
          G4double fraction1 = ((G4double) i)*delta;
          G4double fraction2 = (((G4double) i) + 1.)*delta;
          G4ThreeVector subpoint1=position1*(1-fraction1) + position2*fraction1;
          G4ThreeVector subpoint2=position1*(1-fraction2) + position2*fraction2;
          
          G4StepPoint *startpoint = new G4StepPoint(*(aStep->GetPreStepPoint()));
          G4StepPoint *endpoint = new G4StepPoint(*(aStep->GetPostStepPoint()));
          startpoint->SetPosition(subpoint1);
          endpoint->SetPosition(subpoint2);

          G4Step* newstep = new G4Step(*aStep);
          if(newstep->GetPreStepPoint()) delete newstep->GetPreStepPoint();
          if(newstep->GetPostStepPoint()) delete newstep->GetPostStepPoint(); 
          newstep->SetPreStepPoint(startpoint);
          newstep->SetPostStepPoint(endpoint);
          newstep->SetStepLength( (subpoint1-subpoint2).mag());
          newstep->SetTotalEnergyDeposit(aStep->GetTotalEnergyDeposit()/nsub_step); 
          steps.push_back(newstep);
        }
        shortstep = true;
      }
    else
      {
        steps.push_back(aStep);
      }

    //std::cout <<"ZH Step size: "<<steps.size()<<" full step length "<<StepLength<<std::endl;
    for (unsigned int istep = 0; istep <steps.size(); istep++)
      {
        //need to update the position!
        pos = 0.5*(steps[istep]->GetPreStepPoint()->GetPosition()+steps[istep]->GetPostStepPoint()->GetPosition());
        //std::cout <<"ZH Processing step: "<<istep<<" at position "<<pos<<std::endl;
        bool istep_ok = false;
        if (m_current_calculator->Process(steps[istep])) 
          {
            //if (!shortstep) G4cout <<"ZH process step ok: "<<m_current_calculator->getNumHits()<<G4endl;
            int nlarh = m_current_calculator->getNumHits();
            for (int i=0; i<nlarh; ++i) 
              {
                et += (m_current_calculator->energy(i));
                //std::cout <<"ZH Hit: "<<i<<" E: "<<m_current_calculator->energy(i)<<" in: "<<(std::string) m_current_calculator->identifier(i)<<std::endl;
              }
            istep_ok = true;
          } 
        
        // drop hits with zero deposited energy (could still happen with negative corrections from calculator)
        //Or if total energy is <0
        if (istep_ok)
          {
            int nlarh = m_current_calculator->getNumHits();
            std::vector<G4ThreeVector> subhitcells;
            subhitcells.resize(nlarh);
            G4ThreeVector origpos = pos;
            double e_subhitmax = -999.;
            int e_subhitmaxindex =-1;
            if (nlarh>0)
              {
                e_subhitmax = m_current_calculator->energy(0);
                e_subhitmaxindex = 0;
              }
            //Figure out the subhit with most energy
            for (int i=1; i<nlarh; ++i)
              {
                if (e_subhitmax< m_current_calculator->energy(i))
                  {
                    e_subhitmax = m_current_calculator->energy(i);
                    e_subhitmaxindex = i;
                  }
              }
            //Identifier for the subhit with max energy
            LArG4Identifier e_subhitmax_ident = m_current_calculator->identifier(e_subhitmaxindex);
            Identifier e_subhitmax_id = m_lar_helper->convertID(e_subhitmax_ident);
            
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
                
                //need to get the cell information
                if (nlarh>1)
                  {
                    //it didn't seem to happen outside em_barrel
                    if (m_lar_emID->is_em_barrel(id))
                      {
                        if (m_shift_lar_subhit)
                          {
                            //find subhit with largest energy
                            
                            if (e_subhitmaxindex == -1)
                              {
                                std::cout <<"ZH no subhit index with e>-999??? "<<std::endl;
                                for (unsigned int istep = 0; istep<steps.size(); istep++)
                                  {
                                    delete steps[istep];
                                  }
                                steps.clear();
                                return;
                              }
                            //from sampling, hit_eta, hit_phi (simple geometry)
                            //CaloDetDescrElement *bestcell = m_calo_dd_man->get_element(m_calo_dd_man->get_element(id)->getSampling(),origpos.eta(), origpos.phi());
                            CaloDetDescrElement *highestcell = m_calo_dd_man->get_element(e_subhitmax_id);
                            //from identifier
                            CaloDetDescrElement *thiscell = m_calo_dd_man->get_element(id);
                            //delete them afterwards?
                            if (!highestcell)
                              {
                                //How often does this happen, do not shift
                                std::cout <<"ZH highestEcell failed: "<<e_subhitmax_id.getString()<<std::endl;
                                //" "<<m_calo_dd_man->get_element(id)->getSampling()<<" "<<origpos.eta()<<" "<< origpos.phi()<<std::endl;
                                //do no shift?
                                pos = origpos;
                              }
                            else if (highestcell == thiscell)
                              {
                                pos = origpos;
                              }
                            else
                              {
                                //the two cells do not match => shift
                                G4ThreeVector diff(thiscell->x()-highestcell->x(), thiscell->y()-highestcell->y(), thiscell->z()-highestcell->z());
                                pos = origpos+diff;
                              }
                          }
                      }
                    else
                      {
                        //Does this happen?
                        std::cout <<"More subhits, but not in LAr barrel "<<(std::string) ident<<std::endl;
                      }
                  }
                
                //Finalize time for LAr hits?: NO
                //double time = m_current_calculator->energy(i)==0 ? 0. : (double) m_current_calculator->time(i)/m_current_calculator->energy(i)/CLHEP::ns;
                double time = m_current_calculator->time(i);
                double energy = m_current_calculator->energy(i)/CLHEP::MeV;
                
                update_map(pos, id, energy, time, true, nlarh); //store nlarh as info 
                //This one stores also StepLength, but it is not yet in SVN...
                //std::cout <<"Adding new step info: "<<i<<" at: "<<pos<<" Id: "<<id<<" E: "<<energy<<" time: "<<time<<std::endl;
              }//nlarh
          }//istep ok
        //std::cout <<"----"<<std::endl;
      } //istep
    
    //Delete steps?
    if (shortstep && steps.size()>1)
      {
        //only when doing substeps, don't want to delete the original a4step
        //while(!steps.empty()) delete steps.back(), steps.pop_back();
        for (unsigned int istep = 0; istep<steps.size(); istep++)
          { 
            delete steps[istep];
          }
        steps.clear();
      }
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
        return;
      }
      else {
        // Store TileHits Information
        //std::cout <<"ZH Step: Tile: "<<StepLength<<std::endl;
        //if ((micHit.pmt_up == m_invalid_id) ||  (micHit.pmt_down == m_invalid_id))
        //  {
        //    //std::cout <<"Something wrong in identifier: One tile pmt: "<<micHit.pmt_up<<" "<<micHit.pmt_down<<std::endl;
        //    //std::cout <<"E up: "<<micHit.e_up<<" E down: "<<micHit.e_down<<" T up: "<<micHit.time_up<<" T down: "<<micHit.time_down<<std::endl;
        //  }
        update_map(pos, micHit.pmt_up, micHit.e_up, micHit.time_up, true,1);            
        //Commented out version needs ISF_Event which is not yet in SVN..
        //            ISF_FCS_Parametrization::FCS_StepInfo* theInfo_Tile_up = new ISF_FCS_Parametrization::FCS_StepInfo(pos, micHit.pmt_up, micHit.e_up, micHit.time_up, true,1,StepLength);
        update_map(pos, micHit.pmt_down, micHit.e_down,micHit.time_down , true,1);
        //ISF_FCS_Parametrization::FCS_StepInfo* theInfo_Tile_down = new ISF_FCS_Parametrization::FCS_StepInfo(pos, micHit.pmt_down, micHit.e_down,micHit.time_down , true,1,StepLength);
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

void FastCaloSimParamAction::ParseProperties()
{
  G4cout <<"FastCaloSimParamAction ParseProperties() "<<std::endl;
  if(theProperties.find("shift_lar_subhit")==theProperties.end()){
    //property not found set it to true                                                                          
    G4cout <<"ZH shift_lar_subhit Property not found!"<<G4endl;
    SetProperty("shift_lar_subhit","1");
  }
  if (theProperties.find("shorten_lar_step")==theProperties.end()){
    //property not found set it to true
    G4cout <<"ZH LAr step length property not found"<<G4endl;
    SetProperty("shorten_lar_step","1");
  }

  if (theProperties["shift_lar_subhit"]=="0"){
      m_shift_lar_subhit = false;
      G4cout <<"FastCaloSimParamAction: not shifting LAr subhits"<<G4endl;
  } else {
      m_shift_lar_subhit = true;
      G4cout <<"FastCaloSimParamAction: shifting LAr subhits"<<G4endl;
  }

  if (theProperties["shorten_lar_step"]=="0"){
      m_shorten_lar_step = false;
      G4cout <<"FastCaloSimParamAction: using default LAr step length"<<G4endl;
  } else {
      m_shorten_lar_step = true;
      G4cout <<"FastCaloSimParamAction: making LAr steps shorter than 0.2mm!"<<G4endl;
  }

  if (theProperties.find("MaxRadius")!=theProperties.end())
    m_maxRadius=strtod(theProperties["MaxRadius"].c_str(),0);
  if (theProperties.find("MaxRadiusLAr")!=theProperties.end())
    m_maxRadiusLAr=strtod(theProperties["MaxRadiusLAr"].c_str(),0);
  if (theProperties.find("MaxRadiusHEC")!=theProperties.end())
    m_maxRadiusHEC=strtod(theProperties["MaxRadiusHEC"].c_str(),0);
  if (theProperties.find("MaxRadiusFCAL")!=theProperties.end())
    m_maxRadiusFCAL=strtod(theProperties["MaxRadiusFCAL"].c_str(),0);
  if (theProperties.find("MaxRadiusTile")!=theProperties.end())
    m_maxRadiusTile=strtod(theProperties["MaxRadiusTile"].c_str(),0);

  if (theProperties.find("MaxTime")!=theProperties.end())
    m_maxTime=strtod(theProperties["MaxTime"].c_str(),0);
  if (theProperties.find("MaxTimeLAr")!=theProperties.end())
    m_maxTimeLAr=strtod(theProperties["MaxTimeLAr"].c_str(),0);
  if (theProperties.find("MaxTimeHEC")!=theProperties.end())
    m_maxTimeHEC=strtod(theProperties["MaxTimeHEC"].c_str(),0);
  if (theProperties.find("MaxTimeFCAL")!=theProperties.end())
    m_maxTimeFCAL=strtod(theProperties["MaxTimeFCAL"].c_str(),0);
  if (theProperties.find("MaxTimeTile")!=theProperties.end())
    m_maxTimeTile=strtod(theProperties["MaxTimeTile"].c_str(),0);

  m_isinit = true;
}

// We are keeping a map instead of trying to keep the full vector
// At the end of run we'll push the map back into the flat vector for storage in StoreGate
void FastCaloSimParamAction::update_map(const CLHEP::Hep3Vector & l_vec, const Identifier & l_cell, double l_energy, double l_time, bool l_valid, int l_detector)
{
  // Drop any hits that don't have a good identifier attached
  if (!m_calo_dd_man->get_element(l_cell)) return;

  auto map_item = m_hit_map.find( l_cell );
  if (map_item==m_hit_map.end()){
    m_hit_map[l_cell] = new std::vector< ISF_FCS_Parametrization::FCS_StepInfo* >;
    m_hit_map[l_cell]->push_back( new ISF_FCS_Parametrization::FCS_StepInfo( l_vec , l_cell , l_energy , l_time , l_valid , l_detector ) );
  } else {

    // Get the appropriate merging limits
    double tsame;
    const CaloCell_ID::CaloSample& layer = m_calo_dd_man->get_element(l_cell)->getSampling();
    if (layer >= CaloCell_ID::PreSamplerB && layer <= CaloCell_ID::EME3){
      tsame = m_maxTimeLAr;
    } else if (layer >= CaloCell_ID::HEC0  && layer <= CaloCell_ID::HEC3){
      tsame = m_maxTimeHEC;
    } else if (layer >= CaloCell_ID::TileBar0 && layer <= CaloCell_ID::TileExt2){
      tsame = m_maxTimeTile;
    } else if (layer >=CaloCell_ID::FCAL0 && layer <= CaloCell_ID::FCAL2){
      tsame = m_maxTimeFCAL;
    } else {
      tsame = m_maxTime;
    }

    bool match = false;
    for (auto map_it : * map_item->second){
      // Time check is straightforward
      if ( fabs(map_it->time()-l_time)>=tsame ) continue;
      // Distance check is less straightforward
      CLHEP::Hep3Vector a_diff = l_vec - map_it->position();
      double a_inv_length = 1./a_diff.mag();
      if (layer==CaloCell_ID::PreSamplerB || layer==CaloCell_ID::PreSamplerE){
        // 5mm in eta, 5mm in phi, no cut in r
        if (fabs(sin(l_vec.phi()-map_it->position().phi())*a_diff.mag())>=5) continue; // phi
        if (fabs(sin(l_vec.theta()-map_it->position().theta())*a_diff.mag())>=5) continue; // eta
      } else if (layer==CaloCell_ID::EMB1 || layer==CaloCell_ID::EME1){
        // 1mm in eta, 5mm in phi, 15mm in r
        if ( a_diff.dot( l_vec ) * a_inv_length >= 15 ) continue; // r
        if (fabs(sin(l_vec.phi()-map_it->position().phi())*a_diff.mag())>=5) continue; // phi
        if (fabs(sin(l_vec.theta()-map_it->position().theta())*a_diff.mag())>=1) continue; // eta
      } else if (layer==CaloCell_ID::EMB2 || layer==CaloCell_ID::EME2){
        // 5mm in eta, 5mm in phi, 60mm in r
        if ( a_diff.dot( l_vec ) * a_inv_length >= 60 ) continue; // r
        if (fabs(sin(l_vec.phi()-map_it->position().phi())*a_diff.mag())>=5) continue; // phi
        if (fabs(sin(l_vec.theta()-map_it->position().theta())*a_diff.mag())>=5) continue; // eta
      } else if (layer==CaloCell_ID::EMB3 || layer==CaloCell_ID::EMB3){
        // 5mm in eta, 5mm in phi, 8mm in r
        if ( a_diff.dot( l_vec ) * a_inv_length >= 8 ) continue; // r
        if (fabs(sin(l_vec.phi()-map_it->position().phi())*a_diff.mag())>=5) continue; // phi
        if (fabs(sin(l_vec.theta()-map_it->position().theta())*a_diff.mag())>=5) continue; // eta
      } else if (layer >= CaloCell_ID::PreSamplerB && layer <= CaloCell_ID::EME3){
        if ( map_it->position().diff2( l_vec ) >= m_maxRadiusLAr ) continue;
      } else if (layer >= CaloCell_ID::HEC0  && layer <= CaloCell_ID::HEC3){
        if ( map_it->position().diff2( l_vec ) >= m_maxRadiusHEC ) continue;
      } else if (layer >= CaloCell_ID::TileBar0 && layer <= CaloCell_ID::TileExt2){
        if ( map_it->position().diff2( l_vec ) >= m_maxRadiusTile ) continue;
      } else if (layer >=CaloCell_ID::FCAL0 && layer <= CaloCell_ID::FCAL2){
        if ( map_it->position().diff2( l_vec ) >= m_maxRadiusFCAL ) continue;
      } else {
        if ( map_it->position().diff2( l_vec ) >= m_maxRadius ) continue;
      }

      // Found a match.  Make a temporary that will be deleted!
      ISF_FCS_Parametrization::FCS_StepInfo my_info( l_vec , l_cell , l_energy , l_time , l_valid , l_detector );
      *map_it += my_info;
      match = true;
      break;
    } // End of search for match in time and space
    if (!match){
      map_item->second->push_back( new ISF_FCS_Parametrization::FCS_StepInfo( l_vec , l_cell , l_energy , l_time , l_valid , l_detector ) );
    } // Didn't match
  } // ID already in the map

} // That's it for updating the map!

