/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#ifndef PixelChargeInterpolationPlot_h
#define PixelChargeInterpolationPlot_h

#include "CxxUtils/checker_macros.h"

class string;
template < class T, class Allocator > class vector;
class TH1F;
class TCanvas;
class TGaxis;

namespace PixelCalib{

class PixelChargeInterpolationParameters;

class PixelChargeInterpolationPlot{

public :
  PixelChargeInterpolationPlot(const PixelChargeInterpolationParameters &parameters, bool oneconst = false);
	virtual ~PixelChargeInterpolationPlot();
	void AddReference(const PixelChargeInterpolationParameters &parameters,
                          const std::string& title = "Reference", int color = 0,
                          const std::string& drawopt = "HIST"); 
	void Plot(const std::string& output);
	void Write();
	
private:

  PixelChargeInterpolationPlot(const PixelChargeInterpolationPlot &);
  PixelChargeInterpolationPlot &operator=(const PixelChargeInterpolationPlot&);

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
	void PlotDirection(const std::string& filename, const std::string& direction = "phi");

	void DrawOneHisto(TH1F *histo, const std::string& direction = "phi",float maximum = 0);
	void DrawHistoMarkers(TH1F* histo, const std::string& options  = "P0same",  int goodj = 0);
	void DrawLayerLegend(float xlegend, float ylegend);
	void DrawAxis(float y1, float y2, float x1, float x2, const std::string& direction = "phi");

	std::vector < TH1F*> *HistogramsFromConstants(
			const PixelChargeInterpolationParameters &parameters,
			const std::string& direction = "phi", int color = 1, const std::string& title = "");
};

}
#endif
