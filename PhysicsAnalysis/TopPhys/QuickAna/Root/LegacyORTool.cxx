/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

//
// includes
//

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wdeprecated-declarations"
#include <QuickAna/LegacyORTool.h>

#include <QuickAna/DefinitionArgs.h>
#include <QuickAna/DefinitionMaker.h>
#include <QuickAna/IEventObjects.h>
#include <QuickAna/InternalConfiguration.h>
#include <QuickAna/MessageCheck.h>
#include <QuickAna/TopOverlapRemovalTool.h>

//
// method implementations
//

namespace ana
{
  LegacyORTool ::
  LegacyORTool (const std::string& name)
    : AsgTool (name), AnaTool (name),
      m_ortool ("OverlapRemovalTool", this)
  {
    declareProperty("ORInputLabel", m_orInputLabel = "dummy");
    declareProperty("OROutputLabel", m_orOutputLabel = "dummy");

    declareProperty("OR_Type", m_or_type = Default);
  }



  StatusCode LegacyORTool ::
  setObjectType (ObjectType type, const std::string& workingPoint)
  {
    if (type != OBJECT_OVERLAP_REMOVAL)
    {
      ATH_MSG_ERROR ("unsupported object type");
      return StatusCode::FAILURE;
    }
    if (!workingPoint.empty())
    {
      ATH_MSG_ERROR ("can't have working point for overlap removal tool");
      return StatusCode::FAILURE;
    }
    return StatusCode::SUCCESS;
  }



  StatusCode LegacyORTool ::
  initialize()
  {
    if (m_or_type==run1_SSbjets || m_or_type==run1_top_default)
    {
      ATH_CHECK (ASG_MAKE_ANA_TOOL (m_ortool, TopOverlapRemovalTool));
      ATH_CHECK (m_ortool.setProperty("MuORJetVariDR", false));

      if (m_or_type==run1_SSbjets){
        ATH_CHECK (m_ortool.setProperty("MuORJetVariDR", true));
        ATH_CHECK (m_ortool.setProperty("MuonJetDRConeEtaPhi", 0.04));
      }
    }
    else
    {
      ATH_CHECK (ASG_MAKE_ANA_TOOL (m_ortool, OverlapRemovalTool));
    }
    ATH_CHECK (m_ortool.setProperty("InputLabel", m_orInputLabel));
    ATH_CHECK (m_ortool.initialize());
    // Not a systematics tool
    //registerTool (&*m_ortool);

    return StatusCode::SUCCESS;
  }



  AnalysisStep LegacyORTool ::
  step () const
  {
    return STEP_OVERLAP_REMOVAL;
  }



  unsigned LegacyORTool ::
  inputTypes () const
  {
    return (1 << OBJECT_ELECTRON) | (1 << OBJECT_MUON) | (1 << OBJECT_JET) |
           (1 << OBJECT_TAU) | (1 << OBJECT_PHOTON);
  }



  unsigned LegacyORTool ::
  outputTypes () const
  {
    return (1 << OBJECT_ELECTRON) | (1 << OBJECT_MUON) | (1 << OBJECT_JET) |
           (1 << OBJECT_TAU) | (1 << OBJECT_PHOTON);
  }



