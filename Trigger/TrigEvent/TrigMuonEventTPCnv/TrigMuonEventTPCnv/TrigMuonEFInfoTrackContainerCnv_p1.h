/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/**********************************************************************************
 * @Project: Trigger
 * @Package: TrigMuonEventTPCnv
 * @Class  : TrigMuonEFInfoTraclContainerCnv_p1
 *
 * @brief transient-persistent converter for TrigMuonEFInfoTrackContainer_p1
 *
 * @author Andrew Hamilton  <Andrew.Hamilton@cern.ch>  - U. Geneva
 * @author Francesca Bucci  <F.Bucci@cern.ch>          - U. Geneva
 * @author Sergio Grancagnolo  <Sergio.Grancagnolo@le.infn.it>  - U.Salento/INFN Le
 * @author Alexander Oh <alexander.oh@cern.ch>         - U. Manchester
 *
 **********************************************************************************/
#ifndef TRIGMUONEVENTTPCNV_TRIGMUONEFINFOTRACKCONTAINER_CNV_P1_H
#define TRIGMUONEVENTTPCNV_TRIGMUONEFINFOTRACKCONTAINER_CNV_P1_H

#include "AthenaPoolCnvSvc/T_AthenaPoolTPConverter.h"

#include "TrigMuonEventTPCnv/TrigMuonEFInfoTrackContainer_p1.h"
#include "TrigMuonEventTPCnv/TrigMuonEFInfoTrackCnv_p1.h"

#include "TrigMuonEvent/TrigMuonEFInfoTrackContainer.h"

class MsgStream;

class TrigMuonEFInfoTrackContainerCnv_p1 :
	public T_AthenaPoolTPPtrVectorCnv<
	TrigMuonEFInfoTrackContainer,
	TrigMuonEFInfoTrackContainer_p1,
	ITPConverterFor< TrigMuonEFInfoTrack> >

{

 public:
  TrigMuonEFInfoTrackContainerCnv_p1() {}

}; //end of class definitions

#endif
