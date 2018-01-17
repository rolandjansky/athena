/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// My class header file
#include "Geant4SetupChecker.h"

// For getting information out of Geant4
#include "G4LogicalVolumeStore.hh"
#include "G4PhysicalVolumeStore.hh"
#include "G4RegionStore.hh"
#include "G4FastSimulationManager.hh"
#include "G4Region.hh"
#include "globals.hh"

// For reference file reading
#include <fstream>
#include <sstream>

namespace G4UA{ 

Geant4SetupChecker::Geant4SetupChecker(const std::string& file_location, const bool test=true) : m_file_location(file_location) , m_test(test) {;
}

void Geant4SetupChecker::BeginOfRunAction(const G4Run*){

  // Print the sizes of several stores
  G4LogicalVolumeStore* g4_logical_volume_store = G4LogicalVolumeStore::GetInstance();
  G4PhysicalVolumeStore* g4_physical_volume_store = G4PhysicalVolumeStore::GetInstance();
  G4RegionStore* g4_region_store = G4RegionStore::GetInstance();

  // For each region: GetNumberOfMaterials GetFastSimulationManager ( GetFastSimulationModelList().size() )
  
  // Might be nice to get at some kind of field information? Stepper?
//  G4TransportationManager* g4_transportation_manager = G4TransportationManager::GetTransportationManager();

   if (m_test){
     // open a file for comparing these numbers to
     std::ifstream infile(m_file_location); 
     for (std::string line; std::getline(infile, line); ){
       std::istringstream in(line);
       std::string id;
       in >> id;
       // Basic geometry counts
       if ("Counts"==id){
         unsigned int lvs=0,pvs=0,regs=0;
         in >> lvs >> pvs >> regs;
         if (lvs!=g4_logical_volume_store->size()){
           G4ExceptionDescription ed;
           ed << "ERROR! Reference had " << lvs << " and this setup has " << g4_logical_volume_store->size() << " logical volumes" << G4endl;
           G4Exception("Geant4SetupChecker","LogVolMismatch",FatalException,ed);
         }
         if (pvs!=g4_physical_volume_store->size()){
           G4ExceptionDescription ed;
           ed << "ERROR! Reference had " << pvs << " and this setup has " << g4_physical_volume_store->size() << " physical volumes" << G4endl;
           G4Exception("Geant4SetupChecker","PhysVolMismatch",FatalException,ed);
         }
         if (regs!=g4_region_store->size()){
           G4ExceptionDescription ed;
           ed << "ERROR! Reference had " << regs << " and this setup has " << g4_region_store->size() << " regions" << G4endl;
           G4Exception("Geant4SetupChecker","RegionMismatch",FatalException,ed);
         }
       } // This was the basic geometry counting part
       else { // This is a region - check materials and fast sim
         G4Region* areg = g4_region_store->GetRegion(id);
         if (!areg){
           G4ExceptionDescription ed;
           ed << "ERROR! Reference had region named " << id << "that is not in this configuration, or has changed name" << G4endl;
           G4Exception("Geant4SetupChecker","RegionNotFound",FatalException,ed);
         }
         unsigned int nmat=0, nfs=0;
         in >> nmat >> nfs;
         if (nmat!=areg->GetNumberOfMaterials()){
           G4ExceptionDescription ed;
           ed << "ERROR! Region " << id << " had " << nmat << " materials in ref and " << areg->GetNumberOfMaterials() << " in this setup" << G4endl;
           G4Exception("Geant4SetupChecker","MaterialMismatch",FatalException,ed);
         }
         if ( (!areg->GetFastSimulationManager() && nfs!=0) ||
              ( areg->GetFastSimulationManager() && nfs!=areg->GetFastSimulationManager()->GetFastSimulationModelList().size()) ){
           G4ExceptionDescription ed;
           ed << "ERROR! Region " << id << " had " << nfs << " fast sims in ref and " << (areg->GetFastSimulationManager()?areg->GetFastSimulationManager()->GetFastSimulationModelList().size():0) << " in this setup" << G4endl; 
           G4Exception("Geant4SetupChecker","FastSimMismatch",FatalException,ed);
         }
       } // End of region handling
     } // loop over lines
     infile.close();
     G4cout << "All ok!" << G4endl;
  } // Done with the test!
  else {
    std::ofstream outfile(m_file_location);
    outfile << "Counts " << g4_logical_volume_store->size() << " " << g4_physical_volume_store->size() << " " << g4_region_store->size() << std::endl;
    // Now go through all the regions
    for (auto* areg : *g4_region_store){
      outfile << areg->GetName() << " " << areg->GetNumberOfMaterials() << " " << (areg->GetFastSimulationManager()?areg->GetFastSimulationManager()->GetFastSimulationModelList().size():0) << std::endl;
    }
    outfile.close();
  } // Done with writing a reference file
}

} // namespace G4UA 
