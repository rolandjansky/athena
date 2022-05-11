/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TileCalibObjects_TileSimpleStat_h
#define TileCalibObjects_TileSimpleStat_h

#include <cmath>

class TileSimpleStat {
 public:
  TileSimpleStat():m_entries(0),m_sum(0.0),m_sqSum(0.0) {
  }
  
  inline void add(double val) {
    m_entries++;
    m_sum += val;
    m_sqSum += val*val;
  }
  
  inline void getStat(double &mean, double &sigma) {
    mean = m_sum/static_cast<double>(m_entries);
    sigma = sqrt((m_sqSum - static_cast<double>(m_entries)*mean*mean)/static_cast<double>(m_entries-1));
  }

 private:
  unsigned int m_entries;
  double m_sum;
  double m_sqSum;
};

#endif // TileCalibObjects_TileSimpleStat_h
