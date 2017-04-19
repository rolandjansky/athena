/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

//
// includes
//

#include <QuickAna/SusyORTool.h>

// CP OR Tools
#include <AssociationUtils/EleMuSharedTrkOverlapTool.h>
#include <AssociationUtils/EleJetOverlapTool.h>
#include <AssociationUtils/MuJetOverlapTool.h>
#include <AssociationUtils/AltMuJetOverlapTool.h>
#include <AssociationUtils/DeltaROverlapTool.h>

// Local package includes
#include <QuickAna/DefinitionArgs.h>
#include <QuickAna/DefinitionMaker.h>
#include <QuickAna/IEventObjects.h>
#include <QuickAna/InternalConfiguration.h>
#include <QuickAna/MessageCheck.h>

// Framework includes
#include "CxxUtils/make_unique.h"

// Same helper function as OR tool in an anonymous namespace
namespace
{

  //---------------------------------------------------------------------------
  // Helper function to create and pre-configure one overlap tool
  //---------------------------------------------------------------------------
  template<class ToolType, class HandleType>
  StatusCode preConfigTool(HandleType& handle,
                           const std::string& inLabel)
                           //const std::string& outLabel)
  {
    using namespace ana::msgObjectDefinition;

    ANA_CHECK( ASG_MAKE_ANA_TOOL(handle, ToolType) );
    ANA_CHECK( handle.setProperty("InputLabel", inLabel) );
    //ANA_CHECK( handle.setProperty("OutputLabel", outLabel) ); // keep "overlaps"
    //ANA_CHECK( handle.setProperty("OutputLevel", MSG::DEBUG) );

    // The tool will assign 'true' to objects that pass overlap removal.
    // Disabling for now, in favor of post-processing.
    //static const bool outPassVal = true;
    //ANA_CHECK( handle.setProperty("OutputPassValue", outPassVal) );

    return StatusCode::SUCCESS;
  }

}


//
// method implementations
//

namespace ana
{
  SusyORTool ::
  SusyORTool(const std::string& name)
    : AsgTool(name), AnaTool(name),
      m_eleMuTool ("EleMuORT", this),
      m_eleJetTool ("EleJetORT", this),
      m_muJetTool ("MuJetORT", this),
      m_phoEleTool ("PhoEleORT", this),
      m_phoJetTool ("PhoJetORT", this)
  {
    declareProperty("ORInputLabel", m_orInputLabel = "dummy");
    declareProperty("OROutputLabel", m_orOutputLabel = "dummy");
    declareProperty("BJetLabel", m_bJetLabel = "bjet_OR");
    declareProperty("CustomMuons", m_customMuons = false);
  }



  StatusCode SusyORTool ::
  setObjectType(ObjectType type, const std::string& workingPoint)
  {
    if (type != OBJECT_OVERLAP_REMOVAL) {
      ATH_MSG_ERROR("unsupported object type");
      return StatusCode::FAILURE;
    }
    if (!workingPoint.empty()) {
      ATH_MSG_ERROR("can't have working point for overlap removal tool");
      return StatusCode::FAILURE;
    }
    return StatusCode::SUCCESS;
  }



  StatusCode SusyORTool ::
  initialize()
  {
    ATH_MSG_INFO("initialize");

    // Initialize the decoration helper.
    // Might not need this now, but let's try with it in first.
    const bool outPassVal = false; /*true;*/ // back to default for now.
    m_decHelper =
      CxxUtils::make_unique<ORUtils::OverlapDecorationHelper>
        (m_orInputLabel, "overlaps", outPassVal);
    // Make sure the output pass value is set to TRUE!

    // Configure the individual overlap tools

    // Electron-muon
    ATH_CHECK( preConfigTool<ORUtils::EleMuSharedTrkOverlapTool>
               (m_eleMuTool, m_orInputLabel) );
    ATH_CHECK( m_eleMuTool.initialize() );

    // Electron-jet
    ATH_CHECK( preConfigTool<ORUtils::EleJetOverlapTool>
               (m_eleJetTool, m_orInputLabel) );
    ANA_CHECK( m_eleJetTool.setProperty("BJetLabel", m_bJetLabel) );
    ATH_CHECK( m_eleJetTool.initialize() );

    // Muon-jet
    if (!m_customMuons)
    {
      ATH_CHECK( preConfigTool<ORUtils::MuJetOverlapTool>
                 (m_muJetTool, m_orInputLabel) );
    } else {
      ATH_CHECK( preConfigTool<ORUtils::AltMuJetOverlapTool>
                 (m_muJetTool, m_orInputLabel) );
      ANA_CHECK( m_muJetTool.setProperty("BJetLabel", m_bJetLabel) );
      ANA_CHECK( m_muJetTool.setProperty("SlidingDRMaxCone", 0.4) );
    }
    ATH_CHECK( m_muJetTool.initialize() );

    // Photon-electron
    ATH_CHECK( preConfigTool<ORUtils::DeltaROverlapTool>
               (m_phoEleTool, m_orInputLabel) );
    ATH_CHECK( m_phoEleTool.initialize() );

    // Photon-jet
    ATH_CHECK( preConfigTool<ORUtils::DeltaROverlapTool>
               (m_phoJetTool, m_orInputLabel) );
    ATH_CHECK( m_phoJetTool.initialize() );

    // No need for a master tool - doing the OR by hand :-)
    return StatusCode::SUCCESS;
  }

