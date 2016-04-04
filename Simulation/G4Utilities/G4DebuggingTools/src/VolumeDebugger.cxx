/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "VolumeDebugger.h"

#include "G4GDMLParser.hh"
#include "G4VPhysicalVolume.hh"
#include "G4LogicalVolume.hh"
#include "G4LogicalVolumeStore.hh"
#include "G4TransportationManager.hh"
#include "G4Navigator.hh"

#include <iostream>
#include <vector>

VolumeDebugger::VolumeDebugger(const std::string& type, const std::string& name, const IInterface* parent):
  UserActionBase(type, name, parent),m_path("./test_volume.gdml"),m_targetVolume("CALO::CALO"),
  m_res(1000000),m_verbose(true),m_tol(0.000001),m_volumeCheck(false){


  declareProperty("OutputPath",m_path);
  declareProperty("TargetVolume",m_targetVolume);
  declareProperty("Resolution",m_res);
  declareProperty("Tolerance",m_tol);
  declareProperty("Verbose",m_verbose);
  declareProperty("RunGeoTest",m_volumeCheck);

}


// Pull the EMEC
void VolumeDebugger::PullVolumes( G4LogicalVolume* v ) const {
  if (v==0) return;
  std::vector<G4VPhysicalVolume*> pv_to_remove;
  for (int i=0;i<v->GetNoDaughters();++i){
    G4VPhysicalVolume * n_v = v->GetDaughter(i);
    if ( n_v->GetName() == "LArMgr::LAr::EMEC::Pos::InnerWheel" ||
         n_v->GetName() == "LArMgr::LAr::EMEC::Neg::InnerWheel" ||
         n_v->GetName() == "LArMgr::LAr::EMEC::Pos::OuterWheel" ||
         n_v->GetName() == "LArMgr::LAr::EMEC::Neg::OuterWheel" ){
      // This is one to remove
      pv_to_remove.push_back(n_v);
    } else {
      // Recurse
      PullVolumes( n_v->GetLogicalVolume() );
    }
  }
  for (unsigned int j=0;j<pv_to_remove.size();++j){
    v->RemoveDaughter( pv_to_remove[j] );
  }
}


void VolumeDebugger::EndOfRun(const G4Run* )
{

    G4VPhysicalVolume* W = G4TransportationManager::GetTransportationManager()->GetNavigatorForTracking() ->GetWorldVolume();

    // Clear out the EMEC if necessary
    PullVolumes( W->GetLogicalVolume() );

    if(m_targetVolume!=""){
      G4LogicalVolumeStore *lvs = G4LogicalVolumeStore::GetInstance();
      for (unsigned int i=0;i<lvs->size();++i){
        for (int j=0;j<(*lvs)[i]->GetNoDaughters();++j){
          if ( (*lvs)[i]->GetDaughter(j)->GetName().c_str()==m_targetVolume ){
            W = (*lvs)[i]->GetDaughter(j);
            break;
          } // If we found the volume
        } // Loop over PVs in the LV
      } // Loop over volumes
    } // Requested a volume

    ATH_MSG_INFO( "Writing to GDML volume " << W->GetName() << " to path " << m_path );
    G4GDMLParser parser;
    parser.Write(m_path, W, true);

    if(m_volumeCheck){
      ATH_MSG_INFO( "Running overlap test with parameters " << m_res << " " << m_tol << " " << m_verbose );
      W->CheckOverlaps(m_res,m_tol,m_verbose);
    }

}


StatusCode VolumeDebugger::queryInterface(const InterfaceID& riid, void** ppvInterface)
{
  if ( IUserAction::interfaceID().versionMatch(riid) ) {
    *ppvInterface = dynamic_cast<IUserAction*>(this);
    addRef();
  } else {
    // Interface is not directly available : try out a base class
    return UserActionBase::queryInterface(riid, ppvInterface);
  }
  return StatusCode::SUCCESS;
}
