/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef PixelChargeInterpolationPlot_h
#define PixelChargeInterpolationPlot_h

class string;
template < class T, class Allocator > class vector;
class TH1F;
class TCanvas;
class TGaxis;

namespace PixelCalib{

class PixelChargeInterpolationParameters;

class PixelChargeInterpolationPlot{

public :
	PixelChargeInterpolationPlot(const PixelChargeInterpolationParameters &parameters,
			bool oneconst = false);
	virtual ~PixelChargeInterpolationPlot();
	void AddReference(const PixelChargeInterpolationParameters &parameters,
			std::string title = "Reference", int color = 0,
			std::string drawopt = "HIST"); 
	void Plot(const std::string output);
	void Write();
	
private:

	// Histograms to be used
	std::vector < TH1F* > *m_histogramsX;
	std::vector < TH1F* > *m_histogramsY;
	std::vector < std::vector < TH1F* > > *m_RefHistosX;
	std::vector < std::vector < TH1F* > > *m_RefHistosY;


	// bins
	double* m_etabins;
	double* m_phibins;
	int m_netabins;
	int m_nphibins;
	static const int m_nlayers; // = 3;
	std::vector <std::string> *m_referenceDrawOpt;
	bool m_oneconst;

	// utility methods!
	void PlotDirection(std::string filename, std::string direction = "phi");

	void DrawOneHisto(TH1F *histo, std::string direction = "phi",float maximum = 0);
	void DrawHistoMarkers(TH1F* histo, std::string options  = "P0same",  int goodj = 0);
	void DrawLayerLegend(float xlegend, float ylegend);
	void DrawAxis(float y1, float y2, float x1, float x2, std::string direction = "phi");

	std::vector < TH1F*> *HistogramsFromConstants(
			const PixelChargeInterpolationParameters &parameters,
			std::string direction = "phi", int color = 1, std::string title = "");
};

}
#endif
