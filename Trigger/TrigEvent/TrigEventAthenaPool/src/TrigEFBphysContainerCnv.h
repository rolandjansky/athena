/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/**********************************************************************************
 * @Project: Trigger
 * @Package: TrigEventAthenaPool
 * @Class  : TrigEFBphysContainerCnv
 *
 * @brief transient persistent converter for TrigEFBphysContainer
 *
 * @author Andrew Hamilton  <Andrew.Hamilton@cern.ch>  - U. Geneva
 * @author Francesca Bucci  <F.Bucci@cern.ch>          - U. Geneva
 *
 * File and Version Information:
 * $Id: TrigEFBphysContainerCnv.h,v 1.2 2009-02-23 18:59:19 ssnyder Exp $
 * $Id: TrigEFBphysContainerCnv.h,v 1.3 2010-08-12 demelian Exp $
 **********************************************************************************/
#ifndef TRIGEFBPHYSCONTAINER_CNV_H
#define TRIGEFBPHYSCONTAINER_CNV_H

#include "GaudiKernel/MsgStream.h"

#include "AthenaPoolCnvSvc/T_AthenaPoolCustomCnv.h"

#include "TrigParticle/TrigEFBphysContainer.h"
class TrigEFBphysContainer_tlp2;

//-----------------------------------------------------------------------------
// Base class definition
//-----------------------------------------------------------------------------
typedef TrigEFBphysContainer_tlp2   TrigEFBphysContainer_PERS;
 
typedef T_AthenaPoolCustomCnv<TrigEFBphysContainer, TrigEFBphysContainer_PERS> TrigEFBphysContainerCnvBase;
 
//-----------------------------------------------------------------------------
// Converter for TrigEFBphysCollection object
//-----------------------------------------------------------------------------
struct TrigEFBphysContainerCnv_impl;
class TrigEFBphysContainerCnv : public TrigEFBphysContainerCnvBase
 {
  
  friend class CnvFactory<TrigEFBphysContainerCnv>;
 
  protected:
   
  TrigEFBphysContainerCnv( ISvcLocator *svcloc );
  ~TrigEFBphysContainerCnv();
  
  
   virtual TrigEFBphysContainer_PERS *createPersistent( TrigEFBphysContainer *transObj);
   virtual TrigEFBphysContainer      *createTransient();
   
  private:
   TrigEFBphysContainerCnv_impl* m_impl;
 };//end of class definitions
  
 
#endif 
