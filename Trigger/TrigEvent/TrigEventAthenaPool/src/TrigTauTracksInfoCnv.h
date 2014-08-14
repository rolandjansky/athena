/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/**********************************************************************************
 * @Project: Trigger
 * @Package: TrigEventAthenaPool
 * @Class  : TrigTauTracksInfoCnv
 *
 * @brief transient persistent converter for TrigTauTracksInfo
 *
 * @author Andrew Hamilton  <Andrew.Hamilton@cern.ch>  - U. Geneva
 * @author Francesca Bucci  <f.bucci@cern.ch>          - U. Geneva
 *
 * File and Version Information:
 * $Id: TrigTauTracksInfoCnv.h,v 1.2 2009-02-23 18:59:20 ssnyder Exp $
 **********************************************************************************/
#ifndef TRIGEVENTATHENAPOOL_TRIGTAUTRACKSINFOCNV_H
#define TRIGEVENTATHENAPOOL_TRIGTAUTRACKSINFOCNV_H

#include "TrigInDetEvent/TrigTauTracksInfo.h"
#include "AthenaPoolCnvSvc/T_AthenaPoolCustomCnv.h"
class TrigTauTracksInfo_tlp1;

// typedef to the latest persistent version
typedef TrigTauTracksInfo_tlp1  TrigTauTracksInfo_PERS;

class MsgStream;

class TrigTauTracksInfoCnv_tlp1;
class TrigTauTracksInfoCnv : public T_AthenaPoolCustomCnv<TrigTauTracksInfo, TrigTauTracksInfo_PERS > 
{

  friend class CnvFactory<TrigTauTracksInfoCnv>;

 protected:
  
  TrigTauTracksInfoCnv(ISvcLocator* svcloc);
  ~TrigTauTracksInfoCnv();

  TrigTauTracksInfo_PERS*  createPersistent(TrigTauTracksInfo* transCont);

  TrigTauTracksInfo*       createTransient ();

 private:

  TrigTauTracksInfoCnv_tlp1* m_TPConverter;

};


#endif
