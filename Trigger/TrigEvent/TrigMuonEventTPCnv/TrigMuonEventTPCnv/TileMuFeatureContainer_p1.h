/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/**********************************************************************************
 * @Project: Trigger
 * @Package: TrigMuonEventTPCnv
 * @Class  : TileMuFeatureContainer_p1
 *
 * @brief persistent partner for TileMuFeatureContainer
 *
 * @author Heuijin Lim <hjlim@hep.anl.gov>
 * @Created: Jul. 3. 2008 
 *
 **********************************************************************************/
#ifndef TRIGMUONEVENTTPCNV_TILEMUFEATURECONTAINER_P1_H
#define TRIGMUONEVENTTPCNV_TILEMUFEATURECONTAINER_P1_H

#include "AthenaPoolUtilities/TPObjRef.h"
#include <vector>
  
class TileMuFeatureContainer_p1 : public std::vector<TPObjRef>
{
 public:
    
  TileMuFeatureContainer_p1(){}
  friend class TileMuFeatureContainerCnv_p1;

 private:
  std::vector<TPObjRef> m_TileMuFeature;

};// end of class definitions
 
 
#endif
