/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef ANP_TIMEHIST_H
#define ANP_TIMEHIST_H

/**********************************************************************************
 * @Package: TrigCostExec
 * @Class  : TimeHist
 * @Author : Rustem Ospanov
 *
 * @Brief  : Helper histogram class which dynamically adjusts X axis binning
 *
 **********************************************************************************/

// C/C++
#include <string>

// Local
#include "TrigCostBase/Handle.h"
#include "TrigCostBase/Hist1d.h"
#include "TrigCostBase/Registry.h"

namespace Anp
{
  class TimeHist: virtual public Base {
  public:

    TimeHist(const std::string &name = "");
    ~TimeHist() {}

    void Config(const Registry &reg);
    
    void Fill(double value);

    void Finalize();
    
    bool IsValid() const { return fIsValid; }

    unsigned int GetNFill()  const { return fNFill; }
    unsigned int GetNLower() const { return fNLower; }
    unsigned int GetNUpper() const { return fNUpper; }

    double GetLowerEdge()  const { return fLowerEdge; }
    double GetUpperEdge()  const { return fUpperEdge; }
    
    Hist1d<double>& GetHist()     { return fHist; }
    Hist1d<double>& GetHistOver() { return fHistOver; }

    TH1* GetTH1() const;

  private: 

    void CreateHist();

  private: 

    std::string          fName;         // Group name
    bool                 fDebug;        // Print debug info?
    
    unsigned int         fNBins;        // Number of histogram bins
    unsigned int         fNWait;        // Wait for a few fills before binning histograms
    double               fLowerEdge;    // Fixed lower edge
    double               fUpperEdge;    // Fixed upper edge

    double               fIgnLowerFrac; // Ignore top frac for range computation
    double               fIgnUpperFrac; // Ignore low frac for range computation
    double               fPadLowerFrac; // Increase histogram range by this fraction
    double               fPadUpperFrac; // Increase histogram range by this fraction
    bool                 fFixLowerEdge; // Set fixed lower edge?
    bool                 fFixUpperEdge; // Set fixed upper edge?

    bool                 fIsValid;      // Do I have a valid histogram?
    unsigned int         fNFill;        // Number of Fill() calls
    unsigned int         fNLower;       // Overflow count
    unsigned int         fNUpper;       // Underflow count

    std::vector<double>  fOver;         // Overflow entries
    std::vector<double>  fWait;         // Temporary vector stores fill values
    Hist1d<double>       fHist;         // Central histogram
    Hist1d<double>       fHistOver;     // Overflow histogram
  };
}

#endif
