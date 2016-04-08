/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: Types.h 667905 2015-05-18 19:07:55Z wsfreund $
#ifndef  RINGERSELECTORTOOLS_TOOLS_PROCEDURES_TYPES_H
#define  RINGERSELECTORTOOLS_TOOLS_PROCEDURES_TYPES_H

// STL includes:
#include <string>
#include <stdexcept>

/**
 *
 * WARNING: Do not change the order of the enumerations for backward files
 * compatibility!!
 *
 * This means that if a new value is needed, add it to the end of the
 * enumeration, instead adding it to any place in the enumeration.
 *
 * Every time a Ringer Procedure is created, it is needed to add it in this
 * file: this is necessary to IO. Otherwise it won't even compile.
 *
 * After adding a enumeration type, also add it to the methods in the
 * Types.cxx file.
 *
 **/

namespace Ringer {

namespace PreProcessing {
namespace Type {

enum PreProcessorTypes {
  IPreProcessorVarDep,
  IPreProcessor,
  Norm1,
  Norm2,
  Sqrt,
  ConstantValue,
  Sequential,
  Spherization,
  MinMax
};
} // namespace Type
} // namespace PreProcessing

namespace Discrimination {
namespace Type {

enum DiscriminatorTypes {
  IDiscriminatorVarDep,
  IDiscriminator,
  NNFeedForward
};

enum ThresholdTypes {
  IThresholdVarDep,
  IThreshold,
  UniqueThreshold
};
} // namespace Type
} // namespace Discrimination

// We are at Ringer namespace again

typedef PreProcessing::Type::PreProcessorTypes preProcEnum_t;
typedef Discrimination::Type::DiscriminatorTypes discrEnum_t;
typedef Discrimination::Type::ThresholdTypes thresEnum_t;

/**
 * Return Ringer enumeration of type T identifying string type:
 **/
template<typename T>
T getType(const char* cStr);

/**
 * Specialization for PreProcessorTypes:
 **/
template<>
preProcEnum_t getType(const char* cStr);

/**
 * Specialization for DiscriminatorTypes:
 **/
template<>
discrEnum_t getType(const char* cStr);

/**
 * Specialization for ThresholdTypes:
 **/
template<>
thresEnum_t getType(const char* cStr);

/**
 * Transform enumeration types to string
 **/
const char* toStr(preProcEnum_t e);

/**
 * Transform enumeration types to string
 **/
const char* toStr(discrEnum_t e);

/**
 * Transform enumeration types to string
 **/
const char* toStr(thresEnum_t t);

} // namespace Ringer

#endif // RINGERSELECTORTOOLS_TOOLS_PROCEDURES_TYPES_H

