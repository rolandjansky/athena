/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/

#ifndef DERIVATIONFRAMEWORKINDET_TAUTRACKPARTICLETHINNING_H
#define DERIVATIONFRAMEWORKINDET_TAUTRACKPARTICLETHINNING_H

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

namespace DerivationFramework {

   /// Tool selecting track particles from tau objects to keep
   ///
   /// This tool is used in derivation jobs to set up the thinning such that
   /// track particles associated with tau objects would be kept for a given
   /// output stream.
   ///
   /// @author James Catmore (James.Catmore@cern.ch)
   ///
   class TauTrackParticleThinning : public AthAlgTool, public IThinningTool {

   public:
      /// AlgTool constructor
      TauTrackParticleThinning( const std::string& type,
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
      /// @name Tool properties
      /// @{

      /// SG key of the tau container to use
      std::string m_tauSGKey;
      /// SG key of the track particle container to use
      std::string m_inDetSGKey = "InDetTrackParticles";
      /// Selection string to use with the expression evaluation
      std::string m_selectionString;
      /// Cone around tau objects to keep track particles in
      float m_coneSize = -1.0;
      /// Flag for using @c IThinningSvc::Operator::And (instead of "or")
      bool m_and = false;
      /// Apply thinning to 'classifiedCharged' tau tracks in addition to ID tracks
      bool m_doTauTracksThinning = false;
      /// SG key of the tau track container to use
      std::string m_tauTracksSGKey = "TauTracks";
      /// @}

      /// Handle for accessing the thinning service
      ServiceHandle< IThinningSvc > m_thinningSvc;
      /// Variables keeping statistics information about the job
      mutable unsigned int m_ntot = 0, m_npass = 0;
      /// The expression evaluation helper object
      std::unique_ptr< ExpressionParserHelper > m_parser;

   }; // class TauTrackParticleThinning

} // namespace DerivationFramework

#endif // DERIVATIONFRAMEWORKINDET_TAUTRACKPARTICLETHINNING_H
