// Dear emacs, this is -*- c++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: TTransTrees.h 645462 2015-02-10 10:04:54Z krasznaa $
#ifndef XAODROOTACCESS_TOOLS_TTRANSTREES_H
#define XAODROOTACCESS_TOOLS_TTRANSTREES_H

// Forward declaration(s):
class TTree;

namespace xAOD {

   /// A simple helper class holding both event and metadata transient trees
   ///
   /// In order to access both the event and metadata information in PyROOT in
   /// a user friendly way, we use this helper class. It's extremely simple, it
   /// just points to two transient trees that describe the event- and metadata
   /// of the input file(s), without owning those trees. The trees are always
   /// owned by the xAOD::MakeTransientTrees functions.
   ///
   /// @author Attila Krasznahorkay <Attila.Krasznahorkay@cern.ch>
   ///
   /// $Revision: 645462 $
   /// $Date: 2015-02-10 11:04:54 +0100 (Tue, 10 Feb 2015) $
   ///
   class TTransTrees {

   public:
      /// Constructor with two transient tree pointers
      TTransTrees( ::TTree* eventTree, ::TTree* metaTree )
         : m_eventTree( eventTree ), m_metaTree( metaTree ) {}

      /// Get a pointer to the transient event tree
      ::TTree* eventTree() const { return m_eventTree; }

      /// Get a pointer to the transient metadata tree
      ::TTree* metaTree() const { return m_metaTree; }

   private:
      /// Pointer to the transient event tree
      ::TTree* m_eventTree;
      /// Pointer to the transient metadata tree
      ::TTree* m_metaTree;

   }; // class TTransTrees

} // namespace xAOD

#endif // XAODROOTACCESS_TOOLS_TTRANSTREES_H
