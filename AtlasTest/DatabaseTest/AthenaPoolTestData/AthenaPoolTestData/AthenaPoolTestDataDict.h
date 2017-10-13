/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/**
 * @file AthenaPoolTestDataDict.h
 * @brief This file includes header files which will be used by the 
 * lcgdict pattern to create SEAL dictionaries.
 */ 
/***************************************************************************
 Athena Pool Test Data package
 -----------------------------------------
 ***************************************************************************/

//<doc><file>	$Id: AthenaPoolTestDataDict.h,v 1.18 2009-01-08 16:23:51 schaffer Exp $
//<version>	$Name: not supported by cvs2svn $

#ifndef ATHENAPOOLTESTDATA_ATHENAPOOLTESTDATADICT_H
# define ATHENAPOOLTESTDATA_ATHENAPOOLTESTDATADICT_H

//<<<<<< INCLUDES                                                       >>>>>>

#include "AthenaPoolTestData/IAthenaPoolTestData.h"
#include "AthenaPoolTestData/AthenaPoolTestA.h"
#include "AthenaPoolTestData/AthenaPoolTestB.h"
#include "AthenaPoolTestData/AthenaPoolTestC.h"
#include "AthenaPoolTestData/AthenaPoolTestD.h"
#include "AthenaPoolTestData/IAthenaPoolTestCollection.h"
#include "AthenaPoolTestData/AthenaPoolTestMatrix.h"
#include "AthenaPoolTestData/AthenaPoolTestMap.h"
#include "AthenaPoolTestData/AthenaPoolTestMapData.h"
#include "AthenaPoolTestData/TrigPath.h"
#include "AthenaPoolTestData/FauxTriggerMap.h"
#include "AthenaPoolTestData/dummy_A.h"
#include "AthenaPoolTestData/dummy_E.h"

//<<<<<< PUBLIC DEFINES                                                 >>>>>>
//<<<<<< PUBLIC CONSTANTS                                               >>>>>>
//<<<<<< PUBLIC TYPES                                                   >>>>>>
//<<<<<< PUBLIC VARIABLES                                               >>>>>>
//<<<<<< PUBLIC FUNCTIONS                                               >>>>>>
//<<<<<< CLASS DECLARATIONS                                             >>>>>>

// Must add in some instatiated objects to get them into the
// dictionary
namespace AthenaPoolTestDataDict 
{
    std::map<unsigned int, CLHEP::HepMatrix> AthenaPoolDataDummyMap;
    std::pair< AthenaPoolTestMapDataLink, float > Deposit; 
}


//<<<<<< INLINE PUBLIC FUNCTIONS                                        >>>>>>
//<<<<<< INLINE MEMBER FUNCTIONS                                        >>>>>>

#endif // ATHENAPOOLTESTDATA_ATHENAPOOLTESTDATADICT_H
