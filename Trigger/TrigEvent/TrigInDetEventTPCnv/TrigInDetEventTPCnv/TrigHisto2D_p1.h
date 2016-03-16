/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRIGINDETEVENTTPCNV_TRIGHISTO2D_P1_H
#define TRIGINDETEVENTTPCNV_TRIGHISTO2D_P1_H

#include <vector>

/** @class TrigHisto2D_p1
 *
 * @author W. H. Bell <W.Bell@cern.ch>
 * 
 * @brief The persistent partner of the TrigHisto2D class.
 *
 */
class TrigHisto2D_p1 {
 public:
  TrigHisto2D_p1() : m_nbins_x(0), m_min_x(0.), m_max_x(0),
                     m_nbins_y(0), m_min_y(0.), m_max_y(0) {}
    
  //private:
  std::vector<float> m_contents;
  unsigned int m_nbins_x;
  float m_min_x;
  float m_max_x;
  unsigned int m_nbins_y;
  float m_min_y;
  float m_max_y;
};

#endif

