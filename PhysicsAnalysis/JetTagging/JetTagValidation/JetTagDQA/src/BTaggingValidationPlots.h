/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef JETTAGDQA_BTagPLOTS_H
#define JETTAGDQA_BTagPLOTS_H
	
#include "xAODBase/IParticle.h"
#include "TrkValHistUtils/PlotBase.h"
#include "CLHEP/Units/SystemOfUnits.h"
#include "xAODBTagging/BTagging.h" 
#include "xAODJet/Jet.h"
//#include "GAFlavourLabel.h"
	
namespace JetTagDQA{
 
	class BTaggingValidationPlots:public PlotBase {
		public:
		BTaggingValidationPlots(PlotBase* pParent, std::string sDir, std::string sParticleType);
		void fill(const xAOD::BTagging* btag);
		void fill(unsigned int nbtag);
		void fill(int truthLabel);
		void fill(const xAOD::Jet* jet);
		//void fill(const xAOD::Jet* jet, const xAOD::BTagging* btag);
		void fill(const xAOD::Jet* jet, const xAOD::BTagging* btag, const xAOD::Vertex *myVertex);

		void makeEfficiencyVsPtPlot(TH1* hReco, TProfile* pEff);
		void makeEfficiencyPlot(TH1* hReco, TProfile* pEff);
		void makeEfficiencyRejectionPlot(TProfile* pLEff, TProfile* pEffRej);
		void setTaggerInfos();	
		void bookEffHistos();


		// Reco only information
		std::string m_sParticleType;

		//JVT jet flag 
		bool m_isJVT_defined;
		float m_jvt_cut;

		TH1* m_truthLabel = nullptr;
//		TH1* m_GAFinalHadronLabel;
//		TH1* m_GAInitialHadronLabel;
//		TH1* m_GAFinalPartonLabel;

//		TH1* m_GAFinalHadronC_dR;
//		TH1* m_GAInitialHadronC_dR;
//		TH1* m_GAFinalPartonC_dR;
//		TH1* m_GAFinalHadronTau_dR;

		TH1* m_truthPt_b = nullptr;
		TH1* m_truthPt_u = nullptr;
		TH1* m_truthPt_c = nullptr;
		TH1* m_truthPt_tau = nullptr;

		TH1* m_IP3D_pb = nullptr;
		TH1* m_IP3D_pc = nullptr;
		TH1* m_IP3D_pu = nullptr;

		TH1* m_SV1_pb = nullptr;
		TH1* m_SV1_pc = nullptr;
		TH1* m_SV1_pu = nullptr;
		
		TH1* m_SV1_NGTinSvx = nullptr;
		TH1* m_SV0_NGTinSvx = nullptr;
		TH1* m_IP3D_nTracks = nullptr;
		TH1* m_IP2D_nTracks = nullptr;

		TH1* m_e = nullptr;
		TH1* m_pt = nullptr;
	      	TH1* m_eta = nullptr;
	      	TH1* m_phi = nullptr;

		//ANDREA --- Store tranking info
		TH1* m_track_d0 = nullptr;
		TH1* m_track_z0 = nullptr;
		TH1* m_track_sigd0 = nullptr;
		TH1* m_track_sigz0 = nullptr;

		//ANDREA -- IPTag categories
		TH1* m_IP3D_trackGrading = nullptr;
		TH1* m_IP2D_trackGrading = nullptr;
		TH1* m_tmpD0 = nullptr;
		TH1* m_tmpZ0 = nullptr;
		TH1* m_tmpD0sig = nullptr;
		TH1* m_tmpZ0sig = nullptr;
		TH1* m_tmpIP3DBwgt = nullptr;
		TH1* m_tmpIP3DUwgt = nullptr;
		TH1* m_tmpIP2DBwgt = nullptr;
		TH1* m_tmpIP2DUwgt = nullptr;
		
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
