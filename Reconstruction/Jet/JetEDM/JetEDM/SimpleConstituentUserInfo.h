/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// SimpleConstituentUserInfo.h

#ifndef jet__SimpleConstituentUserInfo_H
#define jet__SimpleConstituentUserInfo_H

// David Adams
// January 2014
//
// Implementation of IConstituentUserInfo without indexing.

#include "JetEDM/IConstituentUserInfo.h"

namespace jet {

class SimpleConstituentUserInfo : public IConstituentUserInfo {

public:

  // Default ctor.
  // Sets the particle null.
  SimpleConstituentUserInfo();

  // Ctor from IParticle and label.
  SimpleConstituentUserInfo(const xAOD::IParticle& par, std::string label);

  // Return the particle pointer.
  const xAOD::IParticle* particle() const;

  // Return the label.
  Label label() const;

private:

  const xAOD::IParticle* m_ppar;
  Label m_label;

};

}  // end namespace jet

#endif
