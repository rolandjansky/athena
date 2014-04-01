/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/**********************************************************************************
 * @Project: Trigger
 * @Package: TrigCaloEventTPCnv
 * @Class  : TrigT2JetContainerCnv_p1
 *
 * @brief transient persistent converter for TrigT2JetContainer
 *
 * @author Andrew Hamilton  <Andrew.Hamilton@cern.ch>  - U. Geneva
 * @author Francesca Bucci  <F.Bucci@cern.ch>          - U. Geneva
 * @modified Tobias Kruker  <kruker@cern.ch>		  		 - U. Bern
 *
 * File and Version Information:
 * $Id: TrigT2JetContainerCnv_p1.h,v 1.2 2009-04-01 22:01:37 salvator Exp $
 **********************************************************************************/
#ifndef TRIGCALOEVENTTPCNV_TRIGT2JETCONTAINER_CNV_P1_H
#define TRIGCALOEVENTTPCNV_TRIGT2JETCONTAINER_CNV_P1_H
  
#include "AthenaPoolCnvSvc/T_AthenaPoolTPConverter.h"
 
#include "TrigCaloEvent/TrigT2JetContainer.h"
#include "TrigCaloEventTPCnv/TrigT2JetContainer_p1.h"
#include "TrigCaloEventTPCnv/TrigT2JetCnv_p1.h"
#include "TrigCaloEventTPCnv/TrigT2JetCnv_p2.h"     // to be added? 
 
//class TrigT2JetContainerCnv_p1 : public T_AthenaPoolTPPtrVectorCnv< TrigT2JetContainer, TrigT2JetContainer_p1, TrigT2JetCnv_p1 >
class TrigT2JetContainerCnv_p1 : public T_AthenaPoolTPPtrVectorCnv< TrigT2JetContainer, TrigT2JetContainer_p1, ITPConverterFor<TrigT2Jet> >
{
 
 public:
  TrigT2JetContainerCnv_p1() {}
 
}; //end of class definitions
 
#endif
