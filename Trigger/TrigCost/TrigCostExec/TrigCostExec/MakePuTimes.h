/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef ANP_MAKEPUTIME_H
#define ANP_MAKEPUTIME_H

/**********************************************************************************
 * @Package: TrigCostExec
 * @Class  : MakePuTimes
 * @Author : Rustem Ospanov
 *
 * @Brief  : Plot algorithm timing as a function of number of pileup interactions
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
#include "TrigCostBase/NtupleSvc.h"
#include "TrigCostBase/PlotEvent.h"
#include "TrigCostBase/HtmlABC.h"

// Local
#include "TrigCostExec/StudyTime.h"

namespace Anp
{
  struct PuTime
  {
    PuTime() : npileup(0), alg_id(0) {}

    unsigned npileup;
    unsigned alg_id;

    std::map<std::string, double> vars;
  };

  struct PuTimeHist {
    
    PuTimeHist() : alg_id(0), hist(0), func(0), key(0.0), par0(0.0), par1(0.0), chi2(0), ndof(0) {}
    
    unsigned    alg_id;
    std::string alg_name;
    std::string var_name;
    std::string var_title;
    
    TH1 *hist;
    TF1 *func;

    double key;
    double par0;
    double par1;
    double chi2;
    double ndof;

    bool operator<(const PuTimeHist &rhs) const {
      return key > rhs.key;
    }

    void Print(const std::string &opt = "", unsigned name_width = 0) const;
  };

  class MakePuTimes: public virtual AlgEvent, public virtual HtmlABC {
  public:
    
    MakePuTimes();
    virtual ~MakePuTimes();
    
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

    PuTimeHist FitEff(TH1 *h, const std::string &fname);

  private:

    typedef std::map<unsigned, Anp::Handle<Anp::StudyTime> > TimeMap;
    typedef std::map<std::string, std::vector<PuTimeHist> > HistMap;

  private:

    Handle<ConfigABC>           fConf;         // Configuration algorithm
    TrigMonConfig               fConfig;       // Local copy of confguration object

    Branch<TrigMonEvent>        fEntry;        // Trigger data

    Handle<PlotEvent>           fPlot;         // Plot event variables
    std::vector<std::string>    fVarNames;     // Variable names for plots

    TDirectory                 *fDir;          // Output directory
    Anp::Registry               fReg;          // Registry configuration
    Anp::Registry               fRegIndex;     // Registry configuration

    std::string                 fDirName;      // Output directory name
    std::string                 fFitFunc;      // Fit function name
    std::string                 fCanvasType;   // Canvas type: no, png, eps

    bool                        fDebug;        // Activate verbose messages
    bool                        fIsConfig;     // Algorithm holds valid configuration?
    double                      fMinPass;      // Minimum trigger pass events for efficiency plot(s)
    double                      fMaxEff;       // Maximum efficiency value for a fit
    double                      fMaxVal;       // Maximum x-axis value for a fit
    double                      fMidVal;       // Middle x-axis value for comprison
    double                      fPrintMin;     // Minimum rate to print rates
    unsigned int                fFitKey;       // Fit histogram with this key
    unsigned int                fFitNpt;       // Number of fit points

    TimeMap                     fTimes;
    HistMap                     fHists;
  };
}

//
// Efficiency (trigger pass fraction) function
//
double TrigPuTimeFunc(double *x, double *par);

#endif
