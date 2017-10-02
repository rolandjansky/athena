/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "FCS_StepInfoSD.h"

#include "LArG4Code/ILArCalculatorSvc.h"
#include "CaloIdentifier/CaloIdManager.h"
#include "CaloIdentifier/LArID_Exception.h"
#include "CaloIdentifier/LArEM_ID.h"
#include "CaloIdentifier/LArFCAL_ID.h"
#include "CaloIdentifier/LArHEC_ID.h"
#include "CaloIdentifier/LArMiniFCAL_ID.h"
#include "CaloDetDescr/CaloDetDescrManager.h"
#include "CaloDetDescr/CaloDetDescrElement.h"
//#include "StoreGate/StoreGateSvc.h"

#include "G4Step.hh"
#include "G4ThreeVector.hh"

FCS_StepInfoSD::FCS_StepInfoSD(G4String a_name, ILArCalculatorSvc* calc, const Config& config)
  : G4VSensitiveDetector(a_name)
  , m_config(config)
  , m_calculator(calc)
  , m_larEmID(nullptr)
  , m_larFcalID(nullptr)
  , m_larHecID(nullptr)
  , m_larMiniFcalID(nullptr)
  , m_calo_dd_man(nullptr)
{
  m_calo_dd_man  = CaloDetDescrManager::instance(); //FIXME Move somewhere else!!
}

FCS_StepInfoSD::~FCS_StepInfoSD()
{
}

G4bool FCS_StepInfoSD::ProcessHits(G4Step* a_step,G4TouchableHistory*)
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

void FCS_StepInfoSD::update_map(const CLHEP::Hep3Vector & l_vec, const Identifier & l_cell, double l_energy, double l_time, bool l_valid, int l_detector)
{
  // Drop any hits that don't have a good identifier attached
  if (!m_calo_dd_man->get_element(l_cell)) { return; }

  auto map_item = m_hit_map.find( l_cell );
  if (map_item==m_hit_map.end()) {
    m_hit_map[l_cell] = new std::vector< ISF_FCS_Parametrization::FCS_StepInfo* >;
    m_hit_map[l_cell]->push_back( new ISF_FCS_Parametrization::FCS_StepInfo( l_vec , l_cell , l_energy , l_time , l_valid , l_detector ) );
  }
  else {

    // Get the appropriate merging limits
    double tsame;
    const CaloCell_ID::CaloSample& layer = m_calo_dd_man->get_element(l_cell)->getSampling();
    if (layer >= CaloCell_ID::PreSamplerB && layer <= CaloCell_ID::EME3) {
      tsame = m_config.m_maxTimeLAr;
    }
    else if (layer >= CaloCell_ID::HEC0  && layer <= CaloCell_ID::HEC3) {
      tsame = m_config.m_maxTimeHEC;
    }
    else if (layer >= CaloCell_ID::TileBar0 && layer <= CaloCell_ID::TileExt2) {
      tsame = m_config.m_maxTimeTile;
    }
    else if (layer >=CaloCell_ID::FCAL0 && layer <= CaloCell_ID::FCAL2) {
      tsame = m_config.m_maxTimeFCAL;
    }
    else {
      tsame = m_config.m_maxTime;
    }

    bool match = false;
    for (auto map_it : * map_item->second) {
      // Time check is straightforward
      double delta_t = fabs(map_it->time()-l_time);
      if ( delta_t >= tsame ) { continue; }

      // Distance check is less straightforward
      CLHEP::Hep3Vector a_diff = l_vec - map_it->position();
      double a_inv_length = 1./a_diff.mag();
      double delta_phi = fabs(sin(l_vec.phi()-map_it->position().phi())*a_diff.mag());
      double delta_eta = fabs(sin(l_vec.theta()-map_it->position().theta())*a_diff.mag());
      double delta_r = fabs(a_diff.dot( l_vec ) * a_inv_length);
      double hit_diff2 = map_it->position().diff2( l_vec );

      // Overall merging scheme
      if (layer >= CaloCell_ID::PreSamplerB && layer <= CaloCell_ID::EME3) {
        // Customized merging scheme for LAr barrel and endcap, use only if we're not changing maxRadiusLAr value
        if(m_config.m_maxRadiusLAr == 25) {
          if (layer==CaloCell_ID::PreSamplerB || layer==CaloCell_ID::PreSamplerE) {
            // PS default is 1mm in eta, 5mm in phi, no cut in r
            if (delta_r >= m_config.m_maxrPS) continue;
            if (delta_eta >= m_config.m_maxEtaPS) continue;
            if (delta_phi >= m_config.m_maxPhiPS) continue;
          }
          else if (layer==CaloCell_ID::EMB1 || layer==CaloCell_ID::EME1) {
            // EM1 default is 1mm in eta, 5mm in phi, 15mm in r
            if (delta_r >= m_config.m_maxrEM1) continue;
            if (delta_eta >= m_config.m_maxEtaEM1) continue;
            if (delta_phi >= m_config.m_maxPhiEM1) continue;
          }
          else if (layer==CaloCell_ID::EMB2 || layer==CaloCell_ID::EME2) {
            // EM2 default is 1mm in eta, 5mm in phi, 60mm in r
            if (delta_r >= m_config.m_maxrEM2) continue;
            if (delta_eta >= m_config.m_maxEtaEM2) continue;
            if (delta_phi >= m_config.m_maxPhiEM2) continue;
          }
          else if (layer==CaloCell_ID::EMB3 || layer==CaloCell_ID::EME3) {
            // EM3 default is 1mm in eta, 5mm in phi, 8mm in r
            if (delta_r >= m_config.m_maxrEM3) continue;
            if (delta_eta >= m_config.m_maxEtaEM3) continue;
            if (delta_phi >= m_config.m_maxPhiEM3) continue;
          }
        }
        else { // Merging schemes done by changing maxRadiusLAr
          if ( hit_diff2 >= m_config.m_maxRadiusLAr ) continue;
        }
      }
      else if (layer >= CaloCell_ID::HEC0  && layer <= CaloCell_ID::HEC3) {
        if ( hit_diff2 >= m_config.m_maxRadiusHEC ) continue;
      }
      else if (layer >= CaloCell_ID::TileBar0 && layer <= CaloCell_ID::TileExt2) {
        if ( hit_diff2 >= m_config.m_maxRadiusTile ) continue;
      }
      else if (layer >=CaloCell_ID::FCAL0 && layer <= CaloCell_ID::FCAL2) {
        if ( hit_diff2 >= m_config.m_maxRadiusFCAL ) continue;
      }
      else {
        if ( hit_diff2 >= m_config.m_maxRadius ) continue;
      }
      // Found a match.  Make a temporary that will be deleted!
      ISF_FCS_Parametrization::FCS_StepInfo my_info( l_vec , l_cell , l_energy , l_time , l_valid , l_detector );
      *map_it += my_info;
      match = true;
      break;
    } // End of search for match in time and space
    if (!match) {
      map_item->second->push_back( new ISF_FCS_Parametrization::FCS_StepInfo( l_vec , l_cell , l_energy , l_time , l_valid , l_detector ) );
    } // Didn't match
  } // ID already in the map

} // That's it for updating the map!

