/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

// Local includes
#include "AssociationUtils/BaseOverlapTool.h"

namespace
{
  /// Unit conversion constant
  const float invGeV = 1e-3;
}

namespace ORUtils
{

  //---------------------------------------------------------------------------
  // Constructor
  //---------------------------------------------------------------------------
  BaseOverlapTool::BaseOverlapTool(const std::string& name)
    : asg::AsgTool(name)
  {
    declareProperty("InputLabel", m_inputLabel = "selected",
                    "Decoration which specifies input objects");
    declareProperty("OutputLabel", m_outputLabel = "overlaps",
                    "Decoration given to objects that fail OR");
    declareProperty("OutputPassValue", m_outputPassValue = false,
                    "Set the result assigned to objects that pass");
    declareProperty("LinkOverlapObjects", m_linkOverlapObjects = false,
                    "Turn on overlap object link decorations");
    declareProperty("EnableUserPriority", m_enableUserPrio = false,
                    "Turn on user priority score");
  }

  //---------------------------------------------------------------------------
  // Initialize
  //---------------------------------------------------------------------------
  StatusCode BaseOverlapTool::initialize()
  {
    ATH_MSG_DEBUG("Initializing " << name());
    ATH_MSG_DEBUG("Base config options: InputLabel " << m_inputLabel <<
                  " OutputLabel " << m_outputLabel <<
                  " OutputPassValue " << m_outputPassValue <<
                  " UserPrio " << m_enableUserPrio);

    // Initialize the decoration helper
    m_decHelper = std::make_unique<OverlapDecorationHelper>
      (m_inputLabel, m_outputLabel, m_outputPassValue);

    // Initialize the obj-link helper
    if(m_linkOverlapObjects)
      m_objLinkHelper = std::make_unique<OverlapLinkHelper>("overlapObject");

    // Initialize the derived tool
    ATH_CHECK( initializeDerived() );

    return StatusCode::SUCCESS;
  }

  //---------------------------------------------------------------------------
  // Handle overlap condition
  //---------------------------------------------------------------------------
  StatusCode BaseOverlapTool::
  handleOverlap(const xAOD::IParticle* testParticle,
                const xAOD::IParticle* refParticle) const
  {
    // Apply user-priority override
    if(!m_enableUserPrio ||
       m_decHelper->getObjectPriority(*testParticle) <=
       m_decHelper->getObjectPriority(*refParticle))
    {
      ATH_MSG_DEBUG("  Found overlap " << testParticle->type() <<
                    " pt " << testParticle->pt()*invGeV);
      m_decHelper->setObjectFail(*testParticle);
      if(m_objLinkHelper) {
        ATH_CHECK( m_objLinkHelper->addObjectLink(*testParticle, *refParticle) );
      }
    }
    return StatusCode::SUCCESS;
  }

} // namespace ORUtils
