
#include "xAODBTagging/BTaggingUtilities.h"

using namespace xAOD;

const xAOD::BTagging* BTaggingUtilities::getBTagging( const SG::AuxElement& auxElement,
						      const std::string& linkName ) {
  const SG::AuxElement::Accessor< ElementLink< xAOD::BTaggingContainer > > btagAcc( linkName );
  
  if ( btagAcc.isAvailable( auxElement ) && btagAcc( auxElement ).isValid() )
    return *( btagAcc( auxElement ) );
  
  return nullptr;
}

const ElementLink< xAOD::BTaggingContainer >& BTaggingUtilities::getBTaggingLink( const SG::AuxElement& auxElement,
										  const std::string& linkName ) {
  const SG::AuxElement::Accessor< ElementLink< xAOD::BTaggingContainer > > btagAcc( linkName );
  return btagAcc( auxElement );
}

void BTaggingUtilities::setBTaggingLink( const SG::AuxElement& auxElement,
					 const ElementLink< xAOD::BTaggingContainer >& el,
					 const std::string& linkName ) {
  const SG::AuxElement::Decorator< ElementLink< xAOD::BTaggingContainer > > btagDec( linkName );
  btagDec( auxElement ) = el;
}
