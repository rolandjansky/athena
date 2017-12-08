// -*- c++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRIGGER_DECISION_TOOL_Feature_H
#define TRIGGER_DECISION_TOOL_Feature_H

/**********************************************************************************
 * @Project:
 * @Package: TrigDecision
 * @class  : Feature
 *
 *
 * @author Nicolas Berger  <Nicolas.Berger@cern.ch>  - LAPP Annecy
 * @author Tomasz Bold     <tomasz.bold@cern.ch>     - UC Irvine, AGH-UST Krakow
 * @author Lukas Heinrich  <lukas.heinrich@cern.ch>  - NYU
 *
 ***********************************************************************************/

#include <string>
#include <set>
#include "boost/foreach.hpp"
#include "boost/type_traits/is_same.hpp"
#include "boost/shared_ptr.hpp"
#include "boost/lexical_cast.hpp"

#include "AsgTools/MsgStream.h"

#include "TrigNavStructure/TriggerElement.h"
#include "xAODTrigger/EmTauRoI.h"
#include "xAODTrigger/EmTauRoIContainer.h"

#include "xAODTrigger/MuonRoI.h"
#include "xAODTrigger/MuonRoIContainer.h"

#include "xAODTrigger/JetRoI.h"
#include "xAODTrigger/JetRoIContainer.h"

#include "TrigDecisionTool/TypelessFeature.h"

#if defined(ASGTOOL_ATHENA) && !defined(XAOD_ANALYSIS)
#include "TrigStorageDefinitions/EDM_TypeInfo.h"
#endif


//forward declare ROI types used in is_storable_type use below
//it's important for this to happen outside of Trig 
//e.g. not in the boost::is_same call directly a la 
// boost::is_same<T,struct Muon_ROI>::value
//so that it does not end up to be a check on is_same<T,Trig::Muon_ROI> 
#if defined(ASGTOOL_ATHENA) && !defined(XAOD_ANALYSIS)
class Muon_ROI;
class EmTau_ROI;
class Jet_ROI;
#endif

#include "xAODBase/IParticle.h"

namespace Trig {
  /**
   * @class Feature is basic vehicle of object access in TDT
   *
   * It is wrapped pointer to trigger object of given type (template argument). 
   *
   * It is also the point in the structure of navigation where 
   * certain objects is attached. This point is identified by HLT::TriggerElement and label of feature.
   *
   * In C++ this class is very handy as it is convertible to HLT::TriggerElement or object through to it's conversion operators.
   * This, in turn gives an access to the particular HLT hypothesis decision 
   * through the state of @see HLT::TriggerElement::getActiveState() .
   *
   * More explicit access is also possible by cptr(), te() and label() methods. It is necessary in the python.
   * If the access is unsuccessful for any reason i.e. actual lack of object the Feature::empty() returns true.
   **/
  
  
  template<typename T> struct is_storable_type{
#if defined(ASGTOOL_ATHENA) && !defined(XAOD_ANALYSIS)
    static const bool value = 
      !(
	boost::is_same<T,Muon_ROI>::value  ||
	boost::is_same<T,EmTau_ROI>::value ||
	boost::is_same<T,Jet_ROI>::value ||
	boost::is_same<T,xAOD::EmTauRoI>::value ||
	boost::is_same<T,xAOD::MuonRoI>::value ||
	boost::is_same<T,xAOD::JetRoI>::value  ||
	boost::is_same<T,xAOD::IParticle>::value
	);
#else
    static const bool value = false;
#endif
  };
    
  template<typename T,bool> struct link_or_not;
  

#if defined(ASGTOOL_ATHENA) && !defined(XAOD_ANALYSIS)
  template<typename T> struct link_or_not<T,true>{
    static const bool known =  IsKnownFeature<T>::value; //will cause compile error if not
    typedef typename Features2Container<T>::type container_type;
    typedef typename Features2LinkHelper<T,container_type>::type type;
  };
#endif

  struct ElementLink_is_not_available_for_non_storable_types{};

  template<typename T> struct link_or_not<T,false>{
    typedef ElementLink_is_not_available_for_non_storable_types type;
  };

  template<class T>
  class Feature {
  public:
    /**
     * $brief constructor of empty Feature
     **/
    typedef T ValueType;
    typedef T* PointerType;

    #ifndef __GCCXML__
    typedef typename link_or_not<T,is_storable_type<T>::value >::type link_type;
    #endif

    Feature() 
      : m_feature((const T*)0), m_te(0), m_label(""), m_owned(false) { }

    Feature(const TypelessFeature& feat, const T* const feature) 
      : m_feature(feature), m_te(feat.te()),
	m_label(feat.label()), m_owned(false) { }

    Feature(const TypelessFeature& feat, const std::shared_ptr<const T>& feature) 
      : m_owning_feature(feature),
	m_feature(m_owning_feature.get()),
	m_te(feat.te()), m_label(feat.label()), m_owned(true) { }

    /**
     * @brief constructor of valid Feature object 
     **/    
    #ifndef __GCCXML__
    Feature(const T* feature, const HLT::TriggerElement* te, const std::string& label = "", const bool own = false, const link_type link = link_type()) 
      : m_feature(feature), m_te(te), m_label(label), m_owned(own), m_link(link) {
      //in Athena if we own this feature, it will be deleted by AnyTypeDeleter in CGM
      //so we're fine by just setting the non-owning ptr.
    }
    #endif

    /**
     * @brief destructor taking care of removal
     **/
    ~Feature() {
    }

    /**
     * @brief implicit conversion to object pointer
     **/
    operator const T*() const { return cptr(); }

    /**
     * @brief explicit conversion to object
     **/
    const T* cptr() const { return m_feature; }

    /**
     * @brief implicit conversion to TriggerElement
     **/    
    operator const HLT::TriggerElement*() const { return m_te; }

    /**
     * @brief explicit conversion to TriggerElement
     **/
    const HLT::TriggerElement* te() const { return m_te; }

    /**
     * @brief implicit conversion to feature label
     **/    
    operator const std::string() const { return m_label; }

    /**
     * @brief explicit conversion to feature label
     **/
    const std::string& label() const { return m_label; }

    /**
     * @brief test method to check if the object is truly there
     **/    
    bool empty() const { return (!m_te || !m_feature); }

    bool owned() const { return m_owned; }
    
    #ifndef __GCCXML__
    link_type link() const {return m_link;}
    #endif
   
  private:
    std::shared_ptr<const T> m_owning_feature;
    const T* m_feature;
    const HLT::TriggerElement* m_te;
    std::string m_label;
    bool m_owned;   
    #ifndef __GCCXML__
    link_type m_link;
    #endif
  };


  /**
   * @brief comarison
   **/
  template<class T>
  bool sameObject(const Feature<T>& a, const Feature<T>& b) {
    return a.cptr() == b.cptr();
  }

} // End of namespace

template<class T>
MsgStream& operator<< ( MsgStream& m, const Trig::Feature<T>& d ) {
  m << "TE id: " << d.te()->getId() << "obj ptr: " << d.cptr() 
    << " obj type: " << ClassID_traits<T>::typeName() << " label: " << d.label();
  return m;
}
#endif
