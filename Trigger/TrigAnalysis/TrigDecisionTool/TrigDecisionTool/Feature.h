// -*- c++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRIGGER_DECISION_TOOL_Feature_H
#define TRIGGER_DECISION_TOOL_Feature_H


/**********************************************************************************
 * @Project:
 * @Package: TrigDecision
 * @Class  : TrigDecision
 *
 *
 * @author Nicolas Berger  <Nicolas.Berger@cern.ch>  - LAPP Annecy
 * @author Tomasz Bold     <tomasz.bold@cern.ch>     - UC Irvine, AGH-UST Krakow
 *
 ***********************************************************************************/

#include <string>
#include <set>
#include "boost/foreach.hpp"
#include "boost/type_traits/is_same.hpp"
#include "boost/shared_ptr.hpp"
#include "boost/lexical_cast.hpp"

#include "TrigNavigation/NavigationCore.h"
#include "TrigNavigation/NavigationCore.icc"
#include "TrigDecisionTool/Conditions.h"
#include "TrigDecisionTool/TDTUtilities.h"
#include "TrigDecisionTool/ClassTraits.h"

#include "TrigSteeringEvent/TrigRoiDescriptor.h"
#include "TrigSteeringEvent/TrigPassBits.h"
#include "TrigSteeringEvent/TrigPassFlags.h"
#include "AnalysisTriggerEvent/Muon_ROI.h"
#include "AnalysisTriggerEvent/EmTau_ROI.h"
#include "AnalysisTriggerEvent/Jet_ROI.h"
#include "AnalysisTriggerEvent/JetET_ROI.h"
#include "AnalysisTriggerEvent/EnergySum_ROI.h"
#include "AnalysisTriggerEvent/LVL1_ROI.h"

#include "xAODTrigger/EmTauRoI.h"
#include "xAODTrigger/EmTauRoIContainer.h"

#include "xAODTrigger/MuonRoI.h"
#include "xAODTrigger/MuonRoIContainer.h"

#include "xAODTrigger/JetRoI.h"
#include "xAODTrigger/JetRoIContainer.h"

#include "TrigStorageDefinitions/EDM_TypeInfo.h"

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
  
  /*
    template <class T>
    class feature_deleter
    {
  public:
  feature_deleter( bool del ) : m_del(del) {}
  void operator()(T const * ) const {
  }
    bool deleteable() const { return m_del; }
    bool m_del;
    };
  */

  template<typename T> struct is_storable_type{
    static const bool value = 
      !(
	boost::is_same<T,Muon_ROI>::value  ||
	boost::is_same<T,EmTau_ROI>::value ||
	boost::is_same<T,Jet_ROI>::value ||
	boost::is_same<T,xAOD::EmTauRoI>::value ||
	boost::is_same<T,xAOD::MuonRoI>::value ||
	boost::is_same<T,xAOD::JetRoI>::value
	);

  };
    
  template<typename T,bool> struct link_or_not;
  
  template<typename T> struct link_or_not<T,true>{
    static const bool known =  IsKnownFeature<T>::value; //will cause compile error if not
    typedef typename Features2Container<T>::type container_type;
    typedef typename Features2LinkHelper<T,container_type>::type type;
  };

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
      : m_feature((const T*)0), m_te(0), m_label("") { }

    /**
     * @brief constructor of valid Feature object 
     **/    
    #ifndef __GCCXML__
    Feature(const T* feature, const HLT::TriggerElement* te, const std::string& label = "", const bool own = false, const link_type link = link_type()) 
      : m_feature(feature), m_te(te), m_label(label), m_owned(own), m_link(link) { }
    #endif

    /**
     * @brief destructor taking care of removal
     **/
    ~Feature() { }

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


namespace FeatureAccessImpl {
  // function declaration (see cxx for the deifinition) wanted to have this freedom in case of patches needed
  const TrigPassBits* getBits(size_t sz, const HLT::TriggerElement* te, const std::string& label, HLT::NavigationCore* navigation );

  const TrigPassFlags* getFlags(size_t sz, const HLT::TriggerElement* te, const std::string& label, HLT::NavigationCore* navigation );
  

  // compile time check if the type T displays type ElementProxy (this is specific to the DataVectors only)
  template<typename T>
  struct isDataVector {
  private:
    // these types are defined such that with the sizeof() function they can be evaluated at compile time
    typedef char true_type;
    struct false_type { char dummy[2]; };

