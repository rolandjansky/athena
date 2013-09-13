/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/**********************************************************************************
 * @Project: Trigger
 * @Package: TrigMuonEventTPCnv
 * @Class  : TrigMuonEFContainer_tlp1
 *
 * @brief "top-level" persistent partner for TrigMuonEFContainer
 *
 * @author Andrew Hamilton  <Andrew.Hamilton@cern.ch>  - U. Geneva
 * @author Francesca Bucci  <F.Bucci@cern.ch>          - U. Geneva
 *
 * File and Version Information:
 * $Id: TrigMuonEFContainer_tlp1.h,v 1.2 2009-04-01 22:12:15 salvator Exp $
 **********************************************************************************/
#ifndef TRIGMUONEVENTTPCNV_TRIGMUONEFCONTAINER_TLP1_H
#define TRIGMUONEVENTTPCNV_TRIGMUONEFCONTAINER_TLP1_H

#include "TrigMuonEventTPCnv/TrigMuonEFContainer_p1.h"
#include "TrigMuonEventTPCnv/TrigMuonEF_p1.h"
#include "EventCommonTPCnv/P4IPtCotThPhiM_p1.h"


class TrigMuonEFContainer_tlp1 
{
   
 public:
   
  TrigMuonEFContainer_tlp1(){}
 
  std::vector<TrigMuonEFContainer_p1>       m_TrigMuonEFContainers;
  std::vector<TrigMuonEF_p1>                m_MuonEF;
  std::vector<P4IPtCotThPhiM_p1>            m_P4IPtCotThPhiM;
     
};//end of class definitions
 
#endif
