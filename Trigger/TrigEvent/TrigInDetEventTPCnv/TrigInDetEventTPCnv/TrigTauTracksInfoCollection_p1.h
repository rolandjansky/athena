/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/**********************************************************************************
 * @Project: Trigger
 * @Package: TrigInDetEventTPCnv
 * @Class  : TrigTauTracksInfoCollection_p1
 *
 * @brief persistent partner for TrigTauTracksInfoCollection
 *
 * @author Andrew Hamilton  <Andrew.Hamilton@cern.ch>  - U. Geneva
 * @author Francesca Bucci  <F.Bucci@cern.ch>          - U. Geneva
 *
 * File and Version Information:
 * $Id: TrigTauTracksInfoCollection_p1.h,v 1.2 2009-04-01 22:08:45 salvator Exp $
 **********************************************************************************/
#ifndef TRIGINDETEVENTTPCNV_TRIGTAUTRACKSINFOCONTAINER_P1_H
#define TRIGINDETEVENTTPCNV_TRIGTAUTRACKSINFOCONTAINER_P1_H

#include "AthenaPoolUtilities/TPObjRef.h"
#include <vector>
  
class TrigTauTracksInfoCollection_p1 : public std::vector<TPObjRef>
{
 public:
    
  TrigTauTracksInfoCollection_p1(){}
  friend class TrigTauTracksInfoCollectionCnv_p1;

 private:

  std::vector<TPObjRef> m_trigTauTracksInfo;

};// end of class definitions
 
 
#endif
