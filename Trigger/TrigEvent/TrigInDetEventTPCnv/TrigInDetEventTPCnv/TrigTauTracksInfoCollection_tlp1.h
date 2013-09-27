/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/**********************************************************************************
 * @Project: Trigger
 * @Package: TrigInDetEventTPCnv
 * @Class  : TrigTauTracksInfoCollection_tlp1
 *
 * @brief "top-level" persistent partner for TrigTauTracksInfoCollection
 *
 * @author Andrew Hamilton  <Andrew.Hamilton@cern.ch>  - U. Geneva
 * @author Francesca Bucci  <F.Bucci@cern.ch>          - U. Geneva
 *
 * File and Version Information:
 * $Id: TrigTauTracksInfoCollection_tlp1.h,v 1.2 2009-04-01 22:08:45 salvator Exp $
 **********************************************************************************/
#ifndef TRIGINDETEVENTTPCNV_TRIGTAUTRACKSINFOCONTAINER_TLP1_H
#define TRIGINDETEVENTTPCNV_TRIGTAUTRACKSINFOCONTAINER_TLP1_H

#include "TrigInDetEventTPCnv/TrigTauTracksInfoCollection_p1.h"
#include "TrigInDetEventTPCnv/TrigTauTracksInfo_p1.h"
#include "EventCommonTPCnv/P4PtEtaPhiM_p1.h"

class TrigTauTracksInfoCollection_tlp1 
{
   
 public:
   
  TrigTauTracksInfoCollection_tlp1(){}
 
  std::vector<TrigTauTracksInfoCollection_p1>       m_trigTauTracksInfoCollectionVec;
  std::vector<TrigTauTracksInfo_p1>                 m_trigTauTracksInfoVec;
  std::vector<P4PtEtaPhiM_p1>                       m_p4PtEtaPhiM;
     
};//end of class definitions
 
#endif