void FCS_StepInfoSD::EndOfAthenaEvent( ISF_FCS_Parametrization::FCS_StepInfoCollection* hitContainer )
{
  // Unpack map into vector
  for (auto it : m_hit_map) {
    for (auto a_s : * it.second) {
      // Giving away ownership of the objects!
      hitContainer->push_back( a_s );
    }
    it.second->clear();
    delete it.second;
  } // Vector of IDs in the map
  m_hit_map.clear();
  G4cout << "FCS_StepInfoSD::EndOfAthenaEvent: After initial cleanup, N=" << hitContainer->size() << G4endl;
  return;
}

Identifier FCS_StepInfoSD::ConvertID(const LArG4Identifier& a_ident) const
{
    Identifier id;
    
    if(a_ident[0]==4)
    {
        // is LAr
        if(a_ident[1]==1)
        {
            //is LAr EM
            try
            {
                id = m_larEmID->channel_id(a_ident[2],  // barrel_ec
                                           a_ident[3],  // sampling
                                           a_ident[4],  // region
                                           a_ident[5],  // eta
                                           a_ident[6]); // phi
            }
            catch (LArID_Exception& e)
            {
                G4cout << "ERROR ConvertID: LArEM_ID error code " << e.code() << " "
                << (std::string) e << G4endl;
            }
        }
        else if(a_ident[1]==2)
        {
            //is EM HEC
            try
            {
                id = m_larHecID->channel_id(a_ident[2],  // zSide
                                            a_ident[3],  // sampling
                                            a_ident[4],  // region
                                            a_ident[5],  // eta
                                            a_ident[6]); // phi
            }
            catch(LArID_Exception& e)
            {
                G4cout << "ERROR ConvertID: BuildHitCollections: LArHEC_ID error code " << e.code() << " "
                << (std::string) e << G4endl;
            }
        }
        else if(a_ident[1]==3)
        {
            // FCAL
            if(a_ident[3]>0)
            {
                //is EM FCAL
                try
                {
                    id = m_larFcalID->channel_id(a_ident[2],  // zSide
                                                 a_ident[3],  // sampling
                                                 a_ident[4],  // eta
                                                 a_ident[5]); // phi
                }
                catch(LArID_Exception& e)
                {
                    G4cout << "ERROR ConvertID:: LArFCAL_ID error code " << e.code() << " "
                    << (std::string) e
                    << G4endl;
                }
            }
            else
            {
                //is Mini FCAL
                try
                {
                    id = m_larMiniFcalID->channel_id(a_ident[2],  // zSide
                                                     a_ident[3],  // module
                                                     a_ident[4],  // depth
                                                     a_ident[5],  // eta
                                                     a_ident[6]); // phi
                }
                catch(LArID_Exception& e)
                {
                    G4cout << "ERROR ConvertID:: LArMiniFCAL_ID error code " << e.code() << " "
                    << (std::string) e
                    << G4endl;
                }
            }
        }
    }
    return id;
}
