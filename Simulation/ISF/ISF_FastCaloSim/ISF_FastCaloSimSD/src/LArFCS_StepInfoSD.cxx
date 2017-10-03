/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// class header
#include "LArFCS_StepInfoSD.h"
/// Geant4 headers
#include "G4Step.hh"
#include "G4ThreeVector.hh"
#include "G4TouchableHistory.hh"

/// Athena headers
// #include "CaloIdentifier/CaloIdManager.h"
// #include "CaloIdentifier/LArID_Exception.h"
// #include "CaloIdentifier/LArEM_ID.h"
// #include "CaloIdentifier/LArFCAL_ID.h"
// #include "CaloIdentifier/LArHEC_ID.h"
// #include "CaloIdentifier/LArMiniFCAL_ID.h"
#include "CaloDetDescr/CaloDetDescrManager.h"
#include "CaloDetDescr/CaloDetDescrElement.h"
#include "LArG4Code/ILArCalculatorSvc.h"

LArFCS_StepInfoSD::LArFCS_StepInfoSD(G4String a_name, const FCS_Param::Config& config)
  : FCS_StepInfoSD(a_name, config)
  , m_calculator(m_config.m_LArCalculator)
{
}

LArFCS_StepInfoSD::~LArFCS_StepInfoSD()
{
}

