/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/**********************************************************************************
 * @Project: Trigger
 * @Package: TrigMuonEventTPCnv
 * @Class  : TrigMuonEFInfoTrackContainer_p1
 *
 * @brief persistent partner for TrigMuonEFInfoTrackContainer
 *
 * @author Andrew Hamilton  <Andrew.Hamilton@cern.ch>  - U. Geneva
 * @author Francesca Bucci  <F.Bucci@cern.ch>          - U. Geneva
 * @author Alexander Oh <alexander.oh@cern.ch>         - U. Manchester
 *
 * File and Version Information:
 * $Id: TrigMuonEFInfoTrackContainer_p1.h,v 1.2 2009-04-01 22:12:15 salvator Exp $
 **********************************************************************************/
#ifndef TRIGMUONEVENTTPCNV_TRIGMUONEFINFOTRACKCONTAINER_P1_H
#define TRIGMUONEVENTTPCNV_TRIGMUONEFINFOTRACKCONTAINER_P1_H

#include "AthenaPoolUtilities/TPObjRef.h"
#include <vector>

class TrigMuonEFInfoTrackContainer_p1 : public std::vector<TPObjRef>
{
 public:

  TrigMuonEFInfoTrackContainer_p1(){}
  friend class TrigMuonEFInfoTrackContainerCnv_p1;

 private:

  std::vector<TPObjRef> m_trigMuonEFInfoTrack;

};// end of class definitions


#endif
