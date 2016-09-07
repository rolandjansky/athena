/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/**
 * @file safeDecorator.h
 * implementation file for function of same name
 * @author shaun roe
 * @date 20 July 2016
 */
#include "safeDecorator.h"
#include <cmath>
namespace IDPVM {
  // @name roughlyEqual overloaded functions
  // @{
  bool
  roughlyEqual(const int i, const int j) {
    return i == j;
  }

  bool
  roughlyEqual(const unsigned int i, const unsigned int j) {
    return i == j;
  }

  bool
  roughlyEqual(const float i, const float j) {
    const float epsilon = 0.001 * std::min(i, j);

    return(std::fabs(i - j) < epsilon);
  }
}
