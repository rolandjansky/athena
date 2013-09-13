/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/**********************************************************************************
 * @Project: Trigger
 * @Package: TrigMuonEventTPCnv
 * @Class  : TrigMuonEFContainerCnv_p1
 *
 * @brief transient-persistent converter for TrigMuonContainer_p1
 *
 * @author Heuijin Lim <hjlim@hep.anl.gov>
 * @Created: Jul. 3. 2008 
 * 
 **********************************************************************************/
#ifndef TRIGMUONEVENTTPCNV_TILEMUFEATURECONTAINER_CNV_P1_H
#define TRIGMUONEVENTTPCNV_TILEMUFEATURECONTAINER_CNV_P1_H
  
#include "AthenaPoolCnvSvc/T_AthenaPoolTPConverter.h"
 
#include "TrigMuonEvent/TileMuFeatureContainer.h"
#include "TrigMuonEventTPCnv/TileMuFeatureContainer_p1.h"
#include "TrigMuonEventTPCnv/TileMuFeatureCnv_p1.h"

class TileMuFeatureContainerCnv_p1 : public T_AthenaPoolTPPtrVectorCnv< TileMuFeatureContainer, TileMuFeatureContainer_p1, ITPConverterFor<TileMuFeature> >

{
 
 public:
  TileMuFeatureContainerCnv_p1() {}
 
}; //end of class definitions
 
#endif
