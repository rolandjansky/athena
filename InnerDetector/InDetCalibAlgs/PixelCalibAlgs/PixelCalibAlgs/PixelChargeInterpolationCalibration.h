/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef PixelChargeInterpolationCalibration_h
#define PixelChargeInterpolationCalibration_h

class string;
template < class T, class Allocator > class vector;
class TDirectory;

using namespace std;

namespace PixelCalib{

class PixelChargeInterpolationHistograms;
class PixelChargeInterpolationPlot;
class PixelChargeInterpolationParameters;

class PixelChargeInterpolationCalibration{

public :
	// Modified stuff
	PixelChargeInterpolationCalibration(std::string tag,
			PixelChargeInterpolationParameters &parametersModel);
	virtual ~PixelChargeInterpolationCalibration();
	void Fill(int DetType, double GeVTrkPt,
		double alpha, double DeltaRow, double digresphi,
		double resphi, double OmegaPhi,
		double TrkEta, double DeltaCol, double digreseta,
		double reseta, double OmegaEta);
	void Read(TDirectory *readdir = 0);
	void Write(TDirectory *writedir = 0);
	
	int Analyze( std::string output, std::vector<std::string> &reference_names);

private:

	PixelChargeInterpolationHistograms *DigitalCalibration;
	PixelChargeInterpolationHistograms *AnalogCalibration;
	PixelChargeInterpolationPlot *m_plots;
};

}
#endif
