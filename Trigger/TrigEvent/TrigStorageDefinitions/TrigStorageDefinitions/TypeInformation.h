/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRIGNAVIGATION_TYPEINFORMATION_H
#define TRIGNAVIGATION_TYPEINFORMATION_H
#include <string>
#include <iostream>
#include <vector>
#include <boost/type_traits.hpp>
#include <typeinfo>

#include <boost/function_types/function_type.hpp>
#include <boost/function_types/parameter_types.hpp>
#include <boost/function_types/function_arity.hpp>
#include <boost/typeof/std/utility.hpp>
#include <boost/type_traits/remove_pointer.hpp>
//#include "DataModel/ElementLink.h"


namespace HLT{
namespace TypeInformation{
// a basic structue of the information that we want to store about the trigger EDM
// this includes the lead (element) type
// the type with which such features are stored within the navigation
// the container type that is saved to SG

// we want to know when a list ends (similar to how we terminate const char arrays with a "\0" terminatino sequence)
struct nil{
static const int last_index=-1;
};

// this implements a generic linked list that makes up an array
// from each element we can go the the next one by the rest typedef
// we also define two 'methods' add and join that add a single element
// and concatenates the present list with a new list respectively

/*  MN: ROOT6 Clang is not able to parse definitions with get_list_index<>
template <class type,class list_of_types> struct list;

template<class a_list> struct get_list_index {static const int result = 1+ get_list_index<typename a_list::rest>::result;};
template<class first_element> struct get_list_index<list<first_element,nil> > {static const int result = 0;};
*/

template<class type,class list_of_types>
struct list {
  static const int last_index = 1 + list_of_types::last_index;
  //static const int last_index = get_list_index<list<type,list_of_types> >::result;
  typedef type first;
  typedef list_of_types rest;
  typedef list<type,list_of_types> done;

