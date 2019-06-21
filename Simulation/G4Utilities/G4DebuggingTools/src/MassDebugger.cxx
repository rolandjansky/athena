/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#include "MassDebugger.h"
#include <map>
#include <string>
#include <iomanip>
#include <fstream>

#include "G4LogicalVolume.hh"
#include "G4LogicalVolumeStore.hh"
#include "G4VPhysicalVolume.hh"
#include "G4PhysicalVolumeStore.hh"

#include "G4VSolid.hh"
#include "G4Material.hh"
#include "G4Box.hh"
#include "G4Tubs.hh"

#include "StoreGate/StoreGateSvc.h"
#include "GaudiKernel/Bootstrap.h"
#include "GaudiKernel/ISvcLocator.h"
#include "GaudiKernel/IMessageSvc.h"

using namespace std;

namespace G4UA {
MassDebugger::MassDebugger(const Config &config) : 
    AthMessaging(Gaudi::svcLocator()->service< IMessageSvc >("MessageSvc"), "MassDebugger"), m_config(config) {}

void MassDebugger::BeginOfRunAction(const G4Run *) {
  //ServiceHandle<IMessageSvc> msgh("MessageSvc", "MassDebugger");
  //MsgStream log(&(*msgh), "MassDebugger");
//    map<string, G4LogicalVolume *> logvolMap;
//    map<string, G4LogicalVolume *>::iterator mapElement;
//    string name("");
//
//    ServiceHandle<IMessageSvc> msgh("MessageSvc", "MassDebugger");
//    MsgStream log(&(*msgh), "MassDebugger");
//    ATH_MSG_INFO << "===================  MassDebugger  ========================\n";
////
////    Build up a map with one entry per logvol (G4 list has each many times). Maps automatically refuse to add duplicates.
////
//    G4LogicalVolumeStore *lvs = G4LogicalVolumeStore::GetInstance();
//    for (unsigned int i = 0; i < lvs->size(); ++i) {
//        logvolMap[(*lvs)[i]->GetName()] = (*lvs)[i];
//    }
////
////    Print out information on each volume requested
////
//    log << "\n\n";
//    log << "Logvol                   Volume         Material                      Density        Mass\n" << 
//           "[name]                   [cm3]          [name]                        [g/cm3]        [g]\n\n";
//    for (vector<string>::const_iterator namePtr = m_config.targetLogicalVolume.begin(); 
//                                        namePtr != m_config.targetLogicalVolume.end(); ++namePtr) {
//        name = *namePtr;
//        mapElement = logvolMap.find(name);
//        if (mapElement != logvolMap.end()) {
//
//            G4LogicalVolume* lv = mapElement->second;
//
//            G4VSolid *solid = lv->GetSolid();
//            double volume = solid->GetCubicVolume() / CLHEP::cm3;
//
//            G4Material *mat = lv->GetMaterial();
//            string material = mat->GetName();
//            double density = mat->GetDensity() / (CLHEP::g / CLHEP::cm3);
//
//            double mass = lv->GetMass() / CLHEP::g;
//
//            log << left << setw(25) << name << setw(15) << volume << setw(30) << mat->GetName() << 
//                   setw(15) << density << setw(15) << mass << "\n";
//        }
//        else {
//            log << MSG::FATAL << "Did not find the volume named " << name <<
//                   ". Please set parameter TargetLogicalVolume to a list taken from the following:\n\n";
//            for (mapElement = logvolMap.begin(); mapElement != logvolMap.end(); ++mapElement) {
//                log << mapElement->first << "\n";
//            }
//            log << "================= E N D   O F   L I S T ========================\n\n";
//        }
//    }
//    log << endmsg;
//    log << "======================= End of MassDebugger output =======================\n\n" << endmsg;

    // Added by Xuan

    bool Verbose = m_config.Verbose;
    std::string OutputFile = m_config.OutputFile;
    ATH_MSG_INFO ("===================  MassDebugger  ========================\n");
    ATH_MSG_INFO ("Begin looping over all volumes" );
    std::map<std::string, std::vector<G4LogicalVolume*>> mLogiVols;
    std::map<std::string, std::vector<G4VPhysicalVolume*>> mPhysVols;

    G4PhysicalVolumeStore *pvs = G4PhysicalVolumeStore::GetInstance();
    G4LogicalVolumeStore  *lvs = G4LogicalVolumeStore::GetInstance();

    long nPhysVols = pvs->size();
    long nLogiVols = lvs->size();

    ofstream out_phys(OutputFile.c_str(), ios::out);

    for (long index = 0; index < nLogiVols; ++index)
    {
        G4LogicalVolume* mVolume = lvs->at(index);
        std::string name = mVolume->GetName();
	//        if (name.find("Pixel::") == std::string::npos) continue;
        mLogiVols[name].push_back(mVolume);
    }
    for (long index = 0; index < nPhysVols; ++index)
    {
        G4VPhysicalVolume *mVolume = pvs->at(index);
        std::string name = mVolume->GetName();
        //if (mVolume->GetLogicalVolume()->GetName().find("Pixel::") == std::string::npos) continue;
        mPhysVols[name].push_back(mVolume);
    }

    ATH_MSG_INFO ( "Total Physical volumes:" << nPhysVols);
    long ipix = 0;
out_phys << "name" << "\t" << "volume" << "\t" << "lv_name" << "\t" << "lv_mother_name" << "\t" 
                     << "tranx" << "\t" << "trany" << "\t" << "tranz" << "\t" 
                     << "shape" << "\t" << "material" << "\t" << "mass" << "\t" << "volume" << "\t" << "density" << "\t" 
                     << "par1" << "\t" << "par2" << "\t" << "par3" << "\t" << "index_lv" << "\t" << "index_lv_mother" << endl;
    for (const auto &physicalVolume : mPhysVols)
    {
      if (ipix%2000 == 0) ATH_MSG_INFO( "Processing " << ipix << "/" << nPhysVols );
        std::string name = physicalVolume.first;
        const vector<G4VPhysicalVolume*> volumes = physicalVolume.second;

        int nVolumes = volumes.size();
        if (Verbose) ATH_MSG_INFO("Physical Volume: " << name << " : " << nVolumes );
	
        for (int i = 0; i < nVolumes; ++i)
        {
            G4VPhysicalVolume *pv = volumes.at(i);
            G4LogicalVolume   *lv = pv->GetLogicalVolume();
            G4LogicalVolume   *lv_mother = pv->GetMotherLogical();
            const G4ThreeVector     &trans = pv->GetTranslation();

            string lv_name = "None";
            string lv_mother_name = "None";

            if (lv) lv_name = lv->GetName();
            if (lv_mother) lv_mother_name = lv_mother->GetName();

            G4VSolid *solid = lv->GetSolid();
            double volume = solid->GetCubicVolume() / CLHEP::cm3;
            string shape = solid->GetEntityType();

            G4Material *mat = lv->GetMaterial();
            string material = mat->GetName();
            double density = mat->GetDensity() / (CLHEP::g / CLHEP::cm3);
            double mass = lv->GetMass() / CLHEP::g;

            double par1 = -1.0;
            double par2 = -1.0;
            double par3 = -1.0;

            if (shape == "G4Box")
            {
                G4Box *box = (G4Box*)solid;
                par1 = box->GetXHalfLength() * 2.0;
                par2 = box->GetYHalfLength() * 2.0;
                par3 = box->GetZHalfLength() * 2.0;
            }
            else if (shape == "G4Tubs")
            {
                G4Tubs *tubs = (G4Tubs*)solid;
                par1 = tubs->GetInnerRadius();
                par2 = tubs->GetOuterRadius();
                par3 = tubs->GetZHalfLength() * 2.0;
            }

            auto ite_lv = find(mLogiVols[lv_name].begin(), mLogiVols[lv_name].end(), lv);
            auto ite_lv_mother = find(mLogiVols[lv_mother_name].begin(), mLogiVols[lv_mother_name].end(), lv_mother);

            int index_lv = -1;
            int index_lv_mother = -1;

            if (ite_lv != mLogiVols[lv_name].end())
            {
                index_lv = distance(mLogiVols[lv_name].begin(), ite_lv);  
            }
            if (ite_lv_mother != mLogiVols[lv_mother_name].end())
            {
                index_lv_mother = distance(mLogiVols[lv_mother_name].begin(), ite_lv_mother);
            }

            double tranx = trans.getX();
            double trany = trans.getY();
            double tranz = trans.getZ();

            out_phys << name << "\t" << i << "\t" << lv_name << "\t" << lv_mother_name << "\t" 
                     << tranx << "\t" << trany << "\t" << tranz << "\t" 
                     << shape << "\t" << material << "\t" << mass << "\t" << volume << "\t" << density << "\t" 
                     << par1 << "\t" << par2 << "\t" << par3 << "\t" << index_lv << "\t" << index_lv_mother << endl;
        }
        ipix++;
    }

    out_phys.close();

    ATH_MSG_INFO("Loop ends successfully");
    ATH_MSG_INFO("======================= End of MassDebugger output =======================\n\n" );
}

}// namespace G4UA
