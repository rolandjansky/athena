/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef ANP_MAKEPURATE_H
#define ANP_MAKEPURATE_H

/**********************************************************************************
 * @Package: TrigCostRate
 * @Class  : HtmlRates
 * @Author : Rustem Ospanov
 *
 * @Brief  : Computate rates for pileup MC events
 * 
 **********************************************************************************/

// C/C++
#include <cmath>
#include <map>
#include <string>
#include <sstream>

// Trigger
#include "TrigMonitoringEvent/TrigMonConfig.h"
#include "TrigMonitoringEvent/TrigMonEvent.h"

// Local
#include "TrigCostBase/AlgEvent.h"
#include "TrigCostBase/ConfigABC.h"
#include "TrigCostBase/HtmlABC.h"
#include "TrigCostBase/NtupleSvc.h"
#include "TrigCostBase/PlotEvent.h"

// Local
#include "TrigCostRate/MakeRates.h"

namespace Anp
{
  struct PuData 
  {
    PuData() : npileup(0), rate(0.0), rate_err(0.0), eff(0.0), eff_err(0.0) {}

    unsigned npileup;
    double rate;
    double rate_err;
    double eff;
    double eff_err;
  };

  struct PuDataFit {
    
    PuDataFit() : key(0.0), par0(0.0), par1(0.0), chi2(0), ndof(0) {}
  
    std::string name;
    double key;
    double par0;
    double par1;
    double chi2;
    double ndof;

    bool operator<(const PuDataFit &rhs) const {
      return key > rhs.key;
    }

    void Print(const std::string &opt = "", unsigned name_width = 0) const;
  };

  class MakePuRates: public virtual AlgEvent, public virtual HtmlABC {
  public:
    
    MakePuRates();
    virtual ~MakePuRates();
    
    void Config(const Registry &reg);
       
    void Signal(const Registry &reg, const std::string &signal);
 
    void Save(TDirectory *dir);

    bool Init();

    void Exec();
    
    void Done();
    
    void Write(HtmlNode &inode, HtmlMenu &hmenu);

    const Registry& GetIndexRegistry() const { return fRegIndex; }

    void Run(const TrigMonEvent &event);    
    
  private:

    PuDataFit FitEff(TH1 *h, const std::string &name, const std::string &fname);

  private:

    typedef std::map<unsigned, Anp::Handle<Anp::MakeRates> > RateMap;
    typedef std::map<std::string, TH1 *> HistMap;

  private:

    Handle<ConfigABC>           fConf;         // Configuration algorithm
    TrigMonConfig               fConfig;       // Local copy of confguration object

    Branch<TrigMonEvent>        fEntry;        // Trigger data
    Handle<PlotEvent>           fPlot;         // Plot event variables

    TDirectory                 *fDir;          // Output directory
    Anp::Registry               fReg;          // Registry configuration
    Anp::Registry               fRegIndex;     // Registry configuration
    
    std::string                 fDirName;      // Output directory name
    std::string                 fFitFunc;      // Fit function name
    std::string                 fCanvasType;   // Canvas type: no, png, eps
    
    bool                        fDebug;        // Set debug flag
    bool                        fIsConfig;     // Algorithm holds valid configuration?
    double                      fMinPass;      // Minimum trigger pass events for efficiency plot(s)
    double                      fMinRate;      // Minimum trigger rate for efficiency plot(s)
    double                      fMaxEff;       // Maximum efficiency value for a fit
    double                      fMaxVal;       // Maximum x-axis value for a fit
    double                      fMidVal;       // Middle x-axis value for comprison
    double                      fPrintMin;     // Minimum rate to print rates
    unsigned int                fFitKey;       // Fit histogram with this key
    unsigned int                fFitNpt;       // Number of fit points

    RateMap                     fRates;
    HistMap                     fHistEff;
  };
}

//
// Efficiency (trigger pass fraction) function
//
double TrigPuRateEffFunc(double *x, double *par);

#endif
