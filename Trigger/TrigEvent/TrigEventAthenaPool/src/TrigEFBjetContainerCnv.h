/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/**********************************************************************************
 * @Project: Trigger
 * @Package: TrigEventAthenaPool
 * @Class  : TrigEFBjetContainerCnv
 *
 * @brief transient persistent converter for TrigEFBjetContainer
 *
 * @author Andrew Hamilton  <Andrew.Hamilton@cern.ch>  - U. Geneva
 * @author Francesca Bucci  <F.Bucci@cern.ch>          - U. Geneva
 *
 * File and Version Information:
 * $Id: TrigEFBjetContainerCnv.h,v 1.3 2009-02-23 18:59:19 ssnyder Exp $
 **********************************************************************************/
#ifndef TRIGEFBJETCONTAINER_CNV_H
#define TRIGEFBJETCONTAINER_CNV_H


class MsgStream;

#include "AthenaPoolCnvSvc/T_AthenaPoolCustomCnv.h"

#include "TrigParticle/TrigEFBjetContainer.h"
class TrigEFBjetContainer_tlp2;

//-----------------------------------------------------------------------------
// typedef to the latest top level persistent version
//-----------------------------------------------------------------------------
typedef TrigEFBjetContainer_tlp2   TrigEFBjetContainer_PERS;
 
typedef T_AthenaPoolCustomCnv<TrigEFBjetContainer, TrigEFBjetContainer_PERS> TrigEFBjetContainerCnvBase;
 
//-----------------------------------------------------------------------------
// Converter for TrigEFBjetCollection object
//-----------------------------------------------------------------------------
struct TrigEFBjetContainerCnv_impl;
class TrigEFBjetContainerCnv : public TrigEFBjetContainerCnvBase
 {
  
  friend class CnvFactory<TrigEFBjetContainerCnv>;
 
  protected:
   
  TrigEFBjetContainerCnv( ISvcLocator *svcloc );
  ~TrigEFBjetContainerCnv();
  
  
   virtual TrigEFBjetContainer_PERS *createPersistent( TrigEFBjetContainer *transObj);
   virtual TrigEFBjetContainer      *createTransient();
   
  private:
   TrigEFBjetContainerCnv_impl* m_impl;   
 };//end of class definitions
  
 
#endif 
