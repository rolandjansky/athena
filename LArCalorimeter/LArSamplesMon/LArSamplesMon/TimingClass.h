/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#ifndef LArSamples_TimingClass_H
#define LArSamples_TimingClass_H

#include "TH1F.h"
#include "TF1.h"
#include "TMinuit.h"
#include <iostream>
#include <algorithm>
#include <fstream>
#include <utility>
#include <set>
#include "TGraphErrors.h"
#include "TGraph.h"
#include "TCanvas.h"
#include "TLatex.h"

#include "LArCafJobs/CaloId.h"


namespace LArSamples {
 
  class Interface;
  class AbsShapeErrorGetter;
  class TimingClass  
  { 
  public:
    
  TimingClass( const Interface& interface ) : m_interface( &interface ) { }
         
    TimingClass();
    ~TimingClass(); 
    
    void timePerFebAllFebs(const std::string& nrun, const std::string& name);
    void fitTimePerFebAllFebs( const std::string& nrun, const std::string& name );
    void Time( int dete, const std::string& nrun );
    void PlotFebAverageTime( const std::string& nrun, const std::string& name );
    void MergeFebTime( const std::string& nrun );
    void getFebCorrection( const std::string& nrun );
    void PlotFebtime();
    
    bool EnergyThreshold( int calo, int layer, int quality, int ft, int slot, double energy, double time );

    double Median[2][32][16]{};
    double param[4][2][32][16]{};
    double error[4][2][32][16]{};
    double getTimeWeightedMedian( std::vector<double> time, std::vector<double> time2, std::vector<double> weight, double totalW );
    
    std::vector< std::vector<double> > readTimingFiles(const std::string& file);
    
  private:
    
    const Interface* m_interface = nullptr;
            
  };
}

#endif
