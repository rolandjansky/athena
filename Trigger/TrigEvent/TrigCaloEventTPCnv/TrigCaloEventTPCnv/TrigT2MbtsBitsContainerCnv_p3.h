/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/**********************************************************************************
 * @Project: Trigger
 * @Package: TrigCaloEventTPCnv
 * @Class  : TrigT2MbtsBitsContainerCnv_p3
 *
 * @brief transient persistent converter for TrigT2MbtsBitsContainer
 *
 * @author W.Bell@cern.ch  <w.bell@physics.gla.ac.uk>
 *
 **********************************************************************************/
#ifndef TRIGCALOEVENTTPCNV_TRIGT2MBTSBITSCONTAINER_CNV_P3_H
#define TRIGCALOEVENTTPCNV_TRIGT2MBTSBITSCONTAINER_CNV_P3_H
  
#include "AthenaPoolCnvSvc/T_AthenaPoolTPConverter.h"
 
#include "TrigCaloEvent/TrigT2MbtsBitsContainer.h"
#include "TrigCaloEventTPCnv/TrigT2MbtsBitsContainer_p3.h"
#include "TrigCaloEventTPCnv/TrigT2MbtsBitsCnv_p3.h"

typedef  T_AthenaPoolTPCnvVector< TrigT2MbtsBitsContainer, TrigT2MbtsBitsContainer_p3, TrigT2MbtsBitsCnv_p3>  TrigT2MbtsBitsContainerCnv_p3;

#endif
