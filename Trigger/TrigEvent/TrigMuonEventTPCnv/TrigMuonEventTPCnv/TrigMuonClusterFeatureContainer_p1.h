/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/**********************************************************************************
 * @Project: Trigger
 * @Package: TrigMuonEventTPCnv
 * @Class  : TrigMuonClusterFeatureContainer_p1
 *
 * @brief persistent partner for TrigMuonClusterFatureContainer
 *
 * @author Stefano Giagu <Stefano.Giagu@cern.ch>  - U. of Rome
 * @author Antonio Policicchio <Antonio.Policicchio@cern.ch>  - U. of Washington
 *
 * File and Version Information:
 * $Id: TrigMuonClusterFeatureContainer_p1.h,v 1.2 2009-04-01 22:12:15 salvator Exp $
 **********************************************************************************/
#ifndef TRIGMUONEVENTTPCNV_TRIGMUONCLUSTERFEATURECONTAINER_P1_H
#define TRIGMUONEVENTTPCNV_TRIGMUONCLUSTERFEATURECONTAINER_P1_H

#include "AthenaPoolUtilities/TPObjRef.h"
#include <vector>
  
class TrigMuonClusterFeatureContainer_p1 : public std::vector<TPObjRef>
{
 public:
    
  TrigMuonClusterFeatureContainer_p1(){}
  friend class TrigMuonClusterFeatureContainerCnv_p1;

 private:

  std::vector<TPObjRef> m_trigMuonClusterFeature;

};// end of class definitions
 
 
#endif
