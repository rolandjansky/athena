/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include <QuickAna/TruthJetTools.h>

#include <QuickAna/AnaToolRetrieve.h>
#include <QuickAna/DefinitionArgs.h>
#include <QuickAna/DefinitionMaker.h>
#include <QuickAna/MessageCheck.h>

namespace ana
{

  //---------------------------------------------------------------------------
  // TruthJetToolCorrect
  //---------------------------------------------------------------------------
  TruthJetToolCorrect::TruthJetToolCorrect (const std::string& name)
    : AsgTool(name), AnaToolCorrect<xAOD::JetContainer>(name)
  {}

  //---------------------------------------------------------------------------
  StatusCode TruthJetToolCorrect::initialize()
  {
    ATH_MSG_DEBUG ("initialize");
    return StatusCode::SUCCESS;
  }

  //---------------------------------------------------------------------------
  StatusCode TruthJetToolCorrect::correctObject (xAOD::Jet& /*jet*/)
  {
    ATH_MSG_DEBUG ("correctObject");

    // Accept all jets for now!
    return StatusCode::SUCCESS;
  }


  //---------------------------------------------------------------------------
  // TruthJetToolSelect
  //---------------------------------------------------------------------------
  TruthJetToolSelect::TruthJetToolSelect (const std::string& name)
    : AsgTool(name), AnaToolSelect<xAOD::JetContainer>(name)
  {}

  //---------------------------------------------------------------------------
  StatusCode TruthJetToolSelect::initialize()
  {
    ATH_MSG_DEBUG ("initialize");
    return StatusCode::SUCCESS;
  }

  //---------------------------------------------------------------------------
  StatusCode TruthJetToolSelect::selectObject (xAOD::Jet& /*jet*/)
  {
    ATH_MSG_DEBUG ("selectObject");
    // Accept all jets for now!
    return StatusCode::SUCCESS;
  }

  //---------------------------------------------------------------------------
  // Truth jet tool maker function
  //---------------------------------------------------------------------------
  StatusCode makeTruthJetTool (DefinitionArgs& args,const std::string &truthCollection)
  {
    using namespace msgObjectDefinition;

    ANA_MSG_INFO ("ana::makeTruthJetTool");

    // Retrieval tool
    std::unique_ptr<IAnaTool> toolRetrieve
      (new AnaToolRetrieve(args.prefix() + "_retrieve", truthCollection));
    args.add(std::move(toolRetrieve));

    // Correction tool
    std::unique_ptr<TruthJetToolCorrect> toolCorrect
      (new TruthJetToolCorrect(args.prefix() + "_correct"));
    args.add(std::move(toolCorrect));

    // Selection tool
    std::unique_ptr<TruthJetToolSelect> toolSelect
      (new TruthJetToolSelect(args.prefix() + "_select"));
    args.add(std::move(toolSelect));

    return StatusCode::SUCCESS;
  }

  QUICK_ANA_JET_DEFINITION_MAKER ("truth", makeTruthJetTool(args,"AntiKt4TruthJets"))
  QUICK_ANA_JET_DEFINITION_MAKER ("truthWZ", makeTruthJetTool(args,"AntiKt4TruthWZJets"))

} // namespace ana
