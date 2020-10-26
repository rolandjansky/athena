#ifndef XAODBTAGGING_UTILITIES_H
#define XAODBTAGGING_UTILITIES_H

#include "AthContainers/AuxElement.h"
#include "xAODBTagging/BTaggingContainer.h"

namespace xAOD::BTaggingUtilities {
    const xAOD::BTagging* getBTagging( const SG::AuxElement&,
				       const std::string& linkName="btaggingLink" );
    const ElementLink< xAOD::BTaggingContainer >& getBTaggingLink( const SG::AuxElement&,
								   const std::string& linkName="btaggingLink" );
    void setBTaggingLink( const SG::AuxElement&,
			  const ElementLink< xAOD::BTaggingContainer >&,
			  const std::string& linkName="btaggingLink" );
} // namespace xAOD::BTaggingUtilities

#endif
