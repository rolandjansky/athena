/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef ISF_FASTCALOSIMEVENT_TFCS1DFunctionHistogram_h
#define ISF_FASTCALOSIMEVENT_TFCS1DFunctionHistogram_h

#include "ISF_FastCaloSimEvent/TFCS1DFunction.h"
#include "TH1.h"
#include <vector>

class TFCS1DFunctionHistogram:public TFCS1DFunction
{

  public:
    TFCS1DFunctionHistogram() {};
    TFCS1DFunctionHistogram(TH1* hist, int,double);

    void Initialize(TH1* hist, int,double);

    virtual double rnd_to_fct(double rnd);
    TH1* vector_to_histo();
          double get_inverse(double rnd);
          double linear(double x1,double x2,double y1,double y2,double x);

    void smart_rebin_loop(TH1* hist, int,double);
    TH1D* smart_rebin(TH1D* , double );
    double* histo_to_array(TH1*);
    double sample_from_histo(TH1* hist, double);
    double sample_from_histovalues(double);

    vector<float> get_HistoBorders() {return m_HistoBorders;};
    vector<float> get_HistoContents()  {return m_HistoContents;};

  private:

    vector<float> m_HistoBorders;
    vector<float> m_HistoContents;


  ClassDef(TFCS1DFunctionHistogram,1)  //TFCS1DFunctionHistogram

};

#endif
