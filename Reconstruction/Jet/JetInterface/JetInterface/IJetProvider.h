/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/


#ifndef JETINTERFACE_IJETPROVIDER_H
#define JETINTERFACE_IJETPROVIDER_H

///***********************************************
/// IJetProvider is a dual-use tool interface for a tool
/// that builds a new jet collection :
///  - running fastjet
///  - grooming an existing JetContainer
///  - shallow copying an existing JetContainer
///  - ...

#include <utility>
#include <memory>
#include "AsgDataHandles/WriteHandle.h"
#include "AsgTools/IAsgTool.h"
#include "xAODJet/JetContainer.h"

namespace SG {
  class IAuxStore;
}

class IJetProvider : virtual public asg::IAsgTool {
ASG_TOOL_INTERFACE(IJetProvider)

public:

  /// Destructor.
  virtual ~IJetProvider() { };

  /// Method to build the collection and return it to the caller.
  ///
  /// Unique_ptrs are used to avoid potential memory leaks.
  /// The aux container is returned as an IAuxStore pointer
  /// to support operations such as shallow copy.
  virtual std::pair<std::unique_ptr<xAOD::JetContainer>,std::unique_ptr<SG::IAuxStore> > getJets() const = 0;

  /// Method to allow the client to pass in a WriteHandle for the
  /// container and aux container to be recorded, while the client
  /// remains free to modify the output but does not need to know
  /// the type of the aux container.
  ///
  /// Every class derived from IJetProvider needs to implement
  /// the record, providing the explicit type
  /// -- we should give StoreGate maximal information rather than
  /// recording AuxContainerBase
  ///
  /// The recommended solution is for the concrete tool to inherit
  /// from the templated JetProvider class provided below.
  virtual StatusCode getAndRecordJets(SG::WriteHandle<xAOD::JetContainer>& jetHandle) const = 0;

};


/// Concrete class that implements the recording of jets & aux container
/// to StoreGate via an externally provided WriteHandle.
///
/// This should be templated to the AuxContainer type provided by the
/// tool, which in almost all cases should be JetAuxContainer.
template <typename CONCRETEAUX> class JetProvider
  : virtual public IJetProvider
{

public:

  StatusCode getAndRecordJets(SG::WriteHandle<xAOD::JetContainer>& jetHandle) const {
    std::unique_ptr<xAOD::JetContainer> jets(nullptr);
    std::unique_ptr<SG::IAuxStore> auxCont(nullptr);
    std::tie(jets, auxCont) = getJets();
    if(jets.get()==nullptr || auxCont.get()==nullptr) {return StatusCode::FAILURE;}

    // Convert the aux container pointer to the known base class
    std::unique_ptr<CONCRETEAUX> auxCont_derived(static_cast<CONCRETEAUX*>(auxCont.release()));

    // Write out JetContainer and JetAuxContainer
    return jetHandle.record(std::move(jets), std::move(auxCont_derived));
  }

};
    

#endif
