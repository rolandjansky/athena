/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/

#ifndef IDAlignMonSivsTRT_H
#define IDAlignMonSivsTRT_H

// **********************************************************************
// IDAlignMonSivsTRT.cxx
// AUTHORS: Ben Cooper
// **********************************************************************

#include <vector>

#include "GaudiKernel/StatusCode.h"
#include "GaudiKernel/ToolHandle.h"
#include "GaudiKernel/ServiceHandle.h"
#include "EventPrimitives/EventPrimitives.h"
#include "AthenaMonitoring/AthenaMonManager.h"
#include "AthenaMonitoring/ManagedMonitorToolBase.h"

#include "TrkParameters/TrackParameters.h"


#include "TrkToolInterfaces/ITrackSummaryTool.h"

class TH1F;
class TH2F;
class TProfile;

class AtlasDetectorID;
class PixelID;
class SCT_ID;

namespace InDetAlignMon{
  class TrackSelectionTool;
}

namespace InDetDD{
  class SCT_DetectorManager;
  class PixelDetectorManager; 
}

namespace Trk {
  class Track;
  class TrackStateOnSurface;
}

class IDAlignMonSivsTRT : public ManagedMonitorToolBase
{

public:

	IDAlignMonSivsTRT( const std::string & type, const std::string & name, const IInterface* parent ); 

	virtual ~IDAlignMonSivsTRT();

	virtual StatusCode initialize();
	
	virtual StatusCode bookHistograms();
	virtual StatusCode fillHistograms();
	virtual StatusCode procHistograms();

	void RegisterHisto(MonGroup& mon, TH1* histo);
	void RegisterHisto(MonGroup& mon, TH2* histo);
	void RegisterHisto(MonGroup& mon, TProfile* histo);
	//Not sure should be public
	void InitializeHistograms();

protected:


 private:

	
	StatusCode setupTools();

	std::string m_stream;
	std::string m_triggerChainName;
	int m_checkrate;
	float m_matchdRcut;
	int m_events;
	int m_histosBooked;
	
	//private functions
	StatusCode fillEfficiencyHisto(TH1* num, TH1* den, TH1* eff);
	StatusCode fillEfficiencyProfileHisto(TH1*, TH1*, TProfile*);
       

	//tools
	ToolHandle<InDetAlignMon::TrackSelectionTool>        m_trackSelection;
	ToolHandle<Trk::ITrackSummaryTool>        m_trackSumTool;


	//histograms
	TH1F* m_delta_phi0;
	TH1F* m_delta_eta0;
	TH1F* m_delta_R;
	TH1F* m_delta_qoverp;
	TH1F* m_delta_charge;
	TH1F* m_delta_d0;
	TH1F* m_delta_z0;
	TH1F* m_delta_pt;
	TH1F* m_reldelta_pt;
	TH1F* m_nhitstrt;

	TH1F* m_delta_phi0_b;
	TH1F* m_delta_eta0_b;
	TH1F* m_delta_R_b;
	TH1F* m_delta_qoverp_b;
	TH1F* m_delta_charge_b;
	TH1F* m_delta_d0_b;
	TH1F* m_delta_z0_b;
	TH1F* m_delta_pt_b;
	TH1F* m_reldelta_pt_b;

	TH1F* m_alltrks_phi0;
	TH1F* m_alltrks_phi0_b;
	TH1F* m_alltrks_phi0_eca;
	TH1F* m_alltrks_phi0_ecc;
	TH1F* m_alltrks_eta0;
	TH1F* m_si_phi0;
	TH1F* m_si_eta0;
	TH1F* m_trt_phi0;
	TH1F* m_trt_phi0_b;
	TH1F* m_trt_phi0_eca;
	TH1F* m_trt_phi0_ecc;
	TH1F* m_trt_eta0;
	TH1F* m_matched_phi0;
	TH1F* m_matched_eta0;

	TProfile* m_sieff_phi0;
	TProfile* m_sieff_eta0;
	TProfile* m_trteff_phi0;
	TProfile* m_trteff_phi0_b;
	TProfile* m_trteff_phi0_eca;
	TProfile* m_trteff_phi0_ecc;
	TProfile* m_trteff_eta0;

};

#endif
