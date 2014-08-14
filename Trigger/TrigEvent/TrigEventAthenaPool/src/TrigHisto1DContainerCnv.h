/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/**********************************************************************************
 * @Project: Trigger
 * @Package: TrigEventAthenaPool
 * @Class  : TrigHisto1DContainerCnv
 *
 * @brief transient persistent converter for TrigHisto1DContainer
 *
 * @author W. H. Bell  <W.Bell@cern.ch>
 *
 **********************************************************************************/
#ifndef TRIGEVENTATHENAPOOL_TRIGHISTO1DCONTAINER_CNV_H
#define TRIGEVENTATHENAPOOL_TRIGHISTO1DCONTAINER_CNV_H
 
 
#include "GaudiKernel/MsgStream.h"
 
#include "AthenaPoolCnvSvc/T_AthenaPoolCustomCnv.h"
 
#include "TrigInDetEvent/TrigHisto1DContainer.h"
class  TrigHisto1DContainer_tlp1;

//-----------------------------------------------------------------------------
// Base class definition
//-----------------------------------------------------------------------------
typedef TrigHisto1DContainer_tlp1   TrigHisto1DContainer_PERS;
 
typedef T_AthenaPoolCustomCnv<TrigHisto1DContainer, TrigHisto1DContainer_PERS> TrigHisto1DContainerCnvBase;
 
//-----------------------------------------------------------------------------
// Converter for TrigHisto1DCollection object
//-----------------------------------------------------------------------------
class TrigHisto1DContainerCnv_tlp1;
class TrigHisto1DContainerCnv : public TrigHisto1DContainerCnvBase
 {
  
  friend class CnvFactory<TrigHisto1DContainerCnv>;
 
  protected:
   
  TrigHisto1DContainerCnv( ISvcLocator *svcloc );
  ~TrigHisto1DContainerCnv();
   
   virtual TrigHisto1DContainer_PERS *createPersistent( TrigHisto1DContainer *transObj);
   virtual TrigHisto1DContainer      *createTransient();
  
 private:
   
   TrigHisto1DContainerCnv_tlp1* m_TPConverter;

  };//end of class definitions
  
 
 #endif //TRIGHISTO1DCONTAINER_CNV_H
