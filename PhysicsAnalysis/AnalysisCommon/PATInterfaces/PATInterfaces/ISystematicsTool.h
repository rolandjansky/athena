// Dear emacs, this is -*- c++ -*-
// $Id: ISystematicsTool.h 602616 2014-06-19 11:43:22Z krumnack $
#ifndef PATINTERFACES_ISYSTEMATICSTOOL_H
#define PATINTERFACES_ISYSTEMATICSTOOL_H

//        Copyright Iowa State University 2014.
//                  Author: Nils Krumnack
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

// System include(s):

// Framework include(s):
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
   /// $Revision: 602616 $
   /// $Date: 2014-06-19 13:43:22 +0200 (Thu, 19 Jun 2014) $
   ///
   class ISystematicsTool : public virtual asg::IAsgTool {

      /// Declare the interface that this class provides
      ASG_TOOL_INTERFACE( CP::ISystematicsTool )

   public:
      /// returns: whether this tool is affected by the given systematis
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
