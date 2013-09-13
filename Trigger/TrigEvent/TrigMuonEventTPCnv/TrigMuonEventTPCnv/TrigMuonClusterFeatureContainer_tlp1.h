/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/**********************************************************************************
 * @Project: Trigger
 * @Package: TrigMuonEventTPCnv
 * @Class  : TrigMuonClusterFeatureContainer_tlp1
 *
 * @brief "top-level" persistent partner for TrigMuonClusterFeatureContainer
 *
 * @author Stefano Giagu <Stefano.Giagu@cern.ch>  - U. of Rome
 * @author Antonio Policicchio <Antonio.Policicchio@cern.ch>  - U. of Washington
 *
 * File and Version Information:
 * $Id: TrigMuonClusterFeatureContainer_tlp1.h,v 1.2 2009-04-01 22:12:15 salvator Exp $
 **********************************************************************************/
#ifndef TRIGMUONEVENTTPCNV_TRIGMUONCLUSTERFEATURECONTAINER_TLP1_H
#define TRIGMUONEVENTTPCNV_TRIGMUONCLUSTERFEATURECONTAINER_TLP1_H

#include "TrigMuonEventTPCnv/TrigMuonClusterFeatureContainer_p1.h"
#include "TrigMuonEventTPCnv/TrigMuonClusterFeature_p1.h"
#include "TrigMuonEventTPCnv/MuonFeature_p1.h"
#include "TrigInDetEventTPCnv/TrigInDetTrack_p1.h"

class TrigMuonClusterFeatureContainer_tlp1 
{
   
 public:
   
  TrigMuonClusterFeatureContainer_tlp1(){}
 
  std::vector<TrigMuonClusterFeatureContainer_p1>       m_trigMuonClusterFeatureContainerVec;
  std::vector<TrigMuonClusterFeature_p1>                m_trigMuonClusterFeatureVec;
     
};//end of class definitions
 
#endif
