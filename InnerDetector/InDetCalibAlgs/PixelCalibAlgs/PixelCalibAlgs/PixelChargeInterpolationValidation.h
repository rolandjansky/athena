/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef PixelChargeInterpolationValidation_h
#define PixelChargeInterpolationValidation_h

class string;
class TDirectory;
class TH1F;
class TH2F;

namespace PixelCalib{

class PixelResidualHistograms;
class PixelChargeInterpolationParameters;

class PixelChargeInterpolationValidation{

public:
	
	PixelChargeInterpolationValidation(std::string tag,
			const PixelChargeInterpolationParameters &model);
	
	virtual ~PixelChargeInterpolationValidation();
	
	void Fill(int Layer, double GeVTrkPt,
		double Phi, double CSphi, double MicronDigResPhi, double MicronResPhi, double MicronErrPhi, double PullPhi,
		double Eta, double CSeta, double MicronDigResEta, double MicronResEta, double MicronErrEta, double PullEta, double Charge);
	
	void Analyze(TDirectory *ref_file = 0);
	
	int Write(TDirectory *file = 0);
	int Read(TDirectory *file = 0);

	void WriteErrorsFile(std::string name);

private:

	PixelResidualHistograms *m_eta;
	PixelResidualHistograms *m_phi;
	PixelResidualHistograms *m_etaDig;
	PixelResidualHistograms *m_phiDig;
	PixelResidualHistograms *m_etaPull;
	PixelResidualHistograms *m_phiPull;
	PixelResidualHistograms *m_etaErr;
	PixelResidualHistograms *m_phiErr;
	PixelResidualHistograms *m_eta_phi;
	PixelResidualHistograms *m_phi_eta;
	PixelResidualHistograms *m_etaDig_phi;
	PixelResidualHistograms *m_phiDig_eta;
	PixelResidualHistograms *m_etaPull_phi;
	PixelResidualHistograms *m_phiPull_eta;
	PixelResidualHistograms *m_etaErr_phi;
	PixelResidualHistograms *m_phiErr_eta;
	PixelResidualHistograms *m_charge_eta;
	PixelResidualHistograms *m_charge_phi;
	
	std::vector<PixelResidualHistograms *> m_eta_Layer;
	std::vector<PixelResidualHistograms *> m_phi_Layer;
	std::vector<PixelResidualHistograms *> m_etaDig_Layer;
	std::vector<PixelResidualHistograms *> m_phiDig_Layer;
	std::vector<PixelResidualHistograms *> m_etaPull_Layer;
	std::vector<PixelResidualHistograms *> m_phiPull_Layer;

	std::vector<PixelResidualHistograms *> m_phi_Clustersize;
	std::vector<PixelResidualHistograms *> m_eta_Clustersize;
	std::vector<PixelResidualHistograms *> m_phiDig_Clustersize;
	std::vector<PixelResidualHistograms *> m_etaDig_Clustersize;
	std::vector<PixelResidualHistograms *> m_phiErr_Clustersize;
	std::vector<PixelResidualHistograms *> m_etaErr_Clustersize;
	std::vector<PixelResidualHistograms *> m_phiPull_Clustersize;
	std::vector<PixelResidualHistograms *> m_etaPull_Clustersize;

	std::vector<TH1F *> m_etaClustersize;
	std::vector<TH1F *> m_phiClustersize;

	TH2F *eta_phi_correlation;
	TH2F *CSeta_CSphi_correlation;
	TH2F *ResEtaDig_vs_p;
	TH2F *ResPhiDig_vs_p;
	
	

	std::string m_name;
	
	enum Indexes {
		LayerIndex = 0,
		AngleIndex = 1,
		ClustersizeIndex = 2
	};


};

}


#endif // #ifdef PixelChargeInterpolationValidation_h
