// This file's extension implies that it's C, but it's really -*- C++ -*-.
/*
 * Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration.
 */
/**
 * @file AthenaKernel/TopBase.h
 * @author scott snyder <snyder@bnl.gov>
 * @date Jan, 2018
 * @brief Calculate topmost accessible base accessible via SG_BASES.
 */


#ifndef ATHENAKERNEL_TOPBASE_H
#define ATHENAKERNEL_TOPBASE_H


#include "AthenaKernel/BaseInfo.h"
#include <type_traits>


namespace SG {


/**
 * @brief Calculate topmost base accessible via SG_BASES.
 *
 * For example, if we have
 *
 *@code
 *   SG_BASES(C2, C1);
 *   SG_BASES(C3, C2);
 @endcode
 *
 * then TopBase<C1>::type, TopBase<C2>::type, and TopBase<C3>::type all yield @c C1.
 */
template <class T>
struct TopBase
{
  typedef typename SG::Bases<T>::bases::Base1 Base1;
  static const bool has_base = !std::is_same<Base1, SG::NoBase>::value;
  typedef typename std::conditional<has_base,
                                    typename TopBase<Base1>::type,
                                    T>::type type;
};


template <>
struct TopBase<SG::NoBase>
{
  typedef SG::NoBase type;
};


} // namespace SG


#endif // not ATHENAKERNEL_TOPBASE_H
