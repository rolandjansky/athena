/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#include "sTGCHitsTestTool.h"

#include "EventInfo/EventInfo.h"
#include "EventInfo/EventID.h"
#include "Identifier/Identifier.h"

#include "GeoAdaptors/GeoMuonHits.h"

#include "MuonReadoutGeometry/MuonDetectorManager.h"
#include "MuonReadoutGeometry/sTgcReadoutElement.h"

#include "MuonIdHelpers/sTgcIdHelper.h"
#include "MuonSimEvent/sTgcHitIdHelper.h"

#include "MuonSimEvent/sTGCSimHitCollection.h"
#include "MuonSimEvent/sTGCSimHit.h"

#include "GeneratorObjects/McEventCollection.h"
#include "CLHEP/Vector/LorentzVector.h"

#include "GaudiKernel/NTuple.h"
#include "GaudiKernel/SmartDataPtr.h"
#include "GaudiKernel/IDataProviderSvc.h"
#include "GaudiKernel/ITHistSvc.h"
#include "GaudiKernel/INTupleSvc.h"

#include "TH2D.h"
#include "TTree.h"
#include "TROOT.h"
#include "TFile.h"
#include "TTree.h"
#include "TF1.h"
#include "TH1F.h"

using namespace MuonGM;
 
 
StatusCode sTGCHitsTestTool::processEvent() {
  CHECK(executeCheckEventInfo());

   if (m_DosTGCTest) {
    const DataHandle<sTGCSimHitCollection> p_collection;
    CHECK(evtStore()->retrieve(p_collection,"sTGCSensitiveDetector"));
    for (sTGCSimHitCollection::const_iterator i_hit = p_collection->begin(); i_hit != p_collection->end(); ++i_hit) { 
      Amg::Vector3D u = (*i_hit).globalPosition();
      CHECK(executeFillHistos(u));
      
      
      sTgcHitIdHelper* hitHelper = sTgcHitIdHelper::GetHelper();	
      int simId = (*i_hit).sTGCId();
      std::string sim_stationName = hitHelper->GetStationName(simId);
      
      static std::string TS11("TS11");
      static std::string TS21("TS21");
      static std::string TS31("TS31");
      static std::string TL11("TL11");
      static std::string TL21("TL21");
      static std::string TL31("TL31");
      
      if (sim_stationName==TS11 && u.z()>0){
		m_sTgc_TransverseView_TS11_posZ->Fill(u.x(),u.y());
	  }
		
	
	  if (sim_stationName==TS21 && u.z()>0){
		m_sTgc_TransverseView_TS21_posZ->Fill(u.x(),u.y());
	  }
		
			
	  if (sim_stationName==TS31 && u.z()>0){
		m_sTgc_TransverseView_TS31_posZ->Fill(u.x(),u.y());
	  }	
		
	  if (sim_stationName==TL11 && u.z()>0){
		m_sTgc_TransverseView_TL11_posZ->Fill(u.x(),u.y());
	  }
		
	
	  if (sim_stationName==TL21 && u.z()>0){
		m_sTgc_TransverseView_TL21_posZ->Fill(u.x(),u.y());
	  }
				
	  if (sim_stationName==TL31 && u.z()>0){
		m_sTgc_TransverseView_TL31_posZ->Fill(u.x(),u.y());
	  }
		
	  double r_sTGc = sqrt(u.x()*u.x()+u.y()*u.y());
		
	  if (u.z() > 0){
		m_sTgc_rZview_positiveZ->Fill(u.z(), r_sTGc);	
	  }	
  

      // GeoMMHit ghit(*i_hit);
//       if (!ghit) continue;
//       Amg::Vector3D u = ghit.getGlobalPosition();
//       CHECK(executeFillHistos(u));
    }
  }

  return StatusCode::SUCCESS;
}


StatusCode sTGCHitsTestTool::initialize() {
  CHECK(MuonHitTestToolBase::initialize());
  _TH2D( m_sTgc_TransverseView_TS11_posZ,"sTGC_TransverseView_TS11_posZ",1200,-6000.,6000.,1200,-6000.,6000.);
  _TH2D( m_sTgc_TransverseView_TS21_posZ,"sTGC_TransverseView_TS21_posZ",1200,-6000.,6000.,1200,-6000.,6000.);
  _TH2D( m_sTgc_TransverseView_TS31_posZ,"sTGC_TransverseView_TS31_posZ",1200,-6000.,6000.,1200,-6000.,6000.);
   
  _TH2D( m_sTgc_TransverseView_TL11_posZ,"sTGC_TransverseView_TL11_posZ",1200,-6000.,6000.,1200,-6000.,6000.);
  _TH2D( m_sTgc_TransverseView_TL21_posZ,"sTGC_TransverseView_TL21_posZ",1200,-6000.,6000.,1200,-6000.,6000.);
  _TH2D( m_sTgc_TransverseView_TL31_posZ,"sTGC_TransverseView_TL31_posZ",1200,-6000.,6000.,1200,-6000.,6000.);
 	
  _TH2D(m_sTgc_rZview_positiveZ,"sTGC_rZView_posZ",2000,6500.,8500.,5000,0.,5000.);
  return StatusCode::SUCCESS;
}
