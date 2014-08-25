/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRIGSTORAGEDEF_TYPEINFO_METHODS
#define TRIGSTORAGEDEF_TYPEINFO_METHODS
//#include "TrigStorageDefinitions/EDM_MergePackages.h"
struct TypeInfo_EDM;

#include "TrigStorageDefinitions/EDM_MasterSearch.h"
#include <boost/mpl/assert.hpp>

template<class T, class EDMLIST = TypeInfo_EDM> struct IsKnownFeature{
  typedef typename master_search<typename EDMLIST::map,
    HLT::TypeInformation::get_feat,T>::result::search_result search_result;
  static const bool value = !boost::is_same<HLT::TypeInformation::ERROR_THE_FOLLOWING_TYPE_IS_NOT_KNOWN_TO_THE_EDM<T>,search_result>::value;
  BOOST_MPL_ASSERT_MSG(value,the_following_class_is_not_a_valid_feature_contact_trigger_core_sw_experts_thanks, (T));
};

template<class CONTAINER, class EDMLIST = TypeInfo_EDM> struct Container2Object{
  typedef typename master_search<typename EDMLIST::map,
                        HLT::TypeInformation::get_cont,
                        CONTAINER
				   >::result::search_result::object type;
};

template<class CONTAINER, class EDMLIST = TypeInfo_EDM> struct Container2Aux{
  typedef typename master_search<typename EDMLIST::map,
                        HLT::TypeInformation::get_cont,
                        CONTAINER
				   >::result::search_result::aux type;
};

template<class OBJECT, class EDMLIST = TypeInfo_EDM> struct Object2Container{
  typedef typename master_search<typename EDMLIST::map,
                        HLT::TypeInformation::get_objt,
                        OBJECT
				   >::result::search_result::container type;
};

template<class FEATURE, class EDMLIST = TypeInfo_EDM> struct Features2Container{
  typedef typename master_search<typename EDMLIST::map,
                        HLT::TypeInformation::get_feat,
                        FEATURE
		       >::result::search_result::container type;
};

template<class FEATURE, class EDMLIST = TypeInfo_EDM> struct Features2Object{
  typedef typename master_search<typename EDMLIST::map,
                        HLT::TypeInformation::get_feat,
                        FEATURE
		       >::result::search_result::object type;
};

template<class OBJECT, class EDMLIST = TypeInfo_EDM> struct Object2Features{
  typedef typename master_search<typename EDMLIST::map,
                        HLT::TypeInformation::get_objt,
                        OBJECT
		       >::result::search_result::list_of_features type;
};

template <class CONTAINER, bool isCont> struct lnk_helper;

template <class CONTAINER> struct lnk_helper<CONTAINER,false>{
  typedef ElementLink<CONTAINER> type;
};

template <class CONTAINER> struct lnk_helper<CONTAINER,true>{
  typedef DataLink<CONTAINER> type;
};

template<class REQUESTED,class CONTAINER> struct Features2LinkHelper{
  static const bool isCont = boost::is_same<REQUESTED,CONTAINER>::value;
  typedef typename lnk_helper<CONTAINER,isCont>::type type;
};


#endif
