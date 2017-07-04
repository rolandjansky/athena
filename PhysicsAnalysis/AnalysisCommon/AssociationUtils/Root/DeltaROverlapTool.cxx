// Primary include
#include "AssociationUtils/DeltaROverlapTool.h"

// Framework includes
#include "CxxUtils/make_unique.h"

namespace ORUtils
{

  //---------------------------------------------------------------------------
  // Constructor
  //---------------------------------------------------------------------------
  DeltaROverlapTool::DeltaROverlapTool(const std::string& name)
    : BaseOverlapTool(name)
  {
    declareProperty("DR", m_dR = 0.4, "Maximum dR for overlap match");
    declareProperty("UseRapidity", m_useRapidity = true,
                    "Calculate delta-R using rapidity");
    declareProperty("SwapContainerPrecedence", m_swapContainerPrecedence = false,
                    "Use second container for flagging overlaps");
  }

  //---------------------------------------------------------------------------
  // Initialize
  //---------------------------------------------------------------------------
  StatusCode DeltaROverlapTool::initializeDerived()
  {
    ATH_MSG_DEBUG("Setting up dR matching with cone size " << m_dR);

    // Initialize the dR matcher
    m_dRMatcher = CxxUtils::make_unique<DeltaRMatcher>(m_dR, m_useRapidity);
    return StatusCode::SUCCESS;
  }

  //---------------------------------------------------------------------------
  // Identify overlaps
  //---------------------------------------------------------------------------
  StatusCode DeltaROverlapTool::
  findOverlaps(const xAOD::IParticleContainer& cont1,
               const xAOD::IParticleContainer& cont2) const
  {
    ATH_MSG_DEBUG("Removing overlaps");

    // Initialize output decoration if necessary
    m_decHelper->initializeDecorations(cont1);
    m_decHelper->initializeDecorations(cont2);

    // Reference container is the one given precedence
    const auto& refCont  = m_swapContainerPrecedence? cont1 : cont2;
    // Test container is the one flagged with overlaps
    const auto& testCont = m_swapContainerPrecedence? cont2 : cont1;

    // Loop over surviving input objects
    for(const auto testPar : testCont){
      if(m_decHelper->isSurvivingObject(*testPar)){
        for(const auto refPar : refCont){
          if(m_decHelper->isSurvivingObject(*refPar)){
            // Check for duplicates and overlap
            if(testPar != refPar && m_dRMatcher->objectsMatch(*testPar, *refPar)){
              ATH_CHECK( handleOverlap(testPar, refPar) );
            }
          }
        }
      }
    }
    return StatusCode::SUCCESS;
  }

} // namespace ORUtils
