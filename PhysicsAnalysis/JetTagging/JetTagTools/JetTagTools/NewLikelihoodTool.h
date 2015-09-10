/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef JETTAGTOOLS_NEWLIKELIHOODMULTIDTOOL_H
#define JETTAGTOOLS_NEWLIKELIHOODMULTIDTOOL_H
/******************************************************
   @class NewLikelihoodTool
    DESCRIPTION:
    Helper class for likelihood calculations
********************************************************/

#include "AthenaBaseComps/AthAlgTool.h"
#include "GaudiKernel/ToolHandle.h"
#include "JetTagTools/LikelihoodComponents.h"
#include <string>
#include <vector>

class TH1;

namespace Analysis {
 
class CalibrationBroker;

static const InterfaceID IID_NewLikelihoodTool("Analysis::NewLikelihoodTool", 1, 0);
  
class NewLikelihoodTool : public AthAlgTool {

 public:

  NewLikelihoodTool(const std::string&,const std::string&,const IInterface*);
  virtual ~NewLikelihoodTool();
  
  StatusCode initialize();
  StatusCode finalize();
  
  static const InterfaceID& interfaceID() { return IID_NewLikelihoodTool; };
  
  void defineHypotheses(const std::vector<std::string>&);
  
  void defineHistogram(const std::string& hname);
  TH1* prepareHistogram(const std::string& hypo, const std::string& hname);
  void smoothAndNormalizeHistogram(TH1* histo, const std::string& hname);
  
  void setLhVariableValue(std::vector<Slice>& value);
  void setLhVariableValue(Slice& value);
  
  std::vector<double> calculateLikelihood();
  std::vector<double> tagLikelihood();
 
  void printStatus() const;

  void clear();

  // helper functions:
  double getEff(const std::string& hypo, const std::string& histo, const std::string& suffix);
  std::vector<std::string> gradeList(const std::string& hname);

 private:

  std::string m_taggerName; // tagger this tool refers to
  std::vector<std::string> m_hypotheses; // likelihood hypotheses: usually B or U
  /** Pointer to the calibration tool: */
  ToolHandle<CalibrationBroker> m_calibrationTool;

  /** Treatment of histograms: */
  bool m_normalizedProb;
  bool m_interpolate;
  int m_smoothNTimes;
  std::vector<std::string> m_vetoSmoothingOf;

  /** Contains the value of the likelihood variables. */
  std::vector<Slice> m_lhVariableValues;
  
  /** Contains the combined likelihood for each hypothesis. First entry is for signal. */
  std::vector<double> m_likelihoodVector;

  std::vector<std::string> m_histograms;

};

}
#endif /// JETTAGTOOLS_LIKELIHOODMULTIDTOOL_H