G4bool LArFCS_StepInfoSD::ProcessHits(G4Step* a_step,G4TouchableHistory*)
{
  G4bool result(false);
  // If there's no energy, there's no hit.  (Aside: Isn't this energy
  // the same as the energy from the calculator?  Not necessarily.
  // The calculator may include detector effects such as
  // charge-collection which are not modeled by Geant4.)
  if(a_step->GetTotalEnergyDeposit() <= 0.) { return result; }

  if (m_calculator) {
    const double StepLength = a_step->GetStepLength()/ CLHEP::mm;
    const G4ThreeVector position1 = a_step->GetPreStepPoint()->GetPosition(); //pre step is the position we're interested in
    const G4ThreeVector position2 = a_step->GetPostStepPoint()->GetPosition();
    //G4ThreeVector pos = 0.5*(position1+position2);
    double et(0.); // Total collected charge
    std::vector<const G4Step*> steps;
    if (m_config.shorten_lar_step && StepLength>0.2) {
      //create smaller substeps instead
      G4int nsub_step=(int) (StepLength/0.2) + 1;
      G4double delta=1./((double) nsub_step);
      //std::cout <<"Orig prestep: "<<a_step->GetPreStepPoint()->GetPosition()<<std::endl;
      for (G4int i=0;i<nsub_step;i++) {
        G4double fraction1 = ((G4double) i)*delta;
        G4double fraction2 = (((G4double) i) + 1.)*delta;
        G4ThreeVector subpoint1=position1*(1-fraction1) + position2*fraction1;
        G4ThreeVector subpoint2=position1*(1-fraction2) + position2*fraction2;
        G4StepPoint *startpoint = new G4StepPoint(*(a_step->GetPreStepPoint()));
        G4StepPoint *endpoint = new G4StepPoint(*(a_step->GetPostStepPoint()));
        startpoint->SetPosition(subpoint1);
        endpoint->SetPosition(subpoint2);

        G4Step* newstep = new G4Step(*a_step);
        newstep->SetPreStepPoint(startpoint);
        newstep->SetPostStepPoint(endpoint);
        newstep->SetStepLength( (subpoint1-subpoint2).mag());
        newstep->SetTotalEnergyDeposit(a_step->GetTotalEnergyDeposit()/nsub_step);
        steps.push_back(newstep);
      }
    }
    else {
      steps.push_back(a_step);
    }

    //std::cout <<"ZH Step size: "<<steps.size()<<" full step length "<<StepLength<<std::endl;
    for (unsigned int istep = 0; istep <steps.size(); istep++) {
      //need to update the position!
      G4ThreeVector pos = 0.5*(steps[istep]->GetPreStepPoint()->GetPosition()+steps[istep]->GetPostStepPoint()->GetPosition());
      //std::cout <<"ZH Processing step: "<<istep<<" at position "<<pos<<std::endl;
      std::vector<LArHitData> results;
      if (m_calculator->Process(steps[istep], results)) {
        //if (!shortstep) G4cout <<"ZH process step ok: "<<results.size()<<G4endl;
        for (auto larhit: results) {
          et += larhit.energy;
          //std::cout <<"ZH Hit: "<<i<<" E: "<<larhit.energy<<" in: "<<(std::string) larhit.id <<std::endl;
        }
      }
      else {
        //Maybe 0 hits or something like that...
        //G4cout << "ZH: Error: Hit not processed by calculator! " << istep<<" / "<<steps.size()<<" shortstep: "<<shortstep<<G4endl;
        //G4cout << "ZH: calculator not processed: Volume: "<< pCurrentVolume->GetName()<<" "<<m_calculator<< " position: "<<pos<<" SL: "<<StepLength<<G4endl;
        //G4cout <<"ZH: Orig position: "<<steps[istep]->GetPreStepPoint()->GetPosition()<<"  /  "<<steps[istep]->GetPostStepPoint()->GetPosition()<<"and SL: "<<StepLength<<" step: "<<a_step->GetPreStepPoint()->GetPosition()<<" / "<<a_step->GetPostStepPoint()->GetPosition()<<G4endl;
        return result;
      }

      // drop hits with zero deposited energy (could still happen with negative corrections from calculator)
      //Or if total energy is <0
      if (et <= 0.) {
        std::cout <<"ZH: Total negative energy: "<<et<<" not processing..."<<std::endl;
        return result;
      }

      size_t nlarh = results.size();
      std::vector<G4ThreeVector> subhitcells;
      subhitcells.resize(nlarh);
      const G4ThreeVector origpos = pos;
      double e_subhitmax = -999.;
      int e_subhitmaxindex =-1;
      if (nlarh>0) {
        e_subhitmax = results[0].energy;
        e_subhitmaxindex = 0;
      }
      //Figure out the subhit with most energy
      for (size_t i=1; i<nlarh; ++i) {
        if (e_subhitmax< results[i].energy) {
          e_subhitmax = results[i].energy;
          e_subhitmaxindex = i;
        }
      }
      //Identifier for the subhit with max energy
      LArG4Identifier e_subhitmax_ident = results[e_subhitmaxindex].id;
      Identifier e_subhitmax_id = this->ConvertID(e_subhitmax_ident);

      for (size_t i=0; i<nlarh; ++i) {
        LArG4Identifier ident = results[i].id;
        Identifier id = this->ConvertID(ident);
        Identifier inv_id;
        if (id == inv_id) {
          std::cout <<"Something wrong with identifier (LAr): "<<(std::string) ident;
          std::cout <<" "<<id
                    <<" "<<id.getString()
                    <<" "<< a_step->GetPreStepPoint()->GetPhysicalVolume()->GetLogicalVolume()->GetName()
                    <<" nhit: "<<nlarh<<std::endl;
          std::cout <<inv_id<<std::endl;
        }
        //std::cout <<"Subhit: "<<i<<std::endl;
        //std::cout <<"LArIdent "<<(std::string) ident<<std::endl;
        //std::cout <<"Ident "<<id.getString()<<std::endl;
        //std::cout <<"ZH Step: LAr: "<<StepLength<<std::endl;
        //need to get the cell information
        if (nlarh>1) {
          //it didn't seem to happen outside em_barrel
          if (m_larEmID->is_em_barrel(id)) {
            if (m_config.shift_lar_subhit) {
              //find subhit with largest energy
              if (e_subhitmaxindex == -1) {
                std::cout <<"ZH no subhit index with e>-999??? "<<std::endl;
                return result;
              }
              else {
                //std::cout <<"ZH shifting subhits: largest energy subhit index is "<<e_subhitmaxindex<<" E: "<<e_subhitmax<<" identifier: "<<e_subhitmax_id.getString()<<std::endl;
              }
              //from sampling, hit_eta, hit_phi (simple geometry)
              //CaloDetDescrElement *bestcell = m_calo_dd_man->get_element(m_calo_dd_man->get_element(id)->getSampling(),origpos.eta(), origpos.phi());
              CaloDetDescrElement *highestcell = m_calo_dd_man->get_element(e_subhitmax_id);
              //from identifier
              CaloDetDescrElement *thiscell = m_calo_dd_man->get_element(id);
              //delete them afterwards?
              if (!highestcell) {
                //How often does this happen, do not shift
                std::cout <<"ZH highestEcell failed: "<<e_subhitmax_id.getString()<<std::endl;
                //" "<<m_calo_dd_man->get_element(id)->getSampling()<<" "<<origpos.eta()<<" "<< origpos.phi()<<std::endl;
                //do no shift?
                pos = origpos;
              }
              else if (highestcell == thiscell) {
                //the cells match, do not shift this hit
                //std::cout <<"No shift: index: "<<i<<std::endl;
                //std::cout <<"Orig pos: "<<origpos.x()<<" "<<origpos.y()<<" "<<origpos.z()<<std::endl;
                //std::cout <<"This cell: "<<thiscell->x()<<" "<<thiscell->y()<<" "<<thiscell->z()<<std::endl;
                //std::cout <<"No shift"<<std::endl;
                pos = origpos;
              }
              else {
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
          else {
            //Does this happen?
            std::cout <<"More subhits, but not in LAr barrel "<<(std::string) ident<<std::endl;
          }
        }
        //Finalize time for LAr hits?: NO
        //double time = results[i].energy)==0 ? 0. : (double) results[i].time/results[i].energy/CLHEP::ns;
        double time = results[i].time;
        double energy = results[i].energy/CLHEP::MeV;
        this->update_map(pos, id, energy, time, true, nlarh); //store nlarh as info
      }//nlarh
    } //istep
    //Delete steps?
    if (steps.size()>1) {
      //only when doing substeps, don't want to delete the original a4step
      while(!steps.empty()) delete steps.back(), steps.pop_back();
    }
  }
  return result;
}

Identifier LArFCS_StepInfoSD::ConvertID(const LArG4Identifier& a_ident) const
{
  Identifier id;
  if(a_ident[0]==4) {
    // is LAr
    if(a_ident[1]==1) {
      //is LAr EM
      try {
        id = m_larEmID->channel_id(a_ident[2],  // barrel_ec
                                   a_ident[3],  // sampling
                                   a_ident[4],  // region
                                   a_ident[5],  // eta
                                   a_ident[6]); // phi
      }
      catch (LArID_Exception& e) {
        G4cout << "ERROR ConvertID: LArEM_ID error code " << e.code() << " "
               << (std::string) e << G4endl;
      }
    }
    else if(a_ident[1]==2) {
      //is EM HEC
      try {
        id = m_larHecID->channel_id(a_ident[2],  // zSide
                                    a_ident[3],  // sampling
                                    a_ident[4],  // region
                                    a_ident[5],  // eta
                                    a_ident[6]); // phi
      }
      catch(LArID_Exception& e) {
        G4cout << "ERROR ConvertID: BuildHitCollections: LArHEC_ID error code " << e.code() << " "
               << (std::string) e << G4endl;
      }
    }
    else if(a_ident[1]==3) {
      // FCAL
      if(a_ident[3]>0) {
        //is EM FCAL
        try {
          id = m_larFcalID->channel_id(a_ident[2],  // zSide
                                       a_ident[3],  // sampling
                                       a_ident[4],  // eta
                                       a_ident[5]); // phi
        }
        catch(LArID_Exception& e) {
          G4cout << "ERROR ConvertID:: LArFCAL_ID error code " << e.code() << " "
                 << (std::string) e
                 << G4endl;
        }
      }
      else {
        //is Mini FCAL
        try {
          id = m_larMiniFcalID->channel_id(a_ident[2],  // zSide
                                           a_ident[3],  // module
                                           a_ident[4],  // depth
                                           a_ident[5],  // eta
                                           a_ident[6]); // phi
        }
        catch(LArID_Exception& e) {
          G4cout << "ERROR ConvertID:: LArMiniFCAL_ID error code " << e.code() << " "
                 << (std::string) e
                 << G4endl;
        }
      }
    }
  }
  return id;
}
