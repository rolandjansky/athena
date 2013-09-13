/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/**********************************************************************************
 * @Project: Trigger
 * @Package: TrigMuonEventTPCnv
 * @Class  : CombinedMuonFeatureContainer_p1
 *
 * @brief persistent partner for TrigMuonEFContainer
 *
 * @author Andrew Hamilton  <Andrew.Hamilton@cern.ch>  - U. Geneva
 * @author Francesca Bucci  <F.Bucci@cern.ch>          - U. Geneva
 *
 * File and Version Information:
 * $Id: CombinedMuonFeatureContainer_p1.h,v 1.2 2009-04-01 22:12:14 salvator Exp $
 **********************************************************************************/
#ifndef TRIGMUONEVENTTPCNV_COMBINEDMUONFEATURECONTAINER_P1_H
#define TRIGMUONEVENTTPCNV_COMBINEDMUONFEATURECONTAINER_P1_H

#include "AthenaPoolUtilities/TPObjRef.h"
#include <vector>
  
class CombinedMuonFeatureContainer_p1 : public std::vector<TPObjRef>
{
 public:
    
  CombinedMuonFeatureContainer_p1(){}
  friend class CombinedMuonFeatureContainerCnv_p1;

 private:

  std::vector<TPObjRef> m_combinedMuonFeature;

};// end of class definitions
 
 
#endif
