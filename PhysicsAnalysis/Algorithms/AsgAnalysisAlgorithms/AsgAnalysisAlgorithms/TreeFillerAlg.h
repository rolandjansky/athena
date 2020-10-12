// Dear emacs, this is -*- c++ -*-
//
// Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
//
#ifndef ASGANALYSISALGORITHMS_TREEFILLERALG_H
#define ASGANALYSISALGORITHMS_TREEFILLERALG_H

// System include(s):
#include <unordered_map>
#include <string>
#include <vector>
#include <memory>
#include <list>

// Framework include(s):
#include "AsgMessaging/AsgMessaging.h"
#include "AnaAlgorithm/AnaAlgorithm.h"
#include "SystematicsHandles/SysListHandle.h"

// EDM include(s):
#include "AthContainersInterfaces/IAuxTypeVector.h"
#include "AthContainers/AuxElement.h"

// Forward declaration(s):
class TClass;
class TTree;
class TVirtualCollectionProxy;
namespace SG {
   class AuxVectorBase;
   class IAuxTypeVectorFactory;
}

namespace CP {

   /// Algorithm that writes events to a tree filled by preceding
   /// algorithms to fill
   ///
   /// This is meant in conjunction with \ref TreeMakerAlg and one or
   /// more of tree-variable filler algorithms in-between.  The idea
   /// behind this specific design is that it allows multiple
   /// implementations of tree-variable filler algorithms to work
   /// together in filling different variables in the same tree, as
   /// well as making the configuration for each tree-variable filler
   /// algorithm simpler.
   ///
   /// @author Nils Krumnack <Nils.Erik.Krumnack@cern.ch>
   /// @author Attila Krasznahorkay <Attila.Krasznahorkay@cern.ch>

   class TreeFillerAlg : public EL::AnaAlgorithm {

   public:
      /// Algorithm constructor
      TreeFillerAlg( const std::string& name, ISvcLocator* svcLoc );

      /// @name Functions inherited from @c EL::AnaAlgorithm
      /// @{

      /// Function executed once per event
      StatusCode execute() override;

      /// @}

   private:
      /// @name Algorithm properties
      /// @{

      /// The name of the output tree to write
      std::string m_treeName;

      /// @}

      /// @name Variables used for the TTree filling
      /// @{

      /// The tree being written
      TTree* m_tree = nullptr;

      /// @}

   }; // class TreeFillerAlg

} // namespace CP

#endif // ASGANALYSISALGORITHMS_TREEFILLERALG_H
