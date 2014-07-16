/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// BaseIndexedConstituentUserInfo.h

#ifndef jet__BaseIndexedConstituentUserInfo_H
#define jet__BaseIndexedConstituentUserInfo_H

// David Adams and Pierre-Antoine Delsart
// January 2014

/// @class BaseIndexedConstituentUserInfo
/// BaseIndexedConstituentUserInfo is an base implementation of
/// IConstituentUserInfo.
/// It provides means to attach an label and vertex to a fastjet pseudojet.
/// Subclasses add the pointer to an IParticle or other object.
///
/// The label is held by index (along with a pointer to the label index
/// to decode it). The vertex is missing.
/// The index 0 corresponds to no label and negative values indicate the
/// constituent is a ghost, i.e. has very low energy and so has little effect on
/// the jet four vector.

#include "JetEDM/IConstituentUserInfo.h"

namespace jet {

  class BaseIndexedConstituentUserInfo : public IConstituentUserInfo {

public:

  // Default ctor.
  BaseIndexedConstituentUserInfo();

  /// Constructor from index and map.
  BaseIndexedConstituentUserInfo(Index idx, const LabelIndex* pli);

  /// Return the particle pointer.
  /// Method particle is implemented in subclass.
  const SG::AuxElement* element() const;

  /// Return the label.
  Label label() const;

  /// Return the index.
  Index index() const;

  /// Return the index-to-label map.
  const LabelIndex* labelMap() const;

  /// Returns true if this constituent is a ghost
  bool isGhost() const ;

  /// Return the constituent scale used to build the PseudoJet
  xAOD::JetConstitScale constitScale() const ;

 protected:

  int m_idx;
  const LabelIndex* m_pli;

};

}  // end namespace jet

#endif