    // using SFINAE 
    template<typename U> static true_type trait_test_helper(typename U::ElementProxy*); // if U::ElementProxy does not exist, SFINAE means that this is not substituded
    template<typename U> static false_type  trait_test_helper(...);
  public:
    static const bool value=sizeof(trait_test_helper<T>(0))==sizeof(true_type); // evaluated at compule time
  };
  

  // is not substituded for DataVectors
  template<class T>
  const typename boost::disable_if_c<isDataVector<T>::value, T>::type* 
  use_or_construct(const T* source, const HLT::TriggerElement*, const std::string&, unsigned int, HLT::NavigationCore*  ) {
    return source;
  }

  // is substituded for DataVectors
  template<class T>
  const typename
  boost::enable_if_c<isDataVector<T>::value, T>::type*
  use_or_construct(const T* source, const HLT::TriggerElement* te, const std::string& label, unsigned int condition, HLT::NavigationCore* navigation ) {

    const TrigPassBits* bits(0);
    if ( condition == TrigDefs::Physics ) {// only passing objects
      bits = getBits(source->size(), te, label , navigation);
    }
    if ( bits ) { // the actual filtering
      T* destination = new T();      
      destination->clear(SG::VIEW_ELEMENTS);
      //      std::string name = "TDT_temporary_"+label+"_"+boost::lexical_cast<std::string>( (void*) destination);
      //navigation->getAccessProxy()->record(destination, name);

      
      BOOST_FOREACH(const typename T::base_value_type *obj, *source) {	
        if ( HLT::isPassing(bits, obj, source)  ) // if bits are missing or obj is realy marked as passing
          destination->push_back(const_cast<typename T::value_type>(obj));
      }
      return destination;
    }
    // else
    return source;
  }
  
  


  // 
  template<class T, class CONT, bool flatten, class LINK> struct insert_and_flatten;

  // The specialization below is for the case that requested and stored are the same types
  // It means that requested type is egamma_container and stored is egamma_container
  // or the requested is TrigRoiDescriptor and stored is TrigRoiDescriptor.
  // The actual data storage may be yet diferent of course. In the second case it is TrigRoiDescriptorCollection.
  // Here we have two cases, if the requested object is DataVector and the Physics flag is requested 
  // we need to do the additional filtering. We guess that this is the case sniffing the object T a bit with the has_traits template.
  // This filtering is done as follows. New container is created with in the VIEW_ELEMENTS mode and only selected objects are inserted into it.

  template<class T, class STORED,class LINK>
  struct insert_and_flatten<T,STORED, false, LINK> {
    static void do_it(std::vector<Trig::Feature<T> >& destination, const STORED* source, const HLT::TriggerElement* te, const std::string& label,
                      unsigned int condition, HLT::NavigationCore* navigation, const LINK& lnk) {

      const T* possibly_reduced_possibly_container = use_or_construct<T>(source, te, label, condition, navigation);
      destination.push_back(Trig::Feature<T>(possibly_reduced_possibly_container, te, label, possibly_reduced_possibly_container != source,lnk)); // by 2nd to the last arg == true tell the Feature<T> to delete container at deletion
    }
  };


  //
  template<class T, class CONT, class LINK>
  struct insert_and_flatten<T, CONT, true, LINK> {
    static void do_it(std::vector<Trig::Feature<T> >& destination, const CONT* source, const HLT::TriggerElement* te, const std::string& label, 
                      unsigned int condition, HLT::NavigationCore* navigation,const LINK& lnk) {	
      (void)lnk; //get rid of unused par warning

      //std::cout << "insert_and_flatten<true> " << label << " of container of size " << source->size() <<  std::endl;
      
      const TrigPassBits* bits(0);
      if ( condition == TrigDefs::Physics ) {// only passing objects
        //std::cout << "asking for bits for " << label << std::endl;
        bits =getBits(source->size(), te, label , navigation);
      }
      
      BOOST_FOREACH(const T* obj, *source) {	
        if ( bits==0 || HLT::isPassing(bits, obj, source)  ) {// if bits are missing or obj is realy marked as passing
          //std::cout << "Pushing back new feature with obj " << obj << std::endl;
	   destination.push_back(Trig::Feature<T>(obj, te, label,false,ElementLink<typename LINK::value_type>(obj,*source)));
        }
      }
    }
  };
  
  template<class LINK, bool is_container> struct print_features;

