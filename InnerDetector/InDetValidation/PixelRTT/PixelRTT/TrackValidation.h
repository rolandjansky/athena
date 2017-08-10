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

	MultiHisto<TH1D> *m_PtHistogram;
	MultiHisto<TH1D> *m_AngleHistogram;
	MultiHisto<TH1D> *m_EtaHistogram;
	MultiHisto<TProfile> *m_AngleProfile;
	MultiHisto<TProfile> *m_EtaProfile;
	MultiHisto<TProfile> *m_PtProfile;

	std::string m_globaldirname;
	


};

}


#endif // #ifdef CosmicTrackValidation_h
