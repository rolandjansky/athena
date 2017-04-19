/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "MassDebugger.h"
#include <map>
#include <string>
#include <iomanip>

#include "G4LogicalVolume.hh"
#include "G4LogicalVolumeStore.hh"

#include "G4VSolid.hh"
#include "G4Material.hh"
#include "G4Box.hh"

#include "StoreGate/StoreGateSvc.h"
#include "GaudiKernel/Bootstrap.h"
#include "GaudiKernel/ISvcLocator.h"
#include "GaudiKernel/IMessageSvc.h"

using namespace std;

namespace G4UA {
MassDebugger::MassDebugger(const Config &config) : 
    AthMessaging(Gaudi::svcLocator()->service< IMessageSvc >("MessageSvc"), "MassDebugger"), m_config(config) {}

void MassDebugger::beginOfRun(const G4Run *) {
    map<string, G4LogicalVolume *> logvolMap;
    map<string, G4LogicalVolume *>::iterator mapElement;
    string name("");

    ServiceHandle<IMessageSvc> msgh("MessageSvc", "MassDebugger");
    MsgStream log(&(*msgh), "MassDebugger");
    log << MSG::INFO << "===================  MassDebugger  ========================\n";
//
//    Build up a map with one entry per logvol (G4 list has each many times). Maps automatically refuse to add duplicates.
//
    G4LogicalVolumeStore *lvs = G4LogicalVolumeStore::GetInstance();
    for (unsigned int i = 0; i < lvs->size(); ++i) {
        logvolMap[(*lvs)[i]->GetName()] = (*lvs)[i];
    }
//
//    Print out information on each volume requested
//
    log << "\n\n";
    log << "Logvol                   Volume         Material                      Density        Mass\n" << 
           "[name]                   [cm3]          [name]                        [g/cm3]        [g]\n\n";
    for (vector<string>::const_iterator namePtr = m_config.targetLogicalVolume.begin(); 
                                        namePtr != m_config.targetLogicalVolume.end(); ++namePtr) {
        name = *namePtr;
        mapElement = logvolMap.find(name);
        if (mapElement != logvolMap.end()) {

            G4LogicalVolume* lv = mapElement->second;

            G4VSolid *solid = lv->GetSolid();
            double volume = solid->GetCubicVolume() / CLHEP::cm3;

            G4Material *mat = lv->GetMaterial();
            string material = mat->GetName();
            double density = mat->GetDensity() / (CLHEP::g / CLHEP::cm3);

            double mass = lv->GetMass() / CLHEP::g;

            log << left << setw(25) << name << setw(15) << volume << setw(30) << mat->GetName() << 
                   setw(15) << density << setw(15) << mass << "\n";
        }
        else {
            log << MSG::FATAL << "Did not find the volume named " << name <<
                   ". Please set parameter TargetLogicalVolume to a list taken from the following:\n\n";
            for (mapElement = logvolMap.begin(); mapElement != logvolMap.end(); ++mapElement) {
                log << mapElement->first << "\n";
            }
            log << "================= E N D   O F   L I S T ========================\n\n";
        }
    }
    log << endmsg;
    log << "======================= End of MassDebugger output =======================\n\n" << endmsg;
}

} // namespace G4UA
