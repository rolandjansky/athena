/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/**********************************************************************************
 * @Project: Trigger
 * @Package: TrigParticleTPCnv
 * @Class  : TrigElectronContainerCnv_tlp2
 *
 * @brief "top level" transient-persistent converter for TrigElectronContainer
 *
 * @author Andrew Hamilton  <Andrew.Hamilton@cern.ch>  - U. Geneva
 * @author Francesca Bucci  <f.bucci@cern.ch>          - U. Geneva
 * @author Ricardo Goncalo  <r.goncalo@rhul.ac.uk>     - Royal Holloway 
 *
 * File and Version Information:
 * $Id: TrigElectronContainerCnv_tlp2.h,v 1.2 2009-04-01 22:13:30 salvator Exp $
 **********************************************************************************/
#ifndef TRIGPARTICLETPCNV_TRIGELECTRONCONTAINERCNV_TLP2_H
#define TRIGPARTICLETPCNV_TRIGELECTRONCONTAINERCNV_TLP2_H

#include "AthenaPoolCnvSvc/AthenaPoolTopLevelTPConverter.h"

#include "TrigParticleTPCnv/TrigElectronContainerCnv_p2.h"
#include "TrigParticleTPCnv/TrigElectronContainer_tlp2.h"
#include "EventCommonTPCnv/P4PtEtaPhiMCnv_p1.h"

class TrigElectronContainerCnv_tlp2 : public AthenaPoolTopLevelTPConverter<TrigElectronContainerCnv_p2, TrigElectronContainer_tlp2 >
{

 public:

  TrigElectronContainerCnv_tlp2();
  virtual ~TrigElectronContainerCnv_tlp2() {}

  virtual void setPStorage( TrigElectronContainer_tlp2 *storage );

 protected:

  //tp converters for ALL types used in TrigElectronContainer
  TrigElectronContainerCnv_p2  m_trigElectronContainerCnv;
  TrigElectronCnv_p2           m_trigElectronCnv;
  P4PtEtaPhiMCnv_p1            m_p4PtEtaPhiMCnv;

};



#endif
