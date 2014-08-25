/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRIGSTORAGEDEF_MASTERSEARCH
#define TRIGSTORAGEDEF_MASTERSEARCH
#include "TrigStorageDefinitions/EDM_TypeInformation.h"

//this is a struct that allows us to treat constant integral expressions (such as bools)
//as types
template <typename type, type the_value> struct value_type{
  static const type value = the_value;
};


//since due to the functional nature of template programming we need to pass all results/arguments 
// a la "by reference" (what in normal C++ would be void func(arg1& a1, arg2* a2) we are creating 
// overall holder structes that hold all arguments and all results that we want from the loop
// in a sense this is similar to how main() takes an argv parameter..

//our result structure holds a list of bools (implemented as value type) that indicate in which 
//package list the quesried type was found and a seach_result that gives back the type_info row
//if the element was found in any of the packages or a special NOT_KNOWN_IN_EDM type
template<typename new_list = HLT::TypeInformation::newlist, typename _search_result = HLT::TypeInformation::nil>
struct my_result{
  typedef new_list list;
  typedef _search_result search_result;
};


//this is our argument structure that hold two arguments:
//1) a template meta-function (functor) that hold the search method with which we query a typemap
//2) a type for which we want to seach for
template<template<class E, class L, int I> class _functor, typename _type> struct my_arg{
  
  template<class E,class L, int I> struct functor{
    static const bool result  =  _functor<E,L,I>::result;
  };

  typedef _type type;
};


template<typename oldresult,typename newresult, bool replace> struct get_result;
template<typename oldresult,typename newresult> struct  get_result<oldresult,newresult,true>{
  typedef newresult result;
};
template<typename oldresult,typename newresult> struct  get_result<oldresult,newresult,false>{
  typedef oldresult result;
};

//a result handler that we can use to emit a 'nice' error message
template<typename queried, typename result, typename T> struct result_handler{
  typedef result type;
};

template<typename queried, typename result> struct result_handler<queried, result, HLT::TypeInformation::nil>{
  typedef my_result<typename result::list,HLT::TypeInformation::ERROR_THE_FOLLOWING_TYPE_IS_NOT_KNOWN_TO_THE_EDM<queried> > type;
};

//this is the meta-function that we want to exectue on each package typemap
template<typename element, typename last_result, typename arg, bool isLast>
struct my_functor{

  typedef typename
  HLT::TypeInformation::map_search<typename arg::type,
				   typename element::map,
				   arg::template functor
				   >::type result_type;
  
  typedef HLT::TypeInformation::ERROR_THE_FOLLOWING_TYPE_IS_NOT_KNOWN_TO_THE_EDM<typename arg::type> unknown_object;
 
  static const bool query = !boost::is_same<unknown_object,result_type>::value;
  typedef typename last_result::list::template add<value_type<bool,query> >::go::done added;

  //if we did not find the result we just keep our current result and do not replace it
  typedef typename get_result<typename last_result::search_result,result_type,query>::result result;



  typedef typename result_handler<typename arg::type,my_result<added,result>,typename my_result<added,result>::search_result>::type type;
};

//this structure acutally implements the loop over packages
template<typename MASTER_EDM, template <class,class,int> class functor, typename which> struct master_search{
  typedef typename HLT::TypeInformation::for_each_type_c<
    MASTER_EDM,
    my_functor,
    my_result<>,
    my_arg<functor,which> >::type result;
};


#endif
