/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef PixelChargeInterpolationHistograms_h
#define PixelChargeInterpolationHistograms_h

#include <string>
#include <fstream>

class TProfile;
class TH2F;
class TH1F;
template < class ht > class MultiHisto;

namespace PixelCalib{

class PixelChargeInterpolationParameters;

class PixelChargeInterpolationHistograms{

public :
	PixelChargeInterpolationHistograms(std::string tag,
					const PixelChargeInterpolationParameters &model);
	
	virtual ~PixelChargeInterpolationHistograms();
        
        PixelChargeInterpolationHistograms (const PixelChargeInterpolationHistograms&) = delete;
        PixelChargeInterpolationHistograms& operator = (const PixelChargeInterpolationHistograms&) = delete;
	
	int Fill(int DetType, double GeVTrkPt,
		double TrkEta, double DeltaCol, double reseta, double OmegaEta,
		double alpha, double DeltaRow, double resphi, double OmegaPhi);
	
	PixelChargeInterpolationParameters *Analyze(std::ofstream &logfile);
	
	int Write();
	int Read();
	
private:
	
	std::string m_tag;

	PixelChargeInterpolationParameters *m_parameters;
	MultiHisto<TProfile> *m_etaProfile;
	MultiHisto<TProfile> *m_phiProfile;
	MultiHisto<TH2F> *m_etaH;
	MultiHisto<TH2F> *m_phiH;
	MultiHisto<TH1F> *m_OmegaPhih;
	MultiHisto<TH1F> *m_OmegaEtah;
	TH1F* m_OmegaEtah_model;
	TH1F *m_OmegaPhih_model;

	enum Indexes {
		LayerIndex = 0,
		AngleIndex = 1,
		ClustersizeIndex = 2
	};

	bool Fit(TProfile *swap, double *value, double *error);

};

}

#endif // #ifdef PixelChargeInterpolationHistograms_h
