/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// IndexedTConstituentUserInfo.h

#ifndef jet__IndexedTConstituentUserInfo_H
#define jet__IndexedTConstituentUserInfo_H

// David Adams and Pierre-Antoine Delsart
// January 2014

/// @class IndexedTConstituentUserInfo
/// IndexedTConstituentUserInfo is an implementation of IConstituentUserInfo,
/// i.e.\ it provides means to attach an aux element, label and vertex to a
/// fastjet pseudojet.
///
/// To hold a particle (IParticle), use IndexedConstituentUserInfo.
///
/// The element is held by pointer, the label by index (along with a pointer
/// to the label index to dedcode it), and the vertex is missing.
/// The index 0 corresponds to no label and negative values indicate the
/// constituent is a ghost, i.e. has very low energy and so has little effect on
/// the jet four vector.

#include "JetEDM/BaseIndexedConstituentUserInfo.h"

namespace jet {

template<typename T>
class IndexedTConstituentUserInfo : public BaseIndexedConstituentUserInfo {

public:

  // Default ctor.
  // Sets the particle null.
  IndexedTConstituentUserInfo();

  /// Constructor from an object, index and map.
  /// The object must be IParticle or at least AuxElement.
  IndexedTConstituentUserInfo(const T& par, Index idx, const LabelIndex* pli);

  /// Return the particle pointer.
  const SG::AuxElement* element() const;
  const xAOD::IParticle* particle() const;
  const T* object() const;

 protected:

  const T* m_ppar;

};

}  // end namespace jet

#include "JetEDM/IndexedTConstituentUserInfo.icc"

#endif
