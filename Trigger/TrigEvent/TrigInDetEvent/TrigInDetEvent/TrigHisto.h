/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRIGINDETEVENT_TRIGHISTO_H
#define TRIGINDETEVENT_TRIGHISTO_H

#include <vector>

//---------------------------------------------------------------

namespace TrigHistoCutType {
  enum CutTypeEnum {
    BELOW_X,
    ABOVE_X,
    BELOW_X_BELOW_Y,
    ABOVE_X_BELOW_Y,
    BELOW_X_ABOVE_Y,
    ABOVE_X_ABOVE_Y};
}

//---------------------------------------------------------------

/** @class TrigHisto
 *
 * @author W. H. Bell <W.Bell@cern.ch>
 * 
 * @brief The base class for TrigHisto1D and TrigHisto2D.  This class
 * should never be stored or used directly.
 *
 */
class TrigHisto {
 public:
  TrigHisto(void);
  virtual ~TrigHisto(void);
  
  /** Zero all histogram bins */
  void clear(void);

  /** Return the number of bins along the y-axis, not 
  * including the under and overflow. */
  unsigned int nbins_x(void) const {
    return m_nbins_x;
  }

  /** Return the minimum along the x-axis. */
  float min_x(void) const {
    return m_min_x;
  }

  /** Return the maximum along the x-axis. */
  float max_x(void) const {
    return m_max_x;
  }

  /** Return the bin contents of the histogram, including 
  * the under and overflow bins. */
  const std::vector<float>& contents(void) const {
    return m_contents;
  }

 protected:
  /**
   * @return which bin this value corresponds to.
   * (Supply bin limits such that it might be used for 1D or 2D derived class.)
   */
  unsigned int findBin(unsigned int nbins, 
		       float h_min, float h_max, float binSize, 
		       float value) const;

  std::vector<float> m_contents; //<! A vector to contain the contents of the histogram.
  std::vector<float>::iterator m_itr;
  std::vector<float>::iterator m_itr_end;

  // Histogram limits
  unsigned int m_nbins_x;
  unsigned int m_underflowBin_x;
  unsigned int m_overflowBin_x;
  float m_min_x;
  float m_max_x;
  float m_binSize_x;
};

#endif
