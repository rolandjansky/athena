/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "CxxUtils/make_unique.h"
#include "TestActionTimerTool.h"
#include "GaudiKernel/ITHistSvc.h"
#include "TH1D.h"

namespace G4UA{ 
  
  
  TestActionTimerTool::TestActionTimerTool(const std::string& type, const std::string& name,const IInterface* parent):
    ActionToolBaseReport<TestActionTimer>(type, name, parent),m_histSvc("THistSvc",name){
  }
  std::unique_ptr<TestActionTimer>  TestActionTimerTool::makeAction(){
    ATH_MSG_DEBUG("makeAction");
    auto action = CxxUtils::make_unique<TestActionTimer>();
    return std::move(action);
  }
  StatusCode TestActionTimerTool::queryInterface(const InterfaceID& riid, void** ppvIf){
    
    if(riid == IG4EventActionTool::interfaceID()) {
      *ppvIf = (IG4EventActionTool*) this;
      addRef();
      return StatusCode::SUCCESS;
    }
    if(riid == IG4RunActionTool::interfaceID()) {
      *ppvIf = (IG4RunActionTool*) this;
      addRef();
      return StatusCode::SUCCESS;
    }
    if(riid == IG4SteppingActionTool::interfaceID()) {
      *ppvIf = (IG4SteppingActionTool*) this;
      addRef();
      return StatusCode::SUCCESS;
    }
    return ActionToolBase<TestActionTimer>::queryInterface(riid, ppvIf);
  }
  
  
  
