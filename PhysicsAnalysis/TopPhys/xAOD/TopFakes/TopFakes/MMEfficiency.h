/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/*
 ** author: Michele Pinamonti <michele.pinamonti@cern.ch>
 ** version: v1 - 23aug2013
*/

#ifndef MMEfficiency_H_
#define MMEfficiency_H_

#include "TFile.h"
#include "TH1F.h"
#include "TH2F.h"
#include "TH3F.h"
#include "TF1.h"
#include "TF2.h"
#include "TF3.h"

#include <string>
#include <iostream>


const int MAXvar = 10;


class MMEfficiency{
  public:
    MMEfficiency();
    ~MMEfficiency();
    
    bool AddHisto(TH1F *h);
    bool AddHisto(TH2F *h);
    bool AddHisto(TH3F *h);
    bool AddHisto(TFile *f, std::string histoName, int ndim=1);
    bool AddHisto(std::string fileName, std::string histoName, int ndim=1);
    
    void SetAverage(float value);
    void SetAverageHist(TFile* f, std::string histoName); // this should be a 1-D & 1-bin histogram
    void SetAverageHist(std::string fileName, std::string histoName); // this should be a 1-D & 1-bin histogram
    float SetAverageFromHist(TH1F *h);
    float SetAverageFromHist(TH2F *h);
    float SetAverageFromHist(TH3F *h);
    float SetAverageFromHist(TFile *f, std::string histoName,int ndim=1);
    float SetAverageFromHist(std::string fileName, std::string histoName,int ndim=1);
    
    float GetEfficiency();
    float GetAverageEfficiency();
    float GetEfficiency(float v0,float v1=0,float v2=0,float v3=0,float v4=0,float v5=0,float v6=0,float v7=0,float v8=0,float v9=0);
    void SetParVar(int idx,float value);
    void SetParVars(float v0,float v1=0,float v2=0,float v3=0,float v4=0,float v5=0,float v6=0,float v7=0,float v8=0,float v9=0);    
    
    // control methods
    int GetNvars();
    
  private:
    TH1F* fHist[MAXvar];
    TH2F* fHist2D[MAXvar];
    TH3F* fHist3D[MAXvar];
    float fAverage;
    float fVars[MAXvar];
    int fNvar;
    int fDim[MAXvar];
};

#endif
