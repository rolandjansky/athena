/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// My header first
#include <QuickAna/ORTool.h>

// Toolkit infrastructure includes
#include <CxxUtils/make_unique.h>

// Local includes
#include <QuickAna/DefinitionArgs.h>
#include <QuickAna/DefinitionMaker.h>
#include <QuickAna/IEventObjects.h>
#include <QuickAna/InternalConfiguration.h>
#include <QuickAna/MessageCheck.h>


namespace ana
{

  //---------------------------------------------------------------------------
  // Constructor
  //---------------------------------------------------------------------------
  ORTool ::
  ORTool (const std::string& name)
    : AsgTool(name), AnaTool(name),
      m_orToolBox("ORTBox", this),
      m_orFlags(name + "_ortool"),
      m_anaSelectionName("ana_select")
  {
    declareProperty("ORInputLabel", m_orFlags.inputLabel);
    declareProperty("OROutputLabel", m_orFlags.outputLabel);
    declareProperty("BJetLabel", m_orFlags.bJetLabel);
    declareProperty("AnaSelectionName", m_anaSelectionName);
    declareProperty("BoostedLeptons", m_orFlags.boostedLeptons);
    declareProperty("SlidingConeType", m_slidingConeType);
    declareProperty("JVTPriorities", m_jvtPriorities=false);
    declareProperty("ApplyRelPt", m_applyRelPt=false);
    declareProperty("OnlyRejJets", m_onlyRejJets=false);
    declareProperty("OnlyVBS4l", m_onlyVBS4l=false);
  }

  //---------------------------------------------------------------------------
  // Set object type
  //---------------------------------------------------------------------------
  StatusCode ORTool ::
  setObjectType(ObjectType type, const std::string& workingPoint)
  {
    if(type != OBJECT_OVERLAP_REMOVAL) {
      ATH_MSG_ERROR("unsupported object type");
      return StatusCode::FAILURE;
    }
    if(!workingPoint.empty()) {
      ATH_MSG_ERROR("can't have working point for overlap removal tool");
      return StatusCode::FAILURE;
    }
    return StatusCode::SUCCESS;
  }

  //---------------------------------------------------------------------------
  // Initialize the tool
  //---------------------------------------------------------------------------
  StatusCode ORTool ::
  initialize()
  {
    ATH_MSG_INFO( "Initializing " << name() );

    using namespace ana::msgObjectDefinition;

    // Trying out the new OR config function
    m_orToolBox.msg().setLevel( msg().level() );
    if(m_onlyRejJets) m_orFlags.doEleEleOR = true;
    ATH_CHECK( ORUtils::recommendedTools(m_orFlags, m_orToolBox) );

    // We don't currently have a good way to determine here which object
    // definitions are disabled, so we currently just configure all overlap
    // tools and disable the pointer safety checks
    ATH_CHECK( m_orToolBox.masterTool.setProperty("RequireExpectedPointers", false) );
    ATH_CHECK( m_orToolBox.muJetORT.setProperty("ApplyRelPt", m_applyRelPt) );

    // only remove jets, in early RUN2, this only works for HIGG2DX derivation
    if (m_onlyRejJets) {
      if (m_onlyVBS4l) ATH_CHECK( m_orToolBox.eleEleORT.setProperty("UseClusterMatch", false) );
      else             ATH_CHECK( m_orToolBox.eleEleORT.setProperty("UseClusterMatch", true) );
      ATH_CHECK( m_orToolBox.eleJetORT.setProperty("OuterDR", 0.) );
      ATH_CHECK( m_orToolBox.muJetORT.setProperty("OuterDR", 0.) );
    }

    // Set some global properties
    //ATH_CHECK( m_orToolBox.setGlobalProperty("OutputLevel", MSG::DEBUG) );

    // Deal with JVT priority for the relevant tools
    if (m_jvtPriorities){
      ATH_CHECK(m_orToolBox.eleJetORT.setProperty("EnableUserPriority", true));
      ATH_CHECK(m_orToolBox.muJetORT.setProperty("EnableUserPriority", true));
      ATH_CHECK(m_orToolBox.tauJetORT.setProperty("EnableUserPriority", true));
      ATH_CHECK(m_orToolBox.phoJetORT.setProperty("EnableUserPriority", true));
    }

    // Override boosted OR sliding cone options
    if(!m_slidingConeType.empty()) {
      bool slideEle = false;
      bool slideMu = false;
      if(m_slidingConeType == "electron") {
        slideEle = true;
      } else if(m_slidingConeType == "muon") {
        slideMu = true;
      } else if(m_slidingConeType == "both") {
        slideEle = slideMu = true;
      } else {
        ATH_MSG_ERROR("Unsupported SlidingConeType: " << m_slidingConeType);
        return StatusCode::FAILURE;
      }
      ATH_CHECK( m_orToolBox.eleJetORT.setProperty("UseSlidingDR", slideEle) );
      ATH_CHECK( m_orToolBox.muJetORT.setProperty("UseSlidingDR", slideMu) );
    }

    // Initialzie the toolbox
    ATH_CHECK( m_orToolBox.initialize() );

    return StatusCode::SUCCESS;
  }

