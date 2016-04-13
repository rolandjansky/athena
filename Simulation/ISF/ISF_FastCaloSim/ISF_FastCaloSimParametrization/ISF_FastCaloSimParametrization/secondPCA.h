/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef secondPCA_h
#define secondPCA_h

//#include "TH1D.h"
//#include "TPrincipal.h"
#include "ISF_FastCaloSimParametrization/TreeReader.h"
#include "ISF_FastCaloSimParametrization/firstPCA.h"
//#include "TTree.h"

class secondPCA:public firstPCA
{
  public:
  	
    secondPCA();
    virtual ~secondPCA() {}
    void run(std::string,std::string,int,int,int);
    std::vector<int> getLayerBins(TFile* file, int &bins);
		void do_pca(std::vector<std::string> layer, int bin, TreeReader* read_inputTree, TTree* InputTree, int, TFile*, double* samplings);
    static std::vector<TH1D*> get_histos_data(std::vector<std::string> layer, TreeReader*, TTree*, int);
	
  private:
    
  ClassDef(secondPCA,2);
  
};


#if defined(__MAKECINT__)
#pragma link C++ class secondPCA;
#endif


#endif

