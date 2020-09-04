// Dear emacs, this is -*- c++ -*-
//
// Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
//
#ifndef EGAMMAANALYSISALGORITHMS_EGAMMAISGOODOQSELECTIONTOOL_H
#define EGAMMAANALYSISALGORITHMS_EGAMMAISGOODOQSELECTIONTOOL_H

// Framework include(s):
#include "AsgTools/AsgTool.h"
#include "PATCore/IAsgSelectionTool.h"

namespace CP {

   /// Tool selecting e/gamma objects based on their object quality flags
   ///
   /// This tool can be used in tandem with @c CP::AsgSelectionAlg to flag which
   /// e/gamma objects pass a given object quality requirement. The idea is that
   /// the mask to use by the tool should be taken from xAODEgamma's dictionary,
   /// ideally using a PyROOT configuration.
   ///
   /// @author Attila Krasznahorkay <Attila.Krasznahorkay@cern.ch>
   ///
   class EgammaIsGoodOQSelectionTool final :
   public asg::AsgTool, virtual public IAsgSelectionTool {

   public:
      /// Declare the tool's interface to Gaudi
      ASG_TOOL_CLASS( EgammaIsGoodOQSelectionTool, IAsgSelectionTool )

      /// AsgTool constructor
      EgammaIsGoodOQSelectionTool( const std::string& name );

      /// @name Interface inherited from @c IAsgSelectionTool
      /// @{

      /// Get the results for the last object processed
      virtual const asg::AcceptInfo& getAcceptInfo() const override;

      /// Get the results for a given particle
      virtual asg::AcceptData
      accept( const xAOD::IParticle* part ) const override;

      /// @}

      /// @name Interface inherited from @c asg::AsgTool
      /// @{

      /// Function initialising the tool
      virtual StatusCode initialize() override;

      /// @}

   private:
      /// @name Tool properties
      /// @{

      /// The mask to require good object quality with
      int m_mask;

      /// @}

      /// Object handling the e/gamma selection decision
      asg::AcceptInfo m_accept{ "EgammaOQ" };
      /// Index of the object quality cut
      int m_oqCutIndex{ -1 };

   }; // class EgammaIsGoodOQSelectionTool

} // namespace CP

#endif // EGAMMAANALYSISALGORITHMS_EGAMMAISGOODOQSELECTIONTOOL_H
