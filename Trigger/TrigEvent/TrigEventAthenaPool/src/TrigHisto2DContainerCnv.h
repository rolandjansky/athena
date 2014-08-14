/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/**********************************************************************************
 * @Project: Trigger
 * @Package: TrigEventAthenaPool
 * @Class  : TrigHisto2DContainerCnv
 *
 * @brief transient persistent converter for TrigHisto2DContainer
 *
 * @author W. H. Bell  <W.Bell@cern.ch>
 *
 **********************************************************************************/
#ifndef TRIGEVENTATHENAPOOL_TRIGHISTO2DCONTAINER_CNV_H
#define TRIGEVENTATHENAPOOL_TRIGHISTO2DCONTAINER_CNV_H
 
 
#include "GaudiKernel/MsgStream.h"
 
#include "AthenaPoolCnvSvc/T_AthenaPoolCustomCnv.h"
 
#include "TrigInDetEvent/TrigHisto2DContainer.h"
class TrigHisto2DContainer_tlp1;
 
//-----------------------------------------------------------------------------
// Base class definition
//-----------------------------------------------------------------------------
typedef TrigHisto2DContainer_tlp1   TrigHisto2DContainer_PERS;
 
typedef T_AthenaPoolCustomCnv<TrigHisto2DContainer, TrigHisto2DContainer_PERS> TrigHisto2DContainerCnvBase;
 
//-----------------------------------------------------------------------------
// Converter for TrigHisto2DCollection object
//-----------------------------------------------------------------------------
class TrigHisto2DContainerCnv_tlp1;
class TrigHisto2DContainerCnv : public TrigHisto2DContainerCnvBase
 {
  
  friend class CnvFactory<TrigHisto2DContainerCnv>;
 
  protected:
   
  TrigHisto2DContainerCnv( ISvcLocator *svcloc );
  ~TrigHisto2DContainerCnv();
   
   virtual TrigHisto2DContainer_PERS *createPersistent( TrigHisto2DContainer *transObj);
   virtual TrigHisto2DContainer      *createTransient();
  
 private:
   
   TrigHisto2DContainerCnv_tlp1* m_TPConverter;

  };//end of class definitions
  
 
 #endif //TRIGHISTO2DCONTAINER_CNV_H
