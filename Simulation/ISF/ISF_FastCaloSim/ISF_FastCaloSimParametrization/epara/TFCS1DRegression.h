/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TFCS1DRegression_h
#define TFCS1DRegression_h

// STL includes
#include <string>

#include "TH1.h"
#include "TTree.h"

class TFCS1DRegression
{
  public:
    TFCS1DRegression() {};
    ~TFCS1DRegression() {};
    
    static int testHisto(TH1* hist, std::string, float&, float&, std::string, int, int, double, int);
    static void storeRegression(string, vector<vector<double> > &fWeightMatrix0to1, vector<vector<double> > &fWeightMatrix1to2);
    static void get_weights(string weightfile, vector<vector<double> > &fWeightMatrix0to1, vector<vector<double> > &fWeightMatrix1to2);
    
    static TH1* transform(TH1* h_input, float &rangeval, float& startval);
    static double get_range_low(TH1* hist);
    static void tmvaregression_training(int, TTree *regTree, std::string, std::string, int&);
    static TH1* get_cumul(TH1* hist);
    static double tmvaregression_application(double, std::string);
    static void validate(int,string);
    
};

#if defined(__MAKECINT__)
#pragma link C++ class TFCS1DRegression+;
#endif

#endif
