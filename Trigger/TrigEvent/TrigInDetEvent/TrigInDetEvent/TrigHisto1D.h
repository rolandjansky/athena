/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRIGINDETEVENT_TRIGHISTO1D_H
#define TRIGINDETEVENT_TRIGHISTO1D_H

#include "AthenaKernel/CLASS_DEF.h"

#include "TrigHisto.h"

/** @class TrigHisto1D
 *
 * @author W. H. Bell <W.Bell@cern.ch>
 * 
 * @brief A very basic one dimensional histogram to provide storage of
 * HLT distributions, allowing constraints but preventing excessive
 * memory usage for busy events.  The histogram data is compressed
 * during persistification according to the type of template
 * instantiated.
 *
 */
class TrigHisto1D: public TrigHisto {
 public:

  /** Default constructor used by T/P converters. */
  TrigHisto1D(void);

  /** Standard constructor used by FEX algorithms. */ 
  TrigHisto1D(unsigned int nbins_x, float min_x, float max_x);

  /** Constructor used by TrigHisto2D */
  TrigHisto1D(unsigned int nbins_x, float min_x, float max_x,
              const std::vector<float>& contents);

  /** Destructor */
  virtual ~TrigHisto1D(void);

  /** Copy Constructor */
  TrigHisto1D(const TrigHisto1D& trigHisto);
  TrigHisto1D(TrigHisto1D&& trigHisto);

  /** Assignment operator */
  TrigHisto1D& operator=(const TrigHisto1D& trigHisto);
  TrigHisto1D& operator=(TrigHisto1D&& trigHisto);

  /** Fill a 1D histogram. */
  void fill(float value_x, float weight);

  /** Sum the number of entries within the cut range */
  double sumEntries(float value_x, int cutType) const;

};

// obtained using clid -m "TrigHisto1D" etc
CLASS_DEF( TrigHisto1D , 11655925 , 1 )

#endif
