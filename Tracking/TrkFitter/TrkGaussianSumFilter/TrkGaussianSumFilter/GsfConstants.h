/*
   Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
 */

/**
 * @author Anthony Morley, Christos Anastopoulos
 * @brief  Collect constants we use
 * in GSF and their meaning in one place
 */
#ifndef GSFCONSTANTS_H
#define GSFCONSTANTS_H

#include <cstdint>
#include <cstddef>
namespace GSFConstants {

/**
 * Note the Gaussian sum approach as describe
 * e.g in " Optimal Filtering" Anderson and Moore
 * "Track Fitting with non-Gaussan noise" Fruhwirth
 *
 * The state is described by N Gaussian components
 * The Beth Heitler Material effect are also described
 * by M components
 * Which futher can involve polynomial parametetrization
 * with C coeffiencts (look Readme under Data folder)
 *
 * Each step we have a N x M convolution
 * And then a reduction back to N
 *
 * The max numbers for N , M  are enforced in configuration.
 * It is an error to configure for more.
 *
 * They lead to a max allowed NXM after convolution.
 * Trying to somehow pass (by-passing the config)
 * leads to an exception.
 *
 * Furthermore, the  number of coefficients is also
 * constrained in configuration.
 *
 * So here is a list of these constants all in one
 * place.
 */

/// Maximum number of Gaussian components for the
/// state description.
constexpr int8_t maxNumberofStateComponents = 12;
/// Maximum number of Gaussian components for the
/// Bethe Heitler description
constexpr int8_t maxNumberofBHComponents = 6;
/// Polynomail coefficients , order is this -1
constexpr int8_t polynomialCoefficients = 6;

/**
 * The maximum size State x Bethe-Heitler components
 * The typical number we use is the max 6x12 = 72 i.e as
 * we try to have the maximum practical precision for the GSF.
 */
constexpr int8_t maxComponentsAfterConvolution =
  maxNumberofBHComponents * maxNumberofStateComponents;

/**
 * @brief Alignment used  for SIMD operations
 * internally to GSF.
 */
constexpr size_t alignment = 32;
}
#endif
