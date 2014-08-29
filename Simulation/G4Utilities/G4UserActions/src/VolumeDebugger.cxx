/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "G4UserActions/VolumeDebugger.h"
#include <iostream>
#include <vector>

#include "G4GDMLParser.hh"
#include "G4VPhysicalVolume.hh"
#include "G4LogicalVolume.hh"
#include "G4LogicalVolumeStore.hh"
#include "G4TransportationManager.hh"
#include "G4Navigator.hh"

static VolumeDebugger ts1("VolumeDebugger");

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

// End of event to allow any lazy init to have taken place
void VolumeDebugger::EndOfEventAction(const G4Event* /*anEvent*/)
{
  if (!m_init){
    std::string path = "/tmp/";
    if (getenv("USER")) path += getenv( "USER" );
    path += "/atlas.gdml";
    if(theProperties.find("outputPath")!=theProperties.end()){
      path = theProperties["outputPath"];
    } // Requested an output path

    G4VPhysicalVolume* W = G4TransportationManager::GetTransportationManager()->GetNavigatorForTracking() ->GetWorldVolume();

    // Clear out the EMEC if necessary
    PullVolumes( W->GetLogicalVolume() );

    if(theProperties.find("targetVolume")!=theProperties.end()){
      G4LogicalVolumeStore *lvs = G4LogicalVolumeStore::GetInstance();
      for (unsigned int i=0;i<lvs->size();++i){
        for (int j=0;j<(*lvs)[i]->GetNoDaughters();++j){
          if ( (*lvs)[i]->GetDaughter(j)->GetName().c_str()==theProperties["targetVolume"] ){
            W = (*lvs)[i]->GetDaughter(j);
            break;
          } // If we found the volume
        } // Loop over PVs in the LV
      } // Loop over volumes
    } // Requested a volume

    log() << MSG::INFO << "Writing to GDML volume " << W << " to path " << path << endreq;
    G4GDMLParser parser;
    parser.Write(path, W, true);

    // Now go through the volume checking...
    int res = 10000;
    bool verbose = true;
    float tol = 0.0000001;

    if (theProperties.find("resolution")!=theProperties.end()){
      res = strtol(theProperties["resolution"].c_str(),0,0);
    }
    if (theProperties.find("tolerance")!=theProperties.end()){
      tol = strtod(theProperties["tolerance"].c_str(),0);
    }
    if (theProperties.find("verbose")!=theProperties.end()){
      verbose = strtol(theProperties["verbose"].c_str(),0,0);
    }

    log() << MSG::INFO << "Running overlap test with parameters " << res << " " << tol << " " << verbose << endreq;
    W->CheckOverlaps(res,tol,verbose);

    m_init=true;
  }
}

