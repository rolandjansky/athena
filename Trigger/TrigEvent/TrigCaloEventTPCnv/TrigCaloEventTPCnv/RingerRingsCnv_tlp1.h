/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/**********************************************************************************
 * @Project: Trigger
 * @Package: TrigCaloEventTPCnv
 * @Class  : RingerRingsCnv_tlp1
 *
 * @brief "top level" transient-persistent converter for RingerRings
 *
 * @author Danilo Enoque Ferreira de Lima <dferreir@mail.cern.ch> - UFRJ
 *
 * File and Version Information:
 * $Id: RingerRingsCnv_tlp1.h,v 1.2 2009-04-01 22:01:37 salvator Exp $
 **********************************************************************************/
#ifndef TRIGCALOEVENTTPCNV_RINGERRINGSCNV_TLP1_H
#define TRIGCALOEVENTTPCNV_RINGERRINGSCNV_TLP1_H

#include "AthenaPoolCnvSvc/AthenaPoolTopLevelTPConverter.h"

#include "TrigCaloEventTPCnv/RingerRings_tlp1.h"
#include "TrigCaloEventTPCnv/RingerRingsCnv_p1.h"

class RingerRingsCnv_tlp1 : public AthenaPoolTopLevelTPConverter<RingerRingsCnv_p1, RingerRings_tlp1> {

 public:

  RingerRingsCnv_tlp1();
  virtual ~RingerRingsCnv_tlp1() { }

  virtual void setPStorage(RingerRings_tlp1 *storage);

 protected:

  // T/P converters for ALL types used in RingerRings
  RingerRingsCnv_p1            m_ringerRingsCnv;

};

#endif

