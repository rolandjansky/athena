/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/**
 * @file AthenaPoolTestMap.h 
 * 
 * @brief
 * 
 * @author RD Schaffer <R.D.Schaffer@cern.ch>
 * 
 * $Id: AthenaPoolTestMap.h,v 1.1 2005-02-04 16:12:19 schaffer Exp $
 */

#ifndef ATHENAPOOLTESTDATA_ATHENAPOOLTESTMAP_H
# define ATHENAPOOLTESTDATA_ATHENAPOOLTESTMAP_H

//<<<<<< INCLUDES                                                       >>>>>>
#include "AthenaKernel/CLASS_DEF.h"
#include "AthenaPoolTestData/AthenaPoolTestMapData.h"
#include <map>
//<<<<<< CLASS DECLARATIONS                                             >>>>>>

/**
 * @class AthenaPoolTestMap 
 * 
 * @brief
 * 
 */
class AthenaPoolTestMap : public std::map<unsigned int, AthenaPoolTestMapData> {
 public:
  AthenaPoolTestMap();
  virtual ~AthenaPoolTestMap();
};
CLASS_DEF(AthenaPoolTestMap, 153882669, 1)



//<<<<<< INLINE PUBLIC FUNCTIONS                                        >>>>>>
//<<<<<< INLINE MEMBER FUNCTIONS                                        >>>>>>

#endif // ATHENAPOOLTESTDATA_ATHENAPOOLTESTMAP_H
