// Dear emacs, this is -*- c++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: MuonSelectionTool.h 299883 2014-03-28 17:34:16Z krasznaa $
#ifndef CPANALYSISEXAMPLES_MUONSELECTIONTOOL_H
#define CPANALYSISEXAMPLES_MUONSELECTIONTOOL_H

// Framework include(s):
#include "AsgTools/AsgTool.h"
#include "PATCore/IAsgSelectionTool.h"

// Local include(s):
#include "CPAnalysisExamples/IMuonSelectionToolExample.h"

namespace CP {

   /// Implementation of the muon selector tool
   ///
   /// Example implementation of how an object selector tool should
   /// look according to the TF3 recommendations.
   ///
   /// @author Attila Krasznahorkay <Attila.Krasznahorkay@cern.ch>
   ///
   /// $Revision: 299883 $
   /// $Date: 2014-03-28 18:34:16 +0100 (Fri, 28 Mar 2014) $
   ///
   class MuonSelectionToolExample : public virtual IAsgSelectionTool,
                                    public virtual IMuonSelectionToolExample,
                                    public asg::AsgTool {

      /// Create a proper constructor for Athena
      ASG_TOOL_CLASS2( MuonSelectionToolExample, IAsgSelectionTool,
                       CP::IMuonSelectionToolExample )

   public:
      /// Constructor for standalone usage
      MuonSelectionToolExample( const std::string& name );

      /// @name Function(s) implementing the asg::IAsgTool interface
      /// @{

      /// Function initialising the tool
      virtual StatusCode initialize();

      /// @}

      /// @name Function(s) implementing the IAsgSelectionTool interface
      /// @{

      /// Get an object describing the "selection steps" of the tool
      virtual const Root::TAccept& getTAccept() const;

      /// Get the decision using a generic IParticle pointer
      virtual const Root::TAccept& accept( const xAOD::IParticle* p ) const;

      /// @}

      /// @name Function(s) implementing the IMuonSelectorTool interface
      /// @{

      /// Get the decision for a specific Muon object
      virtual const Root::TAccept& accept( const xAOD::Muon& mu ) const;

      /// @}

   private:
      /// Maximum pseudorapidity for the selected muons
      double m_maxEta;
      /// Author for the selected e-mails
      int m_author;

      /// Object used to store the last decision
      mutable Root::TAccept m_accept;

   }; // class MuonSelectionToolExample

} // namespace CP

#endif // CPANALYSISEXAMPLES_MUONSELECTIONTOOL_H
