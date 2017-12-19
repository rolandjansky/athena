// This file's extension implies that it's C, but it's really -*- C++ -*-.
/*
 * Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration.
 */
// $Id$
/**
 * @file CaloDetDescr/CaloConstIteratorAdaptor.h
 * @author scott snyder <snyder@bnl.gov>
 * @date Oct, 2017
 * @brief Tranform iterator over pointers to iterator over const pointers.
 */


#ifndef CALODETDESCR_CALOCONSTITERATORADAPTOR_H
#define CALODETDESCR_CALOCONSTITERATORADAPTOR_H


#include "boost/iterator/iterator_adaptor.hpp"
#include <iterator>
#include <type_traits>


// Helper: The value type of ITERATOR should be a pointer.
// Give the type as a const pointer.
template <class ITERATOR>
struct CaloClassIteratorValueType
{
  typedef typename std::iterator_traits<ITERATOR>::value_type orig_t;
  typedef typename std::remove_pointer<orig_t>::type base_t;
  typedef const base_t* type;
};


/**
 * @brief Tranform iterator over pointers to iterator over const pointers.
 *
 * If @c ITERATOR is an iterator over @c T*, then
 * @c CaloConstIteratorAdaptor<ITERATOR> is a const iterator over @c const T*.
 */
template <class ITERATOR>
class CaloConstIteratorAdaptor
  : public boost::iterator_adaptor<CaloConstIteratorAdaptor<ITERATOR>,
                                   ITERATOR,
                                   typename CaloClassIteratorValueType<ITERATOR>::type,
                                   typename ITERATOR::iterator_category,
                                   typename CaloClassIteratorValueType<ITERATOR>::type>
{
public:
  CaloConstIteratorAdaptor() {}
  CaloConstIteratorAdaptor (ITERATOR it)
    : CaloConstIteratorAdaptor::iterator_adaptor_ (it) {}
    
private:
  friend class boost::iterator_core_access;
};


#endif // not CALODETDESCR_CALOCONSTITERATORADAPTOR_H
