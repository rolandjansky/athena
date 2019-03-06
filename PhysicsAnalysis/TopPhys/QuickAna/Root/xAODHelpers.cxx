/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

/// @author Nils Krumnack



//
// includes
//

#include <QuickAna/xAODHelpers.h>

#include <QuickAna/MessageCheck.h>
#include <QuickAna/ObjectTypeInfo.h>
#include <RootCoreUtils/Assert.h>
#include <RootCoreUtils/ThrowMsg.h>
#include <xAODBase/IParticleHelpers.h>
#include <xAODCore/ShallowCopy.h>
#include <xAODEventInfo/EventAuxInfo.h>
#include <xAODMissingET/MissingETAuxContainer.h>

#ifdef ROOTCORE
#include <AsgTools/SgTEvent.h>
#else
#include <StoreGate/StoreGateSvc.h>
#endif

//
// method implementations
//

namespace ana
{
  namespace xAODHelpers
  {
    namespace
    {
      template<class T> struct Traits;

      /// \brief perform a shallow copy and register it under the given
      /// name in the store
      template<class T> StatusCode
      makeShallowXAOD (StoreType& store, const std::string& name,
                       T*& pointer, const std::string& inputName);

      /// \brief perform a shallow copy and register it under the given
      /// name in the store. Calls makeShallowXAOD after casting the pointer.
      template<unsigned T> StatusCode
      makeShallowType (StoreType& store, const std::string& name,
                       void*& pointer, const std::string& inputName);

      /// \brief make a new object and register it under the given name
      /// in the store
      template<class T, class A = typename xAODHelpers::Traits<T>::AuxType> StatusCode
      makeNewXAOD (StoreType& store, const std::string& name, T*& object);

      /// \brief make a new object and register it under the given name
      /// in the store. Calls makeNewXAOD after casting the pointer.
      template<unsigned T> StatusCode
      makeNewType (StoreType& store, const std::string& name, void*& pointer);

      /// \brief get the given type converted into an IParticleContainer
      template<unsigned T> xAOD::IParticleContainer *
      getParticlesType (void *pointer);

      template<> struct Traits<xAOD::EventInfo>
      {
        /// \brief whether this is a container
        static const bool isContainer = false;

        /// \brief the auxilliary type
        typedef xAOD::EventAuxInfo AuxType;
      };

      template<> struct Traits<xAOD::MuonContainer>
      {
        /// \brief whether this is a container
        static const bool isContainer = true;
      };

      template<> struct Traits<xAOD::TauJetContainer>
      {
        /// \brief whether this is a container
        static const bool isContainer = true;
      };

      template<> struct Traits<xAOD::ElectronContainer>
      {
        /// \brief whether this is a container
        static const bool isContainer = true;
      };

      template<> struct Traits<xAOD::PhotonContainer>
      {
        /// \brief whether this is a container
        static const bool isContainer = true;
      };

      template<> struct Traits<xAOD::JetContainer>
      {
        /// \brief whether this is a container
        static const bool isContainer = true;
      };

      template<> struct Traits<xAOD::MissingETContainer>
      {
        /// \brief whether this is a container
        // static const bool isContainer = true;

        /// \brief the auxilliary type
        typedef xAOD::MissingETAuxContainer AuxType;
      };



      template<class T,bool container=Traits<T>::isContainer>
      struct ShallowCopy;

      template<class XAODContainer> struct ShallowCopy<XAODContainer,true>
      {
        static StatusCode
        copy (StoreType& store,
              const XAODContainer *input, XAODContainer*& output,
              const std::string& name)
        {
	  using namespace msgStore;

          // Create the shallow copy of the entire container:
          auto copy = xAOD::shallowCopyContainer (*input);

          ANA_CHECK (xAOD::setOriginalObjectLink(*input, *copy.first));
          ANA_CHECK (store.record (copy.first,  name));
          ANA_CHECK (store.record (copy.second, name + "Aux."));
          output = copy.first;
          return StatusCode::SUCCESS;
        }
      };

      template<class XAODObject> struct ShallowCopy<XAODObject,false>
      {
        static StatusCode
        copy (StoreType& store,
              const XAODObject *input, XAODObject*& output,
              const std::string& name)
        {
	  using namespace msgStore;

          // Create the shallow copy of the entire container:
          auto copy = xAOD::shallowCopyObject (*input);

          ANA_CHECK (store.record (copy.first,  name));
          ANA_CHECK (store.record (copy.second, name + "Aux."));
          output = copy.first;
          return StatusCode::SUCCESS;
        }
      };



      template<class T, class A> StatusCode
      makeNewXAOD (StoreType& store, const std::string& name, T*& object)
      {
	using namespace msgStore;

        T* obj = new T();
        A* aux = new A();
        obj->setStore(aux);
        ANA_CHECK (store.record(aux, name+"Aux."));
        ANA_CHECK (store.record(obj, name));
        object = obj;
        return StatusCode::SUCCESS;
      }



