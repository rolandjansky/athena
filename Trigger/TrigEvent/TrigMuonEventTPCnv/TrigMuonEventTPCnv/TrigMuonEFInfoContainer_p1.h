/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/**********************************************************************************
 * @Project: Trigger
 * @Package: TrigMuonEventTPCnv
 * @Class  : TrigMuonEFInfoContainer_p1
 *
 * @brief persistent partner for TrigMuonEFInfoContainer
 *
 * @author Andrew Hamilton  <Andrew.Hamilton@cern.ch>  - U. Geneva
 * @author Francesca Bucci  <F.Bucci@cern.ch>          - U. Geneva
 * @author Sergio Grancagnolo  <Sergio.Grancagnolo@le.infn.it> - U.Salento/INFN Le
 *
 **********************************************************************************/
#ifndef TRIGMUONEVENTTPCNV_TRIGMUONEFINFOCONTAINER_P1_H
#define TRIGMUONEVENTTPCNV_TRIGMUONEFINFOCONTAINER_P1_H

#include "AthenaPoolUtilities/TPObjRef.h"
#include <vector>
  
class TrigMuonEFInfoContainer_p1 : public std::vector<TPObjRef>
{
 public:
    
  TrigMuonEFInfoContainer_p1(){}
  friend class TrigMuonEFInfoContainerCnv_p1;

 private:

  std::vector<TPObjRef> m_trigMuonEFInfo;

};// end of class definitions
 
 
#endif
