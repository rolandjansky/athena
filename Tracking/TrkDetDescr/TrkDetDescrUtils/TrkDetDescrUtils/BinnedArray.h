/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// BinnedArray.h, (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////

#ifndef TRKDETDESCRUTILS_BINNEDARRAY_H
#define TRKDETDESCRUTILS_BINNEDARRAY_H

#include "TrkDetDescrUtils/BinUtility.h"
#include "TrkDetDescrUtils/SharedObject.h"
// GaudiKernel
#include "GaudiKernel/GaudiException.h"
// Eigen
#include "GeoPrimitives/GeoPrimitives.h"
// STL
#include <vector>

class MsgStream;

namespace Trk {

/** @class BinnedArray

   Binned Array for avoiding map searches/

   @author Andreas.Salzburger@cern.ch
   */

template<class T>
class BinnedArrayT
{

public:
  /**Default Constructor - needed for inherited classes */
  BinnedArrayT() {}

  /**Virtual Destructor*/
  virtual ~BinnedArrayT() {}

  /** Implicit constructor */
  virtual BinnedArrayT* clone() const = 0;

  /** Returns the pointer to the templated class object from the BinnedArrayT,
      it returns 0 if not defined, takes local position */
  virtual T* object(const Amg::Vector2D& lp) const = 0;

  /** Returns the pointer to the templated class object from the BinnedArrayT
      it returns 0 if not defined, takes global position */
  virtual T* object(const Amg::Vector3D& gp) const = 0;

  /** Returns the pointer to the templated class object from the BinnedArrayT - entry point*/
  virtual T* entryObject(const Amg::Vector3D&) const = 0;

  /** Returns the pointer to the templated class object from the BinnedArrayT, takes 3D position & direction */
  virtual T* nextObject(const Amg::Vector3D& gp,
                              const Amg::Vector3D& mom,
                              bool associatedResult = true) const = 0;

  /** Return all objects of the Array */
  virtual const std::vector<T*>& arrayObjects() const = 0;

  /** Number of Entries in the Array */
  virtual unsigned int arrayObjectsNumber() const = 0;

  /** Return the BinUtility*/
  virtual const BinUtility* binUtility() const = 0;

private:
};
template<class T>
using BinnedArray=BinnedArrayT<const T>;
} // end of namespace Trk

#endif // TRKDETDESCRUTILS_BINNEDARRAY_H
