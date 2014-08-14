/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/**********************************************************************************
 * @Project: Trigger
 * @Package: TrigEventAthenaPool
 * @Class  : TrigL2BphysContainerCnv
 *
 * @brief transient persistent converter for TrigL2BphysContainer
 *
 * @author Andrew Hamilton  <Andrew.Hamilton@cern.ch>  - U. Geneva
 * @author Francesca Bucci  <F.Bucci@cern.ch>          - U. Geneva
 *
 * File and Version Information:
 * $Id: TrigL2BphysContainerCnv.h,v 1.3 2009-02-23 18:59:19 ssnyder Exp $
 * $Id: TrigL2BphysContainerCnv.h,v 1.4 2010-08-11 demelian Exp $
 **********************************************************************************/
#ifndef TRIGEVENTATHENAPOOL_TRIGL2BPHYSCONTAINER_CNV_H
#define TRIGEVENTATHENAPOOL_TRIGL2BPHYSCONTAINER_CNV_H
 
 
#include "GaudiKernel/MsgStream.h"
 
#include "AthenaPoolCnvSvc/T_AthenaPoolCustomCnv.h"
 
#include "TrigParticle/TrigL2BphysContainer.h"
class TrigL2BphysContainer_tlp2;
 
 
//-----------------------------------------------------------------------------
// Base class definition
//-----------------------------------------------------------------------------
typedef TrigL2BphysContainer_tlp2   TrigL2BphysContainer_PERS;
 
typedef T_AthenaPoolCustomCnv<TrigL2BphysContainer, TrigL2BphysContainer_PERS> TrigL2BphysContainerCnvBase;
 
//-----------------------------------------------------------------------------
// Converter for TrigL2BphysCollection object
//-----------------------------------------------------------------------------
class TrigL2BphysContainerCnv_impl;
class TrigL2BphysContainerCnv : public TrigL2BphysContainerCnvBase
 {
  
  friend class CnvFactory<TrigL2BphysContainerCnv>;
 
  protected:
   
  TrigL2BphysContainerCnv( ISvcLocator *svcloc );

  ~TrigL2BphysContainerCnv();
 
   
   virtual TrigL2BphysContainer_PERS *createPersistent( TrigL2BphysContainer *transObj);
   virtual TrigL2BphysContainer      *createTransient();
 

  private:

   TrigL2BphysContainerCnv_impl  *m_impl;
 
  };//end of class definitions
  
 
 #endif //TRIGPHOTONCONTAINER_CNV_H
