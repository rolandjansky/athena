/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/**********************************************************************************
 * @Project: Trigger
 * @Package: TrigEventAthenaPool
 * @Class  : CombinedMuonFeatureCnv
 *
 * @brief transient persistent converter for CombinedMuonFeature
 *
 * @author Andrew Hamilton  <Andrew.Hamilton@cern.ch>  - U. Geneva
 * @author Francesca Bucci  <F.Bucci@cern.ch>          - U. Geneva
 *
 * File and Version Information:
 * $Id: CombinedMuonFeatureCnv.h,v 1.2 2009-02-23 18:59:19 ssnyder Exp $
 **********************************************************************************/
#ifndef TRIGEVENTATHENAPOOL_COMBINEDMUONFEATURECNV_H
#define TRIGEVENTATHENAPOOL_COMBINEDMUONFEATURECNV_H
 
class MsgStream;
 
#include "AthenaPoolCnvSvc/T_AthenaPoolCustomCnv.h"
 
#include "TrigMuonEvent/CombinedMuonFeature.h"

class CombinedMuonFeature_tlp1;
class CombinedMuonFeatureCnv_tlp1;
 
 
//-----------------------------------------------------------------------------
// Base class definition
//-----------------------------------------------------------------------------
typedef CombinedMuonFeature_tlp1   CombinedMuonFeature_PERS;
 
typedef T_AthenaPoolCustomCnv<CombinedMuonFeature, CombinedMuonFeature_PERS>
         CombinedMuonFeatureCnvBase;
 
 //-----------------------------------------------------------------------------
 // Converter for CombinedMuonFeature object
 //-----------------------------------------------------------------------------
 class CombinedMuonFeatureCnv : public CombinedMuonFeatureCnvBase {
 friend class CnvFactory<CombinedMuonFeatureCnv>;
 
 protected:
 CombinedMuonFeatureCnv( ISvcLocator *svcloc );
 ~CombinedMuonFeatureCnv();
    
   virtual CombinedMuonFeature_PERS* createPersistent( CombinedMuonFeature *transObj);
   virtual CombinedMuonFeature* createTransient();
 
 
 private:
 
   CombinedMuonFeatureCnv_tlp1*   m_TPConverter;
 };
 
 #endif // COMBINEDMUONFEATURE_CNV_H
