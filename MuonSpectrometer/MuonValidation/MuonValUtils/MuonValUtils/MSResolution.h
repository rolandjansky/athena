/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////////////
// Authors:   M.Schott(LMU) - N. Benekos(MPI)  
// MuonValidation-Team
// Feb. 2007
//
// DESCRIPTION:
// Subject: 	This class allows easy Resoltuion Calculations
///////////////////////////////////////////////////////////////////////////

#ifndef MSRESOLUTION_H
#define MSRESOLUTION_H

#include <string>
#include <iostream>
#include <vector>
#include <map>
#include <math.h>

using namespace std;

// Ugly method to use TMinuit

double fit_bw(double *x, double *par);

class MSResolution
{
	public :
		// Default Constructor - Empty
		MSResolution();

		// Copy constructor
		MSResolution(const MSResolution& ob);

		// Overload =
		void operator = (const MSResolution& ob);

		~MSResolution();

		void clear();

		// Setting up a Dimension in which you want to calculate the resolution
		void setupDim(	string id,	// Name of Dimension	e.g. "Eta" ,"pt", "phi"
				double start,	// Lowest Value		e.g. -2.5
				double end,	// Highest Value	e.g. 2.5
				int bins);	// Number of Bins	e.g. 5

		// Adding an event to dimenion "id" and the value "value" in this dimension. Use it for pt-resolution
		void add(string id, double value, double ptTrue, double ptReco);
		// Adding "insert_value" to dimenion "id" and the value "value" in this dimension. Use it for pt-resolution
		void add(string id, double value, double insert_value);

		// Get information about the binning
		int  getBinInfo(string id, double value);
		bool getDimBinInfo(string id, int i, double &meanOfBin, double &widthOfBin);
		int  getNumberOfBins(string id);

		// Get PT-Resolution
		bool getResolution(	string id,		// Name of Dimension
					double value,		// Which value in Dimension
					double &PTResMean,	// Mean of PT-Resolution
					double &ErrMean,	// Error on Mean
					double &PT68Sigma,	// At which width do we have 68% of all events?
					double &PTSigma,	// Which sigma has the fitted function
					double &ErrWidth);	// What is the error of the fitted function

		// Get Z-Boson-Properties
		bool getZBosonProperties(	string id,		// Name of Dimesion
						double value,		// Which value in dimension
						double &ZBosonMean,	// ZBoson Properties
						double &ErrMean,
						double &ZBosonSigma,
						double &ErrSigma);

	private:
		map<string,  map<int, vector<double> > > m_1D_StackInfo;

		bool get95PercentRange(std::string id, double value, double mean, double &width);

		map<string, vector<double> >	vecBins;
		map<string, vector<double> >	vecTrue;
		map<string, vector<double> >	vecReco;
};

#endif


