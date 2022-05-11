// This file's extension implies that it's C, but it's really -*- C++ -*-.
/*
 * Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration.
 */
/**
 * @file CxxUtils/no_bitwise_op_warning.h
 * @author scott snyder <snyder@bnl.gov>
 * @date Apr, 2022
 * @brief Disable clang bitwise operation warning.
 *
 * Including this header will disable the clang warning:
 *
 * warning: use of bitwise '&' with boolean operands [-Wbitwise-instead-of-logical]
 *
 * which we get from some ROOT headers.
 */


#ifndef CXXUTILS_NO_BITWISE_OP_WARNING_H
#define CXXUTILS_NO_BITWISE_OP_WARNING_H


#if defined(__clang__) && __clang_major__ >= 14
# pragma clang diagnostic ignored "-Wbitwise-instead-of-logical"
#endif


#endif // not CXXUTILS_NO_BITWISE_OP_WARNING_H
