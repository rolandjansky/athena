// Dear emacs, this is -*- c++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: MakeTransientTree.h 741414 2016-04-19 17:06:16Z krasznaa $
#ifndef XAODROOTACCESS_MAKETRANSIENTTREE_H
#define XAODROOTACCESS_MAKETRANSIENTTREE_H

// Local include(s):
#include "xAODRootAccess/tools/TTransTrees.h"
#include "xAODRootAccess/TEvent.h"

// Forward declaration(s):
class TTree;
class TChain;
class TFile;

namespace xAOD {

   /// Function creating a transient event and metadata tree using an input file
   ///
   /// In order to access both event-level and metadata in a PyROOT analysis,
   /// the user needs to receive a transient tree representing the event-level
   /// information, and another transient tree representing the file's metadata.
   /// This function takes care of creating both these trees.
   ///
   /// In case you need to access both event and metadata in your PyROOT code,
   /// you need to use this function. As calling MakeTransientTree(...) and
   /// MakeTransientMetaTree(...) one after the other will produce a weird,
   /// likely broken setup.
   ///
   /// @param ifile    Pointer to an xAOD file opened by the user
   /// @param treeName Optional name of the event tree in the input file
   /// @param mode     Access mode of the internal TEvent object
   /// @returns 
   TTransTrees MakeTransientTrees( ::TFile* ifile,
                                   const char* treeName = "CollectionTree",
                                   TEvent::EAuxMode mode =
                                      TEvent::kUndefinedAccess );

   /// Function cleaning up the managed memory
   ///
   /// This function can be used to explicitly clean up the memory managed by
   /// the above functions. This is mostly useful when debugging the code with
   /// valgrind. Users don't need to use this function in an interactive/PyROOT
   /// session.
   ///
   void ClearTransientTrees();

   /// Function creating a transient TTree object using an input file that
   /// was opened by the user.
   ///
   /// Ownership of the TTree is kept in the function, the user is not supposed
   /// to delete it. Also important to know that there can only be a single
   /// transient event tree alive at any given moment.
   ///
   /// The returned object is actually of type xAOD::TEventTree, but in an
   /// interactive session this should make absolutely no difference to the
   /// user.
   ///
   /// @param ifile    Pointer to an xAOD file opened by the user
   /// @param treeName Optional name of the event tree in the input file
   /// @param mode     Access mode of the internal TEvent object
   /// @returns A pointer to the created transient event tree if successful, or
   ///          a null pointer if not
   ///
   ::TTree* MakeTransientTree( ::TFile* ifile,
                               const char* treeName = "CollectionTree",
                               TEvent::EAuxMode mode =
                                  TEvent::kUndefinedAccess );

   /// Function creating a transient TTree object from a TChain pointing to the
   /// persistent trees
   ///
   /// Ownership of the transient TTree is kept in the function, the user is not
   /// supposed to delete it. Also important to know that there can only be a
   /// single transient event tree alive at any given moment.
   ///
   /// The returned object is actually of type xAOD::TEventTree, but in an
   /// interactive session this should make absolutely no difference to the
   /// user.
   ///
   /// @param ichain Pointer to a TChain listing the input files
   /// @param mode   Access mode of the internal TEvent object
   /// @returns A pointer to the created transient event tree if successful, or
   ///          a null pointer if not
   ///
   ::TTree* MakeTransientTree( ::TChain* ichain,
                               TEvent::EAuxMode mode = TEvent::kClassAccess );

   /// Function creating a transient TTree object representing the input file's
   /// metadata
   ///
   /// In order to access the ROOT readable metadata in a PyROOT friendadly way,
   /// one is supposed to create a transient metadata tree out of it with this
   /// function.
   ///
   /// @param ifile         Pointer to an xAOD file opened by the user
   /// @param eventTreeName Name of the event tree in the input files
   /// @param mode          Access mode of the internal TEvent object
   /// @returns A pointer to the created transient metadata tree if successful,
   ///          or a null pointer if not
   ///
   ::TTree* MakeTransientMetaTree( ::TFile* ifile,
                                   const char* eventTreeName = "CollectionTree",
                                   TEvent::EAuxMode mode =
                                      TEvent::kUndefinedAccess );

   /// Function creating a transient TTree object representing the input files's
   /// metadata
   ///
   /// In order to access the ROOT readable metadata in a PyROOT friendadly way,
   /// one is supposed to create a transient metadata tree out of it with this
   /// function.
   ///
   /// Note that the TChain object given to this function always needs to point
   /// to the "MetaData" TTree. But the second argument must name the name of
   /// the event-level TTree in the input files. Which should be correct with
   /// the default value in 99% of the cases.
   ///
   /// @param ichain        Pointer to the TChain listing the input files
   /// @param eventTreeName Name of the event tree in the input files
   /// @param mode          Access mode of the internal TEvent object
   /// @returns A pointer to the created transient metadata tree if successful,
   ///          or a null pointer if not
   ///
   ::TTree* MakeTransientMetaTree( ::TChain* ichain,
                                   const char* eventTreeName =
                                      "CollectionTree",
                                   TEvent::EAuxMode mode =
                                      TEvent::kUndefinedAccess );

} // namespace xAOD

#endif // XAODROOTACCESS_MAKETRANSIENTTREE_H
