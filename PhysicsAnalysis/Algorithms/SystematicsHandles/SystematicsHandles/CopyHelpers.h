/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/

/// @author Nils Krumnack


#ifndef SYSTEMATICS_HANDLES__COPY_HELPERS_H
#define SYSTEMATICS_HANDLES__COPY_HELPERS_H

#include <AnaAlgorithm/AnaAlgorithm.h>
#include <AsgTools/MessageCheck.h>
#include <AsgTools/StatusCode.h>
#include <xAODBase/IParticleContainer.h>
#include <xAODBase/IParticleHelpers.h>
#include <xAODCore/ShallowCopy.h>

namespace CP
{
  namespace detail
  {
    /// \brief check whether type inherits from IParticleContainer
    template <typename T>  
    struct IsIParticleContainer
    {
      template <class, class> class checker;

      template <typename C>
      static std::true_type test(checker<C, decltype((*(const xAOD::IParticle**)nullptr) = ((C*)nullptr)->at(0))> *);

      template <typename C>
      static std::false_type test(...);

      typedef decltype(test<T>(nullptr)) type;
      static const bool value = std::is_same<std::true_type, decltype(test<T>(nullptr))>::value;
    };



    /// \brief a helper class to perform the core operations of
    /// shallow copies (copying and setting original object links)
    template<typename T, bool IP = IsIParticleContainer<T>::value>
    struct ShallowCopyCore;

    template<typename T>
    struct ShallowCopyCore<T,true>
    {
      static auto copy (const T& input)
	-> decltype (xAOD::shallowCopyContainer (input))
      {
	return xAOD::shallowCopyContainer (input);
      }

      static ::StatusCode setLink (const T& input, T& output)
      {
	return xAOD::setOriginalObjectLink(input, output);
      }
    };

    template<typename T>
    struct ShallowCopyCore<T,false>
    {
      static auto copy (const T& input)
	-> decltype (xAOD::shallowCopyObject (input))
      {
	return xAOD::shallowCopyObject (input);
      }

      static ::StatusCode setLink (const T& /*input*/, T& /*output*/)
      {
	return ::StatusCode::SUCCESS;
      }
    };



    /// \brief a helper class to create shallow copies and register
    /// them in the event store
    ///
    /// The main purpose of this class is to make it (fairly)
    /// straightforward to do partial specializations for base classes
    /// which need special handling to register their objects with the
    /// correct type.
    template<typename T>
    struct ShallowCopy
    {
      /// \brief the type of the event store we use
    public:
      typedef std::decay<decltype(*((EL::AnaAlgorithm*)0)->evtStore())>::type StoreType;

      static StatusCode
      getCopy (MsgStream& msgStream, StoreType& store,
               T*& object, const T *inputObject,
               const std::string& outputName, const std::string& auxName)
      {
        const auto msg = [&] (MSG::Level lvl) -> MsgStream& {msgStream << lvl; return msgStream;};

        assert (inputObject != nullptr);
        auto copy = ShallowCopyCore<T>::copy (*inputObject);
        if (!copy.first || !copy.second)
        {
          ANA_MSG_ERROR ("failed to shallow copy object: " << outputName);
          ANA_MSG_ERROR ("likely shallow copying a view container");
          return StatusCode::FAILURE;
        }

        if (ShallowCopyCore<T>::setLink (*inputObject, *copy.first).isFailure())
          return StatusCode::FAILURE;

        if (store.record (copy.second, auxName).isFailure())
          return StatusCode::FAILURE;
        if (store.record (copy.first, outputName).isFailure())
          return StatusCode::FAILURE;
        object = copy.first;
        return StatusCode::SUCCESS;
      }
    };

    template<>
    struct ShallowCopy<xAOD::IParticleContainer>
    {
      /// \brief the type of the event store we use
    public:
      typedef std::decay<decltype(*((EL::AnaAlgorithm*)0)->evtStore())>::type StoreType;

      static StatusCode
      getCopy (MsgStream& msgStream, StoreType& store,
               xAOD::IParticleContainer*& object,
               const xAOD::IParticleContainer *inputObject,
               const std::string& outputName, const std::string& auxName);
    };
  }
}

#endif
