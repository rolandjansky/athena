/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TILE_OFC_H
#define TILE_OFC_H

#include <vector>

//namespace TileDspGain { int Low = 0; int High = 1; }

//namespace TileDspRunType { int Phys = 0; int Laser = 1; int CIS = 2; }

//namespace TileDspAlgorithm { int OF1=0; int OF2=1; }

void Double2Int_calib( double calib_constant, 
		       unsigned int  &scale, 
		       unsigned int  &offset, 
		       bool verbose=false );

void Double2Int_ofc( int w_off_size, 
		     std::vector<double> w_off,
		     std::vector<int> &w_dsp, 
		     int &w_sum_dsp, 
		     int &scale, 
		     bool verbose =false );

bool ConvertOFC( std::vector<std::vector<std::vector<std::vector<double> > > > w_off,
		 std::vector<std::vector<double> > calibration, 
		 int calibtype, 
		 int runtype, 
		 std::vector<unsigned int> &OFC, 
		 bool verbose = false);

bool Format6( std::vector<double> a,
	      std::vector<double> b,
	      std::vector<double> c,
	      std::vector<double> g,
	      std::vector<double> h,
	      unsigned int index_channel,
	      int phase,
	      double calibration,
	      std::vector<unsigned int> &OFC,
	      bool verbose = false);

bool Format5( std::vector<double> a, 
	      std::vector<double> b , 
	      std::vector<double> c, 
	      std::vector<double> g, 
	      std::vector<double> h, 
	      std::vector<unsigned int> &OFC, 
	      bool verbose = false);

bool Format5calib(double calib,
		  std::vector<unsigned int> &OFC,
		  bool verbose = false);

bool FormatInfo(int nsamples, 
		 int calibrationtype, 
		 int algorithm, 
		 int runtype, 
		 std::vector<unsigned int> &OFC, 
		 bool verbose = false);

bool ReadOFfile(std::vector<std::vector<std::vector<std::vector<double> > > > &w_off, 
		char * OFCfile, 
		bool verbose = false);


/*
bool ConvertOFC_iter( TileCalibDrawerOfc ofcDrawer, 
		      std::vector<unsigned int> &OFC, 
		      std::vector<std::vector<float> > calib,
		      int calibtype, 
		      int runtype, 
		      int algorithm,
		      bool verbose);

bool ConvertOFC_HR( TileCalibDrawerOfc ofcDrawer, 
		    TileCalibDrawerFlt phaseDrawer,
		    std::vector<unsigned int> &OFC, 
		    std::vector<std::vector<float> > calib,
		    int calibtype, 
		    int runtype, 
		    int algorithm,
		    bool verbose);
*/

#endif
