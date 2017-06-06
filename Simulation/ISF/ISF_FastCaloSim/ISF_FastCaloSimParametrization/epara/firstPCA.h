/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef firstPCA_h
#define firstPCA_h
#include "TChain.h"
#include "ISF_FastCaloSimParametrization/TreeReader.h"

using namespace std;

class firstPCA
{
  public:
  	
    firstPCA(TChain*, string);
    firstPCA();
    virtual ~firstPCA() {}
    void run();
    vector<TH1D*> get_relevantlayers_inputs(vector<int> &, TreeReader*);
    vector<TH1D*> get_cumul_histos(vector<string> layer, vector<TH1D*>);
    static double get_cumulant(double x, TH1D* h);
    void quantiles(TH1D* h, int nq, double* xq, double* yq);

    void set_cumulativehistobins(int);
    void set_edepositcut(double);
    void set_etacut(double,double);
    void apply_etacut(int);
    void set_pcabinning(int,int);
    
  private:
  	
  	int    m_debuglevel;
  	double m_cut_eta_low;
  	double m_cut_eta_high;
  	int    m_apply_etacut;
  	int    m_nbins1;
  	int    m_nbins2;
  	int    m_numberfinebins;
  	double m_edepositcut;
  	string m_outfilename;
  	TChain* m_chain;
  	
  ClassDef(firstPCA,1);
  
};

#if defined(__MAKECINT__)
#pragma link C++ class firstPCA+;
#endif

#endif

