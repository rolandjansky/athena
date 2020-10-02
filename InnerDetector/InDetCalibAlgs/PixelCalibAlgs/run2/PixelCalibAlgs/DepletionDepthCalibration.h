/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#ifndef DepletionDepthCalibration_h
#define DepletionDepthCalibration_h

class string;
class TProfile;
template <class ht> class MultiHisto;
template <class type> class TVectorT;

namespace PixelCalib{

class DepletionDepthCalibration{

public :
	DepletionDepthCalibration(int layer = 0, const std::string& cosmicORbeam = "beam");
	virtual ~DepletionDepthCalibration(){};
        DepletionDepthCalibration (const DepletionDepthCalibration&) = delete;
        DepletionDepthCalibration& operator= (const DepletionDepthCalibration&) = delete;
	bool Fill(Int_t Layer,Int_t EtaModule, Int_t PhiModule,
			Double_t ClusterSize, Double_t Eta);
	int Analyze(std::ofstream &logfile);
	int Write();
	int Read();
	void PlotValidation(TCanvas *c1, const std::string& outname);
	
private:

	int m_curLayer;
	std::string m_curType;
	std::string m_layername;
	MultiHisto<TProfile> *m_LayerProfile;
	TVectorT<double> *m_DepletionDepth0;
	TVectorT<double> *m_DepletionDepth0Error;
	TVectorT<double> *m_DepletionDepth1;
	TVectorT<double> *m_DepletionDepth1Error;
	TVectorT<double> *m_DepletionDepth2;
	TVectorT<double> *m_DepletionDepth2Error;
	TVectorT<double> *m_EtaModule;
	TVectorT<double> *m_PhiModule;

};

}
#endif // #ifdef DepletionDepthCalibration_h