  StatusCode TestActionTimerTool::finalize(){
    mergeReports();
    // now m_reports contain the global report
    
    if(!m_report.time.empty()){
      G4double sumVTime = 0.;
      G4double sumPTime = 0.;
      for (int i(0); i <= TestActionTimer::eOther; ++i) sumVTime += m_report.time[i];
      for (int i(TestActionTimer::eElec); i < TestActionTimer::eMax; ++i) sumPTime += m_report.time[i];
      
      if (m_report.nev>0){
	ATH_MSG_INFO("****** TestActionTimer: Beginning timer dump ******");
	
	G4double DeadTime  = m_report.runTime - sumVTime;
	G4double OtherPart = m_report.runTime - DeadTime - sumPTime;
	
	ATH_MSG_INFO("Timer\tEvent\tFraction");
	ATH_MSG_INFO("EvtAve\t"<<m_report.runTime/m_report.nev<<"\t100");

	for (int i(0);i <= TestActionTimer::eOther; i++) {
	  ATH_MSG_INFO(m_report.timeName[i] << "\t" << m_report.time[i]/m_report.nev << "\t" << m_report.time[i]/m_report.runTime);
	}
	ATH_MSG_INFO("Dead\t" << DeadTime/m_report.nev << "\t" << DeadTime/m_report.runTime);
	for (int i(TestActionTimer::eElec);i < TestActionTimer::eMax; i++) {
	  ATH_MSG_INFO(m_report.timeName[i] << "\t" << m_report.time[i]/m_report.nev << "\t" << m_report.time[i]/m_report.runTime);
	}
	ATH_MSG_INFO("OtherPart\t" << OtherPart/m_report.nev << "\t" << OtherPart/m_report.runTime);
	
	ATH_MSG_INFO("****** TestActionTimer: Major Subdet Times per Event *********");
	ATH_MSG_INFO("EMB\t" << (m_report.time[TestActionTimer::eEMB])/m_report.nev);
	ATH_MSG_INFO("EMEC\t" << (m_report.time[TestActionTimer::eEMEC])/m_report.nev);
	ATH_MSG_INFO("FCAL\t" << (m_report.time[TestActionTimer::eFC1]+m_report.time[TestActionTimer::eFC23]+m_report.time[TestActionTimer::eFCO])/m_report.nev);
	ATH_MSG_INFO("HEC\t" << (m_report.time[TestActionTimer::eHEC])/m_report.nev);
	ATH_MSG_INFO("Tile\t" << (m_report.time[TestActionTimer::eHCB])/m_report.nev);
	ATH_MSG_INFO("Tracker\t" << (m_report.time[TestActionTimer::ePx]+m_report.time[TestActionTimer::eSct]+m_report.time[TestActionTimer::eTrt]+m_report.time[TestActionTimer::eSev])/m_report.nev);
	ATH_MSG_INFO("Muons\t" << (m_report.time[TestActionTimer::eMu])/m_report.nev);
	ATH_MSG_INFO("Oth.LAr\t" << (m_report.time[TestActionTimer::ePre]+m_report.time[TestActionTimer::TestActionTimer::eCry]+m_report.time[TestActionTimer::eLAr])/m_report.nev);
	ATH_MSG_INFO("Other\t" << (m_report.time[TestActionTimer::eOther])/m_report.nev);
	ATH_MSG_INFO("EvtAve\t" << (m_report.runTime)/m_report.nev);
	ATH_MSG_INFO("******* TestActionTimer: Major Subdet Frac per Event *********");
	ATH_MSG_INFO("EMB\t" << (m_report.time[TestActionTimer::eEMB])/m_report.runTime*100.);
	ATH_MSG_INFO("EMEC\t" << (m_report.time[TestActionTimer::eEMEC])/m_report.runTime*100.);
	ATH_MSG_INFO("FCAL\t" << (m_report.time[TestActionTimer::eFC1]+m_report.time[TestActionTimer::eFC23]+m_report.time[TestActionTimer::eFCO])/m_report.runTime*100.);
	ATH_MSG_INFO("HEC\t" << (m_report.time[TestActionTimer::eHEC])/m_report.runTime*100.);
	ATH_MSG_INFO("Tile\t" << (m_report.time[TestActionTimer::eHCB])/m_report.runTime*100.);
	ATH_MSG_INFO("Tracker\t" << (m_report.time[TestActionTimer::ePx]+m_report.time[TestActionTimer::eSct]+m_report.time[TestActionTimer::eTrt]+m_report.time[TestActionTimer::eSev])/m_report.runTime*100.);
	ATH_MSG_INFO("Muons\t" << (m_report.time[TestActionTimer::eMu])/m_report.runTime*100.);
	ATH_MSG_INFO("Oth.LAr\t" << (m_report.time[TestActionTimer::ePre]+m_report.time[TestActionTimer::eCry]+m_report.time[TestActionTimer::eLAr])/m_report.runTime*100.);
	ATH_MSG_INFO("Other\t" << (m_report.time[TestActionTimer::eOther])/m_report.runTime*100.);
	ATH_MSG_INFO("EvtAve\t" << (m_report.runTime)/m_report.nev);
	ATH_MSG_INFO("******* TestActionTimer: Timer Information *********");
	ATH_MSG_INFO("NOTE: in case of MT runs, the timings for the individual threads are added, i.e. it is");
	ATH_MSG_INFO("      equivalent to running the N threads sequentially instead of in parallel.");
	ATH_MSG_INFO("Timers were run over " << m_report.nev << " events, and the averages were taken by");
	ATH_MSG_INFO("detector portion.  Detectors are determined by volume name.  Pieces include:");
	ATH_MSG_INFO("EMB, EMEC, FCAL, HEC, Tile - the sampling portions of the calorimeters.");
	ATH_MSG_INFO("Pixel, SCT, TRT - the sensitive portions of the tracker only");
	ATH_MSG_INFO("InDetSvc - all remaining portions of the inner detector");
	ATH_MSG_INFO("Oth.LAr - all remaining portions of the LAr, including preshower, solenoid,");
	ATH_MSG_INFO("\tcryostats, and services.");
	ATH_MSG_INFO("Other - volumes not listed, including ATLAS::ATLAS and CALO::CALO");
	ATH_MSG_INFO("");
	ATH_MSG_INFO("Dead time includes overhead per event (file i/o, event loading, timing overhead)");
	ATH_MSG_INFO("");
	ATH_MSG_INFO("All times are user+system time on your processor");
	ATH_MSG_INFO("******* TestActionTimer: End of timer dump *********");
	
	TH1D * r_geo = new TH1D("GeoResults","GeoResults",TestActionTimer::eOther-TestActionTimer::eEMB+1,-0.5,float(TestActionTimer::eOther-TestActionTimer::eEMB+1)-0.5);
	TH1D * r_part = new TH1D("ParticleResults","ParticleResults",TestActionTimer::eMax-TestActionTimer::eElec+1,-0.5,float(TestActionTimer::eMax-TestActionTimer::eElec+1)-0.5);
	
	m_report.runTime -= DeadTime;
	for (int i(0); i <= TestActionTimer::eOther; ++i){
	  r_geo->Fill( i , m_report.time[i] / m_report.runTime*100. );
	  r_geo->GetXaxis()->SetBinLabel( i+1 , m_report.timeName[i].c_str() );
	}
	for (int i(TestActionTimer::eElec); i < TestActionTimer::eMax; ++i){
	  r_part->Fill( i-TestActionTimer::eElec , m_report.time[i] / m_report.runTime*100. );
	  r_part->GetXaxis()->SetBinLabel( i-TestActionTimer::eElec+1 , m_report.timeName[i].c_str() );
	}
	r_part->Fill( TestActionTimer::eMax-TestActionTimer::eElec , OtherPart / m_report.runTime*100. ); // Other particles don't get their own timer
	r_part->GetXaxis()->SetBinLabel( TestActionTimer::eMax-TestActionTimer::eElec+1 , "Others" );
	r_geo->SetXTitle("Detector Region");
	r_part->SetXTitle("Particle Type");
	r_geo->SetYTitle("Percent of Event Time");
	r_part->SetYTitle("Percent of Event Time");
	
	if (m_histSvc){
	  if ( m_histSvc->regHist("/simTime/GeoResults",r_geo).isFailure() ||
	       m_histSvc->regHist("/simTime/ParticleResults",r_part).isFailure() ){
	    ATH_MSG_ERROR("Could not register histograms!");
	  }
	}
	
      } else {
	
	ATH_MSG_INFO("******* TestActionTimer: Nothing to report, no events timed! *********");
      }
    } else {ATH_MSG_INFO("******* TestActionTimer: No timing information recoreded! *********");}
    return StatusCode::SUCCESS;
  }
  
} // namespace G4UA 
