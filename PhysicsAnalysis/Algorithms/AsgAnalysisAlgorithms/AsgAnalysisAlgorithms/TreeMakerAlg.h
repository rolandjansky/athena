// Dear emacs, this is -*- c++ -*-
//
// Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
//
#ifndef ASGANALYSISALGORITHMS_TREEMAKERALG_H
#define ASGANALYSISALGORITHMS_TREEMAKERALG_H

#include <AnaAlgorithm/AnaAlgorithm.h>

class TTree;

namespace CP {

   /// Algorithm that creates an empty tree for subsequent algorithms
   /// to fill
   ///
   /// This is meant in conjunction with \ref TreeFillerAlg and one or
   /// more of tree-variable filler algorithms in-between.  The idea
   /// behind this specific design is that it allows multiple
   /// implementations of tree-variable filler algorithms to work
   /// together in filling different variables in the same tree, as
   /// well as making the configuration for each tree-variable filler
   /// algorithm simpler.
   ///
   /// @author Nils Krumnack <Nils.Erik.Krumnack@cern.ch>
   /// @author Attila Krasznahorkay <Attila.Krasznahorkay@cern.ch>
   ///
   class TreeMakerAlg : public EL::AnaAlgorithm {

   public:
      /// Algorithm constructor
      TreeMakerAlg( const std::string& name, ISvcLocator* svcLoc );

      /// @name Functions inherited from @c EL::AnaAlgorithm
      /// @{

      /// Function executed once per event
      StatusCode execute() override;

      /// @}

   private:
      /// Function setting up the internal data structures on the first event
      StatusCode setupTree();

      /// @name Algorithm properties
      /// @{

      /// The name of the output tree to write
      std::string m_treeName;
      /// Flust setting for the output tree
      int m_treeAutoFlush;

      /// @}

      /// @name Helper variables
      /// @{

      /// Configured tree status
      bool m_treeConfigured{false};

      /// @}

   }; // class TreeMakerAlg

} // namespace CP

#endif //  ASGANALYSISALGORITHMS_TREEMAKERALG_H
