/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/

#ifndef DERIVATIONFRAMEWORKSUSY_TAUTRACKSTHINNING_H
#define DERIVATIONFRAMEWORKSUSY_TAUTRACKSTHINNING_H

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

   /// Tool selecting TauTracks from TauJets objects to keep
   class TauTracksThinning : public AthAlgTool, public IThinningTool {

   public:
      /// AlgTool constructor
      TauTracksThinning( const std::string& type,
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

      /// SG key of the tau jets container to use
      std::string m_tauSGKey;
      /// SG key of the tau tracks container to use
      std::string m_tracksSGKey = "TauTracks";
      /// SG key of the ID track particle container to use (optional)
      std::string m_IDtracksSGKey;
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

   }; // class TauTracksThinning

} // namespace DerivationFramework

#endif // DERIVATIONFRAMEWORKSUSY_TAUTRACKSTHINNING_H
