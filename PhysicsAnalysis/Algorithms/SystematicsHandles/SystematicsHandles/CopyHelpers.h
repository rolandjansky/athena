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

#include <memory>

namespace CP
{
  namespace detail
  {
    /// \brief check what kind of object/container the argument is
    template <typename T>  
    struct ContainerType
    {
      template <class, class> class checker;

      template <typename C>
      static std::true_type test_iparticle(checker<C, decltype((*(const xAOD::IParticle**)nullptr) = ((C*)nullptr)->at(0))> *);

      template <typename C>
      static std::false_type test_iparticle(...);

      template <typename C>
      static std::true_type test_container(checker<C, decltype((*(const SG::AuxVectorBase**)nullptr) = ((C*)nullptr))>*);

      template <typename C>
      static std::false_type test_container(...);

      /// Value evaluating to:
      ///  - 1 for xAOD::IParticleContainer types;
      ///  - 2 for other DataVector types;
      ///  - 3 for non-vector types.
      static const int value = ( std::is_same< std::true_type, decltype( test_iparticle< T >( nullptr ) ) >::value ?
                                 1 : ( std::is_same< std::true_type, decltype( test_container< T >( nullptr ) ) >::value ?
                                       2 : 3 ) );
    };

    /// \brief a helper class to create shallow copies and register
    /// them in the event store
    ///
    /// The main purpose of this class is to make it (fairly)
    /// straightforward to do partial specializations for base classes
    /// which need special handling to register their objects with the
    /// correct type.
    template<typename T, int type = ContainerType<T>::value>
    struct ShallowCopy
    {
      static_assert ((type==1)||(type==2)||(type==3),
                     "Type can not be shallow copied");
    };

    template<typename T>
    struct ShallowCopy<T,1>
    {
      /// \brief the type of the event store we use
    public:
      typedef std::decay<decltype(*((EL::AnaAlgorithm*)0)->evtStore())>::type
        StoreType;

      static StatusCode
      getCopy (MsgStream& msgStream, StoreType& store,
               T*& object, const T *inputObject,
               const std::string& outputName, const std::string& auxName)
      {
        // Define the msg(...) function as a lambda.
        const auto msg = [&] (MSG::Level lvl) -> MsgStream& {msgStream << lvl;
                                                             return msgStream;};

        // Make sure that we get a valid pointer.
        assert (inputObject != nullptr);

        // Handle the case when the input object is a view container.
        if( ! inputObject->getConstStore() ) {

           // Decide how to handle the container.
           if( inputObject->size() ) {
              // Get the pointer to the "owning container" from the first
              // element.
              const T* originContainer =
                 dynamic_cast< const T* >( ( *inputObject )[ 0 ]->container() );
              // Make sure that every element in the view container has the same
              // parent.
              for( size_t i = 1; i < inputObject->size(); ++i ) {
                 if( ( *inputObject )[ i ]->container() != originContainer ) {
                    ANA_MSG_ERROR( "Not all elements of the received view "
                                   "container come from the same container!" );
                    return StatusCode::FAILURE;
                 }
              }
              // Postfix for the shallow-copy container of the origin container.
              static const char* ORIGIN_POSTFIX = "_ShallowCopyOrigin";
              // Make a shallow copy of the origin container.
              auto originCopy = xAOD::shallowCopyContainer( *originContainer );
              if( ( ! originCopy.first ) || ( ! originCopy.second ) ) {
                 ANA_MSG_ERROR( "Failed to shallow copy the origin of a view "
                                << "container, meant for: " << outputName );
                 return StatusCode::FAILURE;
              }
              // ...and record it.
              ANA_CHECK( store.record( originCopy.first,
                                       outputName + ORIGIN_POSTFIX ) );
              ANA_CHECK( store.record( originCopy.second,
                                       outputName + ORIGIN_POSTFIX + "Aux." ) );
              // Make a view copy on top of it.
              auto viewCopy = std::make_unique< T >( SG::VIEW_ELEMENTS );
              auto viewCopyPtr = viewCopy.get();
              for( const auto* element : *inputObject ) {
                 viewCopy->push_back( originCopy.first->at( element->index() ) );
              }
              // Set the origin links on it. Note that
              // xAOD::setOriginalObjectLink's "container version" doesn't work
              // with view containers, we have to call this function one-by-one
              // on the elements.
              for( size_t i = 0; i < inputObject->size(); ++i ) {
                 if( ! xAOD::setOriginalObjectLink( *( ( *inputObject )[ i ] ),
                                                    *( ( *viewCopy )[ i ] ) ) ) {
                    return StatusCode::FAILURE;
                 }
              }
              // Finally, record the view container with the requested name.
              ANA_CHECK( store.record( viewCopy.release(), outputName ) );
              // The copy is done.
              object = viewCopyPtr;
              return StatusCode::SUCCESS;
           } else {
              // If the container was empty, then let's just make a new empty
              // container, and that's that...
              auto viewCopy = std::make_unique< T >( SG::VIEW_ELEMENTS );
              auto viewCopyPtr = viewCopy.get();
              ANA_CHECK( store.record( viewCopy.release(), outputName ) );
              // The copy is done.
              object = viewCopyPtr;
              return StatusCode::SUCCESS;
           }

        } else {

           // We can just copy the container as is.
           auto copy = xAOD::shallowCopyContainer( *inputObject );
           if (!copy.first || !copy.second)
           {
              ANA_MSG_ERROR ("failed to shallow copy object: " << outputName);
              ANA_MSG_ERROR ("likely shallow copying a view container");
              return StatusCode::FAILURE;
           }

           if (!xAOD::setOriginalObjectLink (*inputObject, *copy.first)) {
              return StatusCode::FAILURE;
           }

           ANA_CHECK (store.record (copy.second, auxName));
           ANA_CHECK (store.record (copy.first, outputName));
           object = copy.first;
           return StatusCode::SUCCESS;
        }
      }
    };

