/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/**********************************************************************************
 * @Project: Trigger
 * @Package: TrigCaloEventTPCnv
 * @Class  : RingerRingsContainerCnv_p2
 *
 * @brief transient persistent converter for RingerRingsContainer
 *
 * @author Danilo Enoque Ferreira de Lima <dferreir@mail.cern.ch> - UFRJ
 *
 * $Id: RingerRingsContainerCnv_p2.h,v 1.2 2011-05-31 22:01:37 damazio Exp $
 **********************************************************************************/
#ifndef TRIGCALOEVENTTPCNV_RINGERRINGSCONTAINER_CNV_P2_H
#define TRIGCALOEVENTTPCNV_RINGERRINGSCONTAINER_CNV_P2_H
  
#include "AthenaPoolCnvSvc/T_AthenaPoolTPConverter.h"
 
#include "TrigCaloEvent/RingerRingsContainer.h"
#include "TrigCaloEventTPCnv/RingerRingsContainer_p2.h"
#include "TrigCaloEventTPCnv/RingerRingsCnv_p2.h"
 
typedef T_AthenaPoolTPCnvVector<RingerRingsContainer, RingerRingsContainer_p2, RingerRingsCnv_p2> RingerRingsContainerCnv_p2;
 
#endif

