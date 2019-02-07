/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

/// @author Nils Krumnack



// Dear emacs, this is -*- c++ -*-
// $Id: SystematicsTool.h 638687 2015-01-10 20:31:47Z sfarrell $
#ifndef PATINTERFACES_SYSTEMATICS_TOOL_H
#define PATINTERFACES_SYSTEMATICS_TOOL_H

// System include(s):
#include <unordered_map>

// Local include(s):
#include "PATInterfaces/ISystematicsTool.h"
#include "PATInterfaces/SystematicVariation.h"
#include "PATInterfaces/SystematicCode.h"
#include "PATInterfaces/SystematicSet.h"

namespace CP {

   /// Base class for CP tools providing systematic variations
   ///
   /// This module provides a base class for systematics aware CP tools.
   /// The interface provided in this module is intended for the general
   /// user.  The module is considered to be in the pre-alpha stage.
   ///
   /// CP tools are not required to use this base class to implement the
   /// systematics interface.  They can just implement their own version
   /// of the interface.  Though they should probably take a quick glance
   /// of what this class does, particularly they shouldn't forget to
   /// register all their systematics with the global list.
   ///
   /// The main difference between implementing the interface yourself and
   /// using this base class is that with this class you register all the
   /// systematics with the base class.  The tool then only needs to
   /// override sysApplySystematicVariation (instead of
   /// applySystematicVariation), which gets passed only the systematics
   /// filtered for the affecting systematics (which get also stored in
   /// the base class).
   ///
   /// @author Nils Krumnack <nils.erik.krumnack@cern.ch>
   /// @author Attila Krasznahorkay <Attila.Krasznahorkay@cern.ch>
   ///
   /// $Revision: 638687 $
   /// $Date: 2015-01-10 21:31:47 +0100 (Sat, 10 Jan 2015) $
   ///
   class SystematicsTool : virtual public ISystematicsTool {

   public:
      /// effects: standard constructor
      /// guarantee: strong
      /// failures: out of memory I
      SystematicsTool ();

      /// returns: whether this tool is affected by the given systematic
      /// guarantee: no-fail
      virtual bool
      isAffectedBySystematic( const SystematicVariation& systematic ) const;

      /// returns: the list of all systematics this tool can be affected
      ///   by
      /// guarantee: strong
      /// failures: out of memory II
      virtual SystematicSet
      affectingSystematics() const;

      /// returns: the list of all systematics this tool can be affected
      ///   by
      /// guarantee: strong
      /// failures: out of memory II
      virtual SystematicSet
      recommendedSystematics() const;

      /// returns: the list of systematics that have been applied to
      ///   this tool.  this is not including systematics that were
      ///   applied, but do not affect this tool
      /// guarantee: no-fail
      const SystematicSet& appliedSystematics() const;

      /// returns: a string representation of appliedSystematics()
      /// guarantee: strong
      /// failures: out of memory II
      /// rationale: this is meant to be used when printing out the
      ///   applied systematics for debugging purposes (or when
      ///   embedding them in a string).
      std::string appliedSystematicsString () const;

      /// effects: configure this tool for the given list of systematic
      ///   variations.  any requested systematics that are not
      ///   affecting this tool will be silently ignored (unless they
      ///   cause other errors).
      /// guarantee: basic
      /// failures: out of memory II
      /// failures: systematic unknown
      /// failures: requesting multiple variations on the same
      ///   systematic (e.g. up & down)
      /// failures: requesting an unsupported variation on an otherwise
      ///   supported systematic (e.g. a 2 sigma variation and the tool
      ///   only supports 1 sigma variations)
      /// failures: unsupported combination of supported systematic
      /// failures: other tool specific errors
      virtual SystematicCode applySystematicVariation
      ( const SystematicSet& systConfig );

   protected:
      /// effects: add a systematic to the list of registered systematics.
      ///   It will optionally also add this sytematic to the recommended set.
      /// side effects: this will add the systematic to the global list
      ///   of systematics, if not already there
      /// guarantee: strong
      /// failures: out of memory II
      SystematicCode addAffectingSystematic
      ( const SystematicVariation& systematic, bool recommended );

      /// effects: add a systematic to the list of recommended systematics
      /// failures: unsupported systematic
      SystematicCode addRecommendedSystematic
      ( const SystematicVariation& systematic );

      /// effects: add a SystematicSet to the registered systematics.
      void addAffectingSystematics
      ( const SystematicSet& systematics );

      /// effects: add a SystematicSet to the recommended systematics.
      SystematicCode addRecommendedSystematics
      ( const SystematicSet& systematics );

      /// effects: configure this tool for the given list of systematic
      ///   variations.  systConfig is guaranteed to contain only
      ///   supported systematics.
      /// guarantee: basic
      /// failures: out of memory II
      /// failures: unsupported combination of supported systematics
      /// failures: other tool specific errors
      virtual SystematicCode sysApplySystematicVariation
      ( const SystematicSet& systConfig ) = 0;

   public:
      /// effects: test the invariance of this object
      /// guarantee: no-fail
      void testInvariant() const;

   protected:
      /// description: members directly corresponding to accessors
      SystematicSet m_affectingSystematics;
      SystematicSet m_recommendedSystematics;

   private:
      /// description: pointer to currenty applied systematics
      SystematicSet* m_appliedSystematics;

      /// map of cached systematics filtering
      std::unordered_map<SystematicSet, SystematicSet> m_systFilterMap;

   }; // class SystematicsTool

} // namespace CP

#endif // PATINTERFACES_SYSTEMATICS_TOOL_H
