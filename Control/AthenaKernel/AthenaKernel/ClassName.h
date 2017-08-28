// This file's extension implies that it's C, but it's really -*- C++ -*-.

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: ClassName.h,v 1.1 2009-01-15 19:07:29 binet Exp $
/**
 * @file  AthenaKernel/ClassName.h
 * @author scott snyder
 * @date Jul 2005
 * @brief An interface for getting the name of a class as a string.
 */


#ifndef ATHENAKERNEL_CLASSNAME_H
#define ATHENAKERNEL_CLASSNAME_H


#include <string>


/**
 * @brief An interface for getting the name of a class as a string.
 *
 * This template class provides an interface for getting the name
 * of a class as a string.  By default, it uses @c typeinfoName
 * from @c GaudiKernel, but it may be specialized to override
 * the behavior for specific classes.
 */
template <class T>
class ClassName
{
public:
  /**
   * @brief Return the name of class @c T as a string.
   */
  static std::string name()
#ifdef __GNUC__
    // Force this function to appear as a symbol in the output file,
    // even in an optimized build where it's always inlined.
    // Otherwise, we get complaints from cling that it can't find the symbol
    // (as of root 6.04).
    __attribute__ ((used))
#endif
    ;
};


#include "AthenaKernel/ClassName.icc"


#endif // not ATHENAKERNEL_CLASSNAME_H
