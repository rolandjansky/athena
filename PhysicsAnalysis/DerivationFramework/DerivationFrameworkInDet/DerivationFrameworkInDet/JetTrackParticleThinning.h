/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/

#ifndef DERIVATIONFRAMEWORKINDET_JETTRACKPARTICLETHINNING_H
#define DERIVATIONFRAMEWORKINDET_JETTRACKPARTICLETHINNING_H

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

   /// Tool selecting track particles from jets to keep
   ///
   /// This tool is used in derivation jobs to set up the thinning such that
   /// track particles associated with jet objects would be kept for a given
   /// output stream.
   ///
   /// @author James Catmore (James.Catmore@cern.ch)
   ///
   class JetTrackParticleThinning : public AthAlgTool, public IThinningTool {

   public:
      /// AlgTool constructor
      JetTrackParticleThinning( const std::string& type,
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

      /// SG key for the jet container to use
      std::string m_jetSGKey;
      /// SG key of the track particle container to use
      std::string m_inDetSGKey = "InDetTrackParticles";
      /// Selection string to use with the expression evaluation
      std::string m_selectionString;
      /// DR matching criteria for use in addition to ghost-association
      float m_dRMatch = 0.0;
      /// Flag for using @c IThinningSvc::Operator::And (instead of "or")
      bool m_and = false;

      /// @}

      /// Handle for accessing the thinning service
      ServiceHandle< IThinningSvc > m_thinningSvc;
      /// Variables keeping statistics information about the job
      mutable unsigned int m_ntot = 0, m_npass = 0;
      /// The expression evaluation helper object
      std::unique_ptr< ExpressionParserHelper > m_parser;

   }; // class JetTrackParticleThinning

} // namespace DerivationFramework

#endif // DERIVATIONFRAMEWORKINDET_JETTRACKPARTICLETHINNING_H