  template <class new_element>    struct add;
  template <class new_element>    struct addWithChecking;
  template <class new_list>       struct join;
  template <class element>        struct has;
  template <int i>                struct get;
  template <int i, class element> struct set;
  template <int begin, int end>   struct range;
};

// here we implement the list::add 'method' it takes a new element and defines
// the output of the do_add helper method as its result. do_add adds the element
// *except* when the new element is nil, in which case it doesn't do anything
// this is important for cancatenating two lists (as each one will have its terminating nil)
template<class new_element,class list,bool is_nil = boost::is_same<new_element,nil>::value > struct do_add;

//nil case
template<class new_element,class thelist>
struct do_add<new_element,thelist,true>{
  typedef thelist result;
};

//general case
template<class new_element,class thelist>
struct do_add<new_element,thelist,false>{
  typedef list<new_element,thelist> result;
};

//here if we already have the container/object combination in the map, we just want to add the feature
//otherwise we will want to add the feature of the new combination to the existing record


template <class type, class list_of_types>
template <class new_element>
struct list<type,list_of_types>::add {
  typedef typename do_add<new_element,list<type,list_of_types> >::result go;
};

// here we defined an iterator on the list. calling nx<list> is equivalent to list::rest and defines
// nx<list>::type as list::first. Thus, one can iterate via calling the nx template multiple times, e.g.
// nx<nx<nx<list> > >
template<class list>
struct nx : list::rest {//for structs this is public inheritance by default
  typedef typename list::first type;
};

// here we have a implementation of random access to the i-th element. given an index this iterates on
// the list a given number of types using the nx<> iterator
template<class list, int i>
struct _at{
  typedef nx<typename _at<list,i-1>::it> it;
  typedef typename it::type type;
};

template<class list>
struct _at<list,0> {
  typedef nx<list> it;
  typedef typename it::type type;
};

template<class list, int i>
struct at{
  typedef typename _at<list,list::last_index-i>::type type;
};

// in case we just want to know if a specific element in already in a list we can use
// this search function that iterates over the list and returns a bool with the answer
#ifdef __COVERITY__
template<typename element, typename list>
struct do_search{
    static const bool found = boost::is_same<element,typename list::first>::value or
    do_search<element,typename list::rest>::found;
};

//if list is specified as terminated with 'nil'
template<class element>
struct do_search<element,nil>{
    static const bool found =false;
};
#else
template<typename element, typename list, int i = list::last_index>
struct do_search{
  static const bool found = boost::is_same<element,typename at<list,i>::type>::value ||
                            do_search<element,list,i-1>::found;
};

template<typename element, typename list>
struct do_search<element,list,0>{
  static const bool found = boost::is_same<element,typename at<list,0>::type>::value;
};
#endif

template <class type, class list_of_types>
template <class element>
struct list<type,list_of_types>::has {
  static const bool result = do_search<element,list<type,list_of_types> >::found;
};

template <class the_type, class list_of_types>
template <int i>
struct list<the_type,list_of_types>::get {
  typedef typename at<list<the_type,list_of_types>,i>::type type;
};

template<class thelist, int begin, int end, bool begin_is_end = (begin==end)>
struct do_range{
  typedef typename at<thelist,end>::type last_item;
  typedef typename do_range<thelist,begin,end-1>::type earlier_list;
  typedef typename earlier_list::template add<last_item>::go::done type;  
};

template<class thelist, int begin, int end>
struct do_range<thelist,begin,end,true>{
  typedef typename at<thelist,end>::type last_item;
  typedef list<last_item,nil> type;  
};

template <class the_type, class list_of_types>
template <int begin, int end>
struct list<the_type,list_of_types>::range {
  typedef typename do_range<list<the_type,list_of_types>,
                            (list<the_type,list_of_types>::last_index < begin) ? 
                            list<the_type,list_of_types>::last_index : begin,
                            (list<the_type,list_of_types>::last_index < end) ? 
                            list<the_type,list_of_types>::last_index : end
                            >::type type;
};

template<class new_element, class old_element, bool do_replace> struct replace_if;
template<class new_element, class old_element> struct replace_if<new_element,old_element,true>{typedef new_element result;};
template<class new_element, class old_element> struct replace_if<new_element,old_element,false>{typedef old_element result;};

template<class a_list,class new_element,int i, bool last_is_nil = boost::is_same<typename a_list::rest,nil>::value> struct do_update;

template<class a_list,class  new_element,int i>
struct do_update<a_list,new_element,i,false>{
  typedef list<typename replace_if<new_element,typename a_list::first,a_list::last_index == i>::result,
               typename do_update<typename a_list::rest,new_element,i>::result> result;
};

template<class a_list,class  new_element,int i>
struct do_update<a_list,new_element,i,true>{
  typedef list<typename replace_if<new_element,typename a_list::first,a_list::last_index == i>::result,
               nil> result;
};

template <class the_type, class list_of_types>
template <int i, class new_element>
struct list<the_type,list_of_types>::set {
  typedef typename do_update<list<the_type,list_of_types>,new_element,i>::result go;
};


// here we're implementing the join 'method'. it defines the output of the helper function join_lists as its
// output named 'go'. Thus, we can join a given list 'listA' with another list 'listB' using listA::join<listB>::go
// this supports also method chaining a la listA::join<listB>::go::join<listC>::go::join<listD>::go etc.
template<class first, class second, int index = first::last_index>
struct join_lists{
  typedef typename join_lists<first,second,index-1>::joined::template add<typename at<first,index>::type>::go::done joined;
};

template<class first, class second>
struct join_lists<first,second,0>{
  typedef typename second::template add<typename at<first,0>::type>::go::done joined;
};

template<class type,class list_of_types>
template<class new_list>
struct list<type,list_of_types>::join{
  typedef typename join_lists<new_list,list<type,list_of_types> >::joined go;
};
    

// now we're actually constructing a function that allows us to search our linked list in one 'key'
// of the type_info struct (let's say the object). This will give us the index of the type_info entry
// that contains that 'key'. Once we have this index, we can query that element for any other 'column'
// that would consitute the 'value'.. i.e. search by object and return feature... 
// or seach by feature the container.. mix and match as you like
// template to test


// this is a helper function for iterating over the list (as it need to be done recusively)
// if the tested index is in fact the element we're after we set the result to that index
// if not we just leave the result untouched and set the current value as the result again
// this way we're kinda passing the 'state' of the search along in recursive calls
template<bool is_same, int index, int value> struct test;

template<int index, int value> struct test<true,index,value>{
typedef int output_type;
static const int result = index;
};

template<int index, int value> struct test<false,index,value>{
typedef int output_type;
static const int result = value;
};

struct no_aux{};

template<typename _objt,typename _list_of_feats, typename _cont, typename _aux = no_aux>
struct type_info {
  typedef _objt object;
  typedef _list_of_feats list_of_features;
  typedef _cont container;
  typedef _aux aux;
};

//the actual find function. we test each element to see if the desired type is equal to that returned by the functor that was also 
//passed in the function call
template<class element, class list, template<class E,class L,int I> class by, int index = list::last_index, int fall_back = -1> struct find{
static const int result = test<by<element,list,index>::result,
                               index,
                               find<element,list,by,index-1,fall_back>::result>::result; 
};

template<class element, class list, template<class E,class L,int I> class by, int fall_back> struct find<element,list,by,0,fall_back>{
static const int result = test<by<element,list,0>::result,
                               0,
                               fall_back>::result; 
};


// here come four functors that get the key out of an element, we can pass these functors
// to the find function so it know's by what to search

template<typename element,typename list,int index> struct get_element{
  static const bool result = boost::is_same<element,typename at<list,index>::type>::value;
};
template<typename element,typename list,int index> struct get_cont{
  static const bool result = boost::is_same<element,typename at<list,index>::type::container>::value;
};

template<typename element,typename list,int index> struct get_objt{
  static const bool result = boost::is_same<element,typename at<list,index>::type::object>::value;
};

template<typename element,typename list,int index> struct get_aux{
  static const bool result = boost::is_same<element,typename at<list,index>::type::auxt>::value;
};

template<typename element,typename list,int index> struct get_feat{
  static const bool result = at<list,index>::type::list_of_features::template has<element>::result || 
                             boost::is_same<element,typename at<list,index>::type::object>::value;
};



//this is a shorthand for finding the index and then using the index to get the element with that index
template<typename element,typename list, template<class E,class L,int I> class by, int index = find<element,list,by>::result>
struct map_search {
  typedef typename at<list,index>::type type;
};

template<class T>
struct ERROR_THE_FOLLOWING_TYPE_IS_NOT_KNOWN_TO_THE_EDM{};

template<typename element,typename list, template<class E,class L,int I> class by>
struct map_search<element,list,by,-1> {
  typedef ERROR_THE_FOLLOWING_TYPE_IS_NOT_KNOWN_TO_THE_EDM<element> type;
};

struct newlist{
  template <class first_element>struct add{typedef list<first_element,nil> go;};
  template <class first_element>struct addWithChecking{typedef list<first_element,nil> go;};
};

//type_info case

template<class new_element,class thelist, int index> struct do_my_add;
 
template<class O,class F,class C,class A,class thelist>
struct do_my_add<type_info<O,F,C,A>,thelist,-1>{
  typedef typename thelist::template add<type_info<O,F,C,A> >::go result;
};

template<class O,class F,class C,class A,class thelist, int index>
struct do_my_add<type_info<O,F,C,A>,thelist,index>{
  typedef typename at<thelist,index>::type old_entry;
  typedef typename old_entry::list_of_features old_feats;
  typedef typename type_info<O,F,C,A>::list_of_features new_feats;
  typedef typename old_feats::template join<new_feats>::go extended;
  typedef typename thelist::template set<index,type_info<typename old_entry::object,extended,typename old_entry::container,typename old_entry::aux> >::go result;
};

#ifdef __COVERITY__
template <class type, class list_of_types>
template<class newElement>
struct list<type,list_of_types>::addWithChecking{
    typedef typename do_my_add<newElement,list<type,list_of_types>,find<typename newElement::container,list<type,list_of_types>,get_cont>::result>::result go;
};
#else
template <class type, class list_of_types>
template<class O,class F,class C,class A>
struct list<type,list_of_types>::addWithChecking<type_info<O,F,C,A> >{
  typedef typename do_my_add<type_info<O,F,C,A>,list<type,list_of_types>,find<typename type_info<O,F,C,A>::container,list<type,list_of_types>,get_cont>::result>::result go;
};
#endif

//AND WE'RE DONE

////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////

//compile-time iteration
//parameters:
//     list: list to iterate on
//  functor: a meta function that should be applied to each item in the list
//           it itself has three parameters (the fourth is for internal use only
//           first: element of the list
//          second: result structure that can be used to keep track of things through the loop
//           third: arguments that may want to be passed into the functor
//   result: the result structe to be used
//      arg: the arguments to be passed to the inner body of the loop
 template<typename list, template<typename,typename,typename, bool> class functor, typename result, class arg, int i = list::last_index, bool isLast = true>
struct for_each_type_c{
  typedef typename functor<typename at<list,i>::type, typename for_each_type_c<list,functor,result,arg,i-1,false>::type,arg, isLast>::type type;
};

