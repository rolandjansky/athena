/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/

#ifndef IDAlignMonTruthComparison_H
#define IDAlignMonTruthComparison_H

// **********************************************************************
// IDAlignMonTruthComparison.cxx
// AUTHORS: Beate Heinemann, Tobias Golling
// **********************************************************************

#include <vector>

#include "GaudiKernel/StatusCode.h"
#include "AthenaMonitoring/AthenaMonManager.h"
#include "AthenaMonitoring/ManagedMonitorToolBase.h"
#include "EventPrimitives/EventPrimitives.h"

class TH1F;
class TH2F;
class TProfile;

namespace Trk {
  class ITruthToTrack;
}

namespace InDetAlignMon {
  class TrackSelectionTool;
} 

class IDAlignMonTruthComparison : public ManagedMonitorToolBase
{

public:

	IDAlignMonTruthComparison( const std::string & type, const std::string & name, const IInterface* parent ); 

	virtual ~IDAlignMonTruthComparison();

	virtual StatusCode initialize();
	virtual StatusCode bookHistograms();
	virtual StatusCode fillHistograms();
	virtual StatusCode procHistograms();
  
	void RegisterHisto(MonGroup& mon, TH1* histo);
	void RegisterHisto(MonGroup& mon, TH2* histo);
	void RegisterHisto(MonGroup& mon, TProfile* histo);

protected:

	TH1F* m_truthpT;
	TH1F* m_truthphi;
	TH1F* m_trutheta;

	TH1F* m_dqopt_barrel;
	TH1F* m_deta_barrel;
	TH1F* m_dphi_barrel;
	TProfile* m_dphi_barrel_vs_phi;
	TH1F* m_dz0_barrel;
	TH1F* m_dd0_barrel;

	TH1F* m_dqopt_eca;
	TH1F* m_deta_eca;
	TH1F* m_dphi_eca;
	TH1F* m_dz0_eca;
	TH1F* m_dd0_eca;

	TH1F* m_dqopt_ecc;
	TH1F* m_deta_ecc;
	TH1F* m_dphi_ecc;
	TH1F* m_dz0_ecc;
	TH1F* m_dd0_ecc;

	TH2F* m_Deta_vs_eta;
	TProfile* m_dphi_vs_eta;
	TH1F* m_deta_vs_eta_1;
	TH1F* m_deta_vs_eta_2;
	TH1F* m_deta_vs_eta_chi2;

	TProfile* m_dpt_vs_truthpt_barrel;
	TH2F* m_Dqopt_vs_pt_barrel;
	TH1F* m_dqopt_vs_pt_barrel_1;
	TH1F* m_dqopt_vs_pt_barrel_2;
	TH1F* m_dqopt_vs_pt_barrel_chi2;

	TProfile* m_dpt_vs_truthpt_eca;
	TH2F* m_Dqopt_vs_pt_eca;
	TH1F* m_dqopt_vs_pt_eca_1;
	TH1F* m_dqopt_vs_pt_eca_2;
	TH1F* m_dqopt_vs_pt_eca_chi2;

	TProfile* m_dpt_vs_truthpt_ecc;
	TH2F* m_Dqopt_vs_pt_ecc;
	TH1F* m_dqopt_vs_pt_ecc_1;
	TH1F* m_dqopt_vs_pt_ecc_2;
	TH1F* m_dqopt_vs_pt_ecc_chi2;

	TH2F* m_Dqopt_vs_eta_highpt;
	TH1F* m_dqopt_vs_eta_highpt_1;
	TH1F* m_dqopt_vs_eta_highpt_2;
	TH1F* m_dqopt_vs_eta_highpt_chi2;

	TH2F* m_Dqopt_vs_eta_lowpt;
	TH1F* m_dqopt_vs_eta_lowpt_1;
	TH1F* m_dqopt_vs_eta_lowpt_2;
	TH1F* m_dqopt_vs_eta_lowpt_chi2;

	TH2F* m_Dqopt_vs_phi_highpt_barrel;
	TH1F* m_dqopt_vs_phi_highpt_barrel_1;
	TH1F* m_dqopt_vs_phi_highpt_barrel_2;
	TH1F* m_dqopt_vs_phi_highpt_barrel_chi2;

	TH2F* m_Dqopt_vs_phi_highpt_eca;
	TH1F* m_dqopt_vs_phi_highpt_eca_1;
	TH1F* m_dqopt_vs_phi_highpt_eca_2;
	TH1F* m_dqopt_vs_phi_highpt_eca_chi2;

	TH2F* m_Dqopt_vs_phi_highpt_ecc;
	TH1F* m_dqopt_vs_phi_highpt_ecc_1;
	TH1F* m_dqopt_vs_phi_highpt_ecc_2;
	TH1F* m_dqopt_vs_phi_highpt_ecc_chi2;

	TH2F* m_Dqopt_vs_phi_lowpt_barrel;
	TH1F* m_dqopt_vs_phi_lowpt_barrel_1;
	TH1F* m_dqopt_vs_phi_lowpt_barrel_2;
	TH1F* m_dqopt_vs_phi_lowpt_barrel_chi2;

	TH2F* m_Dqopt_vs_phi_lowpt_eca;
	TH1F* m_dqopt_vs_phi_lowpt_eca_1;
	TH1F* m_dqopt_vs_phi_lowpt_eca_2;
	TH1F* m_dqopt_vs_phi_lowpt_eca_chi2;

	TH2F* m_Dqopt_vs_phi_lowpt_ecc;
	TH1F* m_dqopt_vs_phi_lowpt_ecc_1;
	TH1F* m_dqopt_vs_phi_lowpt_ecc_2;
	TH1F* m_dqopt_vs_phi_lowpt_ecc_chi2;

	TH1F* m_Zmumu;
	TH1F* m_Zmumu_truth;
	TH1F* m_dZmumu;
	TH1F* m_dZmumu_barrel;
	TH1F* m_dZmumu_barrel_eca;
	TH1F* m_dZmumu_barrel_ecc;
	TH1F* m_dZmumu_ecc;
	TH1F* m_dZmumu_eca;

 private:

	float m_Pi;

	const AtlasDetectorID*                m_idHelper;
	std::string m_stream;
	std::string m_tracksName;
	std::string m_tracksTruthName;
	
	ToolHandle<Trk::ITruthToTrack>         m_truthToTrack; //!< tool to create track parameters from a gen particle
	ToolHandle<InDetAlignMon::TrackSelectionTool>         m_trackSelection; //!< tool to get tracks from StoreGate and apply selection
	int m_checkrate;

};

#endif
