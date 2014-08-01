/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef EFLOWCELLSUBTRACTIONMANAGER_H
#define EFLOWCELLSUBTRACTIONMANAGER_H

/********************************************************************

NAME:     eflowCellSubtractionManager.h
PACKAGE:  offline/Reconstruction/eflowRec

AUTHORS:  M.Hodgkinson, R Duxfield (based on R.Duxfields Root package)
CREATED:  18th Aug, 2005

********************************************************************/

//C++ Headers
#include <vector>
#include <map>

#include "eflowRec/eflowFirstIntParameters.h"

typedef std::pair<eflowCaloENUM,short> RingId;

class eflowCellSubtractionManager :  public eflowFirstInt {

 public:

  eflowCellSubtractionManager();
  eflowCellSubtractionManager(const eflowCellSubtractionManager& other);
  ~eflowCellSubtractionManager() {}

  void setParameters(const eflowFirstIntParameters& param, const std::vector<double>& ringThickness, double rMax, double range);
  void clear()  {m_rankMap.clear();}

  std::map<double,RingId>::const_iterator rankBegin() const  {return m_rankMap.begin();}
  std::map<double,RingId>::const_iterator rankEnd() const  {return m_rankMap.end();}
    
  double ringThickness(eflowCaloENUM layer) const  {return m_ringThickness[layer];}

 private:

  double ringMeanRank(const std::vector<double>& p, double rMin, double rMax) const;

  std::vector<double> m_ringThickness;
  std::map<double,RingId> m_rankMap;

};
#endif
