/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// LayerIndexCalculator.h, (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////

#ifndef TRKDETDESCRUTILS_LAYERINDEX_H
#define TRKDETDESCRUTILS_LAYERINDEX_H

// Gaudi
#include "GaudiKernel/MsgStream.h"
// STD
#include <iostream>

// for the robust calculation of layer indices
#ifndef TRKDETDESCR_GEOMETRYSIGNATUREWEIGHT
#define TRKDETDESCR_GEOMETRYSIGNATUREWEIGHT 10000
#endif

#ifndef TRKDETDESCR_MAXROBUSTINDEX
#define TRKDETDESCR_MAXROBUSTINDEX 100000
#endif

namespace Trk {

/** @class LayerIndex

   LayerIndex for the identification of layers in a
   simplified detector geometry of Cylinders and Discs.
   
   @author Andreas.Salzburger@cern.ch
  */

class LayerIndex
{

public:
  /** Default Constructor */
  LayerIndex() = default;

  /** Constructor with value*/
  LayerIndex(int value)
    : m_value(value)
  {}

  /** Copy Constructor */
  LayerIndex(const LayerIndex& layIx) = default;

  /** Move Constructor */
  LayerIndex(LayerIndex&& layIx) = default;

  /** Assignment Operator */
  LayerIndex& operator=(const LayerIndex& layIx) = default;

  /** Move Assignment Operator */
  LayerIndex& operator=(LayerIndex&& layIx) = default;

  /** Destructor */
  ~LayerIndex() = default;

  /** layerIndex expressed in an integer */
  int value() const;

protected:
  int m_value=0;
};

inline int
LayerIndex::value() const
{
  return m_value;
}

/** Overload of operator< | <= | > | >=  for the usage in a map */
bool
operator<(const LayerIndex& one, const LayerIndex& two);
bool
operator<=(const LayerIndex& one, const LayerIndex& two);
bool
operator>(const LayerIndex& one, const LayerIndex& two);
bool
operator>=(const LayerIndex& one, const LayerIndex& two);

/**Overload of << operator for both, MsgStream and std::ostream for debug
 * output*/
MsgStream&
operator<<(MsgStream& sl, const LayerIndex& layx);
std::ostream&
operator<<(std::ostream& sl, const LayerIndex& layx);
}

#endif
