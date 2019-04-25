/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#include "MMHitsTestTool.h"

#include "EventInfo/EventInfo.h"
#include "EventInfo/EventID.h"
#include "Identifier/Identifier.h"

#include "GeoAdaptors/GeoMuonHits.h"

#include "MuonReadoutGeometry/MuonDetectorManager.h"
#include "MuonReadoutGeometry/MMReadoutElement.h"

#include "MuonIdHelpers/MmIdHelper.h"
#include "MuonSimEvent/MicromegasHitIdHelper.h"

#include "MuonSimEvent/MMSimHitCollection.h"
#include "MuonSimEvent/MMSimHit.h"

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

 
 
StatusCode MMHitsTestTool::processEvent() {
  CHECK(executeCheckEventInfo());

  if (m_DoMMTest) {
    const DataHandle<MMSimHitCollection> p_collection;
    CHECK(evtStore()->retrieve(p_collection,"MicromegasSensitiveDetector"));
    for (MMSimHitCollection::const_iterator i_hit = p_collection->begin(); i_hit != p_collection->end(); ++i_hit) {
      Amg::Vector3D u = (*i_hit).globalPosition();
      CHECK(executeFillHistos(u));
      //Useful link on how to retrieve variables: http://acode-browser.usatlas.bnl.gov/lxr/source/atlas/MuonSpectrometer/MuonValidation/MuonPRDTest/src/MMSimHitVariables.cxx
      //Get station names and make plots for each wedge
      MicromegasHitIdHelper* hitHelper = MicromegasHitIdHelper::GetHelper();	
      int simId = (*i_hit).MMId();
      std::string sim_stationName = hitHelper->GetStationName(simId);
      //Declare station name strings
      static std::string s_m1s1("M1S1");
      static std::string s_m2s1("M2S1");
      static std::string s_m1l1("M1L1");
      static std::string s_m2l1("M2L1");
      static std::string s_m1s2("M1S2");
      static std::string s_m2s2("M2S2");
      static std::string s_m1l2("M1L2");
      static std::string s_m2l2("M2L2");
      
      //----------------------------------Wedge 1 Histos begin-------------------------------------------------------------------------
      //M1S1 (Note: M1->Module 1, S1->Small sector, wedge 1)
	  if (sim_stationName==s_m1s1 && u.z()>0){
	  	m_MMTransverseEta1SmallWedge1->Fill(u.x(),u.y());
	  }
	  
	  //M2S1 (Note: M2->Module 2, S1->Small sector, wedge 1)	
	  if (sim_stationName==s_m2s1 && u.z()>0){
		m_MMTransverseEta2SmallWedge1->Fill(u.x(),u.y());
	  }
		
	  //M1L1 (Note: M1->Module 1, S1->Large sector, wedge 1)	
	  if (sim_stationName==s_m1l1 && u.z()>0){
		m_MMTransverseEta1LargeWedge1->Fill(u.x(),u.y());
	  }	
		
	  //M2L1 (Note: M2->Module 2, S1->Large sector, wedge 1.)
	  if (sim_stationName==s_m2l1 && u.z()>0){
		m_MMTransverseEta2LargeWedge1->Fill(u.x(),u.y());
	  }	
		
	  //Plots of transverse and rZ view for wedge 1 in either +ve Z or -ve Z and done separately for large and small sectors
	  double rwedge1 = sqrt(u.x()*u.x()+u.y()*u.y()); //Evaluate r
	  
	  //Small sectors at +ve Z
	  if ((sim_stationName==s_m1s1 || sim_stationName==s_m2s1) && u.z() > 0){
	  	m_MM_SmallWedge1_TransverseView_positiveZ->Fill(u.x(),u.y());
		m_MM_SmallWedge1_rZview_positiveZ->Fill(u.z(), rwedge1);
		m_MM_rPlot_S1_posZ->Fill(rwedge1);
	  }	
		
	  //Small sectors at -ve Z
	  if ((sim_stationName==s_m1s1 || sim_stationName==s_m2s1) && u.z() < 0){
		m_MM_SmallWedge1_TransverseView_negativeZ->Fill(u.x(),u.y());	
		m_MM_SmallWedge1_rZview_negativeZ->Fill(u.z(), rwedge1);
	  }
		
	  //Large sectors at +ve Z
	  if ((sim_stationName==s_m1l1 || sim_stationName==s_m2l1) && u.z() > 0){
		m_MM_LargeWedge1_TransverseView_positiveZ->Fill(u.x(),u.y());
		m_MM_LargeWedge1_rZview_positiveZ->Fill(u.z(), rwedge1);
		m_MM_rPlot_L1_posZ->Fill(rwedge1);
	  }	
		
	  //Large sectors at -ve Z
	  if ((sim_stationName==s_m1l1 || sim_stationName==s_m2l1) && u.z() < 0){
		m_MM_LargeWedge1_TransverseView_negativeZ->Fill(u.x(),u.y());
		m_MM_LargeWedge1_rZview_negativeZ->Fill(u.z(), rwedge1);	
	  }
      //----------------------------------Wedge 1 Histos end-------------------------------------------------------------------------
      
      //----------------------------------Wedge 2 Histos begin---------------------------------------------------------------------
      //M1S2 (Note: M1->Module 1, S2->Small sector, wedge 2)
	  if (sim_stationName==s_m1s2 && u.z()>0){
		m_MMTransverseEta1SmallWedge2->Fill(u.x(),u.y());
	  }	
		
	  //M2S2 (Note: M1->Module 2, S2->Small sector, wedge 2)
	  if (sim_stationName==s_m2s2 && u.z()>0){
		m_MMTransverseEta2SmallWedge2->Fill(u.x(),u.y());
	  }
		
	  //M1L2 (Note: M1->Module 1, L2->Large sector, wedge 2)	
	  if (sim_stationName==s_m1l2 && u.z()>0){
		m_MMTransverseEta1LargeWedge2->Fill(u.x(),u.y());
	  }	
		
	  //M2L2 (Note: M2->Module 2, L2->Large sector, wedge 2)
	  if (sim_stationName==s_m2l2 && u.z()>0){
		m_MMTransverseEta2LargeWedge2->Fill(u.x(),u.y());
	  }	
		
	  //Plots of transverse and rZ view for wedge 1 in either +ve Z or -ve Z and done separately for large and small sectors
	  double rwedge2 = sqrt(u.x()*u.x()+u.y()*u.y()); //Evaluate r
		
	  //Small sectors at +ve Z
	  if ((sim_stationName==s_m1s2 || sim_stationName==s_m2s2) && u.z() > 0){
		m_MM_SmallWedge2_TransverseView_positiveZ->Fill(u.x(),u.y());
		m_MM_SmallWedge2_rZview_positiveZ->Fill(u.z(), rwedge2);
		m_MM_rPlot_S2_posZ->Fill(rwedge2);
	  }
		
	  //Small sectors at -ve Z	
	  if ((sim_stationName==s_m1s2 || sim_stationName==s_m2s2) && u.z() < 0){
		m_MM_SmallWedge2_TransverseView_negativeZ->Fill(u.x(),u.y());	
		m_MM_SmallWedge2_rZview_negativeZ->Fill(u.z(), rwedge2);
	  }
		
	  //Large sectors at +ve Z
	  if ((sim_stationName==s_m1l2 || sim_stationName==s_m2l2) && u.z() > 0){
		m_MM_LargeWedge2_TransverseView_positiveZ->Fill(u.x(),u.y());
		m_MM_LargeWedge2_rZview_positiveZ->Fill(u.z(), rwedge2);
		m_MM_rPlot_L2_posZ->Fill(rwedge2);
	  }	
		
	  //Large sectors at -ve Z
	  if ((sim_stationName==s_m1l2 || sim_stationName==s_m2l2) && u.z() < 0){
		m_MM_LargeWedge2_TransverseView_negativeZ->Fill(u.x(),u.y());
		m_MM_LargeWedge2_rZview_negativeZ->Fill(u.z(), rwedge2);	
	  }
	//-----------------------------------------------Wedge 2 Histos end-----------------------------------------------------------		
      
      
      // GeoMMHit ghit(*i_hit);
//       if (!ghit) continue;
//       Amg::Vector3D u = ghit.getGlobalPosition();
//       CHECK(executeFillHistos(u));
    }
  }

  return StatusCode::SUCCESS;
}


