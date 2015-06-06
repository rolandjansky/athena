#ifndef ATLAS_BOOST_ANY_HPP
#define ATLAS_BOOST_ANY_HPP

#if defined(__GNUC__)
# if !defined(__clang__)
#  pragma GCC diagnostic push
#  pragma GCC diagnostic ignored "-Wmaybe-uninitialized"
# endif
# pragma GCC system_header
#endif

#include_next "boost/any.hpp"

#if defined(__GNUC__) && !defined(__clang__)
#pragma GCC diagnostic pop
#endif

#endif
