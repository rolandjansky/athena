/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// IndexedConstituentUserInfo.h

#ifndef jet__IndexedConstituentUserInfo_H
#define jet__IndexedConstituentUserInfo_H

// David Adams and Pierre-Antoine Delsart
// January 2014

/// @class IndexedConstituentUserInfo
/// IndexedConstituentUserInfo is an implementation of IConstituentUserInfo,
/// i.e.\ it provides means to attach a particle, label and vertex to a
/// fastjet pseudojet.
///
/// The particle is held by pointer, the label by index (along with a pointer
/// to the label index to dedcode it), and the vertex is missing.
/// The index 0 corresponds to no label and negative values indicate the
/// constituent is a ghost, i.e. has very low energy and so has little effect on
/// the jet four vector.

#include "JetEDM/BaseIndexedConstituentUserInfo.h"

namespace jet {

  class IndexedConstituentUserInfo : public BaseIndexedConstituentUserInfo {

public:

  // Default ctor.
  // Sets the particle null.
  IndexedConstituentUserInfo();

  /// Constructor from IParticle, index and map.
  IndexedConstituentUserInfo(const xAOD::IParticle& par, Index idx, const LabelIndex* pli);

  /// Return the particle pointer.
  const xAOD::IParticle* particle() const;

protected:

  const xAOD::IParticle* m_ppar;

};

}  // end namespace jet

#endif
