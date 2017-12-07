/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "ISF_Geant4UserActions/FastCaloSimParamAction.h"


#include <stdexcept>
#include <map>
#include <memory>
#include <string>
#include <stdlib.h>
#include <string.h>

#include "G4Step.hh"
#include "G4Event.hh"
#include "G4EventManager.hh"
#include "G4ThreeVector.hh"

// LAr calculator interface
#include "LArG4Code/ILArCalculatorSvc.h"

// For the identifiers
#include "LArG4Code/LArG4SimpleSD.h"

// TileCAL calculators
#include "TileG4Interfaces/ITileCalculator.h"

#include "ISF_FastCaloSimEvent/FCS_StepInfo.h"
#include "ISF_FastCaloSimEvent/FCS_StepInfoCollection.h"

//Geometry
#include "CaloDetDescr/CaloDetDescrElement.h"
#include "CaloDetDescr/CaloDetDescrManager.h"
#include "CaloIdentifier/CaloIdManager.h"

// For MC Truth information:
#include "GeneratorObjects/McEventCollection.h"


namespace G4UA{


  FastCaloSimParamAction::FastCaloSimParamAction(const Config& config)
    : m_config(config)
    , m_detStore("StoreGateSvc/DetectorStore","FastCaloSimParamAction")
    , m_current_calculator(nullptr)
    , m_current_calculator_Tile(nullptr)
    , m_current_solid(nullptr)
    , m_current_transform(nullptr)
    , m_lar_helper(nullptr)
    , m_lar_emID(nullptr)
    , m_calo_dd_man(nullptr)
    , m_eventSteps(m_config.stepInfoCollName)
    , m_ndetectors(0)
  {

#ifdef _myDEBUG_
    G4cout << "############################################" << G4endl
           << "##  FastCaloSimParamAction - Constructor  ##" << G4endl
           << "############################################" << G4endl;
#endif

  }

  void FastCaloSimParamAction::BeginOfRunAction(const G4Run*){

    //G4cout << "############################################" << G4endl
    // << "##    FastCaloSimParamAction - BeginOfRun ##" << G4endl
    // << "############################################" << G4endl;

    if (m_current_transform == nullptr)
      {
        m_current_transform = new G4AffineTransform ();
      }
    m_lar_helper = new LArG4SimpleSD( "IDHelper" , &(*m_detStore) );

    //get also lar em helper
    m_calo_dd_man  = CaloDetDescrManager::instance();

    const DataHandle<CaloIdManager> caloIdManager;
    if(m_detStore->retrieve(caloIdManager).isSuccess())
      std::cout<<"CaloIDManager retrieved."<<std::endl;
    else
      throw std::runtime_error("ISF_HitAnalysis: Unable to retrieve CaloIDManeger");

    m_lar_emID=caloIdManager->getEM_ID();
    return;
  }

