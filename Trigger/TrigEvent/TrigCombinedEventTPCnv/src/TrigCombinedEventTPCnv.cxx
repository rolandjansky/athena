/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// generate the T/P converter entries
#include "AthenaKernel/TPCnvFactory.h"
	
#include "TrigCombinedEventTPCnv/TrigComposite_p1.h"
#include "TrigCombinedEventTPCnv/TrigCompositeCnv_p1.h"

#include "TrigCombinedEventTPCnv/TrigCompositeContainer_p1.h"
#include "TrigCombinedEventTPCnv/TrigCompositeContainerCnv_p1.h"
	
DECLARE_TPCNV_FACTORY(TrigCompositeCnv_p1,
		      TrigComposite,
		      TrigComposite_p1,
		      Athena::TPCnvVers::Current)
  
DECLARE_TPCNV_FACTORY(TrigCompositeContainerCnv_p1,
		      TrigCompositeContainer,
		      TrigCompositeContainer_p1,
		      Athena::TPCnvVers::Current)
