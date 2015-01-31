/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef EVENTTAGTPCNV_RAWINFOSUMMARYFORTAGCNV_P1_H
#define EVENTTAGTPCNV_RAWINFOSUMMARYFORTAGCNV_P1_H

///////////////////////// -*- C++ -*- /////////////////////////////
/**  RawInfoSummaryForTagCnv_p1.h
 *  @author: Marcin.Nowak@cern.ch
 *  @date March 2010
 *  @brief TP converter for RawInfoSummaryForTag_p1
****************************************************************/


#include "AthenaPoolCnvSvc/T_AthenaPoolTPConverter.h"

#include "TagEvent/RawInfoSummaryForTag.h"
#include "EventTagTPCnv/RawInfoSummaryForTag_p1.h"


class RawInfoSummaryForTagCnv_p1
   : public T_AthenaPoolTPCnvBase< RawInfoSummaryForTag, RawInfoSummaryForTag_p1 >
{
public:
  RawInfoSummaryForTagCnv_p1() {}

  /** The method to convert the persistent to the transient state */
  void persToTrans(const Pers_t* pers, Trans_t* trans, MsgStream& log);

  /** The method to convert the transient to the persistent state */
  void transToPers(const Trans_t* trans, Pers_t* pers, MsgStream& log);
 
};

#endif 
