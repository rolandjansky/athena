/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/**********************************************************************************
 * @Project: Trigger
 * @Package: TrigMuonEventTPCnv
 * @class  : TrigMuonEFContainer_p1
 *
 * @brief persistent partner for TrigMuonEFContainer
 *
 * @author Andrew Hamilton  <Andrew.Hamilton@cern.ch>  - U. Geneva
 * @author Francesca Bucci  <F.Bucci@cern.ch>          - U. Geneva
 *
 * File and Version Information:
 * $Id: TrigMuonEFContainer_p1.h,v 1.2 2009-04-01 22:12:15 salvator Exp $
 **********************************************************************************/
#ifndef TRIGMUONEVENTTPCNV_TRIGMUONEFCONTAINER_P1_H
#define TRIGMUONEVENTTPCNV_TRIGMUONEFCONTAINER_P1_H

#include "AthenaPoolUtilities/TPObjRef.h"
#include <vector>
  
class TrigMuonEFContainer_p1 : public std::vector<TPObjRef>
{
 public:
    
  TrigMuonEFContainer_p1(){}
  friend class TrigMuonEFContainerCnv_p1;

 private:

  std::vector<TPObjRef> m_trigMuonEF;

};// end of class definitions
 
 
#endif
