// Dear emacs, this is -*- c++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: JetCalibrationToolExample3.h 299883 2014-03-28 17:34:16Z krasznaa $
#ifndef CPANALYSISEXAMPLES_JETCALIBRATIONTOOLEXAMPLE_H
#define CPANALYSISEXAMPLES_JETCALIBRATIONTOOLEXAMPLE_H

// Rationale: temporary suppression of
// unused-local-typedef warning in Boost header
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wunused-local-typedefs"
#include "boost/tuple/tuple.hpp"
#pragma GCC diagnostic pop

// Framework include(s):
#include "AsgTools/AsgTool.h"

// Local include(s):
#include "CPAnalysisExamples/IJetCalibrationTool.h"
#include "PATInterfaces/ISystematicsTool.h"
#include "PATInterfaces/CorrectionTool.h"
#include "xAODJet/JetContainer.h"

#include <boost/unordered_map.hpp>

namespace CP {

   /// A simple implementation of the jet calibration tool interface
   ///
   /// This tries to be a simple, but meaningful example of how correction
   /// tools should be implemented for Run 2.
   ///
   /// @author Attila Krasznahorkay <Attila.Krasznahorkay@cern.ch>
   /// @author Iacopo Vivarelli <iacopo.vivarelli@cern.ch>
   ///
   /// $Revision: 299883 $
   /// $Date: 2014-03-28 18:34:16 +0100 (Fri, 28 Mar 2014) $
   ///
   class JetCalibrationToolExample : public virtual IJetCalibrationTool,
                                     public virtual ISystematicsTool,
                                     public virtual CP::CorrectionTool< xAOD::JetContainer >,
                                     public asg::AsgTool
   {
      /// Create a proper constructor for Athena
      ASG_TOOL_CLASS2( JetCalibrationToolExample, CP::IJetCalibrationTool, CP::ISystematicsTool )

   public:
      /// Create a constructor for standalone usage
      JetCalibrationToolExample( const std::string& name );

      /// @name Function(s) implementing the asg::IAsgTool interface
      /// @{

      /// Function initialising the tool
      virtual StatusCode initialize();

      /// @}

      /// @name Function(s) implementing the IJetCalibrationTool interface
      /// @{

      /// Apply the correction on a modifyable object
      virtual CorrectionCode applyCorrection( xAOD::Jet& object );
      /// Create a corrected copy from a constant jet
      virtual CorrectionCode correctedCopy( const xAOD::Jet& input,
                                            xAOD::Jet*& output );

      /// @}

      /// returns: whether this tool is affected by the given systematis
      virtual bool
      isAffectedBySystematic( const SystematicVariation& systematic ) const;

      /// returns: the list of all systematics this tool can be affected by
      virtual SystematicSet
      affectingSystematics() const;

      /// returns: the list of all systematics this tool recommends to use
      virtual SystematicSet
      recommendedSystematics() const;

      /// effects: configure this tool for the given list of systematic
      ///   variations.  any requested systematics that are not
      ///   affecting this tool will be silently ignored (unless they
      ///   cause other errors).
      /// failures: systematic unknown
      /// failures: requesting multiple variations on the same
      ///   systematic (e.g. up & down)
      /// failures: requesting an unsupported variation on an otherwise
      ///   supported systematic (e.g. a 2 sigma variation and the tool
      ///   only supports 1 sigma variations)
      /// failures: unsupported combination of supported systematic
      /// failures: other tool specific errors
      ///
      virtual SystematicCode applySystematicVariation
      ( const SystematicSet& systConfig );

   private:
      struct Calib
      {
         float m_calibrationFactor;
      };
      boost::unordered_map<SystematicSet, Calib> m_calib;
      Calib* m_currentCalib;

   }; // class JetCalibrationToolExample

} // namespace CP

#endif // CPANALYSISEXAMPLES_JETCALIBRATIONTOOLEXAMPLE1_H
