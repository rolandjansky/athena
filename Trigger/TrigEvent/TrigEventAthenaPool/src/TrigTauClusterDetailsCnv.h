/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/**********************************************************************************
 * @Project: Trigger
 * @Package: TrigEventAthenaPool
 * @Class  : TrigTauClusterDetailsCnv
 *
 * @brief transient persistent converter for TrigTauClusterDetails
 *
 * @author Andrew Hamilton  <Andrew.Hamilton@cern.ch>  - U. Geneva
 * @author Francesca Bucci  <f.bucci@cern.ch>          - U. Geneva
 *
 * File and Version Information:
 * $Id: TrigTauClusterDetailsCnv.h,v 1.2 2009-02-23 18:59:20 ssnyder Exp $
 **********************************************************************************/
#ifndef TRIGEVENTATHENAPOOL_TRIGTAUCLUSTERDETAILSCNV_H
#define TRIGEVENTATHENAPOOL_TRIGTAUCLUSTERDETAILSCNV_H

#include "TrigCaloEvent/TrigTauClusterDetails.h"
#include "AthenaPoolCnvSvc/T_AthenaPoolCustomCnv.h"
class TrigTauClusterDetails_tlp1;

// typedef to the latest persistent version
typedef TrigTauClusterDetails_tlp1  TrigTauClusterDetails_PERS;

class MsgStream;

class TrigTauClusterDetailsCnv_tlp1;
class TrigTauClusterDetailsCnv : public T_AthenaPoolCustomCnv<TrigTauClusterDetails, TrigTauClusterDetails_PERS > 
{

  friend class CnvFactory<TrigTauClusterDetailsCnv>;
  
 protected:
  TrigTauClusterDetailsCnv(ISvcLocator* svcloc);
  ~TrigTauClusterDetailsCnv();

  TrigTauClusterDetails_PERS*  createPersistent(TrigTauClusterDetails* transCont);

  TrigTauClusterDetails*       createTransient ();

 private:

  TrigTauClusterDetailsCnv_tlp1* m_TPConverter;

};


#endif
