/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "TrigNavigation/NavigationInit.h"
#include "TrigStorageDefinitions/EDM_TypeInfo.h"

//#include <iostream>

template<typename CONTAINER>
struct register_feature{
  template<class FEATURE>
  void do_it() const {
    //std::cout << "\t\t registering feature: " << typeid(FEATURE).name() <<  " -> ";
    //std::cout << "container: " << typeid(CONTAINER).name() << std::endl;
    
    HLT::RegisterFeatureContainerTypes<FEATURE,CONTAINER>::instan();
  }
};


template<class AUX>
struct register_aux{
  static void do_it() {
    HLT::RegisterAuxType<AUX>::instan();
  }
};


template<>
struct register_aux<HLT::TypeInformation::no_aux>{
  static void do_it() {}
};




struct registertype{
  template<class typeinfo_element>
  void do_it() const {
    typedef typename typeinfo_element::list_of_features FEATURES;
    typedef typename typeinfo_element::container CONTAINER;
    typedef typename typeinfo_element::aux AUX;

    //std::cout << "list of features for contaier " <<  typeid(CONTAINER).name() << " ";
    //HLT::TypeInformation::for_each_type<FEATURES,HLT::TypeInformation::simple_printer >::do_it();
    //std::cout<< std::endl;

    HLT::TypeInformation::for_each_type<FEATURES,register_feature<CONTAINER> >::do_it();
    register_aux<AUX>::do_it();
  }
};

//we want to exectute a function upon loading of a library (i.e. 'static'). since this is not possible
//per se, we take a detour via a default constructor. we declare a class with a static data member
//and initizalize that static member. This will call the default ctor of this member type during 
//which we can register all kinds of types to the navigation. This also uses the looping functionality
//of the typemap


template <typename TYPELIST>
struct a_default_ctor{
  a_default_ctor(){
    //std::cout << "registering package with navi, packae typelist has size: " << TYPELIST::last_index + 1 << std::endl;
    //we need to do this by hand unfortunately because we hit the template instantiaton depth of 99
    HLT::TypeInformation::for_each_type<typename TYPELIST::template range< 0, 9>::type,registertype>::do_it();
    HLT::TypeInformation::for_each_type<typename TYPELIST::template range<10,19>::type,registertype>::do_it();
    HLT::TypeInformation::for_each_type<typename TYPELIST::template range<20,29>::type,registertype>::do_it();
   }
};

template <typename TYPELIST>
struct a_class_that_calls_default_ctor{
  static a_default_ctor<TYPELIST> member;
};

#define REGISTER_PACKAGE_WITH_NAVI(name)\
template<> a_default_ctor<class_##name::map>\
a_class_that_calls_default_ctor<class_##name::map>::member = \
a_default_ctor<class_##name::map>();
