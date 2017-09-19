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


#include "GaudiKernel/Bootstrap.h"
#include "GaudiKernel/ISvcLocator.h"
#include "GaudiKernel/IMessageSvc.h"

namespace G4UA{


  VolumeDebugger::VolumeDebugger(const Config& config):AthMessaging(Gaudi::svcLocator()->service< IMessageSvc >( "MessageSvc" ),"VolumeDebugger"),m_config(config){;
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
  
  
  
  void  VolumeDebugger::DumpGeometry() const{
    
    G4VPhysicalVolume* W = G4TransportationManager::GetTransportationManager()->GetNavigatorForTracking() ->GetWorldVolume();
    
    // Clear out the EMEC if necessary
    PullVolumes( W->GetLogicalVolume() );
    
    if(m_config.targetVolume!=""){
      G4LogicalVolumeStore *lvs = G4LogicalVolumeStore::GetInstance();
      for (unsigned int i=0;i<lvs->size();++i){
        for (int j=0;j<(*lvs)[i]->GetNoDaughters();++j){
          if ( (*lvs)[i]->GetDaughter(j)->GetName().c_str()==m_config.targetVolume ){
            W = (*lvs)[i]->GetDaughter(j);
            goto exitLoop;
          } // If we found the volume
        } // Loop over PVs in the LV
      } // Loop over volumes

      // Did not find the volume
      ATH_MSG_FATAL("Did not find the volume named " << m_config.targetVolume << ". Please set parameter TargetVolume to one of:\n\n");
      
      for (unsigned int i = 0; i < lvs->size(); ++i) {
	for (int j = 0; j < (*lvs)[i]->GetNoDaughters(); ++j) {
	  ATH_MSG_FATAL( (*lvs)[i]->GetDaughter(j)->GetName());
	} // Loop over PVs in the LV
      } // Loop over volumes
      ATH_MSG_FATAL("\n\n ================= E N D   O F   L I S T ========================\n\n");
      
      return; // Really no point in doing anything on the entire Atlas volume
      
    } // Requested a volume
    
  exitLoop:
    if (m_config.dumpGDML) {  
      
      ATH_MSG_INFO( "Writing to GDML volume " << W->GetName() << " to path " << m_config.path );
      G4GDMLParser parser;
      parser.Write(m_config.path, W, true);
    }
    if(m_config.volumeCheck){
      ATH_MSG_INFO( "Running overlap test with parameters " << m_config.res << " " << m_config.tol << " " << m_config.verbose );
      
      bool hasOverlaps = recursiveCheck(W);
      if (hasOverlaps) {
	ATH_MSG_INFO("Overlap check: there were problems with the geometry.");
      }
      else {
	ATH_MSG_INFO("Overlap check: All looks good!");
      }
    }

  }

  void VolumeDebugger::BeginOfRunAction(const G4Run*){

    std::call_once(VolumeDebugger_DumpGeometryOnce,&G4UA::VolumeDebugger::DumpGeometry,this);
    
  }
  
  bool VolumeDebugger::recursiveCheck(G4VPhysicalVolume *topPV) const {
    
    bool somethingOverlapped = false;
    bool hasOverlaps = topPV->CheckOverlaps(m_config.res, m_config.tol, m_config.verbose);
    if (hasOverlaps && m_config.verbose) ATH_MSG_ERROR("Volume " << topPV->GetName() << " has overlaps.");    
    somethingOverlapped |= hasOverlaps;
    //
    //    Make a list of PVs keyed by their LVs
    //
    std::multimap<G4LogicalVolume *, G4VPhysicalVolume *> lv2pvMap;
    G4LogicalVolume *lv = topPV->GetLogicalVolume();
    int nDaughters = lv->GetNoDaughters();
    for (int i = 0; i < nDaughters; ++i) {
      G4VPhysicalVolume *daughterPV = lv->GetDaughter(i);
      G4LogicalVolume *daughterLV = daughterPV->GetLogicalVolume();
      lv2pvMap.insert(std::pair<G4LogicalVolume *, G4VPhysicalVolume *>(daughterLV, daughterPV));
    }
    
    for (std::multimap<G4LogicalVolume *, G4VPhysicalVolume *>::iterator mapEl = lv2pvMap.begin(); mapEl != lv2pvMap.end(); ) {
      //
      //  The first of each LV gets checked externally and internally (recursively).
      //
      G4VPhysicalVolume *daughterPV = mapEl->second;
      hasOverlaps = recursiveCheck(daughterPV);
      somethingOverlapped |= hasOverlaps;
      if (hasOverlaps && m_config.verbose) ATH_MSG_ERROR("Volume " << daughterPV->GetName() << " has overlaps.");    
      //
      //  Subsequent PVs with the same LV get only external checks
      //
      std::pair <std::multimap<G4LogicalVolume *, G4VPhysicalVolume *>::iterator, 
	std::multimap<G4LogicalVolume *, G4VPhysicalVolume *>::iterator> range = lv2pvMap.equal_range(mapEl->first);
      ++mapEl;
      //
      // Sometimes there are huge numbers of the same item, in the same juxtaposition with nearby elements.
      // Takes too long to check them all, and in typical geometries it is a waste of time.
      // So we skip some, controlled by m_targetMaxCopiesToCheck
      //
      int n = std::distance(range.first, range.second); // n is total number in this group.
      int checkEveryNth = int(n / m_config.targetMaxCopiesToCheck + 0.5);
      if (checkEveryNth <= 0) checkEveryNth = 1;
      for (int i = 1; i < n; ++i) { // "i = 0" already done 
	if (i % checkEveryNth == 0) {
	  hasOverlaps = mapEl->second->CheckOverlaps(m_config.res, m_config.tol, m_config.verbose);
	  somethingOverlapped |= hasOverlaps;
	  if (hasOverlaps && m_config.verbose) ATH_MSG_ERROR("Volume " << mapEl->second->GetLogicalVolume()->GetName() << " has overlaps.");
	}
	++mapEl;
      }
    }
    return somethingOverlapped;
  }
  
  
} // namespace G4UA 
