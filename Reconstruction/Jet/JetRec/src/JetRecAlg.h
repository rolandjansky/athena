// this is a -*- C++ -*- file
/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

////////////////////////////////////////////////////
/// \class JetRecAlg
///
/// Algorithm tasked to create a single JetContainer
/// This algorithm makes use of 2 types of tools :
///   - a IJetProvider : used to build "empty" jets (for example : running fastjet on input, grooming an existing JetContainer or copying an existing JetContainer)
///   - a list of IJetModifier : each modifier is run on the created JetContainer to filter, sort and/or add moments to the jets.
///
///

#ifndef JetRecAlg_H
#define JetRecAlg_H

#include "AthenaBaseComps/AthReentrantAlgorithm.h"
#include "GaudiKernel/ToolHandle.h"
#include "StoreGate/WriteHandleKey.h"

#include "JetInterface/IJetProvider.h"
#include "JetInterface/IJetModifier.h"

#if !defined(GENERATIONBASE) && !defined(XAOD_ANALYSIS)
  #include "AthenaMonitoringKernel/GenericMonitoringTool.h"
#endif

class IJetExecuteTool;

class JetRecAlg : public AthReentrantAlgorithm {

public:

  using AthReentrantAlgorithm::AthReentrantAlgorithm;

  /// Athena algorithm's Hooks
  StatusCode  initialize() override;
  StatusCode  execute(const EventContext& ctx) const override;
  StatusCode  finalize() override;

private:

  /// Athena configured tools
  ToolHandle<IJetProvider> m_jetprovider ={this , "Provider" , {} , "Tool providing the jets (fastjet, copy, grooming...)"};
  ToolHandleArray<IJetModifier> m_modifiers = {this , "Modifiers", {}, "moment calculators" };
  SG::WriteHandleKey<xAOD::JetContainer> m_output= {this, "OutputContainer", "AntiKt4LCtopoJets", "The output jet container name"};
#if !defined (GENERATIONBASE) && !defined (XAOD_ANALYSIS)
  ToolHandle<GenericMonitoringTool> m_monTool{this,"MonTool","","Monitoring tool"};
#endif

};

#endif
