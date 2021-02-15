/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// CompactBinnedArray.h, (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////

#ifndef TRKDETDESCRUTILS_COMPACTBINNEDARRAY_H
#define TRKDETDESCRUTILS_COMPACTBINNEDARRAY_H

#include "TrkDetDescrUtils/BinUtility.h"
#include "TrkDetDescrUtils/BinnedArray.h"

// STL
#include <vector>

class MsgStream;

namespace Trk {

/** @class CompactBinnedArrayT

   defines common utilities needed for implementation of binned material

   @author sarka.todorova@cern.ch
   */

template<class T>
class CompactBinnedArrayT : public BinnedArrayT<T>
{

public:
  /**Default Constructor - needed for inherited classes */
  CompactBinnedArrayT()
    : BinnedArray<T>()
  {}

  /**Virtual Destructor*/
  virtual ~CompactBinnedArrayT() {}

  /** Implicit constructor */
  virtual CompactBinnedArrayT* clone() const = 0;
  virtual CompactBinnedArrayT* clone(const std::vector<T*>& ptrs) const = 0;

  /** layer bin utility */
  virtual const Trk::BinUtility* layerBinUtility(const Amg::Vector3D& gp) const = 0;

  /** layer bin  */
  virtual size_t layerBin(const Amg::Vector3D& gp) const = 0;
};

template<class T>
using CompactBinnedArray = CompactBinnedArrayT < const T>;
} // end of namespace Trk
#endif // TRKDETDESCRUTILS_COMPACTBINNEDARRAY_H
