/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// generate the T/P converter entries
#include "AthenaKernel/TPCnvFactory.h"
#include "AthenaKernel/ITPCnvBase.h"

#include <vector>
#include "EventBookkeeperTPCnv/EventBookkeeper_p1.h"
#include "EventBookkeeperTPCnv/EventBookkeeperCollection_p1.h"
#include "EventBookkeeperTPCnv/EventBookkeeperCnv_p1.h"
#include "EventBookkeeperTPCnv/EventBookkeeperCollectionCnv_p1.h"

#include "EventBookkeeperTPCnv/SkimDecision_p1.h"
#include "EventBookkeeperTPCnv/SkimDecisionCollection_p1.h"
#include "EventBookkeeperTPCnv/SkimDecisionCnv_p1.h"
#include "EventBookkeeperTPCnv/SkimDecisionCollectionCnv_p1.h"

DECLARE_TPCNV_FACTORY(EventBookkeeperCnv_p1,
                      EventBookkeeper,
                      EventBookkeeper_p1,
                      Athena::TPCnvVers::Current)
                      
DECLARE_NAMED_TPCNV_FACTORY(EventBookkeeperCollectionCnv_p1,
                            EventBookkeeperCollectionCnv_p1,
                            EventBookkeeperCollection,
                            EventBookkeeperCollection_p1,
                            Athena::TPCnvVers::Current)
                      
DECLARE_TPCNV_FACTORY(SkimDecisionCnv_p1,
                      SkimDecision,
                      SkimDecision_p1,
                      Athena::TPCnvVers::Current)
                      
DECLARE_NAMED_TPCNV_FACTORY(SkimDecisionCollectionCnv_p1,
                            SkimDecisionCollectionCnv_p1,
                            SkimDecisionCollection,
                            SkimDecisionCollection_p1,
                            Athena::TPCnvVers::Current)
                      
