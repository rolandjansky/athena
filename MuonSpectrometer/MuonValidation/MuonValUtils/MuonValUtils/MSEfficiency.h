/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////////////
//
// DESCRIPTION:
// Subject: 	This class allows easy Efficiency Calculations for up to
//		three dimensions
///////////////////////////////////////////////////////////////////////////

#ifndef MSEFFICIENCY_H
#define MSEFFICIENCY_H

#include <string>
#include <iostream>
#include <vector>
#include <map>
#include <math.h>


using namespace std;


class MSEfficiency
{
	public :
		// Default Constructor - Empty
		MSEfficiency();

		// Copy constructor
		MSEfficiency(const MSEfficiency& ob);

		// Overload =
		void operator = (const MSEfficiency& ob);

		void clear();

		// Setting up one dimension
		bool setupDim(	int dim,	// Which Dimension should be set up? Start with 1
				double start,	// Start-Parameter
				double end,	// End-Parameter
				int bins);	// Number of Bins
		bool setupDim(int dim,
		vector <double> &bins);	// Use vector bins if you want to have not aquidistant bins

		// Setting information for each event
		void setEfficiencyForEvent(	bool pass,	// Has the event passed the efficiency-variable
						double dim1);	// Which 1d-value has the variable
		void setEfficiencyForEvent(bool pass, double dim1, double dim2);
		void setEfficiencyForEvent(bool pass, double dim1, double dim2, double dim3);

		// Getting overall Efficiency and error on efficiency
		bool getOverallEfficiency(double &Eff, double &EffError);
		// Getting the efficiency for the 3d-bin
		bool getEfficiency123(double dim1, double dim2, double dim3, double &Eff, double &EffError);
		// Getting the efficiency for the 1d-bin
		bool getEfficiency1(double dim1, double &Eff, double &EffError);
		// Getting the efficiency for the 2d-bin
		bool getEfficiency2(double dim2, double &Eff, double &EffError);
		// Getting the efficiency for the 3d-bin
		bool getEfficiency3(double dim3, double &Eff, double &EffError);
		bool getEfficiency12(double dim1, double dim2, double &Eff, double &EffError);
		bool getEfficiency23(double dim2, double dim3, double &Eff, double &EffError);
		bool getEfficiency13(double dim1, double dim3, double &Eff, double &EffError);

		// Getting the mean and the width of the bin "id" in dimension "dim"
		bool getDimBinInfo(int dim, int id, double &meanOfBin, double &widthOfBin);
		int  getNumberOfBins(int dim);

		void  getTransformInformation(	vector<int>	&vecBinInfo, 
						vector<double>	&vecDimensionInfo,
						vector<int>	&vecStoredInformation);

		void  setTransformInformation(	vector<int>	&vecBinInfo, 
						vector<double>	&vecDimensionInfo,
						vector<int>	&vecStoredInformation);

	private:
		// Calculate error for one bin. I sue Poission-stat-error. If the efficiency is 100% (ok==total), then an erro for (ok-1) is calculated
		double getError(int ok, int total);

		int nDim1Bins;
		int nDim2Bins;
		int nDim3Bins;
		vector<double> vecDim1Bins;
		vector<double> vecDim2Bins;
		vector<double> vecDim3Bins;

		vector< vector <vector <int> > > 	nTotalEvents;
		vector< vector <vector <int> > > 	nPassedEvents;
};

#endif
