/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef DepletionDepthCalibration_h
#define DepletionDepthCalibration_h

class string;
class TProfile;
template <class ht> class MultiHisto;
template <class type> class TVectorT;

using namespace std;

namespace PixelCalib{

class DepletionDepthCalibration{

public :
	DepletionDepthCalibration(int layer = 0, std::string cosmicORbeam = "beam");
	virtual ~DepletionDepthCalibration(){};
	bool Fill(Int_t Layer,Int_t EtaModule, Int_t PhiModule,
			Double_t ClusterSize, Double_t Eta);
	int Analyze(std::ofstream &logfile);
	int Write();
	int Read();
	void PlotValidation(TCanvas *c1, std::string outname);
	
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
