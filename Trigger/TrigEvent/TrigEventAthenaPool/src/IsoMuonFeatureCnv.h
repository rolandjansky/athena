/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/**********************************************************************************
 * @Project: Trigger
 * @Package: TrigEventAthenaPool
 * @Class  : IsoMuonFeatureCnv
 *
 * @brief transient persistent converter for IsoMuonFeature
 *
 * @author Stefano Giagu  <Stefano.Giagu@cern.ch>  - U. of Rome
 *
 * File and Version Information:
 * $Id: IsoMuonFeatureCnv.h,v 1.2 2009-02-23 18:59:19 ssnyder Exp $
 **********************************************************************************/
#ifndef TRIGEVENTATHENAPOOL_ISOMUONFEATURECNV_H
#define TRIGEVENTATHENAPOOL_ISOMUONFEATURECNV_H
 
class MsgStream;
 
#include "AthenaPoolCnvSvc/T_AthenaPoolCustomCnv.h"
 
#include "TrigMuonEvent/IsoMuonFeature.h"
class IsoMuonFeature_tlp1;
class IsoMuonFeatureCnv_tlp1;
 
 
//-----------------------------------------------------------------------------
// Base class definition
//-----------------------------------------------------------------------------
typedef IsoMuonFeature_tlp1   IsoMuonFeature_PERS;
 
typedef T_AthenaPoolCustomCnv<IsoMuonFeature, IsoMuonFeature_PERS>
         IsoMuonFeatureCnvBase;
 
 //-----------------------------------------------------------------------------
 // Converter for IsoMuonFeature object
 //-----------------------------------------------------------------------------
 class IsoMuonFeatureCnv : public IsoMuonFeatureCnvBase {
 friend class CnvFactory<IsoMuonFeatureCnv>;
 
 protected:
 IsoMuonFeatureCnv( ISvcLocator *svcloc );
 ~IsoMuonFeatureCnv();
    
   virtual IsoMuonFeature_PERS* createPersistent( IsoMuonFeature *transObj);
   virtual IsoMuonFeature* createTransient();
 
 
 private:
 
   IsoMuonFeatureCnv_tlp1*   m_TPConverter;
 };
 
#endif // ISOMUONFEATURE_CNV_H
