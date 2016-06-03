/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef JETTAGTOOLS_LIKELIHOODTOOL_H
#define JETTAGTOOLS_LIKELIHOODTOOL_H
/******************************************************
    @class LikelihoodTool
    Package : PhysicsAnalysis/JetTagging/JetTagTools
    Created : January 2005
    DESCRIPTION:
    Helper class for likelihood calculations
    @author Andreas.Wildauer@cern.ch
********************************************************/

#include "AthenaBaseComps/AthAlgTool.h"
#include <string>
#include <vector>
#include <map>
#include "JetTagTools/HistoLimits.h"

class ITHistSvc;
class TH1;
// typedef ITHistSvc histoservice;
// typedef TH1 histo;

namespace Analysis
{

static const InterfaceID IID_LikelihoodTool("Analysis::LikelihoodTool", 1, 0);

class LikelihoodTool : public AthAlgTool
{
 public:

  LikelihoodTool(const std::string&,const std::string&,const IInterface*);
  virtual ~LikelihoodTool();

  /** AlgTool initailize method */
  StatusCode initialize();
  /** AlgTool finalize method */
  StatusCode finalize();

  static const InterfaceID& interfaceID() { return IID_LikelihoodTool; };

  void readInHistosFromFile(const std::string& refFileName);
  void setLhVariableValue(const std::string& lhVariable, std::vector<double>& value);
  void setLhVariableValue(const std::string& lhVariable, double value);
  std::vector<double> calculateLikelihood();
  std::vector<double> tagLikelihood();
  double calculateWeight();
  void printStatus();

 private:

  ITHistSvc* m_histoSvc;

  // Filled once at the beginning of an run
  std::vector<std::string> m_allLhVariables; //!< names of all likelihood variables in the histogram file
  std::vector<std::string> m_useTheseLhVariables; //!< names of lh variables which one wants to use (subset of m_allLhVariables)

  /** For every histogram input file this vector contains a map
  with the names of the histograms as a key and the pointer to the histograms. It is a vector because
  there can be 2+ (sig, bkg1, bkg2, ...) histogram input files.
  */
  std::vector< std::map<std::string, TH1*>* > m_mapOfAllHistos;

  /** To hold the histo limits. If a lh variable is out of range it should use the entries
  in the first or the last bin.
  */
  std::map<std::string, HistoLimits> m_histoLimitsMap;

  // Filled once per event
  /** Contains the value of the likelihood variable with the key std::string.
  It is necessary that the value of a likelihood variable is stored with a key to the
  histogram it should be compared to. Otherwise the likelihood class never knows which lh valuable
  it handles when only a subset of variables is used. It has a vector of double because some LH variabels
  are there more than once per jet/event (e.g. tracksignificance of LifetimeTag). */
  std::map<std::string, std::vector<double> > m_lhVariableValues;

  /** Contains the combined likelihood for each input file category.
  Usually input 1 are the signal distributions - so the first entry is the sigLh of the event. */
  std::vector<double> m_likelihoodVector;
};

}
#endif /// JETTAGTOOLS_LIKELIHOODTOOL_H

