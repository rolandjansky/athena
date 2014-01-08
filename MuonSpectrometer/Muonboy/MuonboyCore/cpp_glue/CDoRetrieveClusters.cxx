/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "GaudiKernel/ISvcLocator.h"
#include "GaudiKernel/IToolSvc.h"
#include "GaudiKernel/Bootstrap.h"
#include "GaudiKernel/MsgStream.h"

#include "StoreGate/StoreGateSvc.h"
#include "TrkRoad/TrackRoadCollection.h"
 
#include <iostream>

extern "C" {
  extern void cdoretrieveclusters_(){
      
    // Retrieve the Cluster Collection from StoreGate
    Trk::TrackRoadCollection* roads = 0;
    ISvcLocator* svcLocator = Gaudi::svcLocator();
    IMessageSvc* msgSvc     = 0;
    StatusCode sc           = svcLocator->service("MessageSvc", msgSvc);
    if (sc.isSuccess() && 0 != msgSvc) {
      MsgStream log(msgSvc, "CDoRetrieveClusters");
      StoreGateSvc* sgSvc = 0;
      StatusCode sc = svcLocator->service("StoreGateSvc", sgSvc);
      if (sc.isSuccess() && 0 != sgSvc) {
////          sc = sgSvc->retrieve(roads, "MuonTrackRoads");
        if ( sc.isSuccess() && 0 != roads) {

          // Push the ROAs retrieved from StoreGate into the F-90 module
          /****************************
          extern void __m_mb_alowcell_MOD_reset_mb_alowcell();
          __m_mb_alowcell_MOD_reset_mb_alowcell();
          int jnum = 0;
          for (Trk::TrackRoadCollection::iterator j=roads->begin(); j != roads->end(); j++) {
            Trk::TrackRoad* a_trk = *j;
            double a_wei          = 1.0;
            MboyTrackRoad* a_mtk  = dynamic_cast<MboyTrackRoad*>(a_trk);
            if ( 0 != a_mtk ) {
              a_wei = a_mtk->weight();
            }
            std::cout << **j << std::endl;
            Trk::GlobalPosition   a_pos(a_trk->globalPosition());
            Trk::GlobalDirection  a_dir(a_trk->globalDirection());
            double                a_deta = a_trk->deltaEta();
            double                a_dphi = a_trk->deltaPhi();
            MboyROA a_roa(a_pos.x(), a_pos.y(), a_pos.z(), a_dir.x(), a_dir.y(), a_dir.z(), a_dphi, a_deta); 
            double a_the = a_roa.Get_ROAThe();
            double a_phi = a_roa.Get_ROAPhi(); 
            double a_r3d = a_roa.Get_ROAR3d();
            extern void __m_mb_alowcell_MOD_buildalowcell(int&, double&, double&, double&, double&);
            __m_mb_alowcell_MOD_buildalowcell(jnum, a_the, a_phi, a_r3d, a_wei);
            jnum++;          
          }
          extern void __m_mb_alowcell_MOD_printalowcell();
          __m_mb_alowcell_MOD_printalowcell();
          ****************************/
        } else {
////          log << MSG::FATAL << "Unable to retrieve MuonTrackRoads collection " << endreq;            
        }
      } else {
        log << MSG::FATAL << "Unable to retrieve StoreGateSvc " << endreq;
      } 
    }     
  }
}

