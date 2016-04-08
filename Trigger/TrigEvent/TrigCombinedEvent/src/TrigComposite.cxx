/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

//#include <iostream>
//#include <cmath>
#include <stdexcept>
#include <boost/foreach.hpp>	
#include "TrigCombinedEvent/TrigComposite.h"


using namespace std;

TrigComposite::TrigComposite()
  : m_name("dummy"),
    m_locked(false)    
{
}

TrigComposite::TrigComposite(const std::string& name)
  : m_name(name),
    m_locked(false)    
{
}


TrigComposite::TrigComposite(const std::string& name, std::string& label1, TrigFeatureLink t1, std::string& label2, TrigFeatureLink t2)
  : m_name(name), 
    m_locked(false)    
{
  addObject(label1, t1);
  addObject(label2, t2);
}



template<typename T>
void TrigComposite::setFormat(const std::vector<std::string>& keys,  bool mustBeSet) {
  BOOST_FOREACH(const std::string& key, keys) {
    addDetail<T>(key);
    if (mustBeSet) {
      mustSet<T>(key);
    }
  }
}

template<class T>
void TrigComposite::mustSet(const std::string& key) {
  detailsMustSetList<T>().insert(key);  
}


// the implementation is identical as in case of the details
// the naming is only distinct to emphasize that the TrigFeatureLinks are quite distinct entities
void TrigComposite::addObject(const std::string& key, TrigFeatureLink link) {
  addDetail(key, link);
}

void TrigComposite::setObject(const std::string& key, TrigFeatureLink link) {
  setDetail(key, link);
}


bool TrigComposite::hasObject(const std::string& key) const {
  return hasDetail<TrigFeatureLink>(key);
}

const TrigFeatureLink& TrigComposite::getObject(const std::string& key) const {
  return getDetail<TrigFeatureLink>(key);
}

bool TrigComposite::isValid() const {
  if ( m_linksToBeSet.empty()
       and m_floatsToBeSet.empty()
       and  m_intsToBeSet.empty()
       and  m_stringsToBeSet.empty()
       and  m_v_intsToBeSet.empty() 
       and  m_v_floatsToBeSet.empty() 
       and  m_v_stringsToBeSet.empty() )
    return true;
  return false;
}

TrigComposite::~TrigComposite(){}




template<typename T>
void TrigComposite::addDetail(const std::string& key, const T& value) {
  if ( isFormatLocked() )
    throw std::invalid_argument("The format of TrigComposite object has been locked, addDetails can not be used anymore, see unlockFormat or setDetail");
  if ( hasDetail<T>(key) ) 
    throw std::invalid_argument("The TrigCompositeObejct already has key: "+ key);
  
  // now it needs to fins appropriate collection and add to it the value
  detailsMap<T>()[key] = value;
}


template<typename T>
void TrigComposite::setDetail(const std::string& key, const T& value) {
  if ( hasDetail<T>(key) == false ) 
    throw std::invalid_argument("The TrigCompositeObejct does not have key: "+ key);
  
  // now it needs to fins appropriate collection and set the value
  detailsMap<T>()[key] = value;
  detailsMustSetList<T>().erase(key);
}


template<typename T>
bool TrigComposite::hasDetail(const std::string& key) const {
  return detailsMap<T>().find(key) != detailsMap<T>().end();
}





template<typename T>
const T&  TrigComposite::getDetail(const std::string& key) const {
  if ( hasDetail<T>(key) ) 
    return detailsMap<T>().find(key)->second;
  return specimen<T>();
}


template<typename T>
void TrigComposite::eraseDetail(const std::string& key) {
  detailsMap<T>().erase(key);
}


template<typename T>
std::map<std::string, T>& TrigComposite::detailsMap() {
  // this should never be needed, need to add compile error here
  int z = sizeof(struct TrigComposite_does_not_support_that_type_as_a_detail);
}

template<typename T>
const std::map<std::string, T>& TrigComposite::detailsMap() const {
  // this should never be needed, need to add compile error here
  int z = sizeof(struct TrigComposite_does_not_support_that_type_as_a_detail);

}

// this macro helps instantiating methods for all necessary template arguments
// disallowing impleicit specializations we have a control over detail which at attached
#define GEN_(type, varaible) \
  template<> std::map<std::string, type>& TrigComposite::detailsMap() { return varaible; } \
  template<> const std::map<std::string, type>& TrigComposite::detailsMap() const { return varaible; } \
  template<> std::set<std::string>& TrigComposite::detailsMustSetList<type>() { return varaible##ToBeSet; } \
  template<> const type& TrigComposite::specimen<type>() const { static type x; return x;} \
  template   void TrigComposite::addDetail<type>(const std::string&, const type&); \
  template   void TrigComposite::setDetail<type>(const std::string&, const type&); \
  template   bool  TrigComposite::hasDetail<type>(const std::string& ) const; \
  template   const type&   TrigComposite::getDetail<type>(const std::string& ) const; \
  template   void  TrigComposite::eraseDetail<type>(const std::string& );  \
  template   void  TrigComposite::setFormat<type>(const std::vector<std::string>&, bool );  \
  template   void TrigComposite::mustSet<type>(const std::string&);
  
GEN_(TrigFeatureLink, m_links)

// singular details
GEN_(float, m_floats)
GEN_(int, m_ints)
GEN_(std::string, m_strings)

// vector details
GEN_(std::vector<float>, m_v_floats)
GEN_(std::vector<int>, m_v_ints)
GEN_(std::vector<std::string>, m_v_strings)
// shall more be needed ... easy to add, remember though about persistancy

namespace {
template<class T>
MsgStream& print(MsgStream& log, const TrigComposite& d, const std::string& detailsName ) {
  if ( ! d.allDetails<T>().empty() ) {
    log << "TrigComposite: Details stored as " << detailsName << " are (key, value): ";

    typedef typename std::map<std::string, T>::value_type key_value;
    BOOST_FOREACH( const key_value& kv, d.allDetails<T>()) {
      log << "(" << kv.first << ", " << kv.second << ")   ";
    }
    log << endreq;
  }
  
  return log;
}
}


MsgStream& operator<< ( MsgStream& log, const TrigComposite& d ) {
  log << "TrigComposite object: " << d.name() << endreq;
  print<float> (log, d, "floats");
  print<int> (log, d, "ints");
  print<std::string> (log, d, "strings");


  print<std::vector<float> > (log, d, "vector of floats") ;
  print<std::vector<int> > (log, d, "vector of ints");
  print<std::vector<string> > (log, d, "vector of strings");

  if ( ! d.allDetails<TrigFeatureLink>().empty() ) {
    typedef std::map<std::string, TrigFeatureLink>::value_type key_value;
    BOOST_FOREACH( const key_value& kv, d.allDetails<TrigFeatureLink>()) {
      log << "(" << kv.first << ", " << "CLID:" << kv.second.clid() <<")   ";
    }
    
  } else {
    log << "There are no objects assocuiated with this TrigComposite object" << endreq;
  }


  return log;
}



