/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/** 
 * @file ${name}SerCnv.h
 * @brief Generated header file which defines a typedef for templated converter class
 * @author Jiri Masik <Jiri.Masik@cern.ch>
 * a copy of poolcnv by RD Schaffer <R.D. Schaffer@cern.ch>
 * $Id: ExampleClassSerCnv.h,v 1.1.1.1 2008-05-22 21:33:31 masik Exp $
 */

#ifndef ${name}SerCnv_H
#define ${name}SerCnv_H

#include "TrigSerializeCnvSvc/TrigSerializeConverter.h"
#include "${package}/${name}.h"

typedef TrigSerializeConverter<${type}> ${libtag}${name}SerCnv;

#endif
