/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/**********************************************************************************
 * @Project: Trigger
 * @Package: TrigMuonEventTPCnv
 * @Class  : TileMuFeatureContainer_tlp1
 *
 * @brief "top-level" persistent partner for TileMuFeatureContainer
 *
 * @author Heuijin Lim <hjlim@hep.anl.gov>
 * @Created: Jul. 3. 2008 
 **********************************************************************************/
#ifndef TRIGMUONEVENTTPCNV_TILEMUFEATURECONTAINER_TLP1_H
#define TRIGMUONEVENTTPCNV_TILEMUFEATURECONTAINER_TLP1_H

#include "TrigMuonEventTPCnv/TileMuFeatureContainer_p1.h"
#include "TrigMuonEventTPCnv/TileMuFeature_p1.h"

class TileMuFeatureContainer_tlp1 
{
   
 public:
   
  TileMuFeatureContainer_tlp1(){}
 
  std::vector<TileMuFeatureContainer_p1>       m_TileMuFeatureContainerVec;
  std::vector<TileMuFeature_p1>                m_TileMuFeatureVec;
     
};//end of class definitions
 
#endif