    template<typename T>
    struct ShallowCopy<T,2>
    {
      /// \brief the type of the event store we use
    public:
      typedef std::decay<decltype(*((EL::AnaAlgorithm*)0)->evtStore())>::type
      StoreType;

      static StatusCode
      getCopy (MsgStream& msgStream, StoreType& store,
               T*& object, const T *inputObject,
               const std::string& outputName, const std::string& auxName)
      {
         // Define the msg(...) function as a lambda.
         const auto msg = [&] (MSG::Level lvl) -> MsgStream& {msgStream << lvl;
            return msgStream;};

         // Make sure that we get a valid pointer.
         assert (inputObject != nullptr);

         // Handle the case when the input object is a view container.
         if( ! inputObject->getConstStore() ) {

            // Decide how to handle the container.
            if( inputObject->size() ) {
               // Get the pointer to the "owning container" from the first
               // element.
               const T* originContainer =
                 dynamic_cast< const T* >( ( *inputObject )[ 0 ]->container() );
               // Make sure that every element in the view container has the same
               // parent.
               for( size_t i = 1; i < inputObject->size(); ++i ) {
                  if( ( *inputObject )[ i ]->container() != originContainer ) {
                     ANA_MSG_ERROR( "Not all elements of the received view "
                                    "container come from the same container!" );
                     return StatusCode::FAILURE;
                  }
               }
               // Postfix for the shallow-copy container of the origin container.
               static const char* ORIGIN_POSTFIX = "_ShallowCopyOrigin";
               // Make a shallow copy of the origin container.
               auto originCopy = xAOD::shallowCopyContainer( *originContainer );
               if( ( ! originCopy.first ) || ( ! originCopy.second ) ) {
                  ANA_MSG_ERROR( "Failed to shallow copy the origin of a view "
                                 << "container, meant for: " << outputName );
                  return StatusCode::FAILURE;
               }
               // ...and record it.
               ANA_CHECK( store.record( originCopy.first,
                                        outputName + ORIGIN_POSTFIX ) );
               ANA_CHECK( store.record( originCopy.second,
                                        outputName + ORIGIN_POSTFIX +
                                        "Aux." ) );
               // Make a view copy on top of it.
               auto viewCopy = std::make_unique< T >( SG::VIEW_ELEMENTS );
               auto viewCopyPtr = viewCopy.get();
               for( const auto* element : *inputObject ) {
                  viewCopy->push_back( originCopy.first->at( element->index() ) );
               }
               // Finally, record the view container with the requested name.
               ANA_CHECK( store.record( viewCopy.release(), outputName ) );
               // The copy is done.
               object = viewCopyPtr;
               return StatusCode::SUCCESS;
            } else {
               // If the container was empty, then let's just make a new empty
               // container, and that's that...
               auto viewCopy = std::make_unique< T >( SG::VIEW_ELEMENTS );
               auto viewCopyPtr = viewCopy.get();
               ANA_CHECK( store.record( viewCopy.release(), outputName ) );
               // The copy is done.
               object = viewCopyPtr;
               return StatusCode::SUCCESS;
              }

         } else {

            // We can just copy the container as is.
            auto copy = xAOD::shallowCopyContainer( *inputObject );
            if (!copy.first || !copy.second)
            {
               ANA_MSG_ERROR ("failed to shallow copy object: " << outputName);
               ANA_MSG_ERROR ("likely shallow copying a view container");
               return StatusCode::FAILURE;
            }

            ANA_CHECK (store.record (copy.second, auxName));
            ANA_CHECK (store.record (copy.first, outputName));
            object = copy.first;
            return StatusCode::SUCCESS;
         }
      }
    };

    template<typename T>
    struct ShallowCopy<T,3>
    {
       /// \brief the type of the event store we use
    public:
       typedef std::decay<decltype(*((EL::AnaAlgorithm*)0)->evtStore())>::type
          StoreType;

       static StatusCode
       getCopy (MsgStream& msgStream, StoreType& store,
                T*& object, const T *inputObject,
                const std::string& outputName, const std::string& auxName)
       {
          // Define the msg(...) function as a lambda.
          const auto msg = [&] (MSG::Level lvl) -> MsgStream& {msgStream << lvl;
             return msgStream;};

          // We can just copy the object as is.
          auto copy = xAOD::shallowCopyObject( *inputObject );
          if (!copy.first || !copy.second)
          {
             ANA_MSG_ERROR ("failed to shallow copy object: " << outputName);
             ANA_MSG_ERROR ("likely shallow copying a view container");
             return StatusCode::FAILURE;
          }

          ANA_CHECK (store.record (copy.second, auxName));
          ANA_CHECK (store.record (copy.first, outputName));
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
