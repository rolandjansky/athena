/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/

/// @author Nils Krumnack


//
// includes
//

#include <SystematicsHandles/CopyHelpers.h>

#include <xAODEgamma/PhotonContainer.h>
#include <xAODEgamma/ElectronContainer.h>
#include <xAODJet/JetContainer.h>
#include <xAODMuon/MuonContainer.h>

//
// method implementations
//

namespace CP
{
  namespace detail
  {
    StatusCode ShallowCopy<xAOD::IParticleContainer> ::
    getCopy (MsgStream& msgStream, StoreType& store,
             xAOD::IParticleContainer*& object,
             const xAOD::IParticleContainer *inputObject,
             const std::string& outputName, const std::string& auxName)
    {
      // this is probably not the best way to do this, but doing this
      // the proper way will require an xAOD expert to do it.

      // using a macro is a bit awkward, but doing this as a template
      // is not practical either
#define COPY(TYPE,IN,OUT)                                               \
      const TYPE *IN = dynamic_cast<const TYPE*> (inputObject);         \
      if (IN) {                                                         \
        TYPE *OUT = nullptr;                                            \
        if (!ShallowCopy<TYPE>::getCopy (msgStream, store, OUT, IN,        \
                                         outputName, auxName).isSuccess()) \
        return StatusCode::FAILURE;                                     \
        object = OUT;                                                   \
        return StatusCode::SUCCESS; }

      COPY (xAOD::JetContainer, jetsInput, jetsOutput);
      COPY (xAOD::MuonContainer, muonsInput, muonsOutput);
      COPY (xAOD::ElectronContainer, electronsInput, electronsOutput);
      COPY (xAOD::PhotonContainer, photonsInput, photonsOutput);

#undef COPY

      const auto msg = [&] (MSG::Level lvl) -> MsgStream& {msgStream << lvl; return msgStream;};
      ANA_MSG_ERROR ("could not determine type to create shallow copy " << outputName);
      ANA_MSG_ERROR ("please extend CopyHelpers.cxx with the appropriate type");
      return StatusCode::FAILURE;
    }
  }
}
