/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef JETEVENT_JETKEYDESCRIPTOR_H
#define JETEVENT_JETKEYDESCRIPTOR_H

#include <string>
#include <vector>
/// //////////////////////////////////////////////////
///
/// \class JetKeyDescriptor
/// \class JetKeyDescriptorInstance
/// 
/// 
/// Theses classes implement a mapping between string and index used by jets.
///  they are used for jet moments, jet associations, etc...
/// There are 2 classes :
///   * JetKeyDescriptor is the data class where the mappings are stored. There is only 
///     1 such class per event. This class is stored in SG and persistified. 
///   * JetKeyDescriptorInstance : the class used to access the mapings in the above
///     data class.
///     Each JetCollection holds a JetKeyDescriptorInstance.
///     There is also a global JetKeyDescriptorInstance, which is used by jets not 
///     associated to a JetCollection.
///  When only 1 event is read in memory, all JetKeyDescriptorInstance point to 
///  the same JetKeyDescriptor. 
///  When 2 events are read at the same time (in some ARA analysis), JetCollection's JetKeyDescriptorInstance
///  point to the JetKeyDescriptor from the event they belong to. The global JetKeyDescriptorInstance points
///   to the last event read.
///
/// ///////////////////////////////////////////////////////

class MsgStream;


// forward declare some conversion class
template <class T1, class T2, class T3>
  class T_AthenaPoolTPPolyCnvBase;
class JetKeyDescriptor_p1;

// payload for the JetKeyDescriptor framework.
class JetKeyDescriptor
{
  // allow access to private data members for these classes
  friend class JetKeyDescriptorCnv_p1;
  friend class JetKeyDescriptorCnv;
  friend class JetKeyDescriptorInstance;
  friend class T_AthenaPoolTPPolyCnvBase<JetKeyDescriptor,JetKeyDescriptor, JetKeyDescriptor_p1>;  
 public:
  
  typedef std::string      category_t;
  typedef std::string      key_t;
  
  typedef std::vector<key_t>       keystore_t;
  typedef std::vector<keystore_t>  storelist_t;
  typedef std::vector<category_t>  catlist_t;
  
  // dummy implementation, needed for the DataLink to call the converter
  void access() const { };
  
  
  virtual ~JetKeyDescriptor();
  


  // dummy implementation, just to be called from the converters
  JetKeyDescriptor();

  // For debugging.
  const storelist_t& keyStore() const { return m_keyStore; }
  const catlist_t&   catStore() const { return m_catStore; }

 protected:  
  
  storelist_t   m_keyStore;
  
  catlist_t     m_catStore;
  
};

#include "CLIDSvc/CLASS_DEF.h"
CLASS_DEF(JetKeyDescriptor,71458009,1)

struct JetKeyConstants {
  typedef JetKeyDescriptor::key_t key_t;

  /** @brief Index category for jet shapes */
  static const key_t ShapeCat;
  /** @brief Index category for jet associations */
  static const key_t AssoCat;
  /** @brief Index category for jet tag info */
  static const key_t TagCat;
  /** @brief Index category for general jet info */
  static const key_t InfoCat;

};


class JetKeyDescriptorInstance
{
  friend class JetKeyDescriptor;
  friend class JetCollection;
  friend class JetKeyDescriptorCnv_p1;

  
 public:
  
  typedef std::string      category_t;
  typedef std::string      key_t;
  
  typedef std::vector<key_t>       keystore_t;
  typedef std::vector<keystore_t>  storelist_t;
  typedef std::vector<category_t>  catlist_t;
  
  // singleton implementation for now
  virtual ~JetKeyDescriptorInstance();
  
  static JetKeyDescriptorInstance* instance()
    {
      return &s_instance;
    }
  static JetKeyDescriptorInstance       s_instance;
  
  // get a unique index
  size_t getIndex(const category_t& cat,const key_t& key, bool createIfMissing = true );
  
  // get key for a given category and index
  const key_t& getKey(const category_t& cat,size_t index) const;
  
  // get all keys for a given category
  const std::vector<key_t>& getKeys(const category_t& cat) const;
  
  // get all categories
  const std::vector<category_t>& getCategories() const;
  
  // check validity
  bool isValid(size_t index) const;
  
  // printout some stuff
  void printOut(MsgStream& msgStream) const;

  // retrieve the key store :
  JetKeyDescriptor* getKeyStore();

 protected:
  
  JetKeyDescriptorInstance(bool create=true);
  
  mutable JetKeyDescriptor*  m_Stores;
  
  void createKeyStore() const;

 private:
  

  static size_t                  m_invalid;
  static std::string             m_notFound;
  static std::vector<key_t>      m_invalidKeys;
  
  static bool                    m_persistified;
};

inline bool JetKeyDescriptorInstance::isValid(size_t index) const
{
  return index != m_invalid;
}

inline const std::vector<JetKeyDescriptorInstance::category_t>& 
JetKeyDescriptorInstance::getCategories() const
{
  if(bool(m_Stores))
    return m_Stores->m_catStore;
  static std::vector<JetKeyDescriptorInstance::category_t> v;
  return v;
}
/**  @class JetKeyDescriptor                                                      
     @brief Maintains one-to-one association between strings and index            
                                                                                  
     The goal of this class is to allow @c Jet users to define quantities associated to      
     jets (e.g. shapes or Btagging info) with strings (thus in a meaning full and dynamic way) 
     while internaly using index (persistance performance). 

     @c JetKeyDescriptor is internal to JetSoftware and users should not be exposed to it 
                                                                                  
*/

#endif
