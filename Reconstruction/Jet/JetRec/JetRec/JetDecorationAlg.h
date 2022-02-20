// this is a -*- C++ -*- file
/*
  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
*/

////////////////////////////////////////////////////
/// \class JetDecorationAlg
///
/// Algorithm which applies a set of decorations to a jet collection.
/// Takes a list of IJetDecorator and applies them all to the jet collection (in order).
///

#ifndef JetDecorationAlg_H
#define JetDecorationAlg_H

#include "AnaAlgorithm/AnaReentrantAlgorithm.h"
#include "AsgTools/ToolHandle.h"
#include "AsgDataHandles/ReadHandleKey.h"

#include "JetInterface/IJetDecorator.h"
#include "xAODJet/JetContainer.h"

class JetDecorationAlg : public EL::AnaReentrantAlgorithm {

public:

  using EL::AnaReentrantAlgorithm::AnaReentrantAlgorithm;

  /// Athena algorithm's Hooks
  StatusCode  initialize() override;
  StatusCode  execute(const EventContext& ctx) const override;

private:

  ToolHandleArray<IJetDecorator> m_decorators = {this , "Decorators", {}, "Jet decorators" };
  SG::ReadHandleKey<xAOD::JetContainer> m_jetKey= {this, "JetContainer", "", "Jet container name"};

};

#endif
