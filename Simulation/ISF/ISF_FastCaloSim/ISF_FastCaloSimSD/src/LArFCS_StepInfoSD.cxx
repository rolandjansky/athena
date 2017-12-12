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
    const G4ThreeVector preStepPosition = a_step->GetPreStepPoint()->GetPosition(); //pre step is the position we're interested in
    const G4ThreeVector postStepPosition = a_step->GetPostStepPoint()->GetPosition();
    double et(0.); // Total collected charge
    std::vector<const G4Step*> steps;
    bool madeSubSteps(false);
    if (m_config.shorten_lar_step && StepLength>0.2) {
      //create smaller substeps instead
      G4int nsub_step=(int) (StepLength/0.2) + 1;
      G4double delta=1./((double) nsub_step);
      //G4cout <<"Orig prestep: "<<preStepPosition<<std::endl;
      for (G4int i=0;i<nsub_step;i++) {
        G4double fraction1 = ((G4double) i)*delta;
        G4double fraction2 = (((G4double) i) + 1.)*delta;
        G4ThreeVector subpoint1=preStepPosition*(1-fraction1) + postStepPosition*fraction1;
        G4ThreeVector subpoint2=preStepPosition*(1-fraction2) + postStepPosition*fraction2;
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
        madeSubSteps=true;
      }
    }
    else {
      steps.push_back(a_step);
    }

    for (const G4Step* substep : steps) {
      G4ThreeVector stepPosition = 0.5*(substep->GetPreStepPoint()->GetPosition()+substep->GetPostStepPoint()->GetPosition());
      std::vector<LArHitData> processedHits;
      if (m_calculator->Process(substep, processedHits)) {
        for (const auto& larhit: processedHits) {
          et += larhit.energy;
        }
      }
      else {
        if (m_config.verboseLevel > 4) {
          //Maybe 0 hits or something like that...
          G4cout << this->GetName()<<" WARNING ProcessHits: Call to ILArCalculatorSvc::Process failed! Details:" << G4endl
                 << "          " << "Volume: "<< a_step->GetPreStepPoint()->GetPhysicalVolume()->GetName()<<" "<<m_calculator<< " position: "<<stepPosition<<" SL: "<<StepLength<<G4endl
                 << "          " << "Orig position: "<<substep->GetPreStepPoint()->GetPosition()<<"  /  "<<substep->GetPostStepPoint()->GetPosition()<<G4endl
                 << "          " << "StepLength: "<<StepLength<<" step: "<<preStepPosition<<" / "<<postStepPosition<<G4endl;
            }
	
	if (madeSubSteps) {
	  //only delete steps when doing substeps. Do not delete the original G4Step!
	  while(!steps.empty()) { delete steps.back(); steps.pop_back(); }
	}
	
        return result;
      }

      // drop hits with zero deposited energy (could still happen with negative corrections from calculator)
      //Or if total energy is <0
      if (et <= 0.) {
        if (m_config.verboseLevel > 4) {
          G4cout << this->GetName()<<" WARNING ProcessHits: Total negative energy: " << et << " not processing..." << G4endl;
        }

	if (madeSubSteps) {
	  //only delete steps when doing substeps. Do not delete the original G4Step!
	  while(!steps.empty()) { delete steps.back(); steps.pop_back(); }
	}
	
        return result;
      }

      const size_t numberOfProcessedHits = processedHits.size();
      const G4ThreeVector originalStepPosition = stepPosition;
      double maxSubHitEnergy = -999.;
      int maxSubHitEnergyindex =-1;
      if (numberOfProcessedHits>0) {
        maxSubHitEnergy = processedHits[0].energy;
        maxSubHitEnergyindex = 0;
      }
      //Figure out the subhit with most energy
      for (size_t i=1; i<numberOfProcessedHits; ++i) {
        if (maxSubHitEnergy<processedHits[i].energy) {
          maxSubHitEnergy = processedHits[i].energy;
          maxSubHitEnergyindex = i;
        }
      }
      //Identifier for the subhit with max energy
      Identifier maxEnergyIdentifier = this->ConvertID(processedHits[maxSubHitEnergyindex].id);
      const CaloDetDescrElement *maxEnergyCell = m_calo_dd_man->get_element(maxEnergyIdentifier);

      Identifier invalidIdentifier;
      //for (size_t i=0; i<numberOfProcessedHits; ++i) {
      for (const auto& larhit: processedHits) {
        Identifier id = this->ConvertID(larhit.id);
        if (id == invalidIdentifier) {
          G4cout << this->GetName()<<" WARNING ProcessHits: Something wrong with LArG4Identifier: "<<(std::string) larhit.id<<G4endl
                 <<"          "<<id<<G4endl
                 <<"          "<<id.getString()<<G4endl
                 <<"          "<< a_step->GetPreStepPoint()->GetPhysicalVolume()->GetLogicalVolume()->GetName()<<G4endl
                 <<"          numberOfProcessedHits: "<<numberOfProcessedHits<<G4endl;
          G4cout <<"          "<<invalidIdentifier<<G4endl;
        }
        //need to get the cell information
        if (numberOfProcessedHits>1) {
          if (!m_larEmID->is_em_barrel(id)) {
            //It didn't seem to happen outside em_barrel, so flag up if it does:
            G4cout << this->GetName()<<" WARNING ProcessHits: Outside LAr barrel, but numberOfProcessedHits="<<numberOfProcessedHits
                   <<", LArG4Identifier: "<<(std::string) larhit.id<<G4endl;
          }
          else {
            if (m_config.shift_lar_subhit) {
              //find subhit with largest energy
              if (maxSubHitEnergyindex == -1) {
                G4cout << this->GetName()<<" WARNING ProcessHits: no subhit index with e>-999??? "<<G4endl;

		if (madeSubSteps) {
		  //only delete steps when doing substeps. Do not delete the original G4Step!
		  while(!steps.empty()) { delete steps.back(); steps.pop_back(); }
		}
		
                return result;
              }
              if(m_config.verboseLevel > 9) {
                G4cout << this->GetName()<<" VERBOSE ProcessHits: shifting subhits: largest energy subhit index is "<<maxSubHitEnergyindex<<" E: "<<maxSubHitEnergy<<" identifier: "<<maxEnergyIdentifier.getString()<<G4endl;
              }
              //from identifier
              const CaloDetDescrElement *thiscell = m_calo_dd_man->get_element(id);
              if (!maxEnergyCell) {
                //How often does this happen? Do not shift.
                G4cout << this->GetName()<<" WARNING ProcessHits: maxEnergyCell failed: "<<maxEnergyIdentifier.getString()<<G4endl
                       <<"          "<<m_calo_dd_man->get_element(id)->getSampling()<<G4endl
                       <<"          "<<originalStepPosition.eta()<<G4endl
                       <<"          "<< originalStepPosition.phi()<<G4endl;
                stepPosition = originalStepPosition;
              }
              else if (maxEnergyCell == thiscell) {
                //The cells match, so do not shift this hit.
                if(m_config.verboseLevel > 9) {
                  G4cout << this->GetName()<<" VERBOSE ProcessHits: Original step position: "<<originalStepPosition.x()<<" "<<originalStepPosition.y()<<" "<<originalStepPosition.z()<<G4endl
                  <<"          "<<"This cell: "<<thiscell->x()<<" "<<thiscell->y()<<" "<<thiscell->z()<<G4endl
                  <<"          "<<"No shift"<<G4endl;
                }
                stepPosition = originalStepPosition;
              }
              else {
                //the two cells do not match => shift
                G4ThreeVector diff(thiscell->x()-maxEnergyCell->x(), thiscell->y()-maxEnergyCell->y(), thiscell->z()-maxEnergyCell->z());
                stepPosition = originalStepPosition+diff;
                if(m_config.verboseLevel > 9) {
                  const CaloDetDescrElement *bestcell = m_calo_dd_man->get_element(m_calo_dd_man->get_element(id)->getSampling(),originalStepPosition.eta(), originalStepPosition.phi());
                  G4cout << this->GetName()<<" VERBOSE ProcessHits: Original step position: "<<originalStepPosition.x()<<" "<<originalStepPosition.y()<<" "<<originalStepPosition.z()<<G4endl
                  <<"          "<<"This cell: "<<thiscell->x()<<" "<<thiscell->y()<<" "<<thiscell->z()<<G4endl
                  <<"          "<<"Highest E cell: "<<maxEnergyCell->x()<<" "<<maxEnergyCell->y()<<" "<<maxEnergyCell->z()<<G4endl
                  <<"          "<<"(Best cell: "<<bestcell->x()<<" "<<bestcell->y()<<" "<<bestcell->z()<<")"<<G4endl
                  <<"          "<<"Shifted step position: "<<stepPosition.x()<<" "<<stepPosition.y()<<" "<<stepPosition.z()<<G4endl;
                }
              }
            }
          }
        }
        //Finalize time for LAr hits?: NO
        //double time = larhit.energy)==0 ? 0. : (double) larhit.time/larhit.energy/CLHEP::ns;
        double time = larhit.time;
        double energy = larhit.energy/CLHEP::MeV;
        this->update_map(stepPosition, id, energy, time, true, numberOfProcessedHits); //store numberOfProcessedHits as info
      }//numberOfProcessedHits
    } //istep
    if (madeSubSteps) {
      //only delete steps when doing substeps. Do not delete the original G4Step!
      while(!steps.empty()) { delete steps.back(); steps.pop_back(); }
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
        G4ExceptionDescription description;
        description << "ConvertID: LArEM_ID error code " << e.code() << " "
                    << (std::string) e;
        G4Exception("LArFCS_StepInfoSD", "ConvertIDEM", FatalException, description);
        abort();
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
        G4ExceptionDescription description;
        description << "ConvertID: LArHEC_ID error code " << e.code() << " "
                    << (std::string) e;
        G4Exception("LArFCS_StepInfoSD", "ConvertIDHEC", FatalException, description);
        abort();
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
          G4ExceptionDescription description;
          description << "ConvertID: LArFCAL_ID error code " << e.code() << " "
                      << (std::string) e;
          G4Exception("LArFCS_StepInfoSD", "ConvertIDFCAL", FatalException, description);
          abort();
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
          G4ExceptionDescription description;
          description << "ConvertID: LArMiniFCAL_ID error code " << e.code() << " "
                      << (std::string) e;
          G4Exception("LArFCS_StepInfoSD", "ConvertIDMiniFCAL", FatalException, description);
          abort();
        }
      }
    }
  }
  return id;
}
