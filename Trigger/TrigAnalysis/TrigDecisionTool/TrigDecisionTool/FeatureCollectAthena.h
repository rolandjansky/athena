/*
  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
*/

#ifndef XAOD_ANALYSIS // Full Athena only

// -*- c++ -*-
#ifndef TRIGGER_DECISION_TOOL_FeatureCollectAthena_H
#define TRIGGER_DECISION_TOOL_FeatureCollectAthena_H


/**********************************************************************************
 * @Project:
 * @Package: TrigDecision
 *
 *
 * @author Nicolas Berger  <Nicolas.Berger@cern.ch>  - LAPP Annecy
 * @author Tomasz Bold     <tomasz.bold@cern.ch>     - UC Irvine, AGH-UST Krakow
 * @author Lukas Heinrich  <lukas.heinrich@cern.ch>  - NYU
 *
 ***********************************************************************************/

#include <string>
#include <set>
#include "boost/type_traits/is_same.hpp"
#include "boost/shared_ptr.hpp"
#include "boost/lexical_cast.hpp"

#include "AthContainers/ConstDataVector.h"

#include "TrigNavigation/NavigationCore.h"
#include "TrigNavigation/NavigationCore.icc"
#include "TrigDecisionTool/Conditions.h"
#include "TrigDecisionTool/TDTUtilities.h"
#include "TrigDecisionTool/ClassTraits.h"

#include "TrigSteeringEvent/TrigPassBits.h"
#include "TrigSteeringEvent/TrigPassFlags.h"

// for template specializations

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

// for smart type information

#include "TrigStorageDefinitions/EDM_TypeInfo.h"


#include "TrigDecisionTool/Feature.h"

namespace Trig {

  /**
   * This is the implementation of the Feature collection for Athena
   **/
  
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
        auto destination = new ConstDataVector<T>(SG::VIEW_ELEMENTS);