  //---------------------------------------------------------------------------
  // Scheduling boiler plate methods
  //---------------------------------------------------------------------------
  AnalysisStep SusyORTool ::
  step() const { return STEP_OVERLAP_REMOVAL; }
  //---------------------------------------------------------------------------
  unsigned SusyORTool ::
  inputTypes() const
  {
    return (1 << OBJECT_ELECTRON) | (1 << OBJECT_MUON) | (1 << OBJECT_JET) |
           (1 << OBJECT_TAU) | (1 << OBJECT_PHOTON);
  }
  //---------------------------------------------------------------------------
  unsigned SusyORTool ::
  outputTypes() const { return inputTypes(); } // Same as input types

  //---------------------------------------------------------------------------
  // Execute the overlap removal
  //---------------------------------------------------------------------------
  StatusCode SusyORTool ::
  execute(IEventObjects& objects)
  {
    // Reset all decorations to passing
    if(objects.electrons()) m_decHelper->resetDecorations(*objects.electrons());
    if(objects.muons())     m_decHelper->resetDecorations(*objects.muons());
    if(objects.jets())      m_decHelper->resetDecorations(*objects.jets());
    if(objects.photons())   m_decHelper->resetDecorations(*objects.photons());

    // Very specific order...
    if (objects.electrons() && objects.jets()) {
      ATH_CHECK( m_eleJetTool->findOverlaps(*objects.electrons(),
                                            *objects.jets()) );
    }
    if (objects.muons() && objects.jets()) {
      ATH_CHECK( m_muJetTool->findOverlaps(*objects.muons(),
                                           *objects.jets()) );
    }
    if (objects.electrons() && objects.muons()) {
      ATH_CHECK( m_eleMuTool->findOverlaps(*objects.electrons(),
                                           *objects.muons()) );
    }
    if (objects.photons()) {
      if (objects.electrons()){
        ATH_CHECK( m_phoEleTool->findOverlaps(*objects.photons(),
                                              *objects.electrons()) );
      }
      if (objects.jets()) {
        ATH_CHECK( m_phoJetTool->findOverlaps(*objects.jets(),
                                              *objects.photons()) );
      }
    }

    //
    // Post-processing
    // if "overlaps" is 1, set selection flag to 0
    //

    SG::AuxElement::Accessor<SelectType> selectAcc(m_orOutputLabel);
    SG::AuxElement::ConstAccessor<SelectType> overlapAcc("overlaps");

    if (objects.electrons()) {
      for(auto electron : *(objects.electrons())) {
        if (overlapAcc(*electron)) selectAcc(*electron) = false;
      }
    }
    if (objects.muons()) {
      for(auto muon : *(objects.muons())) {
        if (overlapAcc(*muon)) selectAcc(*muon) = false;
      }
    }
    if (objects.jets()) {
      for(auto jet : *(objects.jets())) {
        if (overlapAcc(*jet)) selectAcc(*jet) = false;
      }
    }
    if (objects.photons()) {
      for(auto photon : *(objects.photons())) {
        if (overlapAcc(*photon)) selectAcc(*photon) = false;
      }
    }

    return StatusCode::SUCCESS;
  }



  StatusCode makeORTool(DefinitionArgs& args,
                        bool customMuons=false)
  {
    using namespace msgObjectDefinition;

    std::unique_ptr<SusyORTool> orTool
      ( new SusyORTool(args.prefix()) );
    ANA_CHECK( orTool->setProperty
      ("ORInputLabel", args.configuration()->selectionName(SelectionStep::OR)) );
    ANA_CHECK( orTool->setProperty
      ("OROutputLabel", args.configuration()->selectionName(SelectionStep::ANALYSIS)) );
    ANA_CHECK( orTool->setProperty
      ("CustomMuons", customMuons) );
    args.add( std::move(orTool) );

    return StatusCode::SUCCESS;
  }

  QUICK_ANA_OR_DEFINITION_MAKER( "susy", makeORTool(args) )
  QUICK_ANA_OR_DEFINITION_MAKER( "susy2L", makeORTool(args,true) )
} // namespace ana