  StatusCode LegacyORTool ::
  execute (IEventObjects& objects)
  {
    if (m_or_type==run1_top_default) {
      if (objects.muons() && objects.jets())
      {
        ATH_CHECK ( m_ortool->removeMuonJetOverlap(*objects.muons(), *objects.jets()) );
      }
      if (objects.electrons() && objects.jets())
      {
        ATH_CHECK ( m_ortool->removeEleJetOverlap(*objects.electrons(), *objects.jets()) );
      }
    }
    else if (m_or_type==run1_susy_default){
      // Very specific order...
      if (objects.electrons() && objects.jets())
      {
        ATH_CHECK( m_ortool->removeEleJetOverlap(*objects.electrons(), *objects.jets()) );
      }
      if (objects.muons() && objects.jets())
      {
        ATH_CHECK( m_ortool->removeMuonJetOverlap(*objects.muons(), *objects.jets()) );
      }
      if (objects.electrons() && objects.muons())
      {
        ATH_CHECK( m_ortool->removeEleMuonOverlap(*objects.electrons(), *objects.muons()) );
      }
      if (objects.photons()) {
        if (objects.electrons()){
          ATH_CHECK( m_ortool->removePhotonEleOverlap(*objects.photons(), *objects.electrons()) );
        }
        if (objects.jets()) {
          ATH_CHECK( m_ortool->removePhotonJetOverlap(*objects.photons(), *objects.jets()) );
        }
      }
    }
    else {
      ATH_CHECK ( m_ortool->removeOverlaps(objects.electrons(), objects.muons(),
            objects.jets(), objects.taus(),
            objects.photons()) );
    }

    //
    // if "overlaps" is 1, set selection flag to 0
    //

    SG::AuxElement::Accessor<SelectType> selectAcc(m_orOutputLabel);
    SG::AuxElement::ConstAccessor<SelectType> overlapAcc("overlaps");

    // electrons
    if (objects.electrons()) {
      for(auto electron : *(objects.electrons())) {
        if (overlapAcc(*electron)) selectAcc(*electron) = false;
      }
    }

    // muons
    if (objects.muons()) {
      for(auto muon : *(objects.muons())) {
        if (overlapAcc(*muon)) selectAcc(*muon) = false;
      }
    }

    // jets
    if (objects.jets()) {
      for(auto jet : *(objects.jets())) {
        if (overlapAcc(*jet)) selectAcc(*jet) = false;
      }
    }

    // taus
    if (objects.taus()) {
      for(auto tau : *(objects.taus())) {
        if (overlapAcc(*tau)) selectAcc(*tau) = false;
      }
    }

    // photons
    if (objects.photons()) {
      for(auto photon : *(objects.photons())) {
        if (overlapAcc(*photon)) selectAcc(*photon) = false;
      }
    }

    return StatusCode::SUCCESS;
  }


  // Print object information
  // TODO: Either remove this or fix it to use proper tool functionality
  // like ATH_MSG logging, StatusCode, etc.
  void LegacyORTool ::
  printObj (const char* APP_NAME, const char* type, const xAOD::IParticle* obj) const
  {
    static SG::AuxElement::ConstAccessor<SelectType> selectAcc(m_orOutputLabel);
    static SG::AuxElement::ConstAccessor<SelectType> overlapAcc("overlaps");

    // Safety check
    // TODO: should use StatusCode here
    if(!overlapAcc.isAvailable(*obj)){
      Error(APP_NAME, "Overlap decoration missing for object");
      abort();
    }

    Info(APP_NAME, "  %s pt %6.2f eta %5.2f phi %5.2f selected %i overlaps %i",
                   type, obj->pt()/1000., obj->eta(), obj->phi(),
                   selectAcc(*obj), overlapAcc(*obj));
  }



  StatusCode makeORTool (DefinitionArgs& args,
                         LegacyORTool::OR_Type or_type)
  {
    using namespace ana::msgObjectDefinition;

    std::unique_ptr<LegacyORTool> orTool
      ( new LegacyORTool(args.prefix()) );
    ANA_CHECK( orTool->setProperty("OR_Type", int (or_type)) );
    ANA_CHECK( orTool->setProperty
      ("ORInputLabel", args.configuration()->selectionName(SelectionStep::OR)));
    ANA_CHECK( orTool->setProperty
      ("OROutputLabel", args.configuration()->selectionName(SelectionStep::ANALYSIS)));
    args.add( std::move(orTool) );

    return StatusCode::SUCCESS;
  }

  QUICK_ANA_OR_DEFINITION_MAKER ("legacy", makeORTool (args, LegacyORTool::Default))
  QUICK_ANA_OR_DEFINITION_MAKER ("run1_top_default", makeORTool (args, LegacyORTool::run1_top_default))
  QUICK_ANA_OR_DEFINITION_MAKER ("run1_SSbjets", makeORTool (args, LegacyORTool::run1_SSbjets))
  QUICK_ANA_OR_DEFINITION_MAKER ("run1_susy_default", makeORTool (args, LegacyORTool::run1_susy_default))

} // namespace ana

#pragma GCC diagnostic pop
