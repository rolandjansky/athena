// Dear emacs, this is -*- c++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: MuonSmearingToolExample.h 300810 2014-06-06 12:28:18Z krasznaa $
#ifndef CPANALYSISEXAMPLES_MUONSMEARINGTOOL_H
#define CPANALYSISEXAMPLES_MUONSMEARINGTOOL_H

// Rationale: temporary suppression of
// unused-local-typedef warning in Boost header
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wunused-local-typedefs"
#include "boost/tuple/tuple.hpp"
#pragma GCC diagnostic pop

// Framework include(s):
#include "AsgTools/AsgTool.h"
#include "PATInterfaces/SystematicsTool.h"
#include "PATInterfaces/CorrectionTool.h"

// EDM include(s):
#include "xAODMuon/MuonContainer.h"

// Local include(s):
#include "CPAnalysisExamples/IMuonSmearingToolExample.h"

namespace CP {

   class SystematicSet;

   /// A simple implementation of the muon correction tool interface
   ///
   /// This tries to be a simple, but meaningful example of how correction
   /// tools should be implemented for Run 2.
   ///
   /// @author Attila Krasznahorkay <Attila.Krasznahorkay@cern.ch>
   ///
   /// $Revision: 300810 $
   /// $Date: 2014-06-06 14:28:18 +0200 (Fri, 06 Jun 2014) $
   ///
   class MuonSmearingToolExample : public virtual IMuonSmearingToolExample,
                                   public virtual CP::SystematicsTool,
                                   public virtual CP::CorrectionTool< xAOD::MuonContainer >,
                                   public asg::AsgTool {

      /// Create a proper constructor for Athena
      ASG_TOOL_CLASS2( MuonSmearingToolExample, CP::ISystematicsTool,
                       CP::IMuonSmearingToolExample )

   public:
      /// Create a constructor for standalone usage
      MuonSmearingToolExample( const std::string& name );

      /// @name Function(s) implementing the asg::IAsgTool interface
      /// @{

      /// Function initialising the tool
      virtual StatusCode initialize();

      /// @}

      /// @name Functions implementing the CP::IMuonSmearingToolExample interface
      /// @{

      /// Apply the correction on a modifyable object
      virtual CorrectionCode applyCorrection( xAOD::Muon& mu );

      /// Create a corrected copy from a constant muon
      virtual CorrectionCode correctedCopy( const xAOD::Muon& input,
                                            xAOD::Muon*& output ) {
         return CP::CorrectionTool< xAOD::MuonContainer >::
            correctedCopy( input, output );
      }

      /// @}

   protected:
      /// Function receiving the systematic variations that should be applied
      virtual SystematicCode sysApplySystematicVariation
      ( const SystematicSet & systConfig );

   }; // class MuonSmearingToolExample

} // namespace CP

#endif // CPANALYSISEXAMPLES_MUONSMEARINGTOOL_H
