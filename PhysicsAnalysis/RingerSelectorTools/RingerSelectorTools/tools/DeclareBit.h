/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: DeclareBit.h 693573 2015-09-07 19:15:49Z wsfreund $
#ifndef RINGER_STANDALONE

#ifndef RINGERSELECTORTOOLS_TOOLS_DECLARE_BIT_H
#define RINGERSELECTORTOOLS_TOOLS_DECLARE_BIT_H

#include <utility>

#include "RingerSelectorTools/tools/cxx/StaticAssert.h"
#include "RingerSelectorTools/tools/cxx/stringify.h"

/**
 * @brief Static counter.
 *
 * Made by David Krauss, used with author concent. 
 *
 * Counts up to 64, taken from http://stackoverflow.com/a/6174263/1162884
 **/
/// @{
#define COUNTER_READ_CRUMB( TAG, RANK, ACC )                                   \
  counter_crumb( TAG(), constant_index< RANK >(), constant_index< ACC >() )

#define COUNTER_READ( TAG )                                                    \
  COUNTER_READ_CRUMB( TAG, 1,                                                  \
  COUNTER_READ_CRUMB( TAG, 2,                                                  \
  COUNTER_READ_CRUMB( TAG, 4,                                                  \
  COUNTER_READ_CRUMB( TAG, 8,                                                  \
  COUNTER_READ_CRUMB( TAG, 16,                                                 \
  COUNTER_READ_CRUMB( TAG, 32,                                                 \
  COUNTER_READ_CRUMB( TAG, 64, 0 ) ) ) ) ) ) )
 
/**
 * Increment tagged macro
 **/
#define COUNTER_INC( TAG )                                                     \
constant_index< COUNTER_READ( TAG ) + 1 >                                      \
constexpr counter_crumb( TAG,                                                  \
  constant_index< ( COUNTER_READ( TAG ) + 1 ) & ~ COUNTER_READ( TAG ) >,       \
  constant_index< ( COUNTER_READ( TAG ) + 1 ) & COUNTER_READ( TAG ) > )        \
{                                                                              \
  return {};                                                                   \
}

template< std::size_t n >
struct constant_index : std::integral_constant< std::size_t, n > {};

// =============================================================================
template< typename id, std::size_t rank, std::size_t acc >
constexpr constant_index< acc > counter_crumb( id,
    constant_index< rank >,
    constant_index< acc > )
{
  return {};
} /* found by ADL via constant_index */

/**
 * Define counter which will be used by counter_read and counter_inc macros
 **/
#define DEFINE_COUNTER( counter )                                             \
struct counter {};
 
/**
 * Makes counter_crumb fcn available at the namespace.
 **/
#define COUNTER_LINK_NAMESPACE using ::counter_crumb;
/// @}

/**
 * This macro is used to declare a TAccept bit with name and decription.
 * The fourth parameter is a static counter defined using above macros.
 * Make sure to make the namespace linked with the counter fcn by using
 * COUNTER_LINK_NAMESPACE inside the namespace.
 **/
#define DECLARE_BIT( bitmask, name, description, bit_counter)                 \
  const int Ringer::bitmask::name =                                           \
    Ringer::bitmask::declareBit(                                              \
      #name,                                                                  \
      description                                                             \
    );                                                                        \
                                                                              \
  RINGER_STATIC_ASSERT( ( COUNTER_READ( bit_counter ) <= 32),                 \
      "Bit " STRINGIFY(name) " couldn't be added to bitmask "                 \
      STRINGIFY(bitmask) ": " STRINGIFY(bitmask)                              \
      " ran out of bits!!");                                                  \
                                                                              \
  COUNTER_INC( bit_counter )
  
#endif // RINGERSELECTORTOOLS_TOOLS_DECLARE_BIT_H

#endif // RINGER_STANDALONE
