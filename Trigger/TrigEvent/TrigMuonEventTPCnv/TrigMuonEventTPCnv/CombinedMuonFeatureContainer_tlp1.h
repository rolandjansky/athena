/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/**********************************************************************************
 * @Project: Trigger
 * @Package: TrigMuonEventTPCnv
 * @Class  : CombinedMuonFeatureContainer_tlp1
 *
 * @brief "top-level" persistent partner for CombinedMuonFeatureContainer
 *
 * @author Andrew Hamilton  <Andrew.Hamilton@cern.ch>  - U. Geneva
 * @author Francesca Bucci  <F.Bucci@cern.ch>          - U. Geneva
 *
 * File and Version Information:
 * $Id: CombinedMuonFeatureContainer_tlp1.h,v 1.2 2009-04-01 22:12:14 salvator Exp $
 **********************************************************************************/
#ifndef TRIGMUONEVENTTPCNV_COMBINEDMUONFEATURECONTAINER_TLP1_H
#define TRIGMUONEVENTTPCNV_COMBINEDMUONFEATURECONTAINER_TLP1_H

#include "TrigMuonEventTPCnv/CombinedMuonFeatureContainer_p1.h"
#include "TrigMuonEventTPCnv/CombinedMuonFeature_p1.h"
#include "TrigMuonEventTPCnv/CombinedMuonFeature_p2.h"
#include "TrigMuonEventTPCnv/MuonFeature_p1.h"
#include "TrigInDetEventTPCnv/TrigInDetTrack_p1.h"

class CombinedMuonFeatureContainer_tlp1 
{
   
 public:
   
  CombinedMuonFeatureContainer_tlp1(){}
 
  std::vector<CombinedMuonFeatureContainer_p1>       m_combinedMuonFeatureContainerVec;
  std::vector<CombinedMuonFeature_p1>                m_combinedMuonFeatureVec;
  std::vector<CombinedMuonFeature_p2>                m_combinedMuonFeatureVec_p2;
     
};//end of class definitions
 
#endif
