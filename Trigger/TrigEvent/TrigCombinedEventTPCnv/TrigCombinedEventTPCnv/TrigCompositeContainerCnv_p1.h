/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/**********************************************************************************
 * @Project: Trigger
 * @Package: TrigCombinedEventTPCnv
 * @Class  : TrigCompositeContainerCnv_p1
 *
 * @brief transient persistent converter for TrigCompositeContainer
 *
 * @author Camille B.-Champagne  <camille.belanger-champagne@cern.ch>
 **********************************************************************************/
#ifndef TRIGCOMBINEDEVENTTPCNV_TRIGCOMPOSITECONTAINER_CNV_P1_H
#define TRIGCOMBINEDEVENTTPCNV_TRIGCOMPOSITECONTAINER_CNV_P1_H

#include "AthenaPoolCnvSvc/T_AthenaPoolTPConverter.h"

#include "TrigCombinedEvent/TrigCompositeContainer.h"
#include "TrigCombinedEventTPCnv/TrigCompositeContainer_p1.h"
#include "TrigCombinedEventTPCnv/TrigCompositeCnv_p1.h"

typedef T_AthenaPoolTPCnvVector<TrigCompositeContainer,TrigCompositeContainer_p1, TrigCompositeCnv_p1> TrigCompositeContainerCnv_p1;

#endif
