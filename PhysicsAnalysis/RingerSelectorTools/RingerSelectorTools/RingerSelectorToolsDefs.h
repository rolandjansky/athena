/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

// $Id: RingerSelectorToolsDefs.h 704615 2015-10-29 18:50:12Z wsfreund $
#ifndef RINGERSELECTORTOOLS_RINGERSELECTORTOOLSDEFS_H
#define RINGERSELECTORTOOLS_RINGERSELECTORTOOLSDEFS_H


#include "RingerSelectorTools/RingerSelectorToolsEnums.h"


/**
 * @brief Namespace dedicated for Ringer utilities
 **/
namespace Ringer {


/**
 * Return number of segments for segment of type @name e.
 **/
unsigned numberOfSegments(const SegmentationType e);

/**
 * Return Ringer enumeration of type T identifying string type:
 **/
template<typename T>
T getType(const char* cStr);

/**
 * Specialization for SegmentationType
 **/
template<>
SegmentationType getType(const char* cStr);

/**
 * Specialization for EtaDependency
 **/
template<>
EtaDependency getType(const char* cStr);

/**
 * Specialization for EtDependency
 **/
template<>
EtDependency getType(const char* cStr);

/**
 * Specialization for Requirement
 **/
template<>
Requirement getType(const char* cStr);

/**
 * Transform enumeration types to string
 **/
const char* toStr(SegmentationType e);

/**
 * Transform enumeration types to string
 **/
const char* toStr(EtaDependency e);

/**
 * Transform enumeration types to string
 **/
const char* toStr(EtDependency e);

/**
 * Transform enumeration types to string
 **/
const char* toStr(Requirement e);

} // namespace Ringer

#endif // RINGERSELECTORTOOLS_RINGERSELECTORTOOLSDEFS_H

