/*
   Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
 */

#include "G4UserActions/VolumeDumper.h"
#include "G4DebuggingTools/G4DebuggingHelper.h"

// Geant4
#include "G4PhysicalVolumeStore.hh"
#include "G4LogicalVolume.hh"

// Gaudi
#include "GaudiKernel/Bootstrap.h"
#include "GaudiKernel/ISvcLocator.h"
#include "GaudiKernel/IMessageSvc.h"

namespace G4UA
{

//---------------------------------------------------------------------------
VolumeDumper::VolumeDumper() :
        AthMessaging(Gaudi::svcLocator()->service<IMessageSvc>("MessageSvc"),
                     "VolumeDumper")
{
}

//---------------------------------------------------------------------------
void VolumeDumper::BeginOfEventAction(const G4Event*)
{
        ATH_MSG_INFO("Dumping G4PhysicalVolumeStore");
        ATH_MSG_INFO("-----------------------------");

        G4PhysicalVolumeStore* store = G4PhysicalVolumeStore::GetInstance();
        std::set<std::pair<G4String, G4String>> volumes;

        ATH_MSG_INFO("Size: " << store->size());

        for (unsigned int i = 0; i < store->size(); i++) {
                volumes.insert(
                    std::pair<G4String, G4String>(
                        (*store)[i]->GetName(), (*store)[i]->GetLogicalVolume()->GetName() 
                    ));
        }

        for (auto& vol : volumes) {
                std::cout << "short: " << G4DebuggingHelpers::ClassifyVolume(vol.first)
                          << " full: " << vol.first
                          << " logical: " << vol.second
                          << std::endl;
        }

        ATH_MSG_INFO("Finished dumbing G4PhysicalVolumeStore");
        ATH_MSG_INFO("-----------------------------");
}

} // namespace G4UA
