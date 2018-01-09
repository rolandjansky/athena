/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// CompactBinnedArray.h, (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////

#ifndef TRKDETDESCRUTILS_COMPACTBINNEDARRAY_H
#define TRKDETDESCRUTILS_COMPACTBINNEDARRAY_H

#include "TrkDetDescrUtils/BinnedArray.h"
#include "TrkDetDescrUtils/BinUtility.h"

//STL
#include <vector>

class MsgStream;

namespace Trk {

/** @class CompactBinnedArray

   defines common utilities needed for implementation of binned material

   @author sarka.todorova@cern.ch
   */

  template <class T> class CompactBinnedArray : public BinnedArray<T> {

  public:
    /**Default Constructor - needed for inherited classes */
    CompactBinnedArray():
    BinnedArray<T>()
      {}
    
    /**Virtual Destructor*/
    virtual ~CompactBinnedArray(){}

    /** Implicit constructor */
    virtual CompactBinnedArray* clone() const = 0;
    
    /** layer bin utility */
    virtual const Trk::BinUtility* layerBinUtility(const Amg::Vector3D& gp) const = 0;

    /** layer bin  */
    virtual size_t layerBin(const Amg::Vector3D& gp) const = 0;

  };


} // end of namespace Trk

#endif // TRKDETDESCRUTILS_COMPACTBINNEDARRAY_H

