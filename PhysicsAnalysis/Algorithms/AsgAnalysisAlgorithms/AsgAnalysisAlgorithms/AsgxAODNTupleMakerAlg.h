// Dear emacs, this is -*- c++ -*-
//
// Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
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
      /// Function setting up the internal data structures on the first event
      StatusCode setupTree();

      /// Function setting up an individual branch on the first event
      StatusCode setupBranch( const std::string &branchDecl,
                              const CP::SystematicSet &sys );

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
         ///
         /// We have to have a default constructor to initialise the
         /// @c asg::AsgMessaging base class correctly. Members of this class
         /// would not need an explicit constructor themselves.
         ///
         ElementProcessor();

         /// Process the object
         ///
         /// This function is called during the event processing to extract
         /// all configured variables from a standalone xAOD object into the
         /// output variables set up using @c ElementProcessor::addBranch.
         ///
         /// @param element The xAOD (interface) object to process
         /// @return The usual @c StatusCode values
         ///
         StatusCode process( const SG::AuxElement& element );

         /// Add one branch to the output tree
         ///
         /// This function is used during the setup of the output tree to create
         /// one branch in it, from one specific auxiliary variable. The type of
         /// the variable is figured out at runtime using the auxiliary store
         /// infrastructure.
         ///
         /// @param tree The tree to create the branch in
         /// @param auxName Name of the auxiliary variable to create the branch
         ///                from
         /// @param branchName The name of the branch to create in the tree
         /// @param allowMissing Set to @c true to print an error message in case
         ///                     of a failure
         /// @param created Used to store if the branch was actually created
         /// @return The usual @c StatusCode values
         ///
         StatusCode addBranch( TTree& tree, const std::string& auxName,
                               const std::string& branchName,
                               bool allowMissing,
                               bool &created );

      private:
         /// Class writing one variable from an xAOD object into a branch
         ///
         /// It is used for both setting up the branch in the outut @c TTree
         /// during the setup of the tree, and then to fill the "output
         /// variable" with the right payload during the event processing.
         ///
         /// Note that since we may have a *lot* of such objects, I didn't want
         /// to make it inherit from @c asg::AsgMessaging. Which means that all
         /// of the class's functions need to receive its parent's message
         /// stream object to be able to log error messages "nicely".
         ///
         /// Also note that since this is very much an internal class, all of
         /// its members are public. Since the owner of such objects should know
         /// perfectly well how they behave.
         ///
         class BranchProcessor {

         public:
            /// Function setting up the object, and the branch
            ///
            /// This is pretty much the constructor of the class. I just decided
            /// to implement it as a regular function and not a "real"
            /// constructor, to be able to return a @c StatusCode value from the
            /// call. Since the setup of the object may very well fail.
            ///
            /// @param tree The tree to set up the new branch in
            /// @param auxName The name of the auxiliary variable to create
            ///                a branch from
            /// @param branchName Name of the branch to create in the tree
            /// @param msg Reference to the parent's @c MsgStream object
            /// @return The usual @c StatusCode values
            ///
            StatusCode setup( TTree& tree, const std::string& auxName,
                              const std::string& branchName,
                              MsgStream& msg );

            /// Function processing the object, filling the variable
            ///
            /// This function is called by @c ElementProcessor, to extract one
            /// variable from the standalone object, and move its payload into
            /// the memory address from which the output tree is writing its
            /// branch.
            ///
            /// @param element The standalone object to get the auxiliary
            ///                variable from
            /// @param msg Reference to the parent's @c MsgStream object
            /// @return The usual @c StatusCode values
            ///
            StatusCode process( const SG::AuxElement& element,
                                MsgStream& msg );

            /// Name of the branch being written
            std::string m_branchName;
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
         ///
         /// Note that when we set up a branch, we tell @c TTree to remember a
         /// physical address in memory. To make sure that the address of the
         /// object held by the branch processors are not moved in memory after
         /// their construction, we have to use an @c std::list container here.
         /// @c std::vector would not work. (As it can relocate objects when
         /// increasing the size of the container.)
         ///
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
         ///
         /// We have to have a default constructor to initialise the
         /// @c asg::AsgMessaging base class correctly. Members of this class
         /// would not need an explicit constructor themselves.
         ///
         ContainerProcessor();

         /// Process the container
         ///
         /// This function is called during the event processing to extract
         /// all configured variables from an xAOD container into the
         /// output variables set up using @c ContainerProcessor::addBranch.
         ///
         /// @param container The xAOD (interface) container to process
         /// @return The usual @c StatusCode values
         ///
         StatusCode process( const SG::AuxVectorBase& container );

         /// Add one branch to the output tree
         ///
         /// This function is used during the setup of the output tree to create
         /// one branch in it, from one specific auxiliary variable. The type of
         /// the variable is figured out at runtime using the auxiliary store
         /// infrastructure.
         ///
         /// @param tree The tree to create the branch in
         /// @param auxName Name of the auxiliary variable to create the branch
         ///                from
         /// @param branchName The name of the branch to create in the tree
         /// @param allowMissing Set to @c true to print an error message in case
         ///                     of a failure
         /// @param created Used to store if the branch was actually created
         /// @return The usual @c StatusCode values
         ///
         StatusCode addBranch( TTree& tree, const std::string& auxName,
                               const std::string& branchName,
                               bool allowMissing,
                               bool &created );

      private:
         /// Class writing one variable from an xAOD object into a branch
         ///
         /// It is used for both setting up the branch in the outut @c TTree
         /// during the setup of the tree, and then to fill the "output
         /// variable" with the right payload during the event processing.
         ///
         /// Note that since we may have a *lot* of such objects, I didn't want
         /// to make it inherit from @c asg::AsgMessaging. Which means that all
         /// of the class's functions need to receive its parent's message
         /// stream object to be able to log error messages "nicely".
         ///
         /// Also note that since this is very much an internal class, all of
         /// its members are public. Since the owner of such objects should know
         /// perfectly well how they behave.
         ///
         /// Finally, note that it is more complicated than the
         /// @c ElementProcessor::BranchProcessor class. Since in this case we
         /// need to explicitly deal with @c std::vector types, which we need to
         /// fill explicitly when extracting the variables from the xAOD
         /// objects.
         ///
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

            /// Name of the branch being written
            std::string m_branchName;
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
         ///
         /// Note that when we set up a branch, we tell @c TTree to remember a
         /// physical address in memory. To make sure that the address of the
         /// object held by the branch processors are not moved in memory after
         /// their construction, we have to use an @c std::list container here.
         /// @c std::vector would not work. (As it can relocate objects when
         /// increasing the size of the container.)
         ///
         std::list< BranchProcessor > m_branches;
         /// Collection proxy used for iterating over the container
         TVirtualCollectionProxy* m_collProxy = nullptr;
         /// Offset of the element type to @c SG::AuxElement
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

      /// Internal status flag, showing whether the algorithm is initialised
      ///
      /// This is necessary because we can only set up the output @c TTree while
      /// processing the first event, we can't do it in
      /// @c CP::AsgxAODNTupleMakerAlg::initialize.
      ///
      bool m_isInitialized = false;

      /// The systematic list to consider during execution
      SysListHandle m_systematicsList{ this };

      /// @}

   }; // class AsgxAODNTupleMakerAlg

} // namespace CP

#endif // ASGANALYSISALGORITHMS_ASGXAODNTUPLEMAKERALG_H
