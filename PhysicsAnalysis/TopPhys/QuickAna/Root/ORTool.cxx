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
    declareProperty("BoostedLeptons", m_orFlags.boostedLeptons);
    declareProperty("AnaSelectionName", m_anaSelectionName);
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
    ATH_CHECK( ORUtils::recommendedTools(m_orFlags, m_orToolBox) );

    // We don't currently have a good way to determine here which object
    // definitions are disabled, so we currently just configure all overlap
    // tools and disable the pointer safety checks
    ATH_CHECK( m_orToolBox.masterTool.setProperty("RequireExpectedPointers", false) );

    // Set some global properties
    //ATH_CHECK( m_orToolBox.setGlobalProperty("OutputLevel", MSG::DEBUG) );

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
           (1 << OBJECT_TAU) | (1 << OBJECT_PHOTON);
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
    auto& orTool = m_orToolBox.masterTool;
    ATH_CHECK( orTool->removeOverlaps(objects.electrons(), objects.muons(),
                                      objects.jets(), objects.taus(),
                                      objects.photons()) );

    //
    // Post-processing: if "overlaps" is true, set selection flag to false
    //
    SG::AuxElement::Accessor<SelectType> selectAcc(m_anaSelectionName);
    SG::AuxElement::ConstAccessor<SelectType> overlapAcc(m_orFlags.outputLabel);

    // List of containers to process.
    std::vector< xAOD::IParticleContainer* > containers {
      objects.electrons(), objects.muons(), objects.jets(),
      objects.taus(), objects.photons()
    };

    // Process all containers in one go!
    for (auto contPtr : containers) {
      if (contPtr) {
        for (auto par : *contPtr) {
          if (overlapAcc(*par)) {
            selectAcc(*par) = false;
          }
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
                        const bool boostedLeptons = false)
  {
    using namespace ana::msgObjectDefinition;

    auto orTool = CxxUtils::make_unique<ana::ORTool>( args.prefix() );
    auto config = args.configuration();
    auto inLabel = config->selectionName(ana::SelectionStep::OR);
    auto anaLabel = config->selectionName(ana::SelectionStep::ANALYSIS);
    ANA_CHECK( orTool->setProperty("ORInputLabel", inLabel) );
    ANA_CHECK( orTool->setProperty("BJetLabel", bJetLabel) );
    ANA_CHECK( orTool->setProperty("BoostedLeptons", boostedLeptons) );
    ANA_CHECK( orTool->setProperty("AnaSelectionName", anaLabel) );
    args.add( std::move(orTool) );

    return StatusCode::SUCCESS;
  }

  // Register the object definitions
  QUICK_ANA_OR_DEFINITION_MAKER( "default", makeORTool(args) )
  QUICK_ANA_OR_DEFINITION_MAKER( "heavyFlavor", makeORTool(args, "bjet_OR") )
  QUICK_ANA_OR_DEFINITION_MAKER( "boosted", makeORTool(args, "", true) )
  QUICK_ANA_OR_DEFINITION_MAKER( "boostedHF", makeORTool(args, "bjet_OR", true) )

} // anonymous namespace
