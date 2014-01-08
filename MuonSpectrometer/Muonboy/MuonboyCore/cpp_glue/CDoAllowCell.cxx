/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "MuonPatternFinderToolInterfaces/IMuon_CDoAllowCellTool.h"
#include "GaudiKernel/ISvcLocator.h"
#include "GaudiKernel/IToolSvc.h"
#include "GaudiKernel/Bootstrap.h"
#include "GaudiKernel/MsgStream.h"

#include "StoreGate/StoreGateSvc.h"

#include "TrkRoad/TrackRoad.h"
#include "TrkRoad/TrackRoadCollection.h"
#include "GeoPrimitives/GeoPrimitives.h"
#include "MboyCoreEvt/MboyROA.h"
 
#include <iostream>

// Global pointer to MuonTrackRoads collection
Trk::TrackRoadCollection* muon_roads;

// Singleton instance
static Muon::IMuon_CDoAllowCellTool* s_instance = 0;

static Muon::IMuon_CDoAllowCellTool*  
instance() {
  if ( 0 == s_instance ) {
    ISvcLocator* svcLocator = Gaudi::svcLocator();
    IMessageSvc* msgSvc     = 0;
    StatusCode sc           = svcLocator->service("MessageSvc", msgSvc);
    if (sc.isSuccess() && 0 != msgSvc) {
      MsgStream log(msgSvc, "CDoAllowCell");
      IToolSvc* toolSvc;
      sc = svcLocator->service("ToolSvc", toolSvc, true);
      if ( sc.isFailure() || toolSvc == 0 ) {
        log << MSG::FATAL << "Unable to retrieve ToolSvc " << endreq;
        return 0;
      }
      sc = toolSvc->retrieveTool("Muon::Muon_CDoAllowCellTool", s_instance);
      if ( sc.isFailure() ) {
        log << MSG::FATAL << "Unable to create Muon_CDoAllowCellTool" << endreq;
      } else {
        log << MSG::INFO << "==> New Muon_CDoAllowCellTool created" << endreq;
      }
    }      
  } 
  return s_instance;
}

extern "C" {
  extern void cdoallowcell_(int& nroa, double* teroa, double* firoa, double* dteroa, double* dfiroa){
    Muon::IMuon_CDoAllowCellTool* theSingleton = instance();
    if ( 0 != theSingleton ) {
      theSingleton->execute();

      muon_roads              = 0;
      ISvcLocator* svcLocator = Gaudi::svcLocator();
      IMessageSvc* msgSvc     = 0;
      StatusCode sc           = svcLocator->service("MessageSvc", msgSvc);
      if (sc.isSuccess() && 0 != msgSvc) {
        MsgStream log(msgSvc, "CDoAllowCell");
        
        // Don't do anything if the "MuonTrackRoads" collection already exists since this
        // implies that this function is being called multiple times within the event.
        StoreGateSvc* sgSvc = 0;
        sc                  = svcLocator->service("StoreGateSvc", sgSvc);
        if (sc.isSuccess() && 0 != sgSvc) {
          const Trk::TrackRoadCollection* roads = sgSvc->tryConstRetrieve<Trk::TrackRoadCollection>("MuonTrackRoads");
          if ( 0 == roads ) {
      
            // The interim implementation just calls the F-90 subroutine
            extern void doallowcell_(int&, double*, double*, double*, double*);
            doallowcell_(nroa, teroa, firoa, dteroa, dfiroa); 
      
            // Retrieve the ROAs from F-90 and push them into a TrackRoadCollection
            int nalow;
            extern int __m_mb_alowcell_MOD_nalow;
            nalow = __m_mb_alowcell_MOD_nalow;
            extern double* __m_mb_alowcell_MOD_alowthe;
            extern double* __m_mb_alowcell_MOD_alowphi;
            extern double* __m_mb_alowcell_MOD_alowr3d;
            extern double* __m_mb_alowcell_MOD_alowwei;
            double* p_the;
            double* p_phi;
            double* p_r3d;
            double* p_wei;
            p_the = __m_mb_alowcell_MOD_alowthe;
            p_phi = __m_mb_alowcell_MOD_alowphi;
            p_r3d = __m_mb_alowcell_MOD_alowr3d;
            p_wei = __m_mb_alowcell_MOD_alowwei;
            muon_roads = new Trk::TrackRoadCollection();
            for ( int i=0; i < nalow; i++ ) {
              log << MSG::DEBUG << "***** i:" << i << ": the=" << *p_the << ", phi=" << *p_phi << ", r3d=" << *p_r3d << ", wei=" << *p_wei << endreq;
              MboyROA a_roa(*p_the, *p_phi, *p_r3d);
              log << MSG::DEBUG << "a_roa pos x:" << a_roa.Get_Point_x() << ", y:" << a_roa.Get_Point_y() << ", z:" << a_roa.Get_Point_z() << " dir x:" << a_roa.Get_Direc_x() << ", y:" << a_roa.Get_Direc_y() << ", z:" << a_roa.Get_Direc_z() << endreq;
              Amg::Vector3D   a_pos(a_roa.Get_Point_x(), a_roa.Get_Point_y(), a_roa.Get_Point_z());
              Amg::Vector3D   a_dir(a_roa.Get_Direc_x(), a_roa.Get_Direc_y(), a_roa.Get_Direc_z());
              Trk::TrackRoad* a_road = new Trk::TrackRoad(a_pos, a_dir, a_roa.Get_DeltEta(), a_roa.Get_DeltPhi(), Trk::TrackRoad::RPCTGC, *p_wei);
              log << MSG::DEBUG << "TrackRoad:" << *a_road << endreq;
              muon_roads->push_back(a_road);
              p_phi++; p_r3d++; p_the++; p_wei++;
            }
          }
        } else {
          log << MSG::ERROR << "Unable to locate StoreGateSvc " << endreq;                      
        }
      } else {
        std::cout << "***** CDoAllowCell: Unable to create MessageSvc" << std::endl;            
      }     
    }
  }
}

