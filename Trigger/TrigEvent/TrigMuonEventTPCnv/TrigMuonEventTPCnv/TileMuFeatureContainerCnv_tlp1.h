/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/**********************************************************************************
 * @Project: Trigger
 * @Package: TrigMuonEventTPCnv
 * @Class  : TileMuFeatureContainerCnv_tlp1
 *
 * @brief "top-level" transient-persistent converter for TileMuFeatureContainer_tlp1
 *
 * @author Heuijin Lim <hjlim@hep.anl.gov>
 * @Created: Jul. 3. 2008 
 * 
 **********************************************************************************/
#ifndef  TRIGMUONEVENTTPCNV_TILEMUFEATURECONTAINER_CNV_TLP1_H
#define  TRIGMUONEVENTTPCNV_TILEMUFEATURECONTAINER_CNV_TLP1_H
 
#include "AthenaPoolCnvSvc/AthenaPoolTopLevelTPConverter.h"

#include "TrigMuonEventTPCnv/TileMuFeatureContainer_tlp1.h"
#include "TrigMuonEventTPCnv/TileMuFeatureContainerCnv_p1.h"   
 
class TileMuFeatureContainerCnv_tlp1 : public AthenaPoolTopLevelTPConverter< TileMuFeatureContainerCnv_p1, TileMuFeatureContainer_tlp1 >
{
 
 public:
 
  TileMuFeatureContainerCnv_tlp1();
  virtual ~TileMuFeatureContainerCnv_tlp1() {}
 
  //default methods  
  virtual void  setPStorage( TileMuFeatureContainer_tlp1 *storage );
 
 protected: 
  TileMuFeatureContainerCnv_p1    m_TileMuFeatureContainerCnv;
  TileMuFeatureCnv_p1             m_TileMuFeatureCnv;
 
}; //end of class definitions
 
#endif
