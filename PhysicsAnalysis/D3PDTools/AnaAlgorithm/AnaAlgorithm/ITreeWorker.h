// Dear emacs, this is -*- c++ -*-
//
// Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
//
#ifndef ANAALGORITHM_ITREEWORKER_H
#define ANAALGORITHM_ITREEWORKER_H

// Bail right away if somebody tries to use this from the wrong environment:
#ifndef XAOD_STANDALONE
#   error only include this header in AnalysisBase
#endif

// System include(s):
#include <string>

// Framework include(s):
#include "AsgMessaging/StatusCode.h"

// Local include(s):
#include "AnaAlgorithm/Global.h"

// Forward declaration(s):
class TTree;

namespace EL {

   /// The interface to TTree storage on the worker
   ///
   /// This interface summarizes the subset of \ref Worker that is
   /// needed for handling TTree outputs.
   ///
   class ITreeWorker {

   public:
      /// Virtual destructor, to make vtable happy...
      virtual ~ITreeWorker () noexcept = default;

      /// Add a tree to the output, into an output stream/file
      ///
      /// @param tree The tree to put into the output stream/file
      /// @param stream The name of the stream/file to put the tree into.
      ///
      virtual ::StatusCode addTree( const TTree& tree,
                                    const std::string& stream ) = 0;

      /// Get the pointer to an output tree with a given name/stream
      ///
      /// @param name The name of the tree to find
      /// @param stream The stream/file to find the tree in
      /// @return A pointer to the tree if successful, or @c nullptr if not
      ///
      virtual TTree* getOutputTree( const std::string& name,
                                    const std::string& stream ) const = 0;

   }; // class ITreeWorker

} // namespace EL

#endif // ANAALGORITHM_ITREEWORKER_H
