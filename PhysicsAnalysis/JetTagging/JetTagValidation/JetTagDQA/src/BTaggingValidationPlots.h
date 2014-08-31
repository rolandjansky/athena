/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef JETTAGDQA_BTagPLOTS_H
#define JETTAGDQA_BTagPLOTS_H
	
#include "TrkValHistUtils/PlotBase.h"
#include "CLHEP/Units/SystemOfUnits.h"
#include "xAODBTagging/BTagging.h" 
//#include "../../Event/xAOD/xAODBTagging/xAODBTagging/BTagging.h"
#include "xAODJet/Jet.h"
	
namespace JetTagDQA{
 
	class BTaggingValidationPlots:public PlotBase {
		public:
		BTaggingValidationPlots(PlotBase* pParent, std::string sDir, std::string sParticleType);
		void fill(const xAOD::BTagging* btag);
		void fill(unsigned int nbtag);
		void fill(int truthLabel);
		void fill(const xAOD::Jet* jet);
		void fill(const xAOD::Jet* jet, const xAOD::BTagging* btag);

		void makeEfficiencyVsPtPlot(TH1* hReco, TProfile* pEff);
		void makeEfficiencyPlot(TH1* hReco, TProfile* pEff);
		void makeEfficiencyRejectionPlot(TProfile* pLEff, TProfile* pEffRej);
		void setTaggerInfos();	
		void bookEffHistos();


		// Reco only information
		std::string m_sParticleType;

		TH1* m_truthLabel;

		TH1* m_truthPt_b;
		TH1* m_truthPt_u;
		TH1* m_truthPt_c;
		TH1* m_truthPt_tau;

		TH1* m_IP3D_pb;
		TH1* m_IP3D_pc;
		TH1* m_IP3D_pu;

		TH1* m_SV1_pb;
		TH1* m_SV1_pc;
		TH1* m_SV1_pu;
		
		TH1* m_SV1_NGTinSvx;
		TH1* m_SV0_NGTinSvx;
		TH1* m_IP3D_nTracks;
		TH1* m_IP2D_nTracks;

		TH1* m_e;
		TH1* m_pt;
	      	TH1* m_eta;
	      	TH1* m_phi;

		std::vector<std::string> m_taggers;
		std::map<std::string, int> m_truthLabels;
		std::map<std::string, double> m_IP3D_workingPoints;
		std::map<std::string, double> m_IP2D_workingPoints;
		std::map<std::string, double> m_SV1_workingPoints;
		std::map<std::string, double> m_SV0_workingPoints;
		std::map<std::string, double> m_IP3DSV1_workingPoints;
		std::map<std::string, double> m_JetFitter_workingPoints;
		std::map<std::string, double> m_JetFitterCombNN_workingPoints;
		std::map<std::string, double> m_MV1_workingPoints;
		std::map<std::string, TH1*> m_weight_histos; 
		std::map<std::string, TProfile*> m_eff_profiles; 
	
		private:
		virtual void initializePlots();     
		virtual void finalizePlots(); 
	};
	
}
	
#endif
