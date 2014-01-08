/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "MuonPatternFinderToolInterfaces/IMuon_CDoMDTROATool.h"
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
extern Trk::TrackRoadCollection* muon_roads;

// Singleton instance
static Muon::IMuon_CDoMDTROATool* s_instance = 0;

static Muon::IMuon_CDoMDTROATool*  instance() {
  if ( 0 == s_instance ) {
    ISvcLocator* svcLocator = Gaudi::svcLocator();
    IMessageSvc* msgSvc = 0;
    StatusCode sc = svcLocator->service("MessageSvc", msgSvc);
    MsgStream log(msgSvc, "CDoMDTROA");
    IToolSvc* toolSvc;
    sc = svcLocator->service("ToolSvc", toolSvc, true);
    if ( sc.isFailure() || toolSvc == 0 ) {
      log << MSG::FATAL << "Unable to retrieve ToolSvc " << endreq;
      return 0;
    }
    sc = toolSvc->retrieveTool("Muon::Muon_CDoMDTROATool", s_instance);
    if ( sc.isFailure() ) {
      log << MSG::ERROR << "Unable to create Muon_CDoMDTROATool" << endreq;
    } else {
      log << MSG::DEBUG << "==> New Muon_CDoMDTROATool created" << endreq;
    }
  } 
  return s_instance;
}

extern "C" {
  extern void cdomdtroa_(){
    Muon::IMuon_CDoMDTROATool* theSingleton = instance();
    if ( 0 != theSingleton ) {
      theSingleton->execute();
 
      // Don't do anything if the global roads collection doesn't exist
      if ( 0 != muon_roads ) {
        ISvcLocator* svcLocator = Gaudi::svcLocator();
        IMessageSvc* msgSvc     = 0;
        StatusCode sc           = svcLocator->service("MessageSvc", msgSvc);
        if (sc.isSuccess() && 0 != msgSvc) {
          MsgStream log(msgSvc, "CDoMDTtoROA");

          // The interim implementation just calls the F-90 subroutine
          extern int __m_mb_alowcell_MOD_nalow;
          int nalow = __m_mb_alowcell_MOD_nalow;
        
          extern void domakeroafrommdt_();
          domakeroafrommdt_(); 

          // Retrieve the ROAs from F-90 and push them into the global TrackRoadCollection
////          std::cout << "***** CDoMDTtoROA: Old NAlow=" << nalow << ", New NAlow=" << __m_mb_alowcell_MOD_nalow << std::endl;   
          if ( __m_mb_alowcell_MOD_nalow > nalow ) {
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
            for ( int i=0; i < nalow; i++ ) {
              p_phi++; p_r3d++; p_the++; p_wei++;              
            }
            for ( int i=nalow; i < __m_mb_alowcell_MOD_nalow; i++ ) {
              log << MSG::DEBUG << "***** i:" << i << ": the=" << *p_the << ", phi=" << *p_phi << ", r3d=" << *p_r3d << ", wei=" << *p_wei << endreq;
              MboyROA a_roa(*p_the, *p_phi, *p_r3d);
              log << MSG::DEBUG << "a_roa pos x:" << a_roa.Get_Point_x() << ", y:" << a_roa.Get_Point_y() << ", z:" << a_roa.Get_Point_z() << " dir x:" << a_roa.Get_Direc_x() << ", y:" << a_roa.Get_Direc_y() << ", z:" << a_roa.Get_Direc_z() << endreq;
              Amg::Vector3D   a_pos(a_roa.Get_Point_x(), a_roa.Get_Point_y(), a_roa.Get_Point_z());
              Amg::Vector3D   a_dir(a_roa.Get_Direc_x(), a_roa.Get_Direc_y(), a_roa.Get_Direc_z());
              Trk::TrackRoad* a_road = new Trk::TrackRoad(a_pos, a_dir, a_roa.Get_DeltEta(), a_roa.Get_DeltPhi(), Trk::TrackRoad::MDT, *p_wei);
              log << MSG::DEBUG << "TrackRoad:" << *a_road << endreq;
              muon_roads->push_back(a_road);
              p_phi++; p_r3d++; p_the++; p_wei++;
            }
          }
          
          // Locate the StoreGateSvc and create the collection if it doesn't already exist. If it
          // does already exist, create a new collection and copy the elements from the old one
          // to the new one and remove the old collection from Storegate. This avoids relocation
          // problems with the collection being moved whilst it's been extended.
          StoreGateSvc* sgSvc = 0;
          sc                  = svcLocator->service("StoreGateSvc", sgSvc);
          if (sc.isSuccess() && 0 != sgSvc) {
            sc = sgSvc->record(muon_roads, "MuonTrackRoads");
            if ( sc.isSuccess() ) {
              log << MSG::DEBUG << "***** CDoMDTtoROA: record MuonTrackRoads successful" << endreq;            
////              log << MSG::INFO << "***** CDoMDTtoROA: record MuonTrackRoads successful" << endreq;            
            } else { 
              log << MSG::ERROR << "Unable to record MuonTrackRoads collection " << endreq;            
            }
          } else {
            log << MSG::ERROR << "Unable to retrieve StoreGateSvc " << endreq;
          } 
        } else {
          std::cout << "***** CDoMDTtoROA: Unable to create MessageSvc" << std::endl;            
        }
      }
    }
  }
}
