/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/** 
 * @file ${name}Cnv.h
 * @brief Generated header file which defines a typedef for templated converter class
 * @author RD Schaffer <R.D. Schaffer@cern.ch>
 */

#ifndef ${name}Cnv_H
#define ${name}Cnv_H

#include "AthenaPoolCnvSvc/T_AthenaPoolCnv.h"
#include "${package}/${name}.h"

typedef T_AthenaPoolCnv<${type}> ${cnv_pfx}${name}Cnv;

#endif
