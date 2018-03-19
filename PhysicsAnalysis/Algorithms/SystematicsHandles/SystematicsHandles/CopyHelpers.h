/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/

/// @author Nils Krumnack


#ifndef SYSTEMATICS_HANDLES__COPY_HELPERS_H
#define SYSTEMATICS_HANDLES__COPY_HELPERS_H

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
  }
}

#endif
