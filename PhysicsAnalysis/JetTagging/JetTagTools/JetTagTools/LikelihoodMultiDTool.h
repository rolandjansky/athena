/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef JETTAGTOOLS_LIKELIHOODMULTIDTOOL_H
#define JETTAGTOOLS_LIKELIHOODMULTIDTOOL_H
/******************************************************
    @class LikelihoodMultiDTool
    DESCRIPTION:
    Helper class for likelihood calculations
********************************************************/

#include "AthenaBaseComps/AthAlgTool.h"
#include <string>
#include <vector>
#include <map>

#include "JetTagTools/HistoLimits.h"

#include "JetTagTools/LikelihoodComponents.h"

class ITHistSvc;
class TH1;

namespace Analysis
{

static const InterfaceID IID_LikelihoodMultiDTool("Analysis::LikelihoodMultiDTool", 1, 0);
  
class LikelihoodMultiDTool : public AthAlgTool
{

 public:

  LikelihoodMultiDTool(const std::string&,const std::string&,const IInterface*);
  virtual ~LikelihoodMultiDTool();
  
  /** AlgTool initailize method */
  StatusCode initialize();
  /** AlgTool finalize method */
  StatusCode finalize();
  
  static const InterfaceID& interfaceID() { return IID_LikelihoodMultiDTool; };
  
  void prepareHistosFromFile(const std::string& refFileName);
  TH1* retrieveHistoFromFile(const std::string& refFileName, 
    const std::string& histoName, const std::string& jetPrefix);
  void setLhVariableValue(std::vector<Slice>& value);
  void setLhVariableValue(Slice& value);
  std::vector<double> calculateLikelihood();
  std::vector<double> tagLikelihood();
  void printStatus();
  double getEff(const std::string& , const std::string& , const std::string&);
  void addLhVariableToUse(const std::string&);
  inline void resetLhVariableToUse() { m_useTheseLhVariables.clear(); }
  void setInterpolFlag(bool f) {m_DoInterpol = f;}
  void defineNbHypotheses(int n) { m_nbHypotheses = n; }

 private:

  ITHistSvc* m_histoSvc;
  
  // Filled once at the beginning of an run
  //std::vector<std::string> m_allLhVariables; //!< names of all likelihood variables in the histogram file
  std::vector<std::string> m_useTheseLhVariables; //!< names of lh variables which one wants to use (subset of m_allLhVariables)
  
  /** For every histogram input file this vector contains a map
  with the names of the histograms as a key and the pointer to the histograms. It is a vector because
  there can be 2+ (sig, bkg1, bkg2, ...) histogram input files.
  */
  std::vector< std::map<std::string, TH1*>* > m_mapOfAllHistos; 
    
  /** To hold the histo limits. If a lh variable is out of range it should use the entries
  in the first or the last bin.
  */
  std::map<std::string, HistoLimits> m_histoLimitsMap1D;
  std::map<std::string, HistoLimits> m_histoLimitsMap2D;
  std::map<std::string, HistoLimits> m_histoLimitsMap3D;

  // Filled once per event
  /** Contains the value of the likelihood variable with the key std::string.
  It is necessary that the value of a likelihood variable is stored with a key to the
  histogram it should be compared to. Otherwise the likelihood class never knows which lh valuable
  it handles when only a subset of variables is used. It has a vector of double because some LH variabels
  are there more than once per jet/event (e.g. tracksignificance of LifetimeTag). */
  std::vector<Slice> m_lhVariableValues;
  
  /** Contains the combined likelihood for each input file category.
  Usually input 1 are the signal distributions - so the first entry is the sigLh of the event. */
  std::vector<double> m_likelihoodVector;

  /** for debugging: */
  int m_nhis1D;
  int m_nhis2D;
  int m_nhis3D;

  //
  bool m_DoInterpol;
  bool m_WriteSmoothedHistos;

  unsigned int m_nbHypotheses; // nb of hypotheses (default 2: b | u)

  // for soft-mu:
  int  m_nSmooth1D;
  bool m_normProb;

};

}
#endif /// JETTAGTOOLS_LIKELIHOODMULTIDTOOL_H

