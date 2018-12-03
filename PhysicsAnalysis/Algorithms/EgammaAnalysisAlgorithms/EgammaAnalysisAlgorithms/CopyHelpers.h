/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/

/// @author Nils Krumnack



#ifndef EGAMMA_ANALYSIS_ALGORITHMS__COPY_HELPERS_H
#define EGAMMA_ANALYSIS_ALGORITHMS__COPY_HELPERS_H

#include <SystematicsHandles/CopyHelpers.h>

#include <xAODEgamma/EgammaContainer.h>

namespace CP
{
  namespace detail
  {
    template<>
    struct ShallowCopy<xAOD::EgammaContainer>
    {
      /// \brief the type of the event store we use
    public:
      typedef std::decay<decltype(*((EL::AnaAlgorithm*)0)->evtStore())>::type StoreType;

      static StatusCode
      getCopy (MsgStream& msgStream, StoreType& store,
               xAOD::EgammaContainer*& object,
               const xAOD::EgammaContainer *inputObject,
               const std::string& outputName, const std::string& auxName)
      {
        const auto msg = [&] (MSG::Level lvl) -> MsgStream& {msgStream << lvl; return msgStream;};

        xAOD::IParticleContainer *subobject = nullptr;
        if (!ShallowCopy<xAOD::IParticleContainer>::getCopy (msgStream, store, subobject, inputObject, outputName, auxName).isSuccess())
          return StatusCode::FAILURE;
        if (!(object = dynamic_cast<xAOD::EgammaContainer*>(subobject)))
        {
          ANA_MSG_ERROR ("copy of EgammaContainer is not an EgammaContainer"); 
          ANA_MSG_ERROR ("check logic in CopyHelpers");
          return StatusCode::FAILURE;
        }
        return StatusCode::SUCCESS;
      }
    };
  }
}

#endif
