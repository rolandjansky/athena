/*
 Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/

//  @file HLTTauMonTool_FailTrackFilterMonitor.cxx
//  created by Eleni-Myrto Asimakopoulou <eleni.myrto.asimakopoulou@cern.ch>
#include "TProfile.h"
#include "HLTTauMonTool.h"
#include "AthenaKernel/Units.h"
#include "xAODEventInfo/EventInfo.h"

using namespace std;
using Athena::Units::GeV;
StatusCode HLTTauMonTool::FailTrackFilterMonitor(const std::string & trigItem){
   
  ATH_MSG_DEBUG ("HLTTauMonTool::FailTrackFilterMonitor");
  
  const xAOD::EventInfo* evtInfo = 0;	
  ATH_CHECK ( evtStore()->retrieve(evtInfo) );
  
  std::string chain = "HLT_"+trigItem;
  
  setCurrentMonGroup("HLT/TauMon/Expert/"+trigItem+"/FailTrackFilterMonitor");
  if(getTDT()->isPassed(chain))
    {
      for(unsigned int t=0;t<m_taus.size();t++){
	// check if this tau has the "failTrackFilter" flag set to true, 
	// ie, none of its core tracks passed the nominal BDT cut, but one core track passed a looser BDT cut
	// --> the tau track is promoted to classifiedCharged
	bool failsTrackFilter = m_taus.at(t)->nTracks(xAOD::TauJetParameters::failTrackFilter);
	
	//if (!failsTrackFilter) continue; // we are only interested to monitor here the cases where that is TRUE.
	if (failsTrackFilter) { // we are only interested to monitor here the cases where that is TRUE.
	  // fill a histogram with the pT of that tau.
	  hist("hFailTrackFilterPt")->Fill(m_taus.at(t)->pt()/GeV);
	    //ATH_MSG_WARNING("In FailTrackFilterMonitor. hFailTrackFilterPt is OK.");
	}
	
      }
    } // end of: "if(getTDT()->isPassed(chain))"
  
  return StatusCode::SUCCESS;
} // end of "HLTTauMonTool::FailTrackFilterMonitor"
