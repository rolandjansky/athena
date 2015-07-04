/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef CXX_FEATURES_CONFIG_H
# define CXX_FEATURES_CONFIG_H

# if !HAVE_NAMESPACE_STD
#  define std
# endif

#if HAVE_CXX_STDC_HEADERS
# define STDC std
#else
# define STDC
#endif

# if !HAVE_BOOL
#  ifndef bool
    typedef int bool;
#  endif
#  ifndef true
#   define true 1
#  endif
#  ifndef false
#   define false 0
#  endif
# endif

# if !HAVE_TYPENAME
#  define typename
# endif

# if !HAVE_EXPLICIT
#  define explicit
# endif

# if !HAVE_RESTRICT
#  if HAVE_PRIVATE_RESTRICT
#   define restrict __restrict
#  else
#   define restrict
#  endif
# endif

# if HAVE_NEW_STYLE_CASTS
#  define STATIC_CAST(type,expr)	static_cast<type> (expr)
#  define CONST_CAST(type,expr)		const_cast<type> (expr)
#  define REINTERPRET_CAST(type,expr)	reinterpret_cast<type> (expr)
# else
#  define STATIC_CAST(type,expr)	((type)(expr))
#  define CONST_CAST(type,expr)		((type)(expr))
#  define REINTERPRET_CAST(type,expr)	((type)(expr))
# endif

# if HAVE_DYNAMIC_CAST && HAVE_TYPEID 
#  define HAVE_FUNCTIONAL_RTTI 1
# endif

# if HAVE_TEMPLATE_DEFAULT_ARGS
#  define CXX_T_DEFAULT(x) , x
#  define CXX_T_ONDEFAULT(def,nodef) def
# else
#  define CXX_T_DEFAULT(x)
#  define CXX_T_ONDEFAULT(def,nodef) nodef
# endif

# if HAVE_TEMPLATE_NULL_ARGS
#  define TEMPLATE_NULL_ARGS <>
# else
#  define TEMPLATE_NULL_ARGS
# endif

# if HAVE_TEMPLATE_NULL_SPEC
#  define TEMPLATE_NULL_SPEC template<>
# else
#  define TEMPLATE_NULL_SPEC
# endif

/* # undef CXX_FUNCTION */
# if HAVE_PRETTY_FUNCTION
#  define CXX_FUNCTION		__PRETTY_FUNCTION__
# elif HAVE_FUNCTION
#  define CXX_FUNCTION		__FUNCTION__
# elif HAVE_FUNC
#  define CXX_FUNCTION		__func__
# endif

# if HAVE_ITERATOR
#  define ITERATOR_BASE(cat,val,diff)	: public std::iterator<std::cat##_tag,val,diff>
# elif HAVE_ITERATOR_TRAITS
#  define ITERATOR_BASE(cat,val,diff)
# else
#  define ITERATOR_BASE(cat,val,diff)	: public std::cat<val,diff>
# endif

# if HAVE_REVERSE_ITERATOR_STYLE == 1
#  define REVERSE_ITERATOR(tname,tag,iter,value,reference,pointer,difference) \
	std::reverse_iterator<iter>
# elif HAVE_REVERSE_ITERATOR_STYLE == 2
#  define REVERSE_ITERATOR(tname,tag,iter,value,reference,pointer,difference) \
	std::reverse_iterator<iter,std::tag,value,reference,pointer,difference>
# elif HAVE_REVERSE_ITERATOR_STYLE == 3
#  define REVERSE_ITERATOR(tname,tag,iter,value,reference,pointer,difference) \
	std::tname<iter,value,reference,pointer,difference>
# elif HAVE_REVERSE_ITERATOR_STYLE == 4
#  define REVERSE_ITERATOR(tname,tag,iter,value,reference,pointer,difference) \
	std::os_##tname<iter,value,reference,pointer,difference>
# else
#  define REVERSE_ITERATOR(tname,tag,iter,value,reference,pointer,difference) \
	I_don_t_know_how_to_define_reverse_iterator
# endif

# if HAVE_BROKEN_TEMPLATE_RESCOPE
#  define TEMPLATE_RESCOPE(scope)
# else
#  define TEMPLATE_RESCOPE(scope)	typename scope::
# endif

# ifdef _MSC_VER
#  pragma warning(disable:4786)		// '255' characters in the debug information
#  pragma warning(disable:4244)		// conversion from double to float
#  pragma warning(disable:4305)		// conversion from double to float (5.0)
# endif //_MSC_VER

#endif /* CXX_FEATURES_CONFIG_H */
