/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// NavBinnedArray1D.h, (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////

#ifndef TRKDETDESCRUTILS_NAVBINNEDARRAY1D_H
#define TRKDETDESCRUTILS_NAVBINNEDARRAY1D_H

#include "TrkDetDescrUtils/BinUtility.h"
#include "TrkDetDescrUtils/SharedObject.h"

// STL
#include <vector>

class MsgStream;

namespace Trk {

/** @class NavBinnedArray1DT

Avoiding a map search, the templated BinnedArray class can help
ordereing geometrical objects by providing a dedicated BinUtility.

For use within navigation objects, global coordinates/transform refer to
the position within mother navigation object

@author Andreas.Salzburger@cern.ch, Sarka.Todorova@cern.ch
*/

template<class T>
class NavBinnedArray1DT : public BinnedArrayT<T>
{

public:
  /**Default Constructor - needed for inherited classes */
  NavBinnedArray1DT()
    : BinnedArrayT<T>()
    , m_array{}
    , m_arrayObjects(nullptr)
    , m_binUtility()
    , m_transf(nullptr)
  {}

  /**Constructor with std::vector and a  BinUtility - reference counted, will
  delete objects at the end, if this deletion should be turned off, the boolean
  deletion should be switched to false the global position is given by pointer
  and then deleted! */
  NavBinnedArray1DT(const std::vector<SharedObject<T>>& tclassvector,
                    BinUtility* bingen,
                    Amg::Transform3D* transform)
    : BinnedArrayT<T>()
    , m_array{}
    , m_arrayObjects(nullptr)
    , m_binUtility(SharedObject<BinUtility>(bingen))
    , m_transf(transform)
  {
    // prepare the binned Array // simplify as the array is ordered when defined
    if (bingen) {
      m_array = std::vector<SharedObject<T>>(tclassvector);
    }
  }

  /**Copy Constructor with shift */
  NavBinnedArray1DT(const NavBinnedArray1DT& barr,
                    std::vector<SharedObject<T>>&& vec,
                    Amg::Transform3D& shift)
    : BinnedArrayT<T>()
    , m_array(std::move(vec))
    , m_arrayObjects{}
    , m_binUtility(barr.m_binUtility)
    , m_transf(new Amg::Transform3D(shift * (*barr.m_transf)))
  {}

  /**Copy Constructor - copies only pointers !*/
  NavBinnedArray1DT(const NavBinnedArray1DT& barr)
    : BinnedArrayT<T>()
    , m_array{}
    , m_arrayObjects(nullptr)
    , m_binUtility(barr.m_binUtility)
    , m_transf(nullptr)
  {
    if (m_binUtility.get()) {
      m_array = std::vector<SharedObject<T>>(m_binUtility.get()->bins(0));
      for (size_t ient = 0; ient < m_binUtility.get()->bins(0); ++ient) {
        m_array[ient] = (barr.m_array)[ient];
      }
    }
    m_transf =
      (barr.m_transf) ? new Amg::Transform3D(*(barr.m_transf)) : nullptr;
  }

  /**Assignment operator*/
  NavBinnedArray1DT& operator=(const NavBinnedArray1DT& barr)
  {
    if (this != &barr) {
      m_arrayObjects.release();
      delete m_transf;
      // now refill
      m_binUtility = barr.m_binUtility;
      // --------------------------------------------------------------------------
      if (m_binUtility.get()) {
        m_array = std::vector<SharedObject<T>>(m_binUtility.get()->bins(0));
        for (size_t ient = 0; ient < m_binUtility.get()->bins(0); ++ient) {
          m_array[ient] = (barr.m_array)[ient];
        }
      }
      m_transf =
        (barr.m_transf) ? new Amg::Transform3D(*barr.m_transf) : nullptr;
    }
    return *this;
  }

  /** Implicit Constructor */
  NavBinnedArray1DT* clone() const { return new NavBinnedArray1DT(*this); }

  /**Virtual Destructor*/
  ~NavBinnedArray1DT()
  {
    delete m_transf;
  }

  /** Returns the pointer to the templated class object from the BinnedArray,
  it returns 0 if not defined;
  */
  T* object(const Amg::Vector2D& lp) const
  {
    if (m_binUtility.get()->inside(lp)){
      return (m_array[m_binUtility.get()->bin(lp)]).get();
    }
    return nullptr;
  }

  /** Returns the pointer to the templated class object from the BinnedArray
  it returns 0 if not defined;
  */
  T* object(const Amg::Vector3D& gp) const
  {
    // transform into navig.coordinates
    const Amg::Vector3D navGP((m_transf->inverse()) * gp);
    if (m_binUtility.get()->inside(navGP)){
      return (m_array[m_binUtility.get()->bin(navGP)]).get();
    }
    return nullptr;
  }

  /** Returns the pointer to the templated class object from the BinnedArray -
   * entry point*/
  T* entryObject(const Amg::Vector3D&) const { return (m_array[0]).get(); }

  /** Returns the pointer to the templated class object from the BinnedArray
   */
  T* nextObject(const Amg::Vector3D& gp,
                const Amg::Vector3D& mom,
                bool associatedResult = true) const
  {
    // transform into navig.coordinates
    const Amg::Vector3D navGP((m_transf->inverse()) * gp);
    const Amg::Vector3D navMom((m_transf->inverse()).linear() * mom);
    // the bins
    size_t firstBin = m_binUtility.get()->next(navGP, navMom, 0);
    // use the information of the associated result
    if (associatedResult) {
      if (firstBin <= m_binUtility.get()->max(0)){
        return (m_array[firstBin]).get();
      }
      else{
        return nullptr;
      }
    }
    // the associated result was 0 -> set to boundary
    firstBin = (firstBin < m_binUtility.get()->bins(0))
                 ? firstBin
                 : m_binUtility.get()->max(0);
    return (m_array[m_binUtility.get()->bin(navGP)]).get();
  }

  /** Return all objects of the Array */
  const std::vector<T*>& arrayObjects() const
  {
    if (!m_arrayObjects) {
      std::unique_ptr<std::vector<T*>> arrayObjects =
        std::make_unique<std::vector<T*>>();
      for (unsigned int ill = 0; ill < m_array.size(); ++ill) {
        arrayObjects->push_back((m_array[ill]).get());
      }
      m_arrayObjects.set(std::move(arrayObjects));
    }
    return (*m_arrayObjects);
  }

  /** Number of Entries in the Array */
  unsigned int arrayObjectsNumber() const { return arrayObjects().size(); }

  /** Return the BinUtility*/
  const BinUtility* binUtility() const { return (m_binUtility.get()); }

  /** Return the transform*/
  Amg::Transform3D* transform() const { return (m_transf); }

  /** Reposition */
  void updateTransform(Amg::Transform3D& transform)
  {
    Amg::Transform3D* tr = m_transf;
    m_transf = new Amg::Transform3D(transform * (*m_transf));
    delete tr;
  }

private:
  //!< vector of pointers to the class T
  std::vector<SharedObject<T>> m_array;
  //!< forced 1D vector of pointers to class T
  CxxUtils::CachedUniquePtr<std::vector<T*>> m_arrayObjects;
  //!< binUtility for retrieving and filling the Array
  SharedObject<BinUtility> m_binUtility;
  // !< transform into local navigation coordinates

  Amg::Transform3D* m_transf;
};
template<class T>
using NavBinnedArray1D = NavBinnedArray1DT<const T>;

} // end of namespace Trk

#endif // TRKDETDESCRUTILS_NAVBINNEDARRAY1D_H
