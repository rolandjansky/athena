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

	int curLayer;
	std::string curType;
	std::string layername;
	MultiHisto<TProfile> *LayerProfile;
	TVectorT<double> *DepletionDepth0;
	TVectorT<double> *DepletionDepth0Error;
	TVectorT<double> *DepletionDepth1;
	TVectorT<double> *DepletionDepth1Error;
	TVectorT<double> *DepletionDepth2;
	TVectorT<double> *DepletionDepth2Error;
	TVectorT<double> *EtaModule;
	TVectorT<double> *PhiModule;

};

}
#endif // #ifdef DepletionDepthCalibration_h
