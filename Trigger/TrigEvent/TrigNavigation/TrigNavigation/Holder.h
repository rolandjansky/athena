// Emacs -*- c++ -*-

/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRIGNAVIGATION_HOLDER_H
#define TRIGNAVIGATION_HOLDER_H
#include <string>
#include <vector>
#include <type_traits>

#include "GaudiKernel/ClassID.h"

#include "xAODCore/AuxSelection.h"
#include "AthContainers/OwnershipPolicy.h"
#include "AthContainers/DataVector.h"
#include "AthLinks/ElementLinkVector.h"
#include "AthenaKernel/StorableConversions.h"

#include "TrigNavigation/AccessProxy.h"
#include "TrigNavigation/TypeProxy.h"
#include "TrigNavigation/TriggerElement.h"

#include "TrigStorageDefinitions/TypeInformation.h"
#include "TrigStorageDefinitions/EDM_TypeInfoMethods.h"

#include "TrigNavStructure/BaseHolder.h"
#include "GaudiKernel/ToolHandle.h"

class MsgStream;
class ITrigSerializerToolBase;

namespace SG {
  class IAuxStoreIO;
  class AuxStoreInternal;
}

namespace HLTNavDetails {

  /**
   * @brief declaration of formatting function.
   */
  //note: needs to be up here to get template compiled.
  std::string formatSGkey(const std::string& prefix, const std::string& containername, const std::string& label);


  /**
   * @class used for features holding
   * In fact this class is here in order to allow STL container for all features
   * This class is showing a common denominator of all Holders for given type
   * The necessary for class to be storable is to have CLID.
   * In order to be serialized by Serializer at least the dictionary must be generated.
   */

  class IHolder : public HLT::BaseHolder {
  public:
    IHolder();
    IHolder(const std::string& prefix, const std::string& label, uint16_t idx );
    virtual ~IHolder();


    virtual IHolder* clone(const std::string& prefix, const std::string& label,  uint16_t idx) = 0; // actual constructor

    /**
     * @brief prepares this holder by setting messaging, StoreGate access and providing serializer
     */
    
    virtual void prepare(MsgStream* log, HLT::AccessProxy* sg, IConversionSvc* objSerializer, bool readonly);
    
    virtual bool syncWithSG(SG::OwnershipPolicy policy = SG::OWN_ELEMENTS) = 0;

    /**
     * @brief returns the CLID of objects stores by this holder
     */
    virtual CLID typeClid() const  = 0;
    virtual CLID containerClid() const = 0;
    virtual CLID auxClidOrZero() const = 0;

    /**
     * @brief returns the label of objects stores by this holder
     */
    inline const std::string& label() const { return  m_label; }

    /**
     * @brief returns the containers StoreGate prefix
     */
    virtual const std::string& prefix() const { return m_prefix; }

    /**
     * @brief returns the containers StoreGate key
     */
    virtual const std::string& key() const = 0;

    /**
     * @brief returns the object's name stored by this holder
     */
    virtual const std::string& typeName() const = 0;

    /**
     * @brief returns the collection's name stored by this holder
     */
    virtual const std::string& collectionName() const = 0;

    /**
     * @brief returns the index (short number used when linking object to the TE)
     * of objects stores by this holder
     */
    inline uint16_t subTypeIndex() const { return  m_subTypeIndex; }

    std::string generateAliasKey(CLID c, uint16_t sti, const std::string& label, unsigned size);

    /**
     * @brief serializes this Holder
     */
    virtual bool serialize(std::vector<uint32_t>& output)  const;

    /**
     * @brief serializes this Holder including payload
     */
    bool serializeWithPayload(const xAOD::AuxSelection& sel,
                              std::vector<uint32_t>& output,
                              size_t& payloadsize);


    /**
     * @brief serializes the payload of this Holder
     */
    bool serializePayload(std::vector<uint32_t>& dataBlob,
                          const xAOD::AuxSelection& sel);



    virtual void print(MsgStream& m) const;

    /**
     * @brief deserialization of holder payload
     */
    bool deserializePayload(const std::vector<uint32_t>& dataBlob, int version);

    // serialization helpers
    virtual DataObject* getDataObject() = 0;
    virtual DataObject* getAuxDataObject() = 0;

    virtual bool setDataObject(DataObject* dobj) = 0;
    virtual bool setAuxDataObject(DataObject* dobjaux) = 0;

    /**
     * Get the proxy for the container
     */
    virtual const ITypeProxy& containerTypeProxy() const = 0;

    virtual bool checkAndSetOwnership(SG::OwnershipPolicy policy) = 0;
    virtual std::string getUniqueKey() = 0; // this is backward compatibility for TrigCaloRec and TrigTauRec, whould be removed
    virtual std::string getNextKey() = 0;   // this is backward compatibility for TrigCaloRec and TrigTauRec, whould be removed

  protected:
    // serialization helpers
    MsgStream*                      m_log{0};
    IConversionSvc*                 m_objectserializerSvc{0};     //!< pointer to object serializer
    HLT::AccessProxy*               m_storeGate{0};               //!< pointer to SG

    bool         m_readonly{false};
    ITypeProxy*  m_aux{0};

