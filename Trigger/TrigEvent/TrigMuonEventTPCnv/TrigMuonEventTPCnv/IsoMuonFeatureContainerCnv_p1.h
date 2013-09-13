/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/**********************************************************************************
 * @Project: Trigger
 * @Package: TrigMuonEventTPCnv
 * @Class  : IsoMuonFeatureContainerCnv_p1
 *
 * @brief transient-persistent converter for IsoMuonFatureContainer_p1
 *
 * @author Stefano Giagu <Stefano.Giagu@cern.ch>  - U. of Rome
 *
 * File and Version Information:
 * $Id: IsoMuonFeatureContainerCnv_p1.h,v 1.2 2009-04-01 22:12:14 salvator Exp $
 **********************************************************************************/
#ifndef TRIGMUONEVENTTPCNV_ISOMUONFEATURECONTAINER_CNV_P1_H
#define TRIGMUONEVENTTPCNV_ISOMUONFEATURECONTAINER_CNV_P1_H
  
#include "AthenaPoolCnvSvc/T_AthenaPoolTPConverter.h"
 
#include "TrigMuonEvent/IsoMuonFeatureContainer.h"
#include "TrigMuonEventTPCnv/IsoMuonFeatureContainer_p1.h"
#include "TrigMuonEventTPCnv/IsoMuonFeatureCnv_p1.h"

class IsoMuonFeatureContainerCnv_p1 : public T_AthenaPoolTPPtrVectorCnv< IsoMuonFeatureContainer, IsoMuonFeatureContainer_p1, ITPConverterFor<IsoMuonFeature> >

{
 
 public:
  IsoMuonFeatureContainerCnv_p1() {}
 
}; //end of class definitions
 
#endif
