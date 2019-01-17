/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/

#ifndef DERIVATIONFRAMEWORKTOOLS_BTAGGEDOBJECTTHINNING_H
#define DERIVATIONFRAMEWORKTOOLS_BTAGGEDOBJECTTHINNING_H

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

   /// Tool performing a string based thinning on any @c xAOD::IParticle container
   ///
   /// This tool is used in derivation jobs to thin an
   /// @c xAOD::IParticleContainer based on a string expression.
   ///
   /// @author James Catmore (James.Catmore@cern.ch)
   ///
   class BtaggedObjectThinning : public AthAlgTool, public IThinningTool {

   public:
      /// AlgTool constructor
      BtaggedObjectThinning( const std::string& type, const std::string& name,
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

      /// SG key for the particle container to use
      std::string m_sgKey;
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

   }; // class BtaggedObjectThinning

} // namespace DerivationFramework

#endif // DERIVATIONFRAMEWORKTOOLS_BTAGGEDOBJECTTHINNING_H
