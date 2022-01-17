/*
  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
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

/**
This stores information, a rank and ring thickness, about cell rings in an ordered way. The rings are ranked in descending energy density order. A ring is a list of calorimeter cells, in a given calorimeter layer, between two radii r1 and r2. This object is stored in the eflowRecTrack object, such that we have an eflowRingSubtractionManager for each track. The data in this object is filled in the simulateShower method of eflowCaloObject.
*/
class eflowRingSubtractionManager {

 public:

  eflowRingSubtractionManager();
  ~eflowRingSubtractionManager() {}

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
  static double ringMeanRank(const std::vector<double>& p, double rMin, double rMax) ;
  static std::vector<eflowCaloENUM> orderedLayersByShowerParam(const eflowFirstIntParameters& param, unsigned int whichParam=0);

 private:

  std::map<double,RingId> m_rankMap;
  std::vector<double> m_ringThickness;
  double m_fudgeMean;
  double m_fudgeStdDev;

  static constexpr double m_rMax = 0.75;
  static constexpr double m_weightRange = 1.0e6;
};
#endif
