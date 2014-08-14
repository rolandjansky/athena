/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/**********************************************************************************
 * @Project: Trigger
 * @Package: TrigEventAthenaPool
 * @Class  : TrigT2JetCnv
 *
 * @brief transient persistent converter for TrigT2Jet
 *
 * @author Andrew Hamilton  <Andrew.Hamilton@cern.ch>  - U. Geneva
 * @author Francesca Bucci  <F.Bucci@cern.ch>          - U. Geneva
 *
 * File and Version Information:
 * $Id: TrigT2JetCnv.h,v 1.2 2009-02-23 18:59:20 ssnyder Exp $
 **********************************************************************************/
#ifndef TRIGEVENTATHENAPOOL_TRIGT2JET_CNV_H
#define TRIGEVENTATHENAPOOL_TRIGT2JET_CNV_H
 
 
#include "GaudiKernel/MsgStream.h"
#include "AthenaPoolCnvSvc/T_AthenaPoolCustomCnv.h"
#include "TrigCaloEvent/TrigT2Jet.h"
class TrigT2Jet_tlp1;
 
 
//-----------------------------------------------------------------------------
// Base class definition
//-----------------------------------------------------------------------------
typedef TrigT2Jet_tlp1   TrigT2Jet_PERS;
 
typedef T_AthenaPoolCustomCnv<TrigT2Jet, TrigT2Jet_PERS> TrigT2JetCnvBase;
 
//-----------------------------------------------------------------------------
// Converter for TrigPhotonCollection object
//-----------------------------------------------------------------------------
class TrigT2JetCnv_tlp1;
class TrigT2JetCnv : public TrigT2JetCnvBase
 {
  
  friend class CnvFactory<TrigT2JetCnv>;
 
  protected:
   
  TrigT2JetCnv( ISvcLocator *svcloc );
  ~TrigT2JetCnv();
 
   
   virtual TrigT2Jet_PERS *createPersistent( TrigT2Jet *transObj);
   virtual TrigT2Jet      *createTransient();
 
  private:
 
   IMessageSvc             *m_msgSvc     ;
   MsgStream                m_log        ;
   
   TrigT2JetCnv_tlp1*        m_TPConverter;
 
  };//end of class definitions
  
 
 #endif //TRIGT2JET_CNV_H
