// This file's extension implies that it's C, but it's really -*- C++ -*-.
/*
 * Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration.
 */
/**
 * @file CxxUtils/no_sizeof_pointer_div_warning.h
 * @author scott snyder <snyder@bnl.gov>
 * @date Apr, 2022
 * Including this header will disable the clang14 warning:
 *
 * warning: warning: 'sizeof (val)' will return the size of the pointer, not the array itself [-Wsizeof-pointer-div]
 *
 * which we get from some ROOT TVirtualQConnection.h header.
 */


#ifndef CXXUTILS_NO_SIZEOF_POINTER_DIV_WARNING_H
#define CXXUTILS_NO_SIZEOF_POINTER_DIV_WARNING_H


#if defined(__clang__) && __clang_major__ >= 14
# pragma clang diagnostic ignored "-Wsizeof-pointer-div"
#endif



#endif // not CXXUTILS_NO_SIZEOF_POINTER_DIV_WARNING_H
