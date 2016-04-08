/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TrigCombinedEvent__TrigComposite_h__
#define TrigCombinedEvent__TrigComposite_h__
#include <string>
#include <vector>
#include <cmath>
#include <limits>
#include "CLIDSvc/CLASS_DEF.h"
//#include "DataModel/DataVector.h"
#include "GaudiKernel/ClassID.h"
#include "TrigNavigation/TrigFeatureLink.h"
#include "GaudiKernel/MsgStream.h"


#include "boost/any.hpp"

/**
 * @class TrigComposite
 * The class is meant to store links (of type TrigFeatureLink) to trigger objects and arbitrary details (of type, float, int, string, or vectors of those) 
 * @author Camille Belanger-Champagne
 * @author Tomasz Bold
 */
class TrigComposite {
  
 public:
  /**
   * @brief Constructs empty composite, no objects no details
   */
  
  TrigComposite(const std::string& name);

  /**
   * @brief Constructs composite with the links to two trigger objects/links to them
   * The validity of the links is not checked during the construction
   */
  TrigComposite(const std::string& name, std::string& label1, TrigFeatureLink t1, std::string& label2, TrigFeatureLink t2);


  inline const std::string& name() const { return m_name; }


  /**
   * @brief defines the content (details which must to be stored)
   * The method can be used to enforce certain content of the object (set the demand that the deail are set).   
   * Each type of the details can be fixed independently.
   * Also the links can be demanded to be set this way.
   *
   * This functionality can be used in some sort factories producing TrigComposite of fixed content/contract for the numer of algorithms
   * which they will be "forced" to fill. The enforcement is weak, the isValid method can be then called and so the
   * fulfillment of the contract.
   */
  template<typename T>
  void setFormat(const std::vector<std::string>& keys,  bool mustBeSet=false);
  
  /**
   * Checks the validity of the format, i.e. if all the details which were meant to be added are added
   */
  bool isValid() const;

  /**
   * @brief makes further addition of the details impossible
   */  
  inline void lockFormat(){m_locked=true;}

  /**
   * @brief return true if the format can not be further changed (no addDetail can be called)
   */  
  inline bool isFormatLocked() const {return m_locked; }


  /**
   * @brief makes addition of further details possible again
   */
  inline void unlockFormat(){m_locked=false;}


  /**
   * @brief adds the link @see TrigFeatuerLink to the object, link is not checked for the validity, 
   * @see addDetail
   */
  void addObject(const std::string& key, TrigFeatureLink link);


  /**
   * @brief sets the link @see TrigFeatuerLink to the object, link is not checked for the validity
   * @see setDetail
   */
  void setObject(const std::string& key, TrigFeatureLink link);

  /**
   * @brief checks if the object has link to the object under the key
   */
  bool hasObject(const std::string& key) const; 

  /**
   * @brief returns @see TrigFeatureLink associated to the key, if there is none, the invalid TrigFeatureLink is returned
   */
  const TrigFeatureLink& getObject(const std::string& key) const;


  /**
   * @brief get all links which point to objects of given type
   * @arg keypart retrieve only objects with matching key, (checked by means of string::find)
   */
  template<typename T>
  std::vector<TrigFeatureLink> getObjectsByType(const std::string& keypart="") const;

  

  /**
   * @brief adds the value user the key, if they detail under that key already exists the exception is thrown
   * If the format is locked the exception is thrown each time
   */  
  template<typename T>
  void addDetail(const std::string& key, const T& value=T());

  /**
   * @brief sets the value user the key, if they detail under that key already exists it's value is overwritten, 
   * If the key does not exists, the exception is thrown
   */  
  template<typename T>
  void setDetail(const std::string& key, const T& value);


  /**
   * @brief Checks if the object contains detail of the type T
   * No verification is made if it is set
   */    
  template<typename T>
  bool hasDetail(const std::string& key) const;


  /**
   * @brief return the value of the detail, if the key is absent the default is returned
   */    
  template<typename T>
  const T&  getDetail(const std::string& key) const;
  
  /**
   * @brief return the map of all the details
   * Notie that the non-const version is not provided because it 
   * would allow to circumvent format fixing whcih may be defined for this object.
   */    

  template<class T>
  const std::map<std::string, T>& allDetails() const {
    return detailsMap<T>();
  }


  /**
   * @brief erases the detail if it existed
   * If the details inder that key did not exist, no action is performed.
   */
  template<typename T>
  void eraseDetail(const std::string& key);



  //std::string name() { return m_name; };
  /**
   * @brief Deafault constructor, should not be normally used, needed by the persistency layer
   */
  TrigComposite();

  /**
   * @brief Destructor, does nothing special in fact, no links are touched
   */
  ~TrigComposite();


private:
  template<class T>
  void mustSet(const std::string& key);

  //  bool nameExists(const std::string& name);
  //  bool isSet(const std::string& name) const;
  //  int setOrAddAny(const std::string& name, boost::any detail, bool isAdd);
  
  //mutable Athena::MsgStreamMember m_log;
  
  typedef std::set<std::string> MustSetList;

  std::map<std::string, TrigFeatureLink > m_links;
  MustSetList m_linksToBeSet;
  
  std::map<std::string, float > m_floats;
  MustSetList m_floatsToBeSet;

  std::map<std::string, int> m_ints;
  MustSetList m_intsToBeSet;

  std::map<std::string, std::string> m_strings;
  MustSetList m_stringsToBeSet;

  std::map<std::string, std::vector<float> > m_v_floats;
  MustSetList m_v_floatsToBeSet;

  std::map<std::string, std::vector<int> > m_v_ints;
  MustSetList m_v_intsToBeSet;

  std::map<std::string, std::vector<std::string> > m_v_strings;
  MustSetList m_v_stringsToBeSet;

  template<typename T>
  std::map<std::string, T>& detailsMap();

  template<typename T>
  const std::map<std::string, T>& detailsMap() const;

  template<typename T>
  MustSetList& detailsMustSetList();

 
  template<typename T>
  const T& specimen() const; // needed in order to produce the "NULL" detail
  

  //typedef std::pair<std::string, const void*> obj_store_t;
  //std::vector<obj_store_t> m_objects;

  std::string m_name;
  bool m_locked;
  //bool m_mustset;

};

/**
 * @brief Prints the content of the object
 */
MsgStream& operator<< ( MsgStream& m, const TrigComposite& d );


template<typename T>
std::vector<TrigFeatureLink> TrigComposite::getObjectsByType(const std::string& keypart) const {
  const CLID requested_clid = ClassID_traits<T>::ID(); 
  std::vector<TrigFeatureLink> result;
  for ( std::map<std::string, TrigFeatureLink>::const_iterator i = m_links.begin(); i != m_links.end(); ++i ) {
    if ( i->second.clid() == requested_clid and i->first.find(keypart) != std::string::npos) 
      result.push_back(i->second);
  }
  return result;
}




///CLASS_DEF( TrigComposite , 94108200 , 1 )
CLASS_DEF( TrigComposite , 231914326 , 1 )

  
 
#endif // __TrigComposite_h__
  
