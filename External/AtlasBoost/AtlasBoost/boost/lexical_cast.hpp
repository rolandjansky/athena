#ifndef ATLAS_BOOST_LEXICAL_CAST_HPP
#define ATLAS_BOOST_LEXICAL_CAST_HPP

#if defined(__GNUC__)
# ifndef __clang__
#  pragma GCC diagnostic push
#  pragma GCC diagnostic ignored "-Wmaybe-uninitialized"
# endif
# pragma GCC system_header
#endif

#include_next "boost/lexical_cast.hpp"

#if defined(__GNUC__) && !defined(__clang__)
#pragma GCC diagnostic pop
#endif

#endif