    // Adapters so we can use ATH_MSG macros
    MsgStream& msg() const { return *m_log; }
    MsgStream& msg(const MSG::Level lvl) const { return msg() << lvl; }
    bool msgLvl(const MSG::Level lvl) const { return msg().level() <= lvl; }

  private:
    /**
     * @brief Serialize all selected dynamic variables for an xAOD object.
     * @param iio The auxiliary store for the xAOD object.
     * @param sel Selection object for variables to write.
     * @param[out] dataBlob Serialized data.
     *
     * Returns true on success, false on failure.
     *
     * Each selected dynamic variable is concatenated to @c dataBlob as follows.
     *
     *   - Length serialized name data (1 word).
     *   - Serialized name data.
     *   - Length of serialized type name (1 word).
     *   - Serialized type name data.
     *   - Length of serialized variable data (1 word).
     *   - Serialized data for the variable, as produced by TrigTSerializer.
     */
    bool serializeDynVars (const SG::IAuxStoreIO& iio,
                           const xAOD::AuxSelection& sel,
                           std::vector<uint32_t>& dataBlob);


    /**
     * @brief Read dynamic auxiliary variables from a serialized buffer.
     * @param dataBlob Serialized data.
     * @param offs Offset in @c dataBlob where dynamic variables start.
     * @param[out] store Auxiliary store for the object being read.
     *
     * @c dataBlob should be formatted as described in the documentation
     * for @c serializeDynVars starting at @c offs.
     * @c store should be convertable to @c IAuxStoreHolder.
     * A new dynamic store will be created and registered with @c store
     * via @c setStore.
     *
     * Returns true on success, false on failure.
     */
    bool deserializeDynVars (const std::vector<uint32_t>& dataBlob,
                             size_t offs,
                             SG::IAuxStore& store);


    /**
     * @brief Read dynamic auxiliary variables from a serialized buffer.
     * @param dataBlob Serialized data.
     * @param offs Offset in @c dataBlob where dynamic variables start.
     * @param sz Number of elements in the xAOD container.
     *
     * @c dataBlob should be formatted as described in the documentation
     * for @c serializeDynVars starting at @c offs.
     * Returns a new dynamic store.
     */
    std::unique_ptr<SG::IAuxStore>
    deserializeDynVars (const std::vector<uint32_t>& dataBlob,
                        size_t offs);

    
    /**
     * @brief Find the type of an element of a vector.
     * @param tname The name of the type to analyze.
     * @param[out[ elementTypeName The name of the type of an element of the vector.
     *
     * Returns the @c type_info for an element of the vector.
     */
    const std::type_info* getElementType (const std::string& tname,
                                          std::string& elementTypeName) const;
    

    std::string  m_prefix;            //!< prefix for key given to the objects
    std::string  m_label;             //!< label given to the objects in this holder (labels given at attachFeature)
    uint16_t     m_subTypeIndex{0};   //!< index to notify how many objects of given type we have (we need to record it in case of slimming will be done latter)
    int          m_uniqueCounter{0};

    /// Serializer tool; used for dynamic variables.
    ToolHandle<ITrigSerializerToolBase> m_serializer;
  };


  //////////////////////////////////////////////////////////////////////////////////////

  /**
   * @brief Helper struct to determine if persistable type is a DataLink<CONTAINER> (in case STORED and CONTAINER are the same)
   * of rather an ElementLink<CONTAINER> in case STORED is the element type of CONTAINER
   */

  template<class STORED, class CONTAINER, bool is_same> struct set_link;

  template<class STORED, class CONTAINER>
  struct set_link<STORED,CONTAINER,true>{
    typedef DataLink<CONTAINER> type;
    static type do_it(const STORED* el, const CONTAINER* src, HLT::TriggerElement::ObjectIndex idx){
      (void)src;(void)idx; //not used here
      type t(el);
      return t;
    }
  };

  template<class STORED, class CONTAINER>
  struct set_link<STORED,CONTAINER,false>{
    typedef ElementLink<CONTAINER> type;
    static type do_it(const STORED* el,const CONTAINER* src, HLT::TriggerElement::ObjectIndex idx){
      (void)el;//not used here
      type t(*src,idx.objectsBegin());
      return t;
    }
  };

  //////////////////////////////////////////////////////////////////////////////////////
  template<class T, class C> class HolderImp;
  /**
   * @class Specialized holder class for each object type
   *******************************************************************
   * This class in addition to the above IHolder provides the interface that only depends on the STORED type
   * and needs to be implemented by the derived HolderImp
   */
  template<class STORED>
  class Holder : public IHolder {
  public:
    Holder();
    Holder(const std::string& prefix, const std::string& label, uint16_t idx);
    virtual ~Holder();

    virtual HLT::TriggerElement::ObjectIndex add( const STORED* f, bool inSG, const std::string& = "" ) = 0;          //!< saved object in this holder
    virtual bool get(const STORED*& dest, HLT::TriggerElement::ObjectIndex idx) = 0;

    template<class CONTAINER2> 
    bool get(ElementLinkVector<CONTAINER2>& cont);

