//
// Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
//

// Local include(s):
#include "EgammaAnalysisAlgorithms/EgammaIsGoodOQSelectionTool.h"

// EDM include(s):
#include "xAODEgamma/Egamma.h"
#include "xAODEgamma/EgammaDefs.h"

// System include(s):
#include <iomanip>

namespace CP {

   EgammaIsGoodOQSelectionTool::
   EgammaIsGoodOQSelectionTool( const std::string& name )
   : asg::AsgTool( name ) {

      // Declare the tool's properties.
      declareProperty( "Mask", m_mask = xAOD::EgammaParameters::ALLOQ,
                       "Mask to require passing object quality bits with" );
   }

   const asg::AcceptInfo& EgammaIsGoodOQSelectionTool::getAcceptInfo() const {

      // Return the internal object.
      return m_accept;
   }

  asg::AcceptData EgammaIsGoodOQSelectionTool::
   accept( const xAOD::IParticle* part ) const {

      // Reset the decision object.
      asg::AcceptData accept {&m_accept};

      // Cast the particle to an e/gamma type.
      const xAOD::Egamma* eg = nullptr;
      if( ( part->type() != xAOD::Type::Electron ) &&
          ( part->type() != xAOD::Type::Photon ) ) {
         ATH_MSG_WARNING( "Non-e/gamma object received" );
         return accept;
      }
      eg = static_cast< const xAOD::Egamma* >( part );

      // Calculate the decision.
      accept.setCutResult( m_oqCutIndex, eg->isGoodOQ( m_mask ) );

      // Return the internal object.
      return accept;
   }

   StatusCode EgammaIsGoodOQSelectionTool::initialize() {

      // Tell the user what is going to happen.
      ATH_MSG_INFO( "Selecting e/gamma objects with OQ mask: 0x"
                    << std::hex << m_mask );

      // Set up the TAccept object.
      m_oqCutIndex = m_accept.addCut( "EgammaOQ", "Egamma object quality cut" );

      // Return gracefully.
      return StatusCode::SUCCESS;
   }

} // namespace CP