  //---------------------------------------------------------------------------
  // Scheduling boiler plate methods
  //---------------------------------------------------------------------------
  AnalysisStep ORTool ::
  step() const { return STEP_OVERLAP_REMOVAL; }
  //---------------------------------------------------------------------------
  unsigned ORTool ::
  inputTypes() const
  {
    return (1 << OBJECT_ELECTRON) | (1 << OBJECT_MUON) | (1 << OBJECT_JET) |
           (1 << OBJECT_TAU) | (1 << OBJECT_PHOTON) | (1 << OBJECT_PFLOW_JET);
  }
  //---------------------------------------------------------------------------
  unsigned ORTool ::
  outputTypes() const { return inputTypes(); } // Same as input types

  //---------------------------------------------------------------------------
  // Execute the overlap removal
  //---------------------------------------------------------------------------
  StatusCode ORTool ::
  execute(IEventObjects& objects)
  {
    if (m_jvtPriorities){
      // Reset the priorities of all objects so that non-JVT jets cannot remove the others
      SG::AuxElement::Accessor<SelectType> inAcc(m_orFlags.inputLabel);
      if (objects.electrons()){
        for (auto e : *objects.electrons()) inAcc(*e) = inAcc(*e)*2;
      }
      if (objects.muons()){
        for (auto m : *objects.muons()) inAcc(*m) = inAcc(*m)*2;
      }
      if (objects.photons()){
        for (auto p : *objects.photons()) inAcc(*p) = inAcc(*p)*2;
      }
      if (objects.taus()){
        for (auto t : *objects.taus()) inAcc(*t) = inAcc(*t)*2;
      }
      if (objects.jets()){
        SG::AuxElement::Accessor<SelectType> jvtAcc("Jvt_pass");
        for (auto j : *objects.jets()){
          if (jvtAcc(*j)) inAcc(*j) = inAcc(*j)*2;
        }
      }
      if (objects.pflow_jets()){
        SG::AuxElement::Accessor<SelectType> jvtAcc("Jvt_pass");
        for (auto j : *objects.pflow_jets()){
          if (jvtAcc(*j)) inAcc(*j) = inAcc(*j)*2;
        }
      }

    }

    auto& orTool = m_orToolBox.masterTool;
    if (objects.jets())
       ATH_CHECK( orTool->removeOverlaps(objects.electrons(), objects.muons(),
                                         objects.jets(), objects.taus(),
                                         objects.photons()) );
    if (objects.pflow_jets())
       ATH_CHECK( orTool->removeOverlaps(objects.electrons(), objects.muons(),
                                         objects.pflow_jets(), objects.taus(),
                                         objects.photons()) );


    //
    // Post-processing: if "overlaps" is true, set selection flag to false
    //
    SG::AuxElement::Accessor<SelectType> selectAcc(m_anaSelectionName);
    SG::AuxElement::ConstAccessor<SelectType> overlapAcc(m_orFlags.outputLabel);
    SG::AuxElement::Accessor<SelectType> orPassAcc("OR_pass");

    // List of containers to process.
    std::vector< xAOD::IParticleContainer* > containers {
      objects.electrons(), objects.muons(), objects.jets(),
      objects.pflow_jets(), objects.taus(), objects.photons()
    };

    // Process all containers in one go!
    for (auto contPtr : containers) {
      if (contPtr) {
        for (auto par : *contPtr) {
          if (overlapAcc(*par)) {
            selectAcc(*par) = false;
            orPassAcc(*par) = false;
          }else
            orPassAcc(*par) = true;
        }
      }
    }

    return StatusCode::SUCCESS;
  }

} // namespace ana


