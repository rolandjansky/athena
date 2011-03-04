/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "BFieldAthTest/MagFAthenaTest.h"
#include "BFieldAthTest/MagFAthenaTest01.h"
#include "BFieldAthTest/MagFAthenaTest02.h"
#include "BFieldAthTest/MagFAthenaValidation.h"
#include "GaudiKernel/DeclareFactoryEntries.h"
 
DECLARE_ALGORITHM_FACTORY( MagFAthenaTest )
DECLARE_ALGORITHM_FACTORY( MagFAthenaTest01 )
DECLARE_ALGORITHM_FACTORY( MagFAthenaTest02 )
DECLARE_ALGORITHM_FACTORY( MagFAthenaValidation )
DECLARE_FACTORY_ENTRIES( BFieldAthTest ) {
	DECLARE_ALGORITHM( MagFAthenaTest );
	DECLARE_ALGORITHM( MagFAthenaTest01 );
	DECLARE_ALGORITHM( MagFAthenaTest02 );
	DECLARE_ALGORITHM( MagFAthenaValidation );
}
 
