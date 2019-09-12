/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
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
#include "JetTagCalibration/JetTagCalibCondData.h"
#include <string>
#include <vector>

class TH1;

namespace Analysis {
 
static const InterfaceID IID_NewLikelihoodTool("Analysis::NewLikelihoodTool", 1, 0);
  
class NewLikelihoodTool : public AthAlgTool {

 public:

  NewLikelihoodTool(const std::string&,const std::string&,const IInterface*);
  virtual ~NewLikelihoodTool();
  
  virtual StatusCode initialize() override;
  virtual StatusCode finalize() override;
  
  static const InterfaceID& interfaceID() { return IID_NewLikelihoodTool; };
  
  void defineHypotheses(const std::vector<std::string>&);
  
  void defineHistogram(const std::string& hname);
  TH1* prepareHistogram(const std::string& hypo, const std::string& hname) const;
  void smoothAndNormalizeHistogram(TH1* histo, const std::string& hname) const;
  
  std::vector<double> calculateLikelihood(const std::vector<Slice>& lhVariableValues) const;
 
  void printStatus() const;

  // helper functions:
  double getEff(const std::string& hypo, const std::string& histo, const std::string& suffix) const;
  std::vector<std::string> gradeList(const std::string& hname) const;

 private:

  std::string m_taggerName; // tagger this tool refers to
  std::vector<std::string> m_hypotheses; // likelihood hypotheses: usually B or U

  /** Key of calibration data: */
  SG::ReadCondHandleKey<JetTagCalibCondData> m_readKey{this, "HistosKey", "JetTagCalibHistosKey", "Key of input (derived) JetTag calibration data"};

  /** Treatment of histograms: */
  bool m_normalizedProb;
  bool m_interpolate;
  int m_smoothNTimes;
  std::vector<std::string> m_vetoSmoothingOf;

  std::vector<std::string> m_histograms;

};

}
#endif /// JETTAGTOOLS_LIKELIHOODMULTIDTOOL_H

