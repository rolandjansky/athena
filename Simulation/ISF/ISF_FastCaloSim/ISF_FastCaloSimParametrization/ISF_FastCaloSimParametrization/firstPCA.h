/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef firstPCA_h
#define firstPCA_h



#include "ISF_FastCaloSimParametrization/TreeReader.h"

class firstPCA
{
  public:
  	
    firstPCA();
    virtual ~firstPCA() {}
    std::string run(std::string, std::string, int, int, int, double);
    std::vector<int> get_relevantlayers(TreeReader*, double);
    std::vector<TH1D*> get_histos_data(std::vector<int> layerNr, std::vector<std::string> layer, TreeReader*, TTree*, int);
    std::vector<TH1D*> get_cumul_data(std::vector<std::string> layer, std::vector<TH1D*>);
    static double get_cumulant(double x, TH1D* h);
    void quantiles(TH1D* h, int nq, double* xq, double* yq);
				
  private:
    
  ClassDef(firstPCA,1);
  
};


#if defined(__MAKECINT__)
#pragma link C++ class firstPCA;
#endif


#endif