 template<typename list, template<typename,typename,typename,bool> class functor,typename result, class arg,bool isLast>
struct for_each_type_c<list,functor,result,arg,0,isLast>{
  typedef typename functor<typename at<list,0>::type,result,arg,false>::type type;
};

//run-time iteration
template <class list,class functor, int last_index = list::last_index>
struct for_each_type{
  static void do_it(const functor* f = 0) {
    //transient object to use in cas no object given, must have default ctor
    functor transient = functor();
    const functor* ptr = (!f) ? &transient : f;

    for_each_type<list,functor,last_index-1>::do_it(ptr);

    // apparently must use function pointer to get all the types resolved properly
    void (functor::*funcptr) () const = &functor::template do_it<typename at<list,last_index>::type>;
    (ptr->*funcptr)();
  };
};

template <class list,class functor>
struct for_each_type<list,functor,0>{
  static void do_it(const functor* f = 0) {
    //apparently must use function pointer to get all the types resolved properly
    functor transient = functor();
    const functor* ptr = (!f) ? &transient : f;

    void (functor::*funcptr) () const = &functor::template do_it<typename at<list,0>::type>;
    (ptr->*funcptr)();
  };
};

struct simple_printer{
  template <class element>
  void do_it() const {std::cout << "["<< typeid(element).name() << "] ";}
};

struct printer{
  template <class element>
  void do_it() const;
};

template <class element>
void printer::do_it() const {
  std::cout << " object ["<< typeid(typename element::object).name() << "] ";
  std::cout << " features [";
  HLT::TypeInformation::for_each_type<typename  element::list_of_features,simple_printer>::do_it();
  std::cout << "]";
  std::cout << " container ["<< typeid(typename element::container).name() << "] ";
  std::cout << std::endl;
}

}//end_of_namespace TypeInformation
}//end_of_namespace HLT

#endif
