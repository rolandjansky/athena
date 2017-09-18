/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef secondPCA_h
#define secondPCA_h

#include "ISF_FastCaloSimParametrization/TreeReader.h"
#include "firstPCA.h"

using namespace std;

class secondPCA: public firstPCA
{
  public:
  	
    secondPCA(string,string);
    //virtual ~secondPCA();
    void run();
    vector<int> getLayerBins(TFile* file, int &bins);
		void do_pca(vector<string>, int, TreeReader*, int*);
    vector<TH1D*> get_histos_data(vector<string> layer, TreeReader*);
	  double get_lowerBound(TH1D* h_cumulative);
	  
	  void set_cumulativehistobins(int);
	  void set_storeDetails(int);
	  void set_skip_regression(int);
    void set_PCAbin(int);
    void set_cut_maxdeviation_regression(double val);
    void set_cut_maxdeviation_smartrebin(double val);
    void set_Ntoys(int val);
    void set_neurons_iteration(int start,int end);
	  
  private:
    
    int m_numberfinebins;
    int m_storeDetails;
    int m_PCAbin;
    int m_skip_regression;
    string m_outfilename,m_firstpcafilename;
    int m_neurons_start,m_neurons_end,m_ntoys;
    double m_maxdev_regression,m_maxdev_smartrebin;
    
  ClassDef(secondPCA,2);
  
};

#if defined(__MAKECINT__)
#pragma link C++ class secondPCA+;
#endif

#endif

