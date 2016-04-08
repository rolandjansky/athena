/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef PixelResidualHistograms_h
#define PixelResidualHistograms_h

template < class T, class Allocator > class vector;
class string;

class TH1D;
class TProfile;
class TCanvas;
template <class ht> class MultiHisto;


namespace PixelCalib{

class PixelResidualHistograms{

public:
	PixelResidualHistograms(const std::string name,
			   const std::string title,
			   double limits,
			   int nbins,
			   const std::vector< std::vector < float > > &binnage,
			   const std::vector< std::string > &binnames);
	
	virtual ~PixelResidualHistograms();

	void SetAxisTitle(std::string title);
	
	void Fill(float residual, std::vector< float > parameters);

	int Write(bool writebins = false);
	int Read();

	TH1D* GetHisto(const int i) const;
	TProfile* GetMeanProfile(const std::string binname);
	TProfile* GetRMSProfile(const std::string binname);
	TProfile* GetProfile(const int binnumber, bool RMS = false, bool savebins = false);

	TH1D *GetGlobalHisto();

	std::vector <TCanvas *> *DrawProfiles(int color,
			  int marker,
			  float labely = 0,
			  std::vector <TCanvas *> *canvasvector = 0,
			  std::string name = "");

	//const std::vector< std::string > *GetBinsNames() const;
	//const std::vector < float > *GetBins(const std::string binname) const;

	//PixelResidualHistograms *Clone() const;

	/*void DrawHisto(int iHisto,
			int color,
			int marker,
			float labelx, float labely,
			std::string label,
			std::string Options);*/

	/*void DrawProfile(const std::string binname,
			int color,
			int marker,
			float labelx, float labely,
			std::string label,
			std::string Options);*/

	/*int Analyze(PixelResidualHistograms *reference = 0,
			std::string referencename = "Reference");*/

private:

	MultiHisto< TH1D > *m_HistogramsVector;
	std::vector< TProfile*> *m_MeanProfilesVector;
	std::vector< TProfile*> *m_RMSProfilesVector;
	TH1D *m_GlobalHisto;
	std::vector< std::string > m_binnames;
	std::vector< std::vector < float > > m_binnage;
	std::string m_axisName;

	PixelResidualHistograms(){};

};


} // end namespace

#endif // #ifdef OfflineCalibValidation_h