// Anonymous namespace to hold the tool factory function
namespace
{

  //---------------------------------------------------------------------------
  // Tool maker function.
  //---------------------------------------------------------------------------
  StatusCode makeORTool(ana::DefinitionArgs& args,
                        const std::string& bJetLabel = "",
                        const std::string& boostedLeptons = "",
                        const bool useJVT = false,
                        const bool applyRelPt = false,
                        const bool onlyRejJets = false,
                        const bool onlyVBS4l   = false)
  {
    using namespace ana::msgObjectDefinition;

    auto orTool = CxxUtils::make_unique<ana::ORTool>( args.prefix() );
    auto config = args.configuration();
    auto inLabel = config->selectionName(ana::SelectionStep::OR);
    auto anaLabel = config->selectionName(ana::SelectionStep::ANALYSIS);
    ANA_CHECK( orTool->setProperty("ORInputLabel", inLabel) );
    ANA_CHECK( orTool->setProperty("BJetLabel", bJetLabel) );
    bool doBoostedLeptons = ! boostedLeptons.empty();
    ANA_CHECK( orTool->setProperty("BoostedLeptons", doBoostedLeptons) );
    ANA_CHECK( orTool->setProperty("SlidingConeType", boostedLeptons) );
    ANA_CHECK( orTool->setProperty("AnaSelectionName", anaLabel) );
    ANA_CHECK( orTool->setProperty("JVTPriorities", useJVT) );
    ANA_CHECK( orTool->setProperty("ApplyRelPt", applyRelPt) );
    ANA_CHECK( orTool->setProperty("OnlyRejJets", onlyRejJets) );
    ANA_CHECK( orTool->setProperty("OnlyVBS4l", onlyVBS4l) );
    args.add( std::move(orTool) );

    return StatusCode::SUCCESS;
  }

  // Register the object definitions
  QUICK_ANA_OR_DEFINITION_MAKER( "default", makeORTool(args) )
  QUICK_ANA_OR_DEFINITION_MAKER( "heavyFlavor", makeORTool(args, "bjet_OR") )
  QUICK_ANA_OR_DEFINITION_MAKER( "boosted", makeORTool(args, "", "both") )
  QUICK_ANA_OR_DEFINITION_MAKER( "boostedHF", makeORTool(args, "bjet_OR", "both") )

  QUICK_ANA_OR_DEFINITION_MAKER( "boostedHF_JVT", makeORTool(args, "bjet_OR", "both", true) )
  QUICK_ANA_OR_DEFINITION_MAKER( "boostedMuHF_JVT", makeORTool(args, "bjet_OR", "muon", true) )
  QUICK_ANA_OR_DEFINITION_MAKER( "zzllvv", makeORTool(args, "", "", true, true) )
  QUICK_ANA_OR_DEFINITION_MAKER( "zzllvv_nojvt", makeORTool(args, "", "", false, true) )
  QUICK_ANA_OR_DEFINITION_MAKER( "zzllll", makeORTool(args, "", "", true, true, true) )
  QUICK_ANA_OR_DEFINITION_MAKER( "vbs_4l", makeORTool(args, "", "", true, true, true, true) )

} // anonymous namespace