  template<class LINK> struct print_features<LINK,true>{
    typedef const typename LINK::value_type* ptr_type;
    static ptr_type get_ptr(const LINK& link){return link.cptr();}
    static void do_it(const LINK& link, bool do_flatten){
      //std::cout << "container at" << link.cptr() << " has size " << link.cptr()->size() << std::endl;;
      for(unsigned int j=0;j<link.cptr()->size();++j){
        //std::cout << "  ----element " << j << ": " << link.cptr()->at(j) << std::endl;
      }
      //std::cout << " .. flatten ? " << (do_flatten ? "yes" : "no") << std::endl;
    }
  };
  
  template<class LINK> struct print_features<LINK,false>{
    typedef typename LINK::ElementType ptr_type;
    static ptr_type get_ptr(const LINK& link){return *link;}
    static void do_it(const LINK& link,bool do_flatten){
      //std::cout << "link to element " << *link << std::endl;
      //std::cout << " .. flatten ? " << (do_flatten ? "yes" : "no") << std::endl;
    }
  };

  struct true_type{};  //different
  struct false_type{}; //types, so we can overload
  template <bool retrieve> struct get_type;
  template <> struct get_type<true>{typedef true_type type;};
  template <> struct get_type<false>{typedef false_type type;};
  
  template<class REQUESTED,class EDMLIST>
  struct get_links {
    get_links():m_result(), m_sourceTE(0){}//empty ctor but need to initialize reference member
    get_links( const HLT::TriggerElement* te, 
               std::vector<Trig::Feature<REQUESTED> >* data, 
               const std::string& label, unsigned int condition,
               const std::string& teName, 
               HLT::NavigationCore* navigation,
               bool* result, 
               const HLT::TriggerElement** sourceTE):
      m_te(te), m_data(data), m_label(label), m_condition(condition), m_teName(teName), m_navigation(navigation), m_result(result), m_sourceTE(sourceTE){}
    
    
    template<class FEATURE>
    void do_it() const {
      //std::cout << "getting links from navi for elemen in feature list: " << ClassID_traits<FEATURE>::typeName() << std::endl;
      //std::cout << "                     type originally  requested is: " << ClassID_traits<REQUESTED>::typeName() << std::endl;

      typedef typename Features2Container<FEATURE,EDMLIST>::type container_type;
      typedef typename Features2Object<FEATURE,EDMLIST>::type object_type;
      typedef typename Features2LinkHelper<FEATURE,container_type>::type link_type;

      const bool do_flatten  = (! boost::is_same<REQUESTED,container_type>::value) && boost::is_same<FEATURE,container_type>::value;
      const bool do_retrieve = ! (boost::is_same<REQUESTED,container_type>::value && boost::is_same<FEATURE,object_type>::value);
      // std::cout << "flatten? (case when requested type is element of feature type) : "  << (do_flatten ? "yes" : "no") << std::endl;
      // std::cout << "retrieve? (don't retrueve when requested type in container but stored type is element): " << (do_retrieve ? "yes" : "no") << std::endl;
      _do_it<FEATURE,do_flatten>(typename get_type<do_retrieve>::type());
    }

    template<class FEATURE,bool do_flatten>
    void _do_it(false_type dummy = false_type()) const {(void)dummy;/* do nothing */;}
    
    template<class FEATURE,bool do_flatten>
    void _do_it(true_type dummy = true_type()) const {
      (void)dummy; // turn of unused par warning

      //const HLT::TriggerElement* sourceTE(0);
      std::string sourceLabel;

      typedef typename Features2Container<FEATURE,EDMLIST>::type container_type;
      typedef typename Features2Object<FEATURE,EDMLIST>::type object_type;
      typedef typename Features2LinkHelper<FEATURE,container_type>::type link_type;
      
      link_type link;
      bool new_result = m_navigation->getRecentFeatureDataOrElementLink( m_te, link, m_label, *m_sourceTE, sourceLabel );
      // std::cout << "Luke, call to m_navigation->getRecentFeatureDataOrElementLink returned " << (new_result?"true":"false") << " and m_sourceTE = " << m_sourceTE << std::endl;

      if (new_result) {
        if (m_teName == "" || m_teName == Trig::getTEName(**m_sourceTE)) {
          //std::cout << "got links from navi " << link.cptr() << std::endl;
          if (link.cptr()) {   
            //std::cout << "actually we got a feature here" << std::endl;
            //print_features<link_type,boost::is_same<FEATURE,container_type>::value>::do_it(link,do_flatten);
            insert_and_flatten<REQUESTED,FEATURE, do_flatten,link_type>::do_it(*m_data,
                                                                               print_features<link_type,boost::is_same<FEATURE,container_type>::value>::get_ptr(link),
                                                                               *m_sourceTE, sourceLabel, m_condition, m_navigation,link);
          }
        }
      }
      *m_result = *m_result && new_result;
    }

