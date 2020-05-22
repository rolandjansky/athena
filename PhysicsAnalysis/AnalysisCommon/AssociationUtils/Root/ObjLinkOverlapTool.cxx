/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

// Local includes
#include "AssociationUtils/ObjLinkOverlapTool.h"

namespace
{
  /// Unit conversion constant
  const float invGeV = 0.001;
}

namespace ORUtils
{

  //---------------------------------------------------------------------------
  // Constructor
  //---------------------------------------------------------------------------
  ObjLinkOverlapTool::ObjLinkOverlapTool(const std::string& name)
    : BaseOverlapTool(name)
  {
  }

  //---------------------------------------------------------------------------
  // Initialize
  //---------------------------------------------------------------------------
  StatusCode ObjLinkOverlapTool::initializeDerived()
  {
    return StatusCode::SUCCESS;
  }

  //---------------------------------------------------------------------------
  // Identify overlaps
  //---------------------------------------------------------------------------
  StatusCode ObjLinkOverlapTool::
  findOverlaps(const xAOD::IParticleContainer& cont1,
               const xAOD::IParticleContainer& cont2) const
  {
    ATH_MSG_DEBUG("Removing overlaps");

    // Initialize output decoration if necessary
    m_decHelper->initializeDecorations(cont1);
    m_decHelper->initializeDecorations(cont2);

    // Loop over surviving input objects in cont1
    for(const auto p1 : cont1){
      if(m_decHelper->isSurvivingObject(*p1)){

        // Check for existence of an object link
        auto linkParticle = m_objLinkHelper->getObjectLink(*p1);
        if(linkParticle){

          // See if the link matches an input in cont2
          if(linkParticle->container() == &cont2 &&
             m_decHelper->isSurvivingObject(*linkParticle)){

            ATH_MSG_DEBUG("  Found overlap " << p1->type() <<
                          " pt " << p1->pt()*invGeV);
            m_decHelper->setObjectFail(*p1);
          }
        }
      }
    }
    return StatusCode::SUCCESS;
  }

} // namespace ORUtils