        for(const typename T::base_value_type *obj : *source) {
          if ( HLT::isPassing(bits, obj, source)  ) // if bits are missing or obj is realy marked as passing
            destination->push_back(obj);
        }
        return destination->asDataVector();
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
			unsigned int condition, HLT::NavigationCore* navigation,const LINK& /*lnk*/) {	

	//std::cout << "insert_and_flatten<true> " << label << " of container of size " << source->size() <<  std::endl;
      
	const TrigPassBits* bits(0);
	if ( condition == TrigDefs::Physics ) {// only passing objects
	  //std::cout << "asking for bits for " << label << std::endl;
	  bits =getBits(source->size(), te, label , navigation);
	}
      
	for(const T* obj : *source) {	
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
      static void do_it(const LINK& link, bool /*do_flatten*/){
	//std::cout << "container at" << link.cptr() << " has size " << link.cptr()->size() << std::endl;
	for(unsigned int j=0;j<link.cptr()->size();++j){
	  //std::cout << "  ----element " << j << ": " << link.cptr()->at(j) << std::endl;
	}
	//std::cout << " .. flatten ? " << (do_flatten ? "yes" : "no") << std::endl;
      }
    };
  
    template<class LINK> struct print_features<LINK,false>{
      typedef typename LINK::ElementType ptr_type;
      static ptr_type get_ptr(const LINK& link){return *link;}
      static void do_it(const LINK& /*link*/,bool /*do_flatten*/){
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
      get_links():m_te(nullptr),
		  m_data(nullptr),
		  m_condition(0),
		  m_navigation(nullptr),
		  m_result(),
		  m_sourceTE(0)
      {}//empty ctor but need to initialize reference member
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
	//std::cout << "TrigDecisionTool::Feature::get_links: getting links from navi for element in feature list: " << ClassID_traits<FEATURE>::typeName() << std::endl;
	//std::cout << "TrigDecisionTool::Feature::get_links:                      type originally  requested is: " << ClassID_traits<REQUESTED>::typeName() << std::endl;

	typedef typename Features2Container<FEATURE,EDMLIST>::type container_type;
	typedef typename Features2Object<FEATURE,EDMLIST>::type object_type;

	const bool do_flatten  = (! boost::is_same<REQUESTED,container_type>::value) && boost::is_same<FEATURE,container_type>::value;
	const bool do_retrieve = ! (boost::is_same<REQUESTED,container_type>::value && boost::is_same<FEATURE,object_type>::value);
	// std::cout << "flatten? (case when requested type is element of feature type) : "  << (do_flatten ? "yes" : "no") << std::endl;
	// std::cout << "retrieve? (don't retrueve when requested type in container but stored type is element): " << (do_retrieve ? "yes" : "no") << std::endl;
	_do_it<FEATURE,do_flatten>(typename get_type<do_retrieve>::type());
      }

      template<class FEATURE,bool do_flatten>
      void _do_it(false_type dummy = false_type()) const {(void)dummy;/* do nothing */;}
    
      template<class FEATURE,bool do_flatten>
      void _do_it(true_type /*dummy*/ = true_type()) const {

	//const HLT::TriggerElement* sourceTE(0);
	std::string sourceLabel;

	typedef typename Features2Container<FEATURE,EDMLIST>::type container_type;
	//typedef typename Features2Object<FEATURE,EDMLIST>::type object_type;
	typedef typename Features2LinkHelper<FEATURE,container_type>::type link_type;

	//std::cout << "TrigDecisionTool::Feature::get_links: link_type is: " << typeid(link_type).name() << std::endl;


      
	link_type link;
	bool new_result = m_navigation->getRecentFeatureDataOrElementLink( m_te, link, m_label, *m_sourceTE, sourceLabel );

	if (new_result) {
	  if (m_teName == "" || m_teName == Trig::getTEName(**m_sourceTE)) {
	    if (link.cptr()) {   
	      //std::cout << "TrigDecisionTool::Feature::get_links: actually we got a feature here" << std::endl;
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
		 const std::string& teName, HLT::TrigNavStructure* navstructure) {

      auto navigation = dynamic_cast<HLT::NavigationCore*>(navstructure);

      //std::cout << "Collecting " << label << " for TE " << te << std::endl;

      if (condition == TrigDefs::Physics && !te->getActiveState() ) return;    
      const HLT::TriggerElement* sourceTE(0);
      std::string sourceLabel;


      bool result = true;
#ifndef __GCCXML__
      //typedef typename Features2Container<T>::type container_type;
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
	  for( const HLT::TriggerElement* predecesor_te : bif_tes ) 
	    collect(predecesor_te, data, label, condition, teName, navigation); 
	}
      }
      //std::cout << "Size after collecting " << data.size() << std::endl;
    }
  

    template<>
    void collect<Muon_ROI>(const HLT::TriggerElement* te, std::vector<Trig::Feature<Muon_ROI> >& data, const std::string&, unsigned int, const std::string&, HLT::TrigNavStructure* navigation);

    template<>
    void collect<EmTau_ROI>(const HLT::TriggerElement* te, std::vector<Trig::Feature<EmTau_ROI> >& data, const std::string&, unsigned int, const std::string&, HLT::TrigNavStructure* navigation);

    template<>
    void collect<Jet_ROI>(const HLT::TriggerElement* te, std::vector<Trig::Feature<Jet_ROI> >& data, const std::string&, unsigned int, const std::string&, HLT::TrigNavStructure* navigation);

    template<>
    void collect<xAOD::EmTauRoI>(const HLT::TriggerElement* te, std::vector<Trig::Feature<xAOD::EmTauRoI> >& data, const std::string&, unsigned int, const std::string&, HLT::TrigNavStructure* navigation);

    template<>
    void collect<xAOD::MuonRoI>(const HLT::TriggerElement* te, std::vector<Trig::Feature<xAOD::MuonRoI> >& data, const std::string&, unsigned int, const std::string&, HLT::TrigNavStructure* navigation);

    template<>
    void collect<xAOD::JetRoI>(const HLT::TriggerElement* te, std::vector<Trig::Feature<xAOD::JetRoI> >& data, const std::string&, unsigned int, const std::string&, HLT::TrigNavStructure* navigation);

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
      for(const typename CONT::base_value_type* obj : *cont) {
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

  } // EOF namespace FeatureAccessImpl

} // EOF namespace Trig

#endif


#endif //XAOD_ANALYSIS
