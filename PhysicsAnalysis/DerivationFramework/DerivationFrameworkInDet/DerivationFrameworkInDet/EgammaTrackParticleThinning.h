/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/

#ifndef DERIVATIONFRAMEWORKINDET_EGAMMATRACKPARTICLETHINNING_H
#define DERIVATIONFRAMEWORKINDET_EGAMMATRACKPARTICLETHINNING_H

// System include(s):
#include <string>
#include <memory>
#include <vector>

// Framework include(s):
#include "GaudiKernel/ServiceHandle.h"
#include "AthenaKernel/IThinningSvc.h"
#include "AthenaBaseComps/AthAlgTool.h"

// DF include(s):
#include "DerivationFrameworkInterfaces/IThinningTool.h"
#include "DerivationFrameworkInterfaces/ExpressionParserHelper.h"

// EDM include(s):
#include "xAODEgamma/EgammaFwd.h"

namespace DerivationFramework {

   /// Tool selecting track particles from e/gamma objects to keep
   ///
   /// This tool is used in derivation jobs to set up the thinning such that
   /// track particles associated with e/gamma objects would be kept for a given
   /// output stream.
   ///
   /// @author James Catmore (James.Catmore@cern.ch)
   ///
   class EgammaTrackParticleThinning : public AthAlgTool, public IThinningTool {

   public:
      /// AlgTool constructor
      EgammaTrackParticleThinning( const std::string& type,
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
      /// @name Function(s) implementing most of the logic
      /// @{

      /// Set up the filtering mask for the track particles from photons
      StatusCode
      setPhotonMasks( std::vector< bool >& tpMask,
                      std::vector< bool >& gsfTpMask,
                      const std::vector< const xAOD::Egamma* >& egammas,
                      const xAOD::TrackParticleContainer* tps,
                      const xAOD::TrackParticleContainer* gsfTps ) const;
      /// Set up the filtering mask for the track particles from electrons
      StatusCode
      setElectronMasks( std::vector< bool >& tpMask,
                        std::vector< bool >& gsfTpMask,
                        const std::vector< const xAOD::Egamma* >& egammas,
                        const xAOD::TrackParticleContainer* tps,
                        const xAOD::TrackParticleContainer* gsfTps ) const;

      /// @}

      /// @name Tool properties
      /// @{

      /// SG key of the e/gamma container to use
      std::string m_sgKey;
      /// SG key of the track particle container to use
      std::string m_inDetSGKey = "InDetTrackParticles";
      /// SG key of the GSF track particle container to use
      std::string m_gsfSGKey = "GSFTrackParticles";
      /// Selection string to use with the expression evaluation
      std::string m_selectionString;
      /// Flag for keeping just the track particles best matching e/g object
      bool m_bestMatchOnly = true;
      /// Cone around e/g objects to keep track particles in
      float m_coneSize = -1.0;
      /// Flag for using @c IThinningSvc::Operator::And (instead of "or")
      bool m_and = false;

      /// @}

      /// Handle for accessing the thinning service
      ServiceHandle< IThinningSvc > m_thinningSvc;
      /// Variables keeping statistics information about the job
      mutable unsigned int m_ntot = 0, m_ntotGSF = 0, m_npass = 0,
                           m_nGSFPass = 0;
      /// The expression evaluation helper object
      std::unique_ptr< ExpressionParserHelper > m_parser;

   }; // class EgammaTrackParticleThinning

} // namespace DerivationFramework

#endif // DERIVATIONFRAMEWORKINDET_EGAMMATRACKPARTICLETHINNING_H
