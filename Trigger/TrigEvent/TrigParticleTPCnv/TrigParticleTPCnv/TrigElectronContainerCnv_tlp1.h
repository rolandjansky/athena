/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/**********************************************************************************
 * @Project: Trigger
 * @Package: TrigParticleTPCnv
 * @Class  : TrigElectronContainerCnv_tlp1
 *
 * @brief "top level" transient-persistent converter for TrigElectronContainer
 *
 * @author Andrew Hamilton  <Andrew.Hamilton@cern.ch>  - U. Geneva
 * @author Francesca Bucci  <f.bucci@cern.ch>          - U. Geneva
 *
 * File and Version Information:
 * $Id: TrigElectronContainerCnv_tlp1.h,v 1.2 2009-04-01 22:13:30 salvator Exp $
 **********************************************************************************/
#ifndef TRIGPARTICLETPCNV_TRIGELECTRONCONTAINERCNV_TLP1_H
#define TRIGPARTICLETPCNV_TRIGELECTRONCONTAINERCNV_TLP1_H

#include "AthenaPoolCnvSvc/AthenaPoolTopLevelTPConverter.h"

#include "TrigParticleTPCnv/TrigElectronContainer_tlp1.h"
#include "TrigParticleTPCnv/TrigElectronContainerCnv_p1.h"
#include "EventCommonTPCnv/P4PtEtaPhiMCnv_p1.h"

class TrigElectronContainerCnv_tlp1 : public AthenaPoolTopLevelTPConverter<TrigElectronContainerCnv_p1, TrigElectronContainer_tlp1>
{

 public:

  TrigElectronContainerCnv_tlp1();
  virtual ~TrigElectronContainerCnv_tlp1() {}

  virtual void setPStorage( TrigElectronContainer_tlp1 *storage );

 protected:

  //tp converters for ALL types used in TrigElectronContainer
  TrigElectronContainerCnv_p1  m_trigElectronContainerCnv;
  TrigElectronCnv_p1           m_trigElectronCnv;
  P4PtEtaPhiMCnv_p1            m_p4PtEtaPhiMCnv;

};



#endif
