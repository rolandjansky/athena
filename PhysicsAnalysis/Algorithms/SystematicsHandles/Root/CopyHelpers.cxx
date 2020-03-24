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
#include <xAODTau/TauJetContainer.h>
#include <xAODTracking/TrackParticleContainer.h>

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

      // Set up a lambda for providing a msg(...) function.
      const auto msg = [&] (MSG::Level lvl) -> MsgStream& {msgStream << lvl; return msgStream;};

      // using a macro is a bit awkward, but doing this as a template
      // is not practical either
#define COPY(TYPE)                                                             \
       do {                                                                    \
          const TYPE *in = dynamic_cast< const TYPE* >( inputObject );         \
          if( in ) {                                                           \
             TYPE *out = nullptr;                                              \
             ANA_CHECK( ShallowCopy<TYPE>::getCopy( msgStream, store, out, in, \
                                                    outputName, auxName ) );   \
             object = out;                                                     \
             return StatusCode::SUCCESS;                                       \
          }                                                                    \
       } while( false )

      COPY( xAOD::JetContainer );
      COPY( xAOD::MuonContainer );
      COPY( xAOD::ElectronContainer );
      COPY( xAOD::PhotonContainer );
      COPY( xAOD::TauJetContainer );
      COPY(xAOD::TrackParticleContainer);

#undef COPY

      ANA_MSG_ERROR ("could not determine type to create shallow copy " << outputName);
      ANA_MSG_ERROR ("please extend CopyHelpers.cxx with the appropriate type");
      return StatusCode::FAILURE;
    }
  }
}
