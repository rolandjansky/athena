/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// IConstituentUserInfo.h

#ifndef jet__IConstituentUserInfo_H
#define jet__IConstituentUserInfo_H

// David Adams
// January 2014
//
// Interface for ATLAS user info for fastjet pseudojet.
// The user info includes a pointer to an IParticle and a label
// for the collection from which the pseudojet was constructed.
// The labels may be indexed (mapped to integer indices).

#include "fastjet/PseudoJet.hh"
#include "xAODBase/IParticleContainer.h"
#include "xAODJet/JetTypes.h"

namespace jet {

class LabelIndex;

class IConstituentUserInfo : public fastjet::PseudoJet::UserInfoBase {

public:

  typedef int Index;
  typedef std::string Label;

  // Return the particle pointer.
  virtual const SG::AuxElement* element() const =0;
  virtual const xAOD::IParticle* particle() const =0;

  // Return the label.
  virtual Label label() const =0;

  // Return the index.
  // Returns 0 if undefined.
  virtual Index index() const;

  // Return the index-to-label map.
  // Returns null if undefined.
  virtual const LabelIndex* labelMap() const;

  /// Returns true if this constituent is a ghost
  virtual bool isGhost() const ;

  /// Return the constituent scale used to build the PseudoJet
  virtual xAOD::JetConstitScale constitScale() const ;


};

}  // end jet namespace

#endif