  void FastCaloSimParamAction::EndOfRunAction(const G4Run*){


    G4cout << "############################################" << G4endl
           << "##    FastCaloSimParamAction - EndOfRun   ##" << G4endl
           << "##          deleting calculators          ##" << G4endl
           << "############################################" << G4endl;

    G4cout << "ZH good detector map: " << G4endl;
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

  void FastCaloSimParamAction::BeginOfEventAction(const G4Event*)
  {

    //G4cout << "############################################" << G4endl
    //     << "##  FastCaloSimParamAction - BeginOfEvent ##" << G4endl
    //     << "############################################" << G4endl;

    if (m_current_transform == nullptr) {
      m_current_transform = new G4AffineTransform ();
    }
    if (!m_eventSteps.isValid()) m_eventSteps = std::make_unique<ISF_FCS_Parametrization::FCS_StepInfoCollection>();
    //G4cout << "############################################" << G4endl
    //     << "## FastCaloSimParamAction - BeginOfEvent2 ##" << G4endl
    //     << "############################################" << G4endl;


    return;
  }

  void FastCaloSimParamAction::EndOfEventAction(const G4Event*){

    //G4cout << "############################################" << G4endl
    //     << "##    FastCaloSimParamAction - EndOfEvent ##" << G4endl
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

    //  G4cout << "FastCaloSimParamAction::EndOfEventAction: After initial cleanup, N=" << m_eventSteps->size() << G4endl;

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

    if (m_eventSteps->size()==0) return; //don't need to play with it
    G4cout << "FastCaloSimParamAction::EndOfEventAction: After initial cleanup, N=" << m_eventSteps->size() << G4endl;

    return;
  }

  void FastCaloSimParamAction::UserSteppingAction(const G4Step* aStep)
{

    //G4cout <<"FastCaloSimParamAction - SteppingAction"<<G4endl;
    m_current_calculator = nullptr;
    m_current_calculator_Tile = nullptr;

    G4ThreeVector position1 = aStep->GetPreStepPoint()->GetPosition(); //pre step is the position i'm interested in
    G4ThreeVector position2 = aStep->GetPostStepPoint()->GetPosition();
    G4ThreeVector pos = 0.5*(position1+position2);
    //  std::cout <<" === NEW STEP ==="<<std::endl;

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
    double StepLength = aStep->GetStepLength()/ CLHEP::mm;;
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
            m_current_calculator = &*m_config.calculator_FCAL1;
            //m_current_calculator = nullptr;
            break;
          }
        else if (CurrentLogicalVolumeName == "LArMgr::LAr::FCAL::Module2::Absorber")
          {
            // shower is inside FCAL2
            m_current_calculator = &*m_config.calculator_FCAL2;
            //m_current_calculator = nullptr; //try disable..
            break;
          }
        else if (CurrentLogicalVolumeName == "LArMgr::LAr::FCAL::Module3::Absorber")
          {
            // shower is inside FCAL3
            m_current_calculator = &*m_config.calculator_FCAL3;
            break;
          }
        else if (CurrentLogicalVolumeName == "LArMgr::LAr::EMEC::Pos::InnerWheel")
          {
            m_current_calculator = &*m_config.calculator_EMECIW_pos;
            break;
          }
        else if (CurrentLogicalVolumeName == "LArMgr::LAr::EMEC::Neg::InnerWheel")
          {
            // shower is inside inner EMEC
            m_current_calculator = &*m_config.calculator_EMECIW_neg;
            break;
          }
        else if (CurrentLogicalVolumeName == "LArMgr::LAr::EMEC::Pos::OuterWheel")
          {
            m_current_calculator = &*m_config.calculator_EMECOW_pos;
            break;
          }
        else if (CurrentLogicalVolumeName == "LArMgr::LAr::EMEC::Neg::OuterWheel")
          {
            // shower is inside outer EMEC positive
            m_current_calculator = &*m_config.calculator_EMECOW_neg;
            break;
          }
        else if (CurrentLogicalVolumeName == "LArMgr::LAr::EMB::STAC")
          {
            // shower is inside EMB positive
            m_current_calculator = &*m_config.calculator_EMB;
            break;
          }
        //else if (CurrentLogicalVolumeName == "LArMgr::LAr::EMEC::FrontInnerBarrette::Module::Phidiv")
        //	{
        //          m_current_calculator = &*m_config.calculator_BIB;
        //          break;
        //	}
        else if (CurrentLogicalVolumeName == "LArMgr::LAr::EMEC::BackOuterBarrette::Module::Phidiv")
          {
            m_current_calculator = &*m_config.calculator_BOB;
            break;
          }
        //else if (CurrentLogicalVolumeName == "LArMgr::LAr::HEC::Module::Depth::Slice::Local")
        //{
        //  m_current_calculator = &*m_config.calculator_HECLocal;
        //  break;
        //} //doesn't exist anymore
        else if (CurrentLogicalVolumeName == "LArMgr::LAr::HEC::Module::Depth::Slice")
          {
            m_current_calculator = &*m_config.calculator_HEC;
            break;
          }
        else if (CurrentLogicalVolumeName == "LArMgr::LAr::Barrel::Presampler::Module")
          {
            m_current_calculator = &*m_config.calculator_EMBPS;
            break;
          }
        else if (CurrentLogicalVolumeName == "LArMgr::LAr::Endcap::Presampler::LiquidArgon")
          {
            m_current_calculator = &*m_config.calculator_EMEPS;
            break;
          }
        else if (CurrentLogicalVolumeName.find(tilestring)!= std::string::npos)
          {
            m_current_calculator_Tile = &*m_config.calculator_TILE;
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
      /*
        std::cout <<" === NEW STEP ==="<<std::endl;
        std::cout <<"Prestep pos: "<<position1.x()<<" "<<position1.y()<<" "<<position1.z()<<std::endl;
        std::cout <<"Poststep pos: "<<position2.x()<<" "<<position2.y()<<" "<<position2.z()<<std::endl;
        std::cout <<"Step pos: "<<pos.x()<<" "<<pos.y()<<" "<<pos.z()<<std::endl;

        std::cout <<"ZH Step length: "<<aStep->GetStepLength() /CLHEP::mm <<" in "<<OrigLogicalVolumeName<<std::endl;
      */
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

        std::vector<const G4Step*> steps;
        //bool shortstep = false;
        if (m_config.shorten_lar_step && StepLength>0.2)
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

              //std::cout <<"ZH substep: "<<i<<" Pos: "<<subpoint1<<" "<<subpoint2<<std::endl;
              G4Step* newstep = new G4Step(*aStep);
              newstep->SetPreStepPoint(startpoint);
              newstep->SetPostStepPoint(endpoint);
              newstep->SetStepLength( (subpoint1-subpoint2).mag());
              newstep->SetTotalEnergyDeposit(aStep->GetTotalEnergyDeposit()/nsub_step);
              //std::cout <<"ZH substep: "<<newstep->GetPreStepPoint()->GetPosition()<<" "<<newstep->GetPostStepPoint()->GetPosition()<<"   /   "<<0.5*(newstep->GetPreStepPoint()->GetPosition()+newstep->GetPostStepPoint()->GetPosition())<<std::endl;
              steps.push_back(newstep);
            }
            //std::cout <<"Orig poststep"<<aStep->GetPostStepPoint()->GetPosition()<<std::endl;
            //shortstep = true;
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

            std::vector<LArHitData> results;
            if (m_current_calculator->Process(steps[istep], results))
              {
                //if (!shortstep) G4cout <<"ZH process step ok: "<<results.size()<<G4endl;

                for (auto larhit: results)
                  {
                    et += larhit.energy;
                    //std::cout <<"ZH Hit: "<<i<<" E: "<<larhit.energy<<" in: "<<(std::string) larhit.id <<std::endl;
                  }
              }
            else
              {
                //Maybe 0 hits or something like that...
                //G4cout << "ZH: Error: Hit not processed by calculator! " << istep<<" / "<<steps.size()<<" shortstep: "<<shortstep<<G4endl;
                //G4cout << "ZH: calculator not processed: Volume: "<< pCurrentVolume->GetName()<<" "<<m_current_calculator<< " position: "<<pos<<" SL: "<<StepLength<<G4endl;
                //G4cout <<"ZH: Orig position: "<<steps[istep]->GetPreStepPoint()->GetPosition()<<"  /  "<<steps[istep]->GetPostStepPoint()->GetPosition()<<"and SL: "<<StepLength<<" step: "<<aStep->GetPreStepPoint()->GetPosition()<<" / "<<aStep->GetPostStepPoint()->GetPosition()<<G4endl;
                return;
              }

            // drop hits with zero deposited energy (could still happen with negative corrections from calculator)
            //Or if total energy is <0
            if (et <= 0.)
              {
                std::cout <<"ZH: Total negative energy: "<<et<<" not processing..."<<std::endl;
                return;
              }

            size_t nlarh = results.size();
            std::vector<G4ThreeVector> subhitcells;
            subhitcells.resize(nlarh);
            G4ThreeVector origpos = pos;
            double e_subhitmax = -999.;
            int e_subhitmaxindex =-1;
            if (nlarh>0)
              {
                e_subhitmax = results[0].energy;
                e_subhitmaxindex = 0;
              }
            //Figure out the subhit with most energy
            for (size_t i=1; i<nlarh; ++i)
              {
                if (e_subhitmax< results[i].energy)
                  {
                    e_subhitmax = results[i].energy;
                    e_subhitmaxindex = i;
                  }
              }
            //Identifier for the subhit with max energy
            LArG4Identifier e_subhitmax_ident = results[e_subhitmaxindex].id;
            Identifier e_subhitmax_id = m_lar_helper->ConvertID(e_subhitmax_ident);

            for (size_t i=0; i<nlarh; ++i)
              {
                LArG4Identifier ident = results[i].id;
                Identifier id = m_lar_helper->ConvertID(ident);

                Identifier inv_id;
                if (id == inv_id)
                  {
                    std::cout <<"Something wrong with identifier (LAr): "<<(std::string) ident;
                    std::cout <<" "<<id<<" "<<id.getString()<<" "<<CurrentLogicalVolumeName  <<" nhit: "<<nlarh<<std::endl;
                    std::cout <<inv_id<<std::endl;
                  }

                //std::cout <<"Subhit: "<<i<<std::endl;
                //std::cout <<"LArIdent "<<(std::string) ident<<std::endl;
                //std::cout <<"Ident "<<id.getString()<<std::endl;
                //std::cout <<"ZH Step: LAr: "<<StepLength<<std::endl;
                //need to get the cell information

                if (nlarh>1)
                  {
                    //it didn't seem to happen outside em_barrel
                    if (m_lar_emID->is_em_barrel(id))
                      {
                        if (m_config.shift_lar_subhit)
                          {
                            //find subhit with largest energy

                            if (e_subhitmaxindex == -1)
                              {
                                std::cout <<"ZH no subhit index with e>-999??? "<<std::endl;
                                return;
                              }
                            else
                              {
                                //std::cout <<"ZH shifting subhits: largest energy subhit index is "<<e_subhitmaxindex<<" E: "<<e_subhitmax<<" identifier: "<<e_subhitmax_id.getString()<<std::endl;
                              }
                            //from sampling, hit_eta, hit_phi (simple geometry)
                            //CaloDetDescrElement *bestcell = m_calo_dd_man->get_element(m_calo_dd_man->get_element(id)->getSampling(),origpos.eta(), origpos.phi());
                            const CaloDetDescrElement *highestcell = m_calo_dd_man->get_element(e_subhitmax_id);
                            //from identifier
                            const CaloDetDescrElement *thiscell = m_calo_dd_man->get_element(id);
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
                                //the cells match, do not shift this hit
                                //std::cout <<"No shift: index: "<<i<<std::endl;
                                //std::cout <<"Orig pos: "<<origpos.x()<<" "<<origpos.y()<<" "<<origpos.z()<<std::endl;
                                //std::cout <<"This cell: "<<thiscell->x()<<" "<<thiscell->y()<<" "<<thiscell->z()<<std::endl;
                                //std::cout <<"No shift"<<std::endl;
                                pos = origpos;
                              }
                            else
                              {
                                //the two cells do not match => shift
                                //std::cout <<"Orig pos: "<<origpos.x()<<" "<<origpos.y()<<" "<<origpos.z()<<std::endl;
                                G4ThreeVector diff(thiscell->x()-highestcell->x(), thiscell->y()-highestcell->y(), thiscell->z()-highestcell->z());
                                pos = origpos+diff;
                                //std::cout <<"Shift: ! index: "<<i<<std::endl;
                                //std::cout <<"This cell: "<<thiscell->x()<<" "<<thiscell->y()<<" "<<thiscell->z()<<std::endl;
                                //std::cout <<"Highest E cell: "<<highestcell->x()<<" "<<highestcell->y()<<" "<<highestcell->z()<<std::endl;
                                //std::cout <<"(Best cell: "<<bestcell->x()<<" "<<bestcell->y()<<" "<<bestcell->z()<<")"<<std::endl;
                                //std::cout <<"Shift pos: "<<pos.x()<<" "<<pos.y()<<" "<<pos.z()<<std::endl;
                              }
                            //delete highestcell;
                            //delete thiscell;
                          }
                      }
                    else
                      {
                        //Does this happen?
                        std::cout <<"More subhits, but not in LAr barrel "<<(std::string) ident<<std::endl;
                      }
                  }

                /*
                //This was only for testing purposes that the shifted hits match to the right cells
                CaloDetDescrElement *pcell = m_calo_dd_man->get_element(id);
                if (pcell)
                {
                G4ThreeVector cellvec(pcell->x(),pcell->y(), pcell->z());
                std::cout <<"Hit-cell dist: "<<(pos-cellvec).mag()<<"  in: "<<i<<"  "<<(std::string) ident<<" sampling: "<<pcell->getSampling()<<" "<<id.getString()<<std::endl;
                CaloDetDescrElement *testcell = m_calo_dd_man->get_element(pcell->getSampling(),pos.eta(), pos.phi());
                if (testcell)
                {
                G4ThreeVector testvec(testcell->x(),testcell->y(), testcell->z());
                std::cout <<"Test cell: "<<testcell->x()<<" "<<testcell->y()<<" "<<testcell->z()<<" "<<testcell->identify().getString()<<std::endl;
                std::cout <<"Hit-test cell dist: "<<(pos-testvec).mag()<<"  in: "<<i<<"  sampling: "<<testcell->getSampling()<<std::endl;
                }
                else
                {
                std::cout <<"Test cell failed: "<<pcell->getSampling()<<" "<<pos.eta()<<" "<<pos.phi()<<std::endl;
                }
                }
                else
                {
                std::cout <<"pcell failed: "<<id.getString()<<std::endl;
                }
                */
                //Finalize time for LAr hits?: NO
                //double time = results[i].energy)==0 ? 0. : (double) results[i].time/results[i].energy/CLHEP::ns;
                double time = results[i].time;
                double energy = results[i].energy/CLHEP::MeV;
                update_map(pos, id, energy, time, true, nlarh); //store nlarh as info
                // ISF_FCS_Parametrization::FCS_StepInfo* theInfo = new ISF_FCS_Parametrization::FCS_StepInfo(pos, id, energy, time, true, nlarh); //store nlarh as info
                //This one stores also StepLength, but it is not yet in SVN...
                //          ISF_FCS_Parametrization::FCS_StepInfo* theInfo = new ISF_FCS_Parametrization::FCS_StepInfo(pos, id, (double) results[i].energy, (double) results[i].time, true, nlarh, StepLength); //store nlarh as info
                //std::cout <<"Adding new step info: "<<i<<" at: "<<pos<<" Id: "<<id<<" E: "<<energy<<" time: "<<time<<std::endl;
                // m_eventSteps->push_back(theInfo);
              }//nlarh
            //std::cout <<"----"<<std::endl;
          } //istep

        //Delete steps?
        if (steps.size()>1)
          {
            //only when doing substeps, don't want to delete the original a4step
            for (std::vector<const G4Step*>::iterator it = steps.begin(); it!=steps.end(); ++it) {
              delete *it;
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
          TileHitData hitData;
          TileMicroHit micHit = m_current_calculator_Tile->GetTileMicroHit(aStep, hitData);
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
            update_map(pos, micHit.pmt_up, micHit.e_up, micHit.time_up, true,1);
            // ISF_FCS_Parametrization::FCS_StepInfo* theInfo_Tile_up = new ISF_FCS_Parametrization::FCS_StepInfo(pos, micHit.pmt_up, micHit.e_up, micHit.time_up, true,1);
            //Commented out version needs ISF_Event which is not yet in SVN..
            //      ISF_FCS_Parametrization::FCS_StepInfo* theInfo_Tile_up = new ISF_FCS_Parametrization::FCS_StepInfo(pos, micHit.pmt_up, micHit.e_up, micHit.time_up, true,1,StepLength);
            // m_eventSteps->push_back(theInfo_Tile_up);
            update_map(pos, micHit.pmt_down, micHit.e_down,micHit.time_down , true,1);
            // ISF_FCS_Parametrization::FCS_StepInfo* theInfo_Tile_down = new ISF_FCS_Parametrization::FCS_StepInfo(pos, micHit.pmt_down, micHit.e_down,micHit.time_down , true,1);
            //ISF_FCS_Parametrization::FCS_StepInfo* theInfo_Tile_down = new ISF_FCS_Parametrization::FCS_StepInfo(pos, micHit.pmt_down, micHit.e_down,micHit.time_down , true,1,StepLength);
            // m_eventSteps->push_back(theInfo_Tile_down);

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
  }
  else {

    // Get the appropriate merging limits
    double tsame;
    const CaloCell_ID::CaloSample& layer = m_calo_dd_man->get_element(l_cell)->getSampling();
    if (layer >= CaloCell_ID::PreSamplerB && layer <= CaloCell_ID::EME3){
      tsame = m_config.m_maxTimeLAr;
    } else if (layer >= CaloCell_ID::HEC0  && layer <= CaloCell_ID::HEC3){
      tsame = m_config.m_maxTimeHEC;
    } else if (layer >= CaloCell_ID::TileBar0 && layer <= CaloCell_ID::TileExt2){
      tsame = m_config.m_maxTimeTile;
    } else if (layer >=CaloCell_ID::FCAL0 && layer <= CaloCell_ID::FCAL2){
      tsame = m_config.m_maxTimeFCAL;
    } else {
      tsame = m_config.m_maxTime;
    }

    bool match = false;
    for (auto map_it : * map_item->second){
      // Time check is straightforward
      double delta_t = fabs(map_it->time()-l_time);
      if ( delta_t >= tsame ) continue;

      // Distance check is less straightforward
      CLHEP::Hep3Vector a_diff = l_vec - map_it->position();
      double a_inv_length = 1./a_diff.mag();
      double delta_phi = fabs(sin(l_vec.phi()-map_it->position().phi())*a_diff.mag());
      double delta_eta = fabs(sin(l_vec.theta()-map_it->position().theta())*a_diff.mag());
      double delta_r = fabs(a_diff.dot( l_vec ) * a_inv_length);
      double hit_diff2 = map_it->position().diff2( l_vec );

      // Overall merging scheme
      if (layer >= CaloCell_ID::PreSamplerB && layer <= CaloCell_ID::EME3){
        // Customized merging scheme for LAr barrel and endcap, use only if we're not changing maxRadiusLAr value
        if(m_config.m_maxRadiusLAr == 25){
          if (layer==CaloCell_ID::PreSamplerB || layer==CaloCell_ID::PreSamplerE){
            // PS default is 1mm in eta, 5mm in phi, no cut in r
            if (delta_r >= m_config.m_maxrPS) continue;
            if (delta_eta >= m_config.m_maxEtaPS) continue;
            if (delta_phi >= m_config.m_maxPhiPS) continue;
          } else if (layer==CaloCell_ID::EMB1 || layer==CaloCell_ID::EME1){
            // EM1 default is 1mm in eta, 5mm in phi, 15mm in r
            if (delta_r >= m_config.m_maxrEM1) continue;
            if (delta_eta >= m_config.m_maxEtaEM1) continue;
            if (delta_phi >= m_config.m_maxPhiEM1) continue;
          } else if (layer==CaloCell_ID::EMB2 || layer==CaloCell_ID::EME2){
            // EM2 default is 1mm in eta, 5mm in phi, 60mm in r
            if (delta_r >= m_config.m_maxrEM2) continue;
            if (delta_eta >= m_config.m_maxEtaEM2) continue;
            if (delta_phi >= m_config.m_maxPhiEM2) continue;
          } else if (layer==CaloCell_ID::EMB3 || layer==CaloCell_ID::EME3){
            // EM3 default is 1mm in eta, 5mm in phi, 8mm in r
            if (delta_r >= m_config.m_maxrEM3) continue;
            if (delta_eta >= m_config.m_maxEtaEM3) continue;
            if (delta_phi >= m_config.m_maxPhiEM3) continue;
          }
        } else{ // Merging schemes done by changing maxRadiusLAr
            if ( hit_diff2 >= m_config.m_maxRadiusLAr ) continue;
        }
      } else if (layer >= CaloCell_ID::HEC0  && layer <= CaloCell_ID::HEC3){
        if ( hit_diff2 >= m_config.m_maxRadiusHEC ) continue;
      } else if (layer >= CaloCell_ID::TileBar0 && layer <= CaloCell_ID::TileExt2){
        if ( hit_diff2 >= m_config.m_maxRadiusTile ) continue;
      } else if (layer >=CaloCell_ID::FCAL0 && layer <= CaloCell_ID::FCAL2){
        if ( hit_diff2 >= m_config.m_maxRadiusFCAL ) continue;
      } else {
        if ( hit_diff2 >= m_config.m_maxRadius ) continue;
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




} // namespace G4UA