      template<class T> StatusCode
      makeShallowXAOD (StoreType& store, const std::string& name,
                       T*& pointer, const std::string& inputName)
      {
	using namespace msgStore;

        RCU_REQUIRE (pointer == nullptr);
        const T *input = nullptr;
        ANA_CHECK (store.retrieve (input, inputName));
        T *copy = nullptr;
        ANA_CHECK (ShallowCopy<T>::copy (store, input, copy, name));
        pointer = copy;
        return StatusCode::SUCCESS;
      }



      template<unsigned T> StatusCode
      makeShallowType (StoreType& store, const std::string& name,
                       void*& pointer, const std::string& inputName)
      {
        // TODO: Line is too long. Clean this up
        return makeShallowXAOD (store, name, reinterpret_cast<typename ObjectTypeInfo::asXAOD<T>::type*&>(pointer), inputName);
      }



      template<unsigned T> StatusCode
      makeNewType (StoreType& store, const std::string& name, void*& pointer)
      {
        // TODO: Line is too long. Clean this up
        return makeNewXAOD (store, name, reinterpret_cast<typename ObjectTypeInfo::asXAOD<T>::type*&>(pointer));
      }



      template<unsigned T> xAOD::IParticleContainer *
      getParticlesType (void *pointer)
      {
        return static_cast<typename ObjectTypeInfo::asXAOD<T>::type*>(pointer);
      }
    }



    StatusCode
    makeShallow (StoreType& store, const std::string& name, ObjectType type,
                 void*& pointer, const std::string& inputName)
    {
      using namespace msgStore;

      RCU_REQUIRE (type < ObjectTypeInfo::numTypes);

      switch (type)
      {
#define TMP(TYPE)                                                       \
        case TYPE:                                                      \
          return makeShallowType<TYPE> (store, name, pointer, inputName);
        TMP (OBJECT_EVENTINFO);
        TMP (OBJECT_EVENT_SELECT);
        TMP (OBJECT_JET);
        TMP (OBJECT_PFLOW_JET);
        TMP (OBJECT_FAT_JET);
        TMP (OBJECT_ELECTRON);
        TMP (OBJECT_PHOTON);
        TMP (OBJECT_MUON);
        TMP (OBJECT_TAU);
#undef TMP
      case OBJECT_NONE:
      case OBJECT_MET:
      case OBJECT_MET2:
      case OBJECT_CLEANING:
      case OBJECT_OVERLAP_REMOVAL:
      case OBJECT_TRIGGER:
        ANA_MSG_ERROR ("unsupported object type " << type);
        return StatusCode::FAILURE;
      }
      return StatusCode::SUCCESS;
    }



    StatusCode
    makeNew (StoreType& store, const std::string& name, ObjectType type,
             void*& pointer)
    {
      using namespace msgStore;

      RCU_REQUIRE (type < ObjectTypeInfo::numTypes);

      switch (type)
      {
#define TMP(TYPE)                                                       \
        case TYPE:                                                      \
          return makeNewType<TYPE> (store, name, pointer);
        TMP (OBJECT_MET);
        TMP (OBJECT_MET2);
        TMP (OBJECT_EVENT_SELECT);
#undef TMP
      case OBJECT_EVENTINFO:
      case OBJECT_JET:
      case OBJECT_PFLOW_JET:
      case OBJECT_FAT_JET:
      case OBJECT_ELECTRON:
      case OBJECT_PHOTON:
      case OBJECT_MUON:
      case OBJECT_TAU:
      case OBJECT_CLEANING:
      case OBJECT_OVERLAP_REMOVAL:
      case OBJECT_TRIGGER:
      case OBJECT_NONE:
        ANA_MSG_ERROR ("unsupported object type " << type);
        return StatusCode::FAILURE;
      }
      return StatusCode::SUCCESS;
    }



    xAOD::IParticleContainer *
    getParticles (void *pointer, ObjectType type)
    {
      RCU_REQUIRE (type < ObjectTypeInfo::numTypes);

      switch (type)
      {
#define TMP(TYPE)                                                       \
        case TYPE:                                                      \
          return getParticlesType<TYPE> (pointer);
        TMP (OBJECT_JET);
        TMP (OBJECT_PFLOW_JET);
        TMP (OBJECT_FAT_JET);
        TMP (OBJECT_ELECTRON);
        TMP (OBJECT_PHOTON);
        TMP (OBJECT_MUON);
        TMP (OBJECT_TAU);
#undef TMP
      case OBJECT_EVENTINFO:
      case OBJECT_EVENT_SELECT:
      case OBJECT_MET:
      case OBJECT_MET2:
      case OBJECT_CLEANING:
      case OBJECT_OVERLAP_REMOVAL:
      case OBJECT_TRIGGER:
      case OBJECT_NONE:
        {
          std::ostringstream str;
          str << "unsupported object type " << type;
          RCU_THROW_MSG (str.str());
        }
      }
      return 0; //compiler dummy
    }
  }
}
