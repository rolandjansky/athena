/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#ifndef DERIVATIONFRAMEWORKTOP_TRKJETTRACKTHINNING_H
#define DERIVATIONFRAMEWORKTOP_TRKJETTRACKTHINNING_H

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
#include "xAODJet/Jet.h"

namespace DerivationFramework {

   /// Tool selecting track particles from jets to keep
   ///
   /// This tool is used in derivation jobs to set up the thinning such that
   /// track particles associated with jet objects would be kept for a given
   /// output stream.
   ///
   class TrkJetTrackThinning : public AthAlgTool, public IThinningTool {

   public:
      /// AlgTool constructor
      TrkJetTrackThinning(const std::string& type, const std::string& name,
            const IInterface* parent);

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

      /// method for thinning track jet constituents
      StatusCode thinConstituents(std::vector<bool>& mask,
            std::vector<const xAOD::Jet*>& jetsToCheck) const;
      /// method for thinning track jet JetFitter tracks
      StatusCode thinJetFitterTracks(std::vector<bool>& mask,
            std::vector<const xAOD::Jet*>& jetsToCheck) const;
      /// method for thinning track jet SV1 tracks
      StatusCode thinSV1Tracks(std::vector<bool>& mask,
            std::vector<const xAOD::Jet*>& jetsToCheck) const;

      /// @name Tool properties
      /// @{

      /// what to thin: "constituents", "JetFitter", or "SV1"
      bool m_thinConstituents = true;
      bool m_thinJetFitter = false;
      bool m_thinSV1 = false;
      /// SG key for the track jet container to use
      std::string m_trkjetSGKey;
      /// SG key of the track particle container to use
      std::string m_inDetSGKey = "InDetTrackParticles";
      /// Selection string to use with the track-jet expression evaluation
      std::string m_selectionString;
      /// Selection string to for whole event -- if fails we will thin away all tracks
      std::string m_event_selectionString = "";
      /// Flag for using @c IThinningSvc::Operator::And (instead of "or")
      bool m_and = false;

      /// @}

      /// Handle for accessing the thinning service
      ServiceHandle< IThinningSvc > m_thinningSvc;
      /// Variables keeping statistics information about the job
      mutable unsigned int m_ntot = 0, m_npass = 0;
      /// The expression evaluation helper object
      std::unique_ptr< ExpressionParserHelper > m_trkjet_parser;
      std::unique_ptr< ExpressionParserHelper > m_event_parser;

   }; // class TrkJetTrackThinning

} // namespace DerivationFramework

#endif // DERIVATIONFRAMEWORKTOP_TRKJETTRACKTHINNING_H
