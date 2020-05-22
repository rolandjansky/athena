/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/

// Local includes
#include "AssociationUtils/OverlapLinkHelper.h"

namespace ORUtils
{

  //---------------------------------------------------------------------------
  // Constructor
  //---------------------------------------------------------------------------
  OverlapLinkHelper::OverlapLinkHelper(const std::string& linkLabel)
    : m_linkDecorator(linkLabel),
      m_linkAccessor(linkLabel)
  {}

  //---------------------------------------------------------------------------
  // Link p1 to p2
  //---------------------------------------------------------------------------
  StatusCode OverlapLinkHelper::addObjectLink
  (const xAOD::IParticle& p1, const xAOD::IParticle& p2)
  {
    const xAOD::IParticleContainer* overlapContainer =
      static_cast<const xAOD::IParticleContainer*>( p2.container() );
    m_linkDecorator(p1) = ort::objLink_t(*overlapContainer, p2.index());
    return StatusCode::SUCCESS;
  }

  //---------------------------------------------------------------------------
  // Retrieve an overlap-linked particle or null.
  //---------------------------------------------------------------------------
  const xAOD::IParticle* OverlapLinkHelper::getObjectLink
  (const xAOD::IParticle& p) const
  {
    // Check if the decoration is present and valid
    if(m_linkAccessor.isAvailable(p)) {
      auto& link = m_linkAccessor(p);
      if(link.isValid()) return *link;
    }
    return nullptr;
  }

} // namespace ORUtils
