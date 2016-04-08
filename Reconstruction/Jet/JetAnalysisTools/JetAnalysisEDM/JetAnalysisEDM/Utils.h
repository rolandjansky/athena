// Dear emacs, this is -*- c++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: CodeGenerator_v2_constants.h 510583 2012-07-18 09:12:16Z krasznaa $
#ifndef JETANALYSISEDM_UTILS_H
#define JETANALYSISEDM_UTILS_H

// ROOT include(s):
#include <TString.h>

namespace JetAnalysisEDM {

  struct Session {
    static bool isInteractive(){return s_interactive;}

    static void setInteractive(bool i){s_interactive=i;}
  protected:
    static bool s_interactive;
  };

  /// A set of template utilities mimicking some c++11 features
  namespace tpl {

    

    template <typename T> struct remove_ref { typedef T type; };
    template <typename T> struct remove_ref<T&> { typedef T type; };

    /// remove_pointer<T*>::type is T, remove_pointer<T>::type is T 
    template <typename T> struct remove_pointer { typedef T type; };
    template <typename T> struct remove_pointer<T*> { typedef T type; };
    template <typename T> struct remove_pointer<T*const> { typedef T type; };
    template <typename T> struct remove_pointer<T*volatile> { typedef T type; };
    template <typename T> struct remove_pointer<T*const volatile> {
      typedef T type; };

    template <typename T> struct remove_const { typedef T type; };
    template <typename T> struct remove_const<const T*> { typedef T* type; };
    template <typename T> struct remove_const<const T&> { typedef T& type; };


    /// Translate a type in to a ('const ref' or 'const pointer') type
    ///  to_const_ref<T>::type is a "pointer to a const" if T is a pointer, else it is a "ref to a const"
    ///    to_const_ref<T*>::type == const T* 
    ///    to_const_ref<T>::type  == const T&  (if T is not a pointer) 
    template <typename T> struct to_const_ref     { typedef const T& type; };
    template <typename T> struct to_const_ref<T&> { typedef const T& type; };
    template <typename T> struct to_const_ref<T*> { typedef const T* type; };

    /// Render a type const if it is a ref or a pointer 
    ///  const_if_ref_or_ptr<T>::type is a "pointer/ref to a const" if T is a pointer/ref else T
    ///    const_if_ref_or_ptr<T*>::type == const T* 
    ///    const_if_ref_or_ptr<T&>::type  == const T& 
    template <typename T> struct const_if_ref_or_ptr { typedef T type; };
    template <typename T> struct const_if_ref_or_ptr<T&> { typedef const T & type; };
    template <typename T> struct const_if_ref_or_ptr<T*> { typedef const T*  type; };
    
    /// is_pointer<T>::value is a true or false according to T
    template <class T>  struct is_pointer { enum { value = false }; };
    template <class T>  struct is_pointer<T*> { enum { value = true }; };
    /// is_ref<T>::value is a true or false according to T
    template <class T>  struct is_ref { enum { value = false }; };
    template <class T>  struct is_ref<T&> { enum { value = true }; };

    /// Select a type or another : conditional< Test, T1, T2>::type is T1 if 'Test' is true else T2
    template <bool condition, class T1, class T2>    struct conditional { };
    template <class T1, class T2>  struct conditional<true, T1, T2>  { typedef T1 type;};
    template <class T1, class T2>  struct conditional<false, T1, T2> { typedef T2 type;};


    

    /// Define a readable typename for ('const ref' or 'plain type') type
    ///   const_if_ref<T>::type  == T (if T is not a reference)
    ///   const_if_ref<T&>::type  == const T& 
    template<typename T> struct const_if_ref{ typedef T type; };
    template<typename T> struct const_if_ref<T&> { typedef const T & type; };

 

  }

   /// Function creating a human-readable elapsed time printout
   ::TString TimeToString( ::Double_t secs );

   /// Function for printing data sizes in human-readable format
   ::TString SizeToString( ::Long64_t bytes );

   /// Function for printing data processing speeds in a human-readable format
   ::TString SpeedToString( ::Double_t bytespersec );



} // namespace JetAnalysisEDM

#endif // JETANALYSISEDM_UTILS_H
