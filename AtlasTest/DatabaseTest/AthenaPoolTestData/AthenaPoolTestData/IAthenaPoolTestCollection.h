/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/**
 * @file IAthenaPoolTestCollection.h
 * @brief Class definition for IAthenaPoolTestCollection
 */
/**
 * @class IAthenaPoolTestCollection
 * @brief Interface class for AthenaPoolTest i/o tests with DataVector
 * @author RD Schaffer
 * Created May 2004 for DC2 validation
 */
/***************************************************************************
 Athena Pool Test Data package
 -----------------------------------------
 ***************************************************************************/

//<doc><file>	$Id: IAthenaPoolTestCollection.h,v 1.2 2005-01-07 23:24:57 cranshaw Exp $
//<version>	$Name: not supported by cvs2svn $

#ifndef ATHENAPOOLTESTDATA_IATHENAPOOLTESTCOLLECTION_H
# define ATHENAPOOLTESTDATA_IATHENAPOOLTESTCOLLECTION_H

//<<<<<< INCLUDES                                                       >>>>>>

#include "AthenaPoolTestData/IAthenaPoolTestData.h"
#include "AthContainers/DataVector.h"
#include "AthenaKernel/CLASS_DEF.h"

//<<<<<< PUBLIC DEFINES                                                 >>>>>>
//<<<<<< PUBLIC CONSTANTS                                               >>>>>>
//<<<<<< PUBLIC TYPES                                                   >>>>>>
//<<<<<< PUBLIC VARIABLES                                               >>>>>>
//<<<<<< PUBLIC FUNCTIONS                                               >>>>>>
//<<<<<< CLASS DECLARATIONS                                             >>>>>>

typedef DataVector< IAthenaPoolTestData >    IAthenaPoolTestCollection ;

CLASS_DEF(IAthenaPoolTestCollection, 1269286659, 0)


//<<<<<< INLINE PUBLIC FUNCTIONS                                        >>>>>>
//<<<<<< INLINE MEMBER FUNCTIONS                                        >>>>>>

#endif // ATHENAPOOLTESTDATA_IATHENAPOOLTESTCOLLECTION_H
