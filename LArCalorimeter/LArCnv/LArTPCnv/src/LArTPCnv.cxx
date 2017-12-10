/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// generate the T/P converter entries
#include "AthenaKernel/TPCnvFactory.h"


#include "LArTPCnv/LArDigitContainer_p1.h"

#include "LArTPCnv/LArRawChannel_p1.h" 
#include "LArTPCnv/LArRawChannel_p2.h" 
#include "LArTPCnv/LArRawChannelCollection_p1.h"
#include "LArTPCnv/LArRawChannelCollection_p2.h"
#include "LArTPCnv/LArRawChannelContainer_p1.h" 
#include "LArTPCnv/LArRawChannelContainer_p2.h" 
#include "LArTPCnv/LArRawChannelContainer_p3.h" 
#include "LArTPCnv/LArRawChannelContainer_p4.h" 

#include "LArRawEvent/LArDigitContainer.h"
#include "LArTPCnv/LArDigitContainerCnv_p1.h"
#include "LArRawEvent/LArRawChannel.h"
#include "LArRawEvent/LArRawChannelContainer.h"
#include "LArTPCnv/LArRawChannelContainerCnv_p1.h" 
#include "LArTPCnv/LArRawChannelContainerCnv_p2.h" 
#include "LArTPCnv/LArRawChannelContainerCnv_p3.h" 
#include "LArTPCnv/LArRawChannelContainerCnv_p4.h" 

#include "LArTPCnv/LArFebErrorSummary_p1.h" 
#include "LArTPCnv/LArFebErrorSummaryCnv_p1.h" 

#include "LArRecEvent/LArNoisyROSummary.h"
#include "LArTPCnv/LArNoisyROSummary_p1.h" 
#include "LArTPCnv/LArNoisyROSummaryCnv_p1.h" 
#include "LArTPCnv/LArNoisyROSummary_p2.h" 
#include "LArTPCnv/LArNoisyROSummaryCnv_p2.h" 
#include "LArTPCnv/LArNoisyROSummary_p3.h" 
#include "LArTPCnv/LArNoisyROSummaryCnv_p3.h" 
#include "LArTPCnv/LArNoisyROSummary_p4.h"
#include "LArTPCnv/LArNoisyROSummaryCnv_p4.h"
#include "LArTPCnv/LArNoisyROSummary_p5.h"
#include "LArTPCnv/LArNoisyROSummaryCnv_p5.h"


DECLARE_TPCNV_FACTORY(LArDigitContainerCnv_p1,
                      LArDigitContainer,
                      LArDigitContainer_p1,
                      Athena::TPCnvVers::Current)
                      
DECLARE_TPCNV_FACTORY(LArFebErrorSummaryCnv_p1,
                      LArFebErrorSummary,
                      LArFebErrorSummary_p1,
                      Athena::TPCnvVers::Current)
                      
DECLARE_TPCNV_FACTORY(LArNoisyROSummaryCnv_p1,
                      LArNoisyROSummary,
                      LArNoisyROSummary_p1,
                      Athena::TPCnvVers::Old)
                      
DECLARE_TPCNV_FACTORY(LArNoisyROSummaryCnv_p2,
                      LArNoisyROSummary,
                      LArNoisyROSummary_p2,
                      Athena::TPCnvVers::Old)

DECLARE_TPCNV_FACTORY(LArNoisyROSummaryCnv_p3,
                      LArNoisyROSummary,
                      LArNoisyROSummary_p3,
                      Athena::TPCnvVers::Old)
                      
DECLARE_TPCNV_FACTORY(LArNoisyROSummaryCnv_p4,
                      LArNoisyROSummary,
                      LArNoisyROSummary_p4,
                      Athena::TPCnvVers::Current)

DECLARE_TPCNV_FACTORY(LArNoisyROSummaryCnv_p5,
                      LArNoisyROSummary,
                      LArNoisyROSummary_p5,
                      Athena::TPCnvVers::Current)

DECLARE_TPCNV_FACTORY(LArRawChannelContainerCnv_p1,
                      LArRawChannelContainer,
                      LArRawChannelContainer_p1,
                      Athena::TPCnvVers::Old)
                      
DECLARE_TPCNV_FACTORY(LArRawChannelContainerCnv_p2,
                      LArRawChannelContainer,
                      LArRawChannelContainer_p2,
                      Athena::TPCnvVers::Old)
                      
DECLARE_TPCNV_FACTORY(LArRawChannelContainerCnv_p3,
                      LArRawChannelContainer,
                      LArRawChannelContainer_p3,
                      Athena::TPCnvVers::Old)
                      
DECLARE_TPCNV_FACTORY(LArRawChannelContainerCnv_p4,
                      LArRawChannelContainer,
                      LArRawChannelContainer_p4,
                      Athena::TPCnvVers::Current)
