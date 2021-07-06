/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

/**
 * @file LATOMEMapping.h
 * @author Gen Tateno
 * @date June 2019
 * @brief Tool to get LATOME SC and SCID mapping file and fill std::map variable with it.
 */

#ifndef LATOMEMAPPING_H
#define LATOMEMAPPING_H
#include <map>
#include "LArByteStream/LArLATOMEDecoder.h"

/** @brief Tool to get LATOME SC and SCID mapping file and fill std::map variable with it.*/
class LATOMEMapping
{
public:
  static void fill(std::map<int, HWIdentifier> *toFill, std::string inputfile);
};

#endif // LATOMEMAPPING_H