    const HLT::TriggerElement* m_te;
    std::vector<Trig::Feature<REQUESTED> >* m_data;
    const std::string m_label;
    unsigned int m_condition;
    std::string m_teName;
    HLT::NavigationCore* m_navigation;
    bool* m_result;
    const HLT::TriggerElement** m_sourceTE;
  };

  /**
   * @brief actual feature acceess implementation
   * It has (thanks to the ClassTraits) functionality to flatten containers of containers.
   **/
  template<class T>
  void collect(const HLT::TriggerElement* te, std::vector<Trig::Feature<T> >& data, const std::string& label, unsigned int condition, 
               const std::string& teName, HLT::NavigationCore* navigation) {

    //std::cout << "Collecting " << label << " for TE " << te << std::endl;

    if (condition == TrigDefs::Physics && !te->getActiveState() ) return;    
    const HLT::TriggerElement* sourceTE(0);
    std::string sourceLabel;


    bool result = true;
#ifndef __GCCXML__
    typedef typename Features2Container<T>::type container_type;
    typedef typename Features2Object<T>::type object_type;
    typedef typename Object2Features<object_type>::type feature_list;
    get_links<T,TypeInfo_EDM> link_getter( te, &data, label, condition, teName, navigation, &result, &sourceTE);
    HLT::TypeInformation::for_each_type<feature_list,get_links<T,TypeInfo_EDM> >::do_it(&link_getter);
#endif

    if (result){/*do nothing anymore*/
    } else {
      // getRecentFeature returned false -> bifurcation?
      const std::vector<HLT::TriggerElement*> bif_tes = navigation->getDirectPredecessors(sourceTE);
      if ( bif_tes.size() <= 1 ) {
        return; // that means it is plain error (it will be printed by the Navigation)
      } else {
        // bifurcation point
        BOOST_FOREACH( const HLT::TriggerElement* predecesor_te, bif_tes ) 
          collect(predecesor_te, data, label, condition, teName, navigation); 
      }
    }
    //std::cout << "Size after collecting " << data.size() << std::endl;
  }
  

  template<>
  void collect<Muon_ROI>(const HLT::TriggerElement* te, std::vector<Trig::Feature<Muon_ROI> >& data, const std::string&, unsigned int, const std::string&, HLT::NavigationCore* navigation);

  template<>
  void collect<EmTau_ROI>(const HLT::TriggerElement* te, std::vector<Trig::Feature<EmTau_ROI> >& data, const std::string&, unsigned int, const std::string&, HLT::NavigationCore* navigation);

  template<>
  void collect<Jet_ROI>(const HLT::TriggerElement* te, std::vector<Trig::Feature<Jet_ROI> >& data, const std::string&, unsigned int, const std::string&, HLT::NavigationCore* navigation);

  template<>
  void collect<xAOD::EmTauRoI>(const HLT::TriggerElement* te, std::vector<Trig::Feature<xAOD::EmTauRoI> >& data, const std::string&, unsigned int, const std::string&, HLT::NavigationCore* navigation);

  template<>
  void collect<xAOD::MuonRoI>(const HLT::TriggerElement* te, std::vector<Trig::Feature<xAOD::MuonRoI> >& data, const std::string&, unsigned int, const std::string&, HLT::NavigationCore* navigation);

  template<>
  void collect<xAOD::JetRoI>(const HLT::TriggerElement* te, std::vector<Trig::Feature<xAOD::JetRoI> >& data, const std::string&, unsigned int, const std::string&, HLT::NavigationCore* navigation);

  // ==============
  //
  // This section deals with the TrigPassFlags creation
  //
  // ==============

