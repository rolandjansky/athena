/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#include "TestActionTimerTool.h"
#include "GaudiKernel/ITHistSvc.h"
#include "TH1D.h"

namespace G4UA
{

  TestActionTimerTool::TestActionTimerTool(const std::string& type,
                                           const std::string& name,
                                           const IInterface* parent)
    : UserActionToolBase<TestActionTimer>(type, name, parent),
      m_histSvc("THistSvc", name)
  {
  }

  StatusCode TestActionTimerTool::initialize()
  {
    ATH_CHECK(m_histSvc.retrieve());
    return StatusCode::SUCCESS;
  }

  std::unique_ptr<TestActionTimer>
  TestActionTimerTool::makeAndFillAction(G4AtlasUserActions& actionList)
  {
    ATH_MSG_DEBUG("Constructing a TestActionTimer");
    auto action = std::make_unique<TestActionTimer>();
    actionList.runActions.push_back( action.get() );
    actionList.eventActions.push_back( action.get() );
    actionList.steppingActions.push_back( action.get() );
    return action;
  }

  StatusCode TestActionTimerTool::finalize()
  {
    // Accumulate results across threads
    TestActionTimer::Report report;
    m_actions.accumulate(report, &TestActionTimer::getReport,
                         &TestActionTimer::Report::merge);

    if(!report.time.empty()){
      G4double sumVTime = 0.;
      G4double sumPTime = 0.;
      for (int i(0); i <= TestActionTimer::eOther; ++i) sumVTime += report.time[i];
      for (int i(TestActionTimer::eElec); i < TestActionTimer::eMax; ++i) sumPTime += report.time[i];

      if (report.nev>0){
	ATH_MSG_INFO("****** TestActionTimer: Beginning timer dump ******");
	
	G4double DeadTime  = report.runTime - sumVTime;
	G4double OtherPart = report.runTime - DeadTime - sumPTime;
	
	ATH_MSG_INFO("Timer\tEvent\tFraction");
	ATH_MSG_INFO("EvtAve\t"<<report.runTime/report.nev<<"\t100");

	for (int i(0);i <= TestActionTimer::eOther; i++) {
	  ATH_MSG_INFO(report.timeName[i] << "\t" << report.time[i]/report.nev << "\t" << report.time[i]/report.runTime);
	}
	ATH_MSG_INFO("Dead\t" << DeadTime/report.nev << "\t" << DeadTime/report.runTime);
	for (int i(TestActionTimer::eElec);i < TestActionTimer::eMax; i++) {
	  ATH_MSG_INFO(report.timeName[i] << "\t" << report.time[i]/report.nev << "\t" << report.time[i]/report.runTime);
	}
	ATH_MSG_INFO("OtherPart\t" << OtherPart/report.nev << "\t" << OtherPart/report.runTime);
	
	ATH_MSG_INFO("****** TestActionTimer: Major Subdet Times per Event *********");
	ATH_MSG_INFO("EMB\t" << (report.time[TestActionTimer::eEMB])/report.nev);
	ATH_MSG_INFO("EMEC\t" << (report.time[TestActionTimer::eEMEC])/report.nev);
	ATH_MSG_INFO("FCAL\t" << (report.time[TestActionTimer::eFC1]+report.time[TestActionTimer::eFC23]+report.time[TestActionTimer::eFCO])/report.nev);
	ATH_MSG_INFO("HEC\t" << (report.time[TestActionTimer::eHEC])/report.nev);
	ATH_MSG_INFO("Tile\t" << (report.time[TestActionTimer::eHCB])/report.nev);
	ATH_MSG_INFO("Tracker\t" << (report.time[TestActionTimer::ePx]+report.time[TestActionTimer::eSct]+report.time[TestActionTimer::eITkPix]+report.time[TestActionTimer::eITkStrip]+report.time[TestActionTimer::eTrt]+report.time[TestActionTimer::eSev])/report.nev);
	ATH_MSG_INFO("Muons\t" << (report.time[TestActionTimer::eMu])/report.nev);
	ATH_MSG_INFO("Oth.LAr\t" << (report.time[TestActionTimer::ePre]+report.time[TestActionTimer::TestActionTimer::eCry]+report.time[TestActionTimer::eLAr])/report.nev);
	ATH_MSG_INFO("Other\t" << (report.time[TestActionTimer::eOther])/report.nev);
	ATH_MSG_INFO("EvtAve\t" << (report.runTime)/report.nev);
	ATH_MSG_INFO("******* TestActionTimer: Major Subdet Frac per Event *********");
	ATH_MSG_INFO("EMB\t" << (report.time[TestActionTimer::eEMB])/report.runTime*100.);
	ATH_MSG_INFO("EMEC\t" << (report.time[TestActionTimer::eEMEC])/report.runTime*100.);
	ATH_MSG_INFO("FCAL\t" << (report.time[TestActionTimer::eFC1]+report.time[TestActionTimer::eFC23]+report.time[TestActionTimer::eFCO])/report.runTime*100.);
	ATH_MSG_INFO("HEC\t" << (report.time[TestActionTimer::eHEC])/report.runTime*100.);
	ATH_MSG_INFO("Tile\t" << (report.time[TestActionTimer::eHCB])/report.runTime*100.);
	ATH_MSG_INFO("Tracker\t" << (report.time[TestActionTimer::ePx]+report.time[TestActionTimer::eSct]+report.time[TestActionTimer::eITkPix]+report.time[TestActionTimer::eITkStrip]+report.time[TestActionTimer::eTrt]+report.time[TestActionTimer::eSev])/report.runTime*100.);
	ATH_MSG_INFO("Muons\t" << (report.time[TestActionTimer::eMu])/report.runTime*100.);
	ATH_MSG_INFO("Oth.LAr\t" << (report.time[TestActionTimer::ePre]+report.time[TestActionTimer::eCry]+report.time[TestActionTimer::eLAr])/report.runTime*100.);
	ATH_MSG_INFO("Other\t" << (report.time[TestActionTimer::eOther])/report.runTime*100.);
	ATH_MSG_INFO("EvtAve\t" << (report.runTime)/report.nev);
	ATH_MSG_INFO("******* TestActionTimer: Timer Information *********");
	ATH_MSG_INFO("NOTE: in case of MT runs, the timings for the individual threads are added, i.e. it is");
	ATH_MSG_INFO("      equivalent to running the N threads sequentially instead of in parallel.");
	ATH_MSG_INFO("Timers were run over " << report.nev << " events, and the averages were taken by");
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
	
	report.runTime -= DeadTime;
	for (int i(0); i <= TestActionTimer::eOther; ++i){
	  r_geo->Fill( i , report.time[i] / report.runTime*100. );
	  r_geo->GetXaxis()->SetBinLabel( i+1 , report.timeName[i].c_str() );
	}
	for (int i(TestActionTimer::eElec); i < TestActionTimer::eMax; ++i){
	  r_part->Fill( i-TestActionTimer::eElec , report.time[i] / report.runTime*100. );
	  r_part->GetXaxis()->SetBinLabel( i-TestActionTimer::eElec+1 , report.timeName[i].c_str() );
	}
	r_part->Fill( TestActionTimer::eMax-TestActionTimer::eElec , OtherPart / report.runTime*100. ); // Other particles don't get their own timer
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
