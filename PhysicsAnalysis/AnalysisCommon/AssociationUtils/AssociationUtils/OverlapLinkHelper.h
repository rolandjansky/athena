/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/

#ifndef ASSOCIATIONUTILS_OVERLAPLINKHELPER_H
#define ASSOCIATIONUTILS_OVERLAPLINKHELPER_H

// Framework includes
#include "AsgMessaging/StatusCode.h"

// EDM includes
#include "xAODBase/IParticleContainer.h"

// Local includes
#include "AssociationUtils/OverlapRemovalDefs.h"

namespace ORUtils
{

  /// @class OverlapLinkHelper
  /// @brief Helper class for setting links between overlapping objects.
  ///
  /// This utility class is used by the OverlapTools, though it could probably
  /// be used by a user as well. It can do two things at the moment:
  /// - Link one object to another
  /// - Retrieve a linked object
  ///
  /// For this simple implementation, use a single object link.
  /// I might later allow for an array of links to include all
  /// possible object overlaps.
  ///
  /// @author Steve Farrell <Steven.Farrell@cern.ch>
  ///
  class OverlapLinkHelper
  {

    public:

      /// Constructor
      OverlapLinkHelper(const std::string& linkLabel);

      /// Decorate p1 with an overlap object link to p2
      StatusCode addObjectLink(const xAOD::IParticle& p1,
                               const xAOD::IParticle& p2);

      /// @brief Retrieve an overlap-linked particle.
      /// Returns null if no ElementLink decoration exists.
      const xAOD::IParticle* getObjectLink(const xAOD::IParticle& p) const;

    private:

      /// Object link decorator
      ort::objLinkDecorator_t m_linkDecorator;
      /// Corresponding object link accessor (for reading only)
      ort::objLinkAccessor_t m_linkAccessor;

  }; // class OverlapLinkHelper

} // namespace ORUtils

#endif
