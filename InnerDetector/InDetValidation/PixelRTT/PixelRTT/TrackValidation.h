/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef CosmicTrackValidation_h
#define CosmicTrackValidation_h

class TProfile;
class RMSProfile;
class TH1D;
template <class ht> class MultiHisto;
class TDirectory;
class TCanvas;

namespace PixelValid{

class CosmicTrackValidation{

public:
	CosmicTrackValidation();
	virtual ~CosmicTrackValidation(){};
	bool Fill(int Layer, double GeVTrkPt, double Angle, double Eta, int nhits);
	int Analyze(TDirectory *ref_file = 0);
	int Write();
	int Read();
	int PlotAll(TCanvas ***c1 = 0, int ncanvas = 0, const char* option = "", int color = 1);
	
private:

	MultiHisto<TH1D> *PtHistogram;
	MultiHisto<TH1D> *AngleHistogram;
	MultiHisto<TH1D> *EtaHistogram;
	MultiHisto<TProfile> *AngleProfile;
	MultiHisto<TProfile> *EtaProfile;
	MultiHisto<TProfile> *PtProfile;

	std::string globaldirname;
	


};

}


#endif // #ifdef CosmicTrackValidation_h
