/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "G4UserActions/GDMLDumper.h"
#include <iostream>
#include <vector>

#include "G4GDMLParser.hh"
#include "G4VPhysicalVolume.hh"
#include "G4LogicalVolume.hh"

#include "G4TransportationManager.hh"
#include "G4Navigator.hh"

static GDMLDumper ts1("GDMLDumper");

void GDMLDumper::PullVolumes( G4LogicalVolume* v ) const {
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
void GDMLDumper::EndOfEventAction(const G4Event* /*anEvent*/)
{
  if (!m_init){
    G4GDMLParser parser;

    G4VPhysicalVolume* W = G4TransportationManager::GetTransportationManager()->GetNavigatorForTracking() ->GetWorldVolume();

    // Remove the EMEC from the volume tree, in case it's there
    PullVolumes( W->GetLogicalVolume() );
    std::string path = "/tmp/";
    if ( getenv("USER") ) path += getenv( "USER" );
    else                  path  = ".";
    path += "/atlas.gdml";

    log() << MSG::INFO << "Writing results to " << path << endreq;

    parser.Write(path, W, true);

    m_init=true;
  }
}

