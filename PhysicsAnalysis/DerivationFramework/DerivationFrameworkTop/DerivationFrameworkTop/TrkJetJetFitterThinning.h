/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#ifndef DERIVATIONFRAMEWORKTOP_TRKJETJETFITTERTHINNING_H
#define DERIVATIONFRAMEWORKTOP_TRKJETJETFITTERTHINNING_H

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
   class TrkJetJetFitterThinning : public AthAlgTool, public IThinningTool {

   public:
      /// AlgTool constructor
      TrkJetJetFitterThinning(const std::string& type,
                              const std::string& name,
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

      /// method for thinning JetFitter vertices
      StatusCode thinJetFitterVertices(std::vector<bool>& mask,
         std::vector<const xAOD::Jet*>& jetsToCheck) const;

      /// @name Tool properties
      /// @{

      /// SG key for the track jet container to use
      std::string m_trkjetSGKey;
      /// SG key for the JetFitter vertices container to use
      std::string m_verticesSGKey;
      /// Selection string to use with the track-jet expression evaluation
      std::string m_selectionString;
      /// Selection string to for whole event -- if fails we will thin away all tracks
      std::string m_event_selectionString = "";
      /// Flag for using @c IThinningSvc::Operator::And (instead of "or")
      bool m_and = false;
      IThinningSvc::Operator::Type m_opType;

      /// @}

      /// Handle for accessing the thinning service
      ServiceHandle< IThinningSvc > m_thinningSvc;
      /// Variables keeping statistics information about the job
      mutable unsigned int m_ntot = 0, m_npass = 0;
      /// The expression evaluation helper object
      std::unique_ptr< ExpressionParserHelper > m_trkjet_parser;
      std::unique_ptr< ExpressionParserHelper > m_event_parser;

   }; // class TrkJetJetFitterThinning

} // namespace DerivationFramework

#endif // DERIVATIONFRAMEWORKTOP_TRKJETJETFITTERHINNING_H
