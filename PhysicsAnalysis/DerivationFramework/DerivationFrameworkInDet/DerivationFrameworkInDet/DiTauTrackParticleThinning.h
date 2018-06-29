/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/

#ifndef DERIVATIONFRAMEWORKINDET_DITAUTRACKPARTICLETHINNING_H
#define DERIVATIONFRAMEWORKINDET_DITAUTRACKPARTICLETHINNING_H

// System include(s):
#include <string>
#include <memory>

// Framework include(s):
#include "GaudiKernel/ServiceHandle.h"
#include "AthenaKernel/IThinningSvc.h"
#include "AthenaBaseComps/AthAlgTool.h"

// DF include(s):
#include "DerivationFrameworkInterfaces/IThinningTool.h"
#include "DerivationFrameworkInterfaces/ExpressionParserHelper.h"

// EDM include(s):
#include "xAODTau/DiTauJet.h"

namespace DerivationFramework {

   /// Tool selecting track particles from DiTau objects to keep
   ///
   /// This tool is used in derivation jobs to set up the thinning such that
   /// track particles associated with DiTau objects would be kept for a given
   /// output stream.
   ///
   /// @author James Catmore (James.Catmore@cern.ch)
   ///
   class DiTauTrackParticleThinning : public AthAlgTool, public IThinningTool {

   public:
      /// AlgTool constructor
      DiTauTrackParticleThinning( const std::string& type,
                                  const std::string& name,
                                  const IInterface* parent );

      /// @name Function(s) implementing the @c IAlgTool interface
      /// @{

      /// Function initialising the tool
      StatusCode initialize() override;
      /// Function finalising the tool
      StatusCode finalize() override;

      /// @}

      /// @name Function(s) implementing the
      ///       @c DerivationFramework::IThinningTool interface
      /// @{

      /// Function performing the configured thinning operation
      StatusCode doThinning() const override;

      /// @}

   private:
      /// Helper function for setting pass flags
      StatusCode setMasks( const xAOD::DiTauJet::TrackParticleLinks_t& links,
                           std::vector< bool >& mask ) const;

      /// @name Tool properties
      /// @{

      /// SG key for the DiTau container to use
      std::string m_ditauSGKey;
      /// SG key of the track particle container to use
      std::string m_inDetSGKey = "InDetTrackParticles";
      /// Selection string to use with the expression evaluation
      std::string m_selectionString;
      /// Flag for using @c IThinningSvc::Operator::And (instead of "or")
      bool m_and = false;

      /// @}

      /// Handle for accessing the thinning service
      ServiceHandle< IThinningSvc > m_thinningSvc;
      /// Variables keeping statistics information about the job
      mutable unsigned int m_ntot = 0, m_npass = 0;
      /// The expression evaluation helper object
      std::unique_ptr< ExpressionParserHelper > m_parser;

   }; // class DiTauTrackParticleThinning

} // namespace DerivationFramework

#endif // DERIVATIONFRAMEWORKINDET_DITAUTRACKPARTICLETHINNING_H
