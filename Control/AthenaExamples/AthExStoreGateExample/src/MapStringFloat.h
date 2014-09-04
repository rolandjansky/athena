/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/***************************************************************************
 a map of float keyed by string
 ------------------------------
 ATLAS Collaboration
 ***************************************************************************/

// $Id: MapStringFloat.h,v 1.4 2008-04-04 21:24:21 binet Exp $

#ifndef MAPSTRINGFLOAT_H
# define MAPSTRINGFLOAT_H

//<<<<<< INCLUDES                                                       >>>>>>

// CLIDs
#include "SGTools/StlMapClids.h"

typedef std::map<std::string,float> MapStringFloat;

//describe the container category to ElementLink
#include "AthLinks/DeclareIndexingPolicy.h"
CONTAINER_IS_MAP( MapStringFloat )


#endif // MAPSTRINGFLOAT_H
