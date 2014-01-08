/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "MboyCoreEvt/MboyROA.h"
#include "MboyAthEvt/MboyROAStoreAth.h"
#include "GaudiKernel/ISvcLocator.h"
#include "GaudiKernel/IToolSvc.h"
#include "GaudiKernel/Bootstrap.h"
#include "GaudiKernel/MsgStream.h"

#include "StoreGate/StoreGateSvc.h"

#include "TrkRoad/TrackRoad.h"
#include "TrkRoad/TrackRoadCollection.h"
#include "GeoPrimitives/GeoPrimitives.h"

#include <iostream>

// Global pointer to MuonTrackRoads collection
extern Trk::TrackRoadCollection* muon_roads;

extern "C" {
  extern void cdoretrieveroa_(){
            
    if ( 0 != muon_roads) {

////      std::cout << "***** CDoRetrieveROA: MuonTrackRoads exists" << std::endl;            

      // Push the ROAs retrieved from StoreGate into the F-90 module
      extern void __m_mb_alowcell_MOD_reset_mb_alowcell();
      __m_mb_alowcell_MOD_reset_mb_alowcell();
      int jnum = 0;
      for (Trk::TrackRoadCollection::iterator j=muon_roads->begin(); j != muon_roads->end(); j++) {
        Trk::TrackRoad* a_road = *j;
////        int a_type             = a_road->type();
        double a_wei           = a_road->weight();
////        std::cout << "***** CDoRetrieveROA: road: " << jnum << *a_road << std::endl;
        Amg::Vector3D         a_pos(a_road->globalPosition());
        Amg::Vector3D         a_dir(a_road->globalDirection());
        double                a_deta = a_road->deltaEta();
        double                a_dphi = a_road->deltaPhi();
        MboyROA a_roa(a_pos.x(), a_pos.y(), a_pos.z(), a_dir.x(), a_dir.y(), a_dir.z(), a_dphi, a_deta); 
        double a_the = a_roa.Get_ROAThe();
        double a_phi = a_roa.Get_ROAPhi(); 
        double a_r3d = a_roa.Get_ROAR3d();
        extern void __m_mb_alowcell_MOD_buildalowcell(int&, double&, double&, double&, double&);
        __m_mb_alowcell_MOD_buildalowcell(jnum, a_the, a_phi, a_r3d, a_wei);
        jnum++;          
      }
      extern void __m_mb_alowcell_MOD_printalowcell();
////      __m_mb_alowcell_MOD_printalowcell();
    }
  }
}

