// Dear emacs, this is -*- c++ -*-
//
// Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
//
#ifndef ASGANALYSISALGORITHMS_ASGXAODNTUPLEMAKERALG_H
#define ASGANALYSISALGORITHMS_ASGXAODNTUPLEMAKERALG_H

// System include(s):
#include <unordered_map>
#include <string>
#include <vector>
#include <memory>
#include <list>

// Framework include(s):
#include "AsgTools/AsgMessaging.h"
#include "AnaAlgorithm/AnaAlgorithm.h"

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

   /// Algorithm that can write a simple ntuple from xAOD objects/variables
   ///
   /// This is meant as a simple tool for creating small ntuples in analyses,
   /// using a simple job configuration. It can create branches from any xAOD
   /// variables that are possible to write as an "xAOD variable" to begin with.
   ///
   /// It is *not* meant as a general purpose DAOD -> NTuple dumper however.
   /// It should only be used to create small ntuples, with highly processed
   /// variables.
   ///
   /// It's "main" property ("Branches") can be filled with entries of the form:
   ///
   /// <code>
   ///    writer = ...<br/>
   ///    writer.Branches = [ "<SG key>.<aux variable> -> <branch name>", ... ]
   /// </code>
   ///
   /// , where each entry sets up one branch for the output tree.
   ///
   /// @author Attila Krasznahorkay <Attila.Krasznahorkay@cern.ch>
   ///
   class AsgxAODNTupleMakerAlg : public EL::AnaAlgorithm {

   public:
      /// Algorithm constructor
      AsgxAODNTupleMakerAlg( const std::string& name, ISvcLocator* svcLoc );

      /// @name Functions inherited from @c EL::AnaAlgorithm
      /// @{

      /// Function executed as part of the job initialisation
      StatusCode initialize() override;

      /// Function executed once per event
      StatusCode execute() override;

      /// Function executed as part of the job finalisation
      StatusCode finalize() override;

      /// @}

   private:
      /// @name Algorithm properties
      /// @{

      /// The name of the output tree to write
      std::string m_treeName;
      /// The branches to write into this output tree
      std::vector< std::string > m_branches;

      /// @}

      /// Class writing all variables from one standalone object
      ///
      /// It is designed to work with any type inheriting from
      /// @c SG::AuxElement. Like @c xAOD::EventInfo. Which is its main user
      /// at the moment...
      ///
      class ElementProcessor : public asg::AsgMessaging {

      public:
         /// Default constructor
         ElementProcessor();

         /// Process the object
         StatusCode process( const SG::AuxElement& element );
         /// Add one branch to the output tree
         StatusCode addBranch( TTree& tree, const std::string& auxName,
                               const std::string& branchName );

      private:
         /// Internal helper class
         class BranchProcessor {

         public:
            /// Function setting up the object, and the branch
            StatusCode setup( TTree& tree, const std::string& auxName,
                              const std::string& branchName,
                              MsgStream& msg );
            /// Function processing the object, filling the variable
            StatusCode process( const SG::AuxElement& element,
                                MsgStream& msg );

         private:
            /// Object accessing the variable in question
            std::unique_ptr< SG::AuxElement::TypelessConstAccessor > m_acc;
            /// Pointer to the helper object that handles this variable
            const SG::IAuxTypeVectorFactory* m_factory = nullptr;
            /// The object managing the memory of the written variable
            std::unique_ptr< SG::IAuxTypeVector > m_data;
            /// Helper variable, pointing at the object to be written
            void* m_dataPtr = nullptr;

         }; // class BranchProcessor

         /// List of branch processors set up for this xAOD object
         std::list< BranchProcessor > m_branches;

      }; // class ElementProcessor

      /// Class writing all variables from one @c DataVector container
      ///
      /// It is designed to work with *any* @c DataVector<SG::AuxElement> type,
      /// it doesn't have to be an @c xAOD::IParticleContainer. But of course
      /// that is the main use case for it...
      ///
      /// It expects an @c SG::AuxVectorBase object from the caller, iterates
      /// over the elements of that container using the ROOT dictionary of the
      /// type, and writes individual variables from the elements of the
      /// container using the same machinery that @c ElementProcessor employs.
      ///
      class ContainerProcessor : public asg::AsgMessaging {

      public:
         /// Default constructor
         ContainerProcessor();

         /// Process the container
         StatusCode process( const SG::AuxVectorBase& container );
         /// Add one branch to the output tree
         StatusCode addBranch( TTree& tree, const std::string& auxName,
                               const std::string& branchName );

      private:
         /// Internal helper class
         class BranchProcessor {

         public:
            /// Function setting up the object, and the branch
            StatusCode setup( TTree& tree, const std::string& auxName,
                              const std::string& branchName,
                              MsgStream& msg );
            /// Function (re)sizing the variable for a new event
            StatusCode resize( size_t size, MsgStream& msg );
            /// Function processing the object, filling the variable
            StatusCode process( const SG::AuxElement& element, size_t index,
                                MsgStream& msg );

         private:
            /// Object accessing the variable in question
            std::unique_ptr< SG::AuxElement::TypelessConstAccessor > m_acc;
            /// Pointer to the helper object that handles this variable
            const SG::IAuxTypeVectorFactory* m_factory = nullptr;
            /// The object managing the memory of the written variable
            std::unique_ptr< SG::IAuxTypeVector > m_data;
            /// Helper variable, pointing at the object to be written
            void* m_dataPtr = nullptr;

         }; // class BranchProcessor

         /// List of branch processors set up for this xAOD object
         std::list< BranchProcessor > m_branches;
         /// Collection proxy used for iterating over the container
         TVirtualCollectionProxy* m_collProxy = nullptr;
         /// Offset of the element type to SG::AuxElement
         int m_auxElementOffset = -1;

      }; // class ContainerProcessor

      /// @name Variables used for the TTree filling
      /// @{

      /// The tree being written
      TTree* m_tree = nullptr;

      /// Objects to write branches from
      std::unordered_map< std::string, ElementProcessor > m_elements;
      /// Containers to write branches from
      std::unordered_map< std::string, ContainerProcessor > m_containers;

      /// Internal status flag
      bool m_isInitialized = false;

      /// @}

   }; // class AsgxAODNTupleMakerAlg

} // namespace CP

#endif // ASGANALYSISALGORITHMS_ASGXAODNTUPLEMAKERALG_H
