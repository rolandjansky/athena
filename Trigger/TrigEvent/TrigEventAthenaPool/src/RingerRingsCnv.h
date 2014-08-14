/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/**********************************************************************************
 * @Project: Trigger
 * @Package: TrigEventAthenaPool
 * @Class  : RingerRingsCnv
 *
 * @brief transient persistent converter for RingerRings
 *
 * @author Danilo Enoque Ferreira de Lima <dferreir@mail.cern.ch> - UFRJ
 *
 * $Id: RingerRingsCnv.h,v 1.2 2009-02-23 18:59:19 ssnyder Exp $
 **********************************************************************************/
#ifndef TRIGEVENTATHENAPOOL_RINGERRINGSCNV_H
#define TRIGEVENTATHENAPOOL_RINGERRINGSCNV_H

#include "TrigCaloEvent/RingerRings.h"
#include "AthenaPoolCnvSvc/T_AthenaPoolCustomCnv.h"
class RingerRings_tlp1;
class RingerRingsCnv_tlp1;


// typedef to the latest persistent version
typedef RingerRings_tlp1  RingerRings_PERS;

class MsgStream;

class RingerRingsCnv  : public T_AthenaPoolCustomCnv<RingerRings, RingerRings_PERS> {

  friend class CnvFactory<RingerRingsCnv>;

 protected:
  
  RingerRingsCnv(ISvcLocator* svcloc);
  ~RingerRingsCnv();

  RingerRings_PERS*  createPersistent(RingerRings* transCont);

  RingerRings*       createTransient ();

 private:

  RingerRingsCnv_tlp1* m_TPConverter;

};

#endif

