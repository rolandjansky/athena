/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/**********************************************************************************
 * @Project: Trigger
 * @Package: TrigCaloEventTPCnv
 * @Class  : TrigT2MbtsBitsContainerCnv_p1
 *
 * @brief transient persistent converter for TrigT2MbtsBitsContainer
 *
 * @author W.Bell@cern.ch  <w.bell@physics.gla.ac.uk>
 *
 **********************************************************************************/
#ifndef TRIGCALOEVENTTPCNV_TRIGT2MBTSBITSCONTAINER_CNV_P1_H
#define TRIGCALOEVENTTPCNV_TRIGT2MBTSBITSCONTAINER_CNV_P1_H
  
#include "AthenaPoolCnvSvc/T_AthenaPoolTPConverter.h"
 
#include "TrigCaloEvent/TrigT2MbtsBits.h"
#include "TrigCaloEvent/TrigT2MbtsBitsContainer.h"
#include "TrigCaloEventTPCnv/TrigT2MbtsBitsContainer_p1.h"

class TrigT2MbtsBitsContainerCnv_p1 : public T_AthenaPoolTPPtrVectorCnv<TrigT2MbtsBitsContainer, TrigT2MbtsBitsContainer_p1, ITPConverterFor<TrigT2MbtsBits> >
{
 
 public:
  TrigT2MbtsBitsContainerCnv_p1() {}

}; //end of class definitions

#endif
