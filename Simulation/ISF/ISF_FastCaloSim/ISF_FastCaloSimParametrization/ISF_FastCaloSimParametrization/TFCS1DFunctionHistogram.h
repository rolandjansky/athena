/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TFCS1DFunctionHistogram_h
#define TFCS1DFunctionHistogram_h

#include "ISF_FastCaloSimParametrization/TFCS1DFunction.h"
#include "TH1.h"
#include <vector>

class TFCS1DFunctionHistogram:public TFCS1DFunction
{
	
  public:
    TFCS1DFunctionHistogram();
    TFCS1DFunctionHistogram(TH1* hist);
    
    virtual void Initialize(TH1* hist);
    virtual double rnd_to_fct(double rnd);
    
    TH1* vector_to_histo();
    void smart_rebinning(TH1* hist, double);
    double* histo_to_array(TH1*);
    double sample_from_histo(TH1* hist, double);
    double sample_from_histovalues(double);
  
  private:

    std::vector<float> m_HistoBorders;
    std::vector<float> m_HistoContents;
  
  ClassDef(TFCS1DFunctionHistogram,1)  //TFCS1DFunctionHistogram

};

#if defined(__MAKECINT__)
#pragma link C++ class TFCS1DFunction;
#pragma link C++ class TFCS1DFunctionHistogram;
#endif

#endif
