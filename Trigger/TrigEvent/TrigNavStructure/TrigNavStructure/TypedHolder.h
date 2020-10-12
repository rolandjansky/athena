// Emacs -*- c++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRIGNAVSTRUCTURE_TYPEDHOLDER_H
#define TRIGNAVSTRUCTURE_TYPEDHOLDER_H

//#include <stdexcept>
#include <type_traits>

#include "TrigNavStructure/TypelessHolder.h"
#include "TrigNavStructure/TriggerElement.h"
#include "TrigNavStructure/TypelessHolder.h"

#include "AthContainers/OwnershipPolicy.h"

#ifdef XAOD_STANDALONE //guarded b/c athena includes its own
#include "xAODCore/ClassID_traits.h"
#endif

#ifdef XAOD_STANDALONE
#include "AsgTools/SgTEvent.h"
typedef asg::SgTEvent* EventPtr;
#endif //XAOD_STANDALONE

#ifndef XAOD_STANDALONE
#include "StoreGate/StoreGateSvc.h"
typedef StoreGateSvc* EventPtr;
#endif //XAOD_STANDALONE

#include "AsgMessaging/AsgMessaging.h"


//forward declarations
class TrigRoiDescriptor;
class TrigRoiDescriptorCollection;

namespace HLTNavDetails{
  /**
   * @brief standalone method implementing trigger core software naming scheme
   **/
  std::string formatSGkey(const std::string& prefix, const std::string& containername, const std::string& label);
}

namespace HLT{


  template<typename FEATURE, typename CONTAINER> class TypedHolder;


  /**
   * @brief specialization for RoIs. The definition is done by the TDT
   **/
  template<>
  class TypedHolder<TrigRoiDescriptor,TrigRoiDescriptorCollection>;
  
  /**
   * @brief doubly templated class interfacing access to feature containers in StoreGate.
   **/
  template<typename FEATURE, typename CONTAINER>
  class TypedHolder : public TypelessHolder, public asg::AsgMessaging {
  public:
    /**
     * @brief shorthand for enable_if with returning StatusCode base on comparison with CONTAINER type
     **/
    template<typename T,bool value> using StatusCode_if = typename std::enable_if<std::is_same<T,CONTAINER>::value == value,StatusCode>::type;

    /**
     * @brief constructor from BaseHolder. Throws runtime exception if clids of BaseHolder and FEATURE type don't match
     **/

    TypedHolder(const BaseHolder& baseholder, EventPtr store, const std::string& container_name = ClassID_traits<CONTAINER>::typeName()) 
      : TypelessHolder(baseholder.typeClid(),baseholder.label(),baseholder.subTypeIndex()), 
	asg::AsgMessaging("TypedHolder"),
	m_store(store),
	m_cont(0) {
      //      if(!clidCheck<FEATURE>())
      //throw std::runtime_error("attempted construction with CLID mismatch! Check template parameter and passed typeless holder");
      m_key = HLTNavDetails::formatSGkey("HLT",container_name,this->label());
    }
    
    /**
     * @brief constructor from BaseHolder. Throws runtime exception if clids of BaseHolder and FEATURE type don't match
     **/
    TypedHolder(const TypelessHolder& typeless, EventPtr store, const std::string& container_name = ClassID_traits<CONTAINER>::typeName()) 
      : TypelessHolder(typeless),
	asg::AsgMessaging("TypedHolder"),
	m_store(store),
	m_cont(0) {
      //if(!clidCheck<FEATURE>())
      //	throw std::runtime_error("attempted construction with CLID mismatch! Check template parameter and passed typeless holder");
      m_key = HLTNavDetails::formatSGkey("HLT",container_name,this->label());
    }

    /**
     * @brief key used to access EventStore
     **/
    std::string key() const {return m_key;}

    /**
     * @brief method creates a new VIEW container containing pointers to the elements pointed to by the ObjectIndex.
     * Caller owns the VIEW container.
     * Only enabled for CONTAINER holders
     **/
    template<typename T> StatusCode_if<T,true> get(const T*& destination, HLT::TriggerElement::ObjectIndex idx){
      if(syncWithStore().isFailure()){
	ATH_MSG_ERROR("accessing holder with key: " << key() << " sync with store failed ");
	return StatusCode::FAILURE;
      }
      if(m_cont->size() < idx.objectsEnd()){
	ATH_MSG_ERROR("accessing holder with key: " << key() << " index past range " << "idx range is: " << idx.objectsBegin() << ":" << idx.objectsEnd() << " container size: " << m_cont->size());
	return StatusCode::FAILURE;
      };

      // make sure we get a fresh pointer as were about to set it to a newly created one
      if(destination){
	//ATH_MSG_ERROR("reference pointe provided is already set");
	return StatusCode::FAILURE;
      }

      CONTAINER* nonConstDestination = new CONTAINER(SG::VIEW_ELEMENTS);

       // need to cast to be able to use non const iterators
      CONTAINER* src = const_cast<CONTAINER*>(m_cont);
      typename CONTAINER::iterator beg = src->begin();
      typename CONTAINER::iterator end = src->begin();
      
      std::advance(beg, idx.objectsBegin());
      std::advance(end, idx.objectsEnd());
      nonConstDestination->insert(nonConstDestination->end(), beg, end);

      // from now own we don't want anybody modifying the container
      destination = nonConstDestination;
      return StatusCode::SUCCESS;
    }

    /**
     * @brief method retrieves container from storegated and returns pointer at passed ObjectIndex
     * Only enabled for ELEMENT holders (i.e. non CONTAINER)
     **/
    template<typename T> StatusCode_if<T,false> get(const T*& destination, HLT::TriggerElement::ObjectIndex idx){
      if((idx.objectsEnd() - idx.objectsBegin())!=1){
	//ATH_MSG_ERROR("accessing holder with key: " << key() << "index is not single element: " << idx.objectsBegin() << ":" << idx.objectsEnd());
	return StatusCode::FAILURE;
      }
      if(syncWithStore().isFailure()){
	//ATH_MSG_ERROR("accessing holder with key: " << key() << " sync with store failed ");
	return StatusCode::FAILURE;
      }
      if(m_cont->size() < idx.objectsBegin()){
	//ATH_MSG_ERROR("accessing holder with key: " << key() << " index past range " << "indexBegin is: " << idx.objectsBegin() << " container size: " << m_cont->size() << std::endl;
	return StatusCode::FAILURE;
      };

      //everything went fine
      destination = m_cont->at(idx.objectsBegin());
      return StatusCode::SUCCESS;
    }

  private:

    /**
     * @brief cache container retrieved from StoreGate
     **/
    StatusCode syncWithStore(){
      if(m_cont) return StatusCode::SUCCESS;

      StatusCode sc = m_store->retrieve(m_cont,key());
      
      //sanity checks
      if(sc.isFailure()) return StatusCode::FAILURE;
      if(!m_cont) return StatusCode::FAILURE;

      return StatusCode::SUCCESS;
    }
    
    TypedHolder(){;}
    EventPtr m_store;
    const CONTAINER* m_cont;
    std::string m_key;
  };    

}//namespace HLT

#endif
