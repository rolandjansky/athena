/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRIGINDETEVENT_TRIGHISTO2D_H
#define TRIGINDETEVENT_TRIGHISTO2D_H

#include "AthenaKernel/CLASS_DEF.h"

#include "TrigHisto.h"
#include "TrigHisto1D.h"

/** @class TrigHisto2D
 *
 * @author W. H. Bell <W.Bell@cern.ch>
 * 
 * @brief A very basic two dimensional histogram to provide storage of
 * HLT distributions, allowing constraints but preventing excessive
 * memory usage for busy events.  The histogram data is compressed
 * during persistification according to the type of template
 * instantiated.
 *
 */
class TrigHisto2D: public TrigHisto {
 public:
  
  /** Default constructor used by T/P converters. */
  TrigHisto2D(void);

  /** Standard constructor used by FEX algorithms. */ 
  TrigHisto2D(unsigned int nbins_x, float min_x, float max_x,
	      unsigned int nbins_y, float min_y, float max_y);

  TrigHisto2D(unsigned int nbins_x, float min_x, float max_x,
              unsigned int nbins_y, float min_y, float max_y,
              const std::vector<float>& contents);

  /** Destructor */
  virtual ~TrigHisto2D(void);
  
  /** Copy Constructor */
  TrigHisto2D(const TrigHisto2D& trigHisto);
  TrigHisto2D(TrigHisto2D&& trigHisto);
  
  /** Assignment operator */
  TrigHisto2D& operator=(const TrigHisto2D& trigHisto);
  TrigHisto2D& operator=(TrigHisto2D&& trigHisto);
  
  /** Fill a 2D histogram */
  void fill(float value_x, float value_y, float weight);
  
  /** Sum the number of entries within the cut range */
  double sumEntries(float value_x, float value_y, int cutType) const;

  /** Collapse the y-axis and return a profile from the x-axis */
  TrigHisto1D profileX(void) const;

  /** Collapse the x-axis and return a profile from the y-axis */
  TrigHisto1D profileY(void) const;

  /** Return the number of bins along the y-axis, not 
  * including the under and overflow. */
  unsigned int nbins_y(void) const {
    return m_nbins_y;
  }

  /** Return the minimum along the y-axis. */
  float min_y(void) const {
    return m_min_y;
  }

  /** Return the maximum along the y-axis. */
  float max_y(void) const {
    return m_max_y;
  }
  

 protected:
  // The other dimension of histogram limits
  unsigned int m_nbins_y;
  unsigned int m_underflowBin_y;
  unsigned int m_overflowBin_y;
  float m_min_y;
  float m_max_y;
  float m_binSize_y;
};


// obtained using clid -m "TrigHisto2D"
CLASS_DEF( TrigHisto2D , 10655800 , 1 )


#endif
