/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef MSEFFICIENCY_H
#define MSEFFICIENCY_H

#include <string>
#include <iostream>
#include <vector>
#include <map>
#include <math.h>

using namespace std;

// Ugly method to use TMinuit


class MSEfficiency
{
	public :
		// Default Constructor - Empty
		MSEfficiency();

		void clear();

		bool setupDim(int dim, float start, float end, int bins);
		bool setupDim(int dim, vector <float> &bins);

		void setEfficiencyForEvent(bool pass, float dim1);
		void setEfficiencyForEvent(bool pass, float dim1, float dim2);
		void setEfficiencyForEvent(bool pass, float dim1, float dim2, float dim3);

		void getOverallEfficiency(float &Eff, float &EffError);
		void getEfficiency123(float dim1, float dim2, float dim3, float &Eff, float &EffError);
		void getEfficiency1(float dim1, float &Eff, float &EffError);
		void getEfficiency2(float dim2, float &Eff, float &EffError);
		void getEfficiency3(float dim3, float &Eff, float &EffError);
		void getEfficiency12(float dim1, float dim2, float &Eff, float &EffError);
		void getEfficiency23(float dim2, float dim3, float &Eff, float &EffError);
		void getEfficiency13(float dim1, float dim3, float &Eff, float &EffError);

		bool getDimBinInfo(int dim, int id, float &meanOfBin, float &widthOfBin);
		int getNumberOfBins(int dim);

	private:
		double getError(int ok, int total);

		int nDim1Bins;
		int nDim2Bins;
		int nDim3Bins;
		vector<float> vecDim1Bins;
		vector<float> vecDim2Bins;
		vector<float> vecDim3Bins;

		vector< vector <vector <int> > > 	nTotalEvents;
		vector< vector <vector <int> > > 	nTotalPassedEvents;
};

#endif