StatusCode MMHitsTestTool::initialize() {
  CHECK(MuonHitTestToolBase::initialize());
  
  //-------------------------------Wedge 1 begin-------------------------------------------------------------------------------
  _TH1D(m_MM_rPlot_S1_posZ,"MM_rPlot_S1_posZ",10000,0.,14000.);
  _TH1D(m_MM_rPlot_L1_posZ,"MM_rPlot_L1_posZ",5000,0.,5000.);
  	
  _TH2D(m_MM_SmallWedge1_rZview_positiveZ,"MM_rZView_S1_posZ",1000,7000.,8000.,5000,0.,5000.);
  _TH2D(m_MM_SmallWedge1_rZview_negativeZ,"MM_rZView_S1_negZ",1000,-8000.,-7000.,5000,0.,5000.);
  _TH2D(m_MM_LargeWedge1_rZview_positiveZ,"MM_rZView_L1_posZ",1000,7000.,8000.,5000,0.,5000.);
  _TH2D(m_MM_LargeWedge1_rZview_negativeZ,"MM_rZView_L1_negZ",1000,-8000.,-7000.,5000,0.,5000.);
  
  _TH2D(m_MMTransverseEta1SmallWedge1,"MM_TransverseView_M1S1_posZ",1200,-6000.,6000.,1200,-6000.,6000.);	
  _TH2D(m_MMTransverseEta2SmallWedge1,"MM_TransverseView_M2S1_posZ",1200,-6000.,6000.,1200,-6000.,6000.);
  _TH2D(m_MMTransverseEta1LargeWedge1,"MM_TransverseView_M1L1_posZ",1200,-6000.,6000.,1200,-6000.,6000.);
  _TH2D(m_MMTransverseEta2LargeWedge1,"MM_TransverseView_M2L1_posZ",1200,-6000.,6000.,1200,-6000.,6000.);
  
  _TH2D(m_MM_SmallWedge1_TransverseView_positiveZ,"MM_TransverseView_S1_posZ",1200,-6000.,6000.,1200,-6000.,6000.);
  _TH2D(m_MM_SmallWedge1_TransverseView_negativeZ,"MM_TransverseView_S1_negZ",1200,-6000.,6000.,1200,-6000.,6000.);
  _TH2D(m_MM_LargeWedge1_TransverseView_positiveZ,"MM_TransverseView_L1_posZ",1200,-6000.,6000.,1200,-6000.,6000.);
  _TH2D(m_MM_LargeWedge1_TransverseView_negativeZ,"MM_TransverseView_L1_negZ",1200,-6000.,6000.,1200,-6000.,6000.);
  //--------------------------------Wedge 1 end--------------------------------------------------------------------------------
  	
  //--------------------------------Wedge 2 begin------------------------------------------------------------------------------
  _TH1D(m_MM_rPlot_S2_posZ,"MM_rPlot_S2_posZ",10000,0.,14000.);
  _TH1D(m_MM_rPlot_L2_posZ,"MM_rPlot_L2_posZ",10000,0.,14000.);
  	
  _TH2D(m_MM_SmallWedge2_rZview_positiveZ,"MM_rZView_S2_posZ",1000,7000.,8000.,5000,0.,5000.);
  _TH2D(m_MM_SmallWedge2_rZview_negativeZ,"MM_rZView_S2_negZ",1000,-8000.,-7000.,5000,0.,5000.);
  _TH2D(m_MM_LargeWedge2_rZview_positiveZ,"MM_rZView_L2_posZ",1000,7000.,8000.,5000,0.,5000.);
  _TH2D(m_MM_LargeWedge2_rZview_negativeZ,"MM_rZView_L2_negZ",1000,-8000.,-7000.,5000,0.,5000.);
  	
  _TH2D(m_MMTransverseEta1SmallWedge2,"MM_TransverseView_M1S2_posZ",1200,-6000.,6000.,1200,-6000.,6000.);
  _TH2D(m_MMTransverseEta2SmallWedge2,"MM_TransverseView_M2S2_posZ",1200,-6000.,6000.,1200,-6000.,6000.);
  _TH2D(m_MMTransverseEta1LargeWedge2,"MM_TransverseView_M1L2_posZ",1200,-6000.,6000.,1200,-6000.,6000.);
  _TH2D(m_MMTransverseEta2LargeWedge2,"MM_TransverseView_M2L2_posZ",1200,-6000.,6000.,1200,-6000.,6000.);
  	
  _TH2D(m_MM_SmallWedge2_TransverseView_positiveZ,"MM_TransverseView_S2_posZ",1200,-6000.,6000.,1200,-6000.,6000.);
  _TH2D(m_MM_SmallWedge2_TransverseView_negativeZ,"MM_TransverseView_S2_negZ",1200,-6000.,6000.,1200,-6000.,6000.);
  _TH2D(m_MM_LargeWedge2_TransverseView_positiveZ,"MM_TransverseView_L2_posZ",1200,-6000.,6000.,1200,-6000.,6000.);
  _TH2D(m_MM_LargeWedge2_TransverseView_negativeZ,"MM_TransverseView_L2_negZ",1200,-6000.,6000.,1200,-6000.,6000.);
  //--------------------------------Wedge 2 end---------------------------------------------------------------------------------------
  
  
  return StatusCode::SUCCESS;
}
