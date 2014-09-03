/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef ATHENAKERNEL_TOOLS_TYPE_TOOLS_H
#define ATHENAKERNEL_TOOLS_TYPE_TOOLS_H
#ifndef BOOST_CONFIG_HPP
 #include <boost/config.hpp>
#endif
#ifndef BOOST_TT_OBJECT_TRAITS_HPP_INLCUDED
 #include <boost/type_traits/object_traits.hpp>
#endif
#ifndef BOOST_TT_TRANSFORM_TRAITS_HPP_INCLUDED
 #include <boost/type_traits/transform_traits.hpp>
#endif
#ifndef SELECT_TYPE_DWA20010206_HPP
 #include <boost/detail/select_type.hpp>
#endif

namespace type_tools {
  ///assign a type to an integer value
  template <int v>
  struct Int2Type { 
    enum { value = v };
  };

  typedef Int2Type<1> true_tag; 
  typedef Int2Type<0> false_tag;


  /**
   * @class Parameter
   * @brief an algorithm to provide an efficient way to pass a parameter:
   * by value for scalars (arithmetic types and pointers),
   * by reference for compound objects.Derived from Loki library
   * @author Paolo Calafiura
   *
   */

  template <typename T>
  struct Parameter {
  private:
    typedef typename boost::add_reference<T>::type TRef;
    typedef const TRef const_TRef;
    BOOST_STATIC_CONSTANT(bool, isScalar = boost::is_scalar<T>::value);
  public:
    typedef typename boost::detail::if_true<(isScalar)>::template
    then<
      T,
      //else
      TRef
      >::type ref_type;

    typedef typename boost::detail::if_true<(isScalar)>::template
    then<
      T,
      //else
      const T&
      //FIXME const_TRef
      >::type const_type;
      
    typedef typename boost::detail::if_true<(isScalar)>::template
    then<
      T,
      //else
      T*
      >::type ptr_type;

    typedef const_type type;
  };


  /**
   * @class Copy
   * @brief an algorithm to define a suitable "copy type": 
   * by default use copy-by-value
   * @author Paolo Calafiura
   *
   */
  template <typename T>
  struct Copy {
    typedef T type;
    typedef const T& const_reference;
    typedef const T* const_pointer;
  };

  /// when T is pointer we "copy" into a const T* 
  template <typename T>
  struct Copy<T*> {
    typedef const T* type;
    typedef const T* const_reference;
    typedef const T* const* const_pointer;
  }; 
}
#endif