    template<class CONTAINER2> 
    bool get(ElementLinkVector<CONTAINER2>& cont, HLT::TriggerElement::ObjectIndex idx);

    template<class CONTAINER2> 
    bool getWithLink(typename set_link<STORED,CONTAINER2,std::is_same<STORED,CONTAINER2>::value>::type& link,
                     HLT::TriggerElement::ObjectIndex& idx) {
      
      bool result = static_cast<HolderImp<STORED,CONTAINER2>*>(this)->getWithLink(link,idx);
      return result;
    }       

    virtual bool contains(const STORED* obj, HLT::TriggerElement::ObjectIndex& idx) const = 0;
  };

  //////////////////////////////////////////////////////////////////////////////////////

  /**
   * @brief This is an implementation class for all Holders
   * It is templated with 2 arguments STORED which is type of stored objects
   * Another type is CONTAINER for that type.
   * Examples when it works are:
   * HolderImp<A, DataVector<A> > --- typical
   * HolderImp< A, Acontainer> >  --- as above, Acontainer is either typedef for DataVector<A> or inherits from it
   * HolderImp< DataVector<A>, DataVector<A> > --- when both are of the same type
   * HolderImp< Acontainer, Acontainer> >      --- as above
   */
  template<class STORED, class CONTAINER>
  class HolderImp: public  Holder<STORED> {
  public:
    typedef Holder<STORED> base_type;
    typedef STORED stored_type;
    typedef CONTAINER container_type;
    typedef HLTNavDetails::TypeProxy<STORED> FeatureProxy;
    typedef HLTNavDetails::TypeProxy<CONTAINER> ContainerProxy;

    HolderImp();
    HolderImp(const std::string& prefix, const std::string& label, uint16_t idx);
    virtual ~HolderImp();

    virtual IHolder* clone(const std::string& prefix, const std::string& label, uint16_t idx );

    /**
     * @brief adds object(s) to be holded
     */
    virtual HLT::TriggerElement::ObjectIndex add( const STORED* f, bool inSG = false, const std::string& = "" );

    /**
     * @brief gets object(s) holded
     */
    virtual bool get(const STORED*& dest, HLT::TriggerElement::ObjectIndex idx);

    /**
     * @brief cehcks if object(s) in dest are holded by this holder
     */
    virtual bool contains(const STORED* obj, HLT::TriggerElement::ObjectIndex& idx) const;

    bool getElementLinks(ElementLinkVector<CONTAINER>& cont,  HLT::TriggerElement::ObjectIndex idx);
    bool getElementLinks(ElementLinkVector<CONTAINER>& cont);
        
    bool getWithLink(typename set_link<STORED,CONTAINER,std::is_same<STORED,CONTAINER>::value>::type& link,
                     HLT::TriggerElement::ObjectIndex& idx);
        
    virtual std::string getUniqueKey(); // this is backward compatibility for TrigCaloRec and TrigTauRec, whould be removed
    virtual std::string getNextKey();   // this is backward compatibility for TrigCaloRec and TrigTauRec, whould be removed

    virtual void prepare(MsgStream* log, HLT::AccessProxy* sg, IConversionSvc* objSerializer, bool readonly = false);
    virtual bool syncWithSG(SG::OwnershipPolicy policy=SG::OWN_ELEMENTS);
    virtual bool checkAndSetOwnership(SG::OwnershipPolicy policy);

    virtual bool serialize(std::vector<uint32_t>& output) const;

    virtual const std::string& key() const { return m_key; }
    virtual void print(MsgStream& m) const;

    virtual DataObject* getDataObject();
    virtual DataObject* getAuxDataObject();
    virtual bool setDataObject(DataObject* dobj);
    virtual bool setAuxDataObject(DataObject* dobjaux);

    virtual CLID typeClid() const { return ClassID_traits<STORED>::ID(); }
    virtual const std::string& typeName() const { return ClassID_traits<STORED>::typeName(); }
    virtual CLID containerClid() const { return ClassID_traits<CONTAINER>::ID(); }
    virtual const std::string& collectionName() const { return ClassID_traits<CONTAINER>::typeName(); }
    virtual CLID auxClidOrZero() const;

    virtual const ITypeProxy& containerTypeProxy() const { return m_containerProxy; }

    // for memory management
    struct MemoryMgr {
      MemoryMgr();
      MemoryMgr(const FeatureProxy& st, bool inSG);
      void clear();
      FeatureProxy proxy;
      bool inSG;
    };

    mutable ContainerProxy m_containerProxy;

    typedef std::multimap<HLT::TriggerElement::ObjectIndex, MemoryMgr> MemoryMgrMap;
    MemoryMgrMap m_memMgr;

  private:
    std::string m_key; //<! StoreGate key
  };

  MsgStream& operator<< ( MsgStream& m, const HLTNavDetails::IHolder& nav ); //<! printing helper
  template<class T>
  MsgStream& operator<< ( MsgStream& m, const HLTNavDetails::Holder<T>& nav ); //<! printing helper
  template<class T, class C>
  MsgStream& operator<< ( MsgStream& m, const HLTNavDetails::HolderImp<T, C>& nav ); //<! printing helper
} // end of namespace
#endif