  // access by container, stored as container
  template<class CONT> TrigPassFlags
  build_flags (const typename boost::enable_if_c<isDataVector<CONT>::value, CONT>::type *orig_cont, const CONT* cont, const TrigPassFlags * orig_tpf) {
    TrigPassFlags tpf(cont->size(), orig_tpf->flagSize());

    if(orig_cont->size() != orig_tpf->size()) {
      //std::cout << "WARNING: original constainer size (" << orig_cont->size() << ") different for size of TrigPassFlags (" << orig_tpf->size() << ")." << std::endl;
      return tpf;
    }

    unsigned int currentPos=0;
    BOOST_FOREACH(const typename CONT::base_value_type* obj, *cont) {
      typename CONT::const_iterator orig_obj = std::find(orig_cont->begin(),orig_cont->end(),obj);

      if(orig_obj == orig_cont->end()) {
        //std::cout << "WARNING: object in reduced container can' be found in original." << std::endl;
      } else {
        size_t idx = orig_obj-orig_cont->begin();
        tpf.setFlag(currentPos, orig_tpf->getFlag(idx));
      }
      currentPos++;
    }

    return tpf;
  }


  template<class T> TrigPassFlags
  build_flags (const typename boost::disable_if_c<isDataVector<T>::value, T>::type *orig, const T* feature, const TrigPassFlags * orig_tpf) {
    if(orig != feature) return TrigPassFlags(); // a problem TODO: print a ERROR

    TrigPassFlags tpf(1, orig_tpf->flagSize());
    tpf.setFlag(0,orig_tpf->getFlag(0));
    return tpf;
  }



  // access by single object, stored as container
  template<class T, class STORED> TrigPassFlags
  build_flags2(const STORED* orig_cont, const T* obj, const TrigPassFlags * orig_tpf)
  {
    if(orig_cont->size() != orig_tpf->size()) {
      //std::cout << "WARNING: original constainer size (" << orig_cont->size() << ") different for size of TrigPassFlags (" << orig_tpf->size() << ")." << std::endl;
      return TrigPassFlags();
    }

    TrigPassFlags tpf(1, orig_tpf->flagSize());

    typename STORED::const_iterator orig_obj = std::find(orig_cont->begin(),orig_cont->end(), obj);

    if(orig_obj == orig_cont->end()) {
      //std::cout << "WARNING: object in reduced container can' be found in original." << std::endl;
    } else {
      size_t idx = orig_obj-orig_cont->begin();
      tpf.setFlag(0, orig_tpf->getFlag(idx));
    }
    return tpf;
  }





  template<class T, class STORED, bool same> struct getFlagsHelper;

  // partial specialization for T==STORED
  template<class T, class STORED> struct getFlagsHelper<T, STORED, true> {
    static TrigPassFlags do_build(const STORED* orig_feat, const T* feat, const TrigPassFlags * orig_tpf) {
      return build_flags(orig_feat, feat, orig_tpf);
    }
  };


  // partial specialization for T!=STORED
  template<class T, class STORED> struct getFlagsHelper<T, STORED, false> {
    static TrigPassFlags do_build(const STORED* orig_feat, const T* feat, const TrigPassFlags * orig_tpf) {
      return build_flags2(orig_feat, feat, orig_tpf);
    }
  };





  template<class T> TrigPassFlags
  getFlags(const Trig::Feature<T>& f, const TrigPassFlags *orig_tpf, HLT::NavigationCore* navigation ) {

    typedef typename TrigDec::ClassTraits<T>::type STORED;

    const STORED* orig(0);
    const HLT::TriggerElement* sourceTE(0);
    std::string sourceLabel("");
    if (navigation->getRecentFeature(f.te(), orig, f.label(), sourceTE, sourceLabel)) {
      TrigPassFlags tpf = getFlagsHelper<T,STORED, boost::is_same<T,STORED>::value>::do_build(orig, f.cptr(), orig_tpf);
      return tpf;
    }
    return TrigPassFlags();

  }

  // specialization for types without getRecentFeature implementation
  template<> TrigPassFlags getFlags(const Trig::Feature<EmTau_ROI>& f, const TrigPassFlags *orig_tpf, HLT::NavigationCore* navigation );
  template<> TrigPassFlags getFlags(const Trig::Feature<Muon_ROI>& f, const TrigPassFlags *orig_tpf, HLT::NavigationCore* navigation );
  template<> TrigPassFlags getFlags(const Trig::Feature<Jet_ROI>& f, const TrigPassFlags *orig_tpf, HLT::NavigationCore* navigation );

} // EOF namespace




template<class T>
MsgStream& operator<< ( MsgStream& m, const Trig::Feature<T>& d ) {
  m << "TE id: " << d.te()->getId() << "obj ptr: " << d.cptr() 
    << " obj type: " << ClassID_traits<T>::typeName() << " label: " << d.label();
  return m;
}

#endif
