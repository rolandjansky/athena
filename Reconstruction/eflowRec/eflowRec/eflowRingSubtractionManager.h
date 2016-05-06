/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef EFLOWCELLSUBTRACTIONMANAGER_H
#define EFLOWCELLSUBTRACTIONMANAGER_H

/********************************************************************

NAME:     eflowRingSubtractionManager.h
PACKAGE:  offline/Reconstruction/eflowRec

AUTHORS:  M.Hodgkinson, R Duxfield (based on R.Duxfields Root package)
CREATED:  18th Aug, 2005

********************************************************************/

//C++ Headers
#include <vector>
#include <map>
#include "eflowRec/eflowCaloRegions.h"

class eflowEEtaBinnedParameters;
class eflowFirstIntParameters;

typedef std::pair<eflowCaloENUM,short> RingId;

class eflowRingSubtractionManager {

 public:

  eflowRingSubtractionManager();
  ~eflowRingSubtractionManager() {}

  static void setRMaxAndWeightRange(double rMax, double weightRange) {
    m_rMax = rMax;
    m_weightRange = weightRange;
  }
  double fudgeMean() const    {return m_fudgeMean;}
  double fudgeStdDev() const  {return m_fudgeStdDev;}
  /* for eflowTauTool */
  void setFudgeMean(double fudgeMean)      {m_fudgeMean = fudgeMean;}
  void setFudgeStdDev(double fudgeStdDev)  {m_fudgeStdDev = fudgeStdDev;}

  void clear()  {m_rankMap.clear();}
  std::map<double,RingId>::const_iterator rankBegin() const  {return m_rankMap.begin();}
  std::map<double,RingId>::const_iterator rankEnd() const  {return m_rankMap.end();}
    
  double ringThickness(eflowCaloENUM layer) const  {return m_ringThickness[layer];}

  bool getOrdering(const eflowEEtaBinnedParameters* binnedParameters, double e, double eta, eflowFirstIntENUM j1st);

 private:

  void setParameters(const eflowFirstIntParameters& param, const std::vector<double>& ringThickness);
  double ringMeanRank(const std::vector<double>& p, double rMin, double rMax) const;
  std::vector<eflowCaloENUM> orderedLayersByShowerParam(const eflowFirstIntParameters& param, unsigned int whichParam=0);

 private:

  std::map<double,RingId> m_rankMap;
  std::vector<double> m_ringThickness;
  double m_fudgeMean;
  double m_fudgeStdDev;

  static double m_rMax;
  static double m_weightRange;
};
#endif
