/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

/// @author Nils Krumnack



// Dear emacs, this is -*- c++ -*-
// $Id: ISystematicsTool.h 676499 2015-06-18 20:13:50Z sfarrell $
#ifndef PATINTERFACES_ISYSTEMATICSTOOL_H
#define PATINTERFACES_ISYSTEMATICSTOOL_H

#include "AsgTools/IAsgTool.h"

// Local include(s):
#include "PATInterfaces/SystematicCode.h"
#include "PATInterfaces/SystematicVariation.h"
#include "PATInterfaces/SystematicSet.h"

namespace CP {

   /// Interface for all CP tools supporting systematic variations
   ///
   /// This interface is meant to be used by the systematics handling
   /// system to configure different CP tools to change their behaviour
   /// at runtime.
   ///
   /// @author Nils Krumnack <nils.erik.krumnack@cern.ch>
   /// @author Attila Krasznahorkay <Attila.Krasznahorkay@cern.ch>
   ///
   /// $Revision: 676499 $
   /// $Date: 2015-06-18 22:13:50 +0200 (Thu, 18 Jun 2015) $
   ///
   class ISystematicsTool : public virtual asg::IAsgTool {

      /// Declare the interface that this class provides
      ASG_TOOL_INTERFACE( CP::ISystematicsTool )

   public:

      /// Virtual destructor
      virtual ~ISystematicsTool(){};

      /// returns: whether this tool is affected by the given systematics
      virtual bool
      isAffectedBySystematic( const SystematicVariation& systematic ) const = 0;

      /// returns: the list of all systematics this tool can be affected by
      virtual SystematicSet
      affectingSystematics() const = 0;

      /// returns: the list of all systematics this tool recommends to use
      virtual SystematicSet
      recommendedSystematics() const = 0;

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
      ( const SystematicSet& systConfig ) = 0;

   }; // class ISystematicsTool

} // namespace CP

#endif // PATINTERFACES_ISYSTEMATICSTOOL_H
