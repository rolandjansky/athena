// Dear emacs, this is -*- c++ -*-
//
// Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
//
#ifndef XAODBTAGGING_BTAGGINGUTILITIES_H
#define XAODBTAGGING_BTAGGINGUTILITIES_H

// Local include(s).
#include "xAODBTagging/BTaggingContainer.h"

// EDM include(s).
#include "AthContainers/AuxElement.h"

// System include(s).
#include <string>

namespace xAOD::BTaggingUtilities {

   /// The default key for the @c xAOD::BTagging link
   static const std::string DEFAULT_BTAG_LINK_NAME = "btaggingLink";

   /// Access the default @c xAOD::BTagging object associated to an object
   const BTagging* getBTagging( const SG::AuxElement& part );
   /// Access a non-standard @c xAOD::BTagging object associated to an object
   const BTagging* getBTagging( const SG::AuxElement& part,
                                const std::string& linkName );

   /// Access the default @c xAOD::BTagging link for an object.
   const ElementLink< BTaggingContainer >&
   getBTaggingLink( const SG::AuxElement& part );
   /// Access a non-standard @c xAOD::BTagging link for an object.
   const ElementLink< BTaggingContainer >&
   getBTaggingLink( const SG::AuxElement& part, const std::string& linkName );

   /// Set the default @c xAOD::BTagging link for an object
   void setBTaggingLink( const SG::AuxElement& part,
                         const ElementLink< BTaggingContainer >& link );
   /// Set a non-standard @c xAOD::BTagging link for an object
   void setBTaggingLink( const SG::AuxElement& part,
                         const ElementLink< BTaggingContainer >& link,
                         const std::string& linkName );

} // namespace xAOD::BTaggingUtilities

#endif // XAODBTAGGING_BTAGGINGUTILITIES_H
