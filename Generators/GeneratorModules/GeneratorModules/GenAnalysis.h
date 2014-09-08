/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef GENERATORMODULES_GENANALYSIS_H
#define GENERATORMODULES_GENANALYSIS_H

#include "GeneratorModules/GenBase.h"
#include "GaudiKernel/ITHistSvc.h"
#include "TH1D.h"
#include "TH2D.h"
#include "TProfile.h"


/// @brief Base class for evgen-level analyses
///
/// @author Andy Buckley
///
/// GenAnalysis is a base class which provides convenient access to functions
/// used to make a compact, correct and robust analysis of truth-level data.
///
/// The following virtual methods are to be overloaded in the child class:
///   StatusCode init()     [optional]
///   StatusCode execute()  [mandatory]
///   StatusCode finalize() [optional]
///
class GenAnalysis : public GenBase {
public:

  GenAnalysis(const std::string& name, ISvcLocator* pSvcLocator);
  virtual ~GenAnalysis() { }

  /// Setup analysis tools and call user-supplied init()
  StatusCode initialize();
  /// Per-event analysis routine: calls user-supplied analyze()
  StatusCode execute();

  /// @name Methods for user implementation
  //@{
  virtual StatusCode init() { return StatusCode::SUCCESS; }
  virtual StatusCode analyze() = 0;
  virtual StatusCode finalize() { return StatusCode::SUCCESS; }
  //@}


protected:

  /// @name Histogramming
  //@{

  /// @todo Use shorter names (aliases?) like bookH1D, bookP1D, and histo -> hist (or getH1D / getP1D)

  TH1D* bookHisto1D(const std::string& name, const std::string& title, const std::vector<double>& binedges,
                    const std::string& xtitle="", const std::string& ytitle="");
  TH1D* bookHisto1D(const std::string& name, const std::string& title, size_t numbins, double low, double high,
                    const std::string& xtitle="", const std::string& ytitle="");

  TProfile* bookProfile1D(const std::string& name, const std::string& title, const std::vector<double>& binedges,
                          const std::string& xtitle="", const std::string& ytitle="");
  TProfile* bookProfile1D(const std::string& name, const std::string& title, size_t numbins, double low, double high,
                          const std::string& xtitle="", const std::string& ytitle="");

  TH2D* bookHisto2D(const std::string& name, const std::string& title,
                    const std::vector<double>& binedgesx, const std::vector<double>& binedgesy,
                    const std::string& xtitle="", const std::string& ytitle="", const std::string& ztitle="");
  TH2D* bookHisto2D(const std::string& name, const std::string& title,
                    size_t numbinsx, double xlow, double xhigh,
                    size_t numbinsy, double ylow, double yhigh,
                    const std::string& xtitle="", const std::string& ytitle="", const std::string& ztitle="");

  /// Get a histogram's full path, given the name local to the analysis
  std::string fullhistopath(const std::string localpath) {
    return "/" + m_histkey + "/" + name() + "/" + localpath;
  }

  /// Retrieve a histogram (as TH1* base class pointer)
  TH1* histo(std::string key);

  /// Retrieve a profile histogram
  TProfile* profile(std::string key);

  /// Directly access the histogram service
  const ServiceHandle<ITHistSvc> histSvc() const {
    return m_histSvc;
  }

  //@}


private:

  /// Container name for the MC event collection to be analysed
  std::string m_histkey;
  ServiceHandle<ITHistSvc> m_histSvc;

};


#endif
