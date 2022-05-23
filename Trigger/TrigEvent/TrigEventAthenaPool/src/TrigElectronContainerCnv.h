/*
  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRIGEVENTATHENAPOOL_TRIGELECTRONCONTAINERCNV_H
#define TRIGEVENTATHENAPOOL_TRIGELECTRONCONTAINERCNV_H

#include "GaudiKernel/MsgStream.h"
// transient class
#include "AthenaPoolCnvSvc/T_AthenaPoolCustomCnv.h"
#include "TrigParticle/TrigElectronContainer.h"
#include "TrigParticleTPCnv/TrigElectronContainer_p3.h"
#include "TrigParticleTPCnv/TrigElectronContainerCnv_tlp1.h"
#include "TrigParticleTPCnv/TrigElectronContainerCnv_tlp2.h"
#include "TrigParticleTPCnv/TrigElectronContainerCnv_p3.h"

typedef TrigElectronContainer_p3  TrigElectronContainer_PERS;
typedef T_AthenaPoolCustomCnv<TrigElectronContainer, TrigElectronContainer_PERS> TrigElectronContainerCnvBase;

class TrigElectronContainerCnv  : public TrigElectronContainerCnvBase { 

  friend class CnvFactory<TrigElectronContainerCnv>;

  protected:
  
public:
   TrigElectronContainerCnv(ISvcLocator* svcloc): TrigElectronContainerCnvBase(svcloc){}
protected:

   virtual TrigElectronContainer_PERS*  createPersistent(TrigElectronContainer* transCont);
   virtual TrigElectronContainer*       createTransient ();

private:
  TrigElectronContainerCnv_tlp1  m_converter_tlp1;
  TrigElectronContainerCnv_tlp2  m_converter_tlp2;
  TrigElectronContainerCnv_p3    m_converter;


};


#endif
