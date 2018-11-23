/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/

#ifndef DERIVATIONFRAMEWORKCALO_JETPFLOWTHINNING_H
#define DERIVATIONFRAMEWORKCALO_JETPFLOWTHINNING_H

// System include(s):
#include <string>
#include <memory>
#include <vector>

// Framework include(s):
#include "GaudiKernel/ServiceHandle.h"
#include "AthenaKernel/IThinningSvc.h"
#include "AthenaBaseComps/AthAlgTool.h"

#include "xAODPFlow/PFOContainer.h"
#include "xAODPFlow/PFOAuxContainer.h"
#include "xAODPFlow/PFO.h"
#include "xAODCore/ShallowCopy.h"

// DF include(s):
#include "DerivationFrameworkInterfaces/IThinningTool.h"
#include "DerivationFrameworkInterfaces/ExpressionParserHelper.h"

namespace DerivationFramework {

   /// Tool selecting calo clusters from jets to keep
   ///
   /// This tool is used in derivation jobs to set up the thinning such that
   /// calo clusters associated with jet objects would be kept for a given
   /// output stream.
   ///
   /// @author Danilo E. Ferreira de Lima <dferreir@cern.ch>
   ///
   class JetPFlowThinning : public AthAlgTool, public IThinningTool {

   public:
      /// AlgTool constructor
      JetPFlowThinning( const std::string& type, const std::string& name,
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
      std::string m_sgKey;
      /// SG key of the cluster container to use
      std::string m_PFOSGKey = "JetETMiss";
      /// Selection string to use with the expression evaluation
      std::string m_selectionString;
      /// Additional cluster containers to set up the same thinning on
      std::vector< std::string > m_addPFOSGKey;
      /// Flag for using @c IThinningSvc::Operator::And (instead of "or")
      bool m_and = false;

      /// @}

      /// Handle for accessing the thinning service
      ServiceHandle< IThinningSvc > m_thinningSvc;
      /// The expression evaluation helper object
      std::unique_ptr< ExpressionParserHelper > m_parser;



   }; // class JetPFlowThinning






} // namespace DerivationFramework

#endif // DERIVATIONFRAMEWORKCALO_JETCALOCLUSTERTHINNING_H
