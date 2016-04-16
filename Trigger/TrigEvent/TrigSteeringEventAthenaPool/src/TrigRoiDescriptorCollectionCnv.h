/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/**********************************************************************************
 * @Project: Trigger
 * @Package: TrigEventAthenaPool
 * @Class  : TrigRoiDescriptorCollectionCnv
 *
 * @brief transient persistent converter for TrigRoiDescriptorCollection
 *
 * @author Andrew Hamilton  <Andrew.Hamilton@cern.ch>  - U. Geneva
 * @author Francesca Bucci  <f.bucci@cern.ch>          - U. Geneva
 *
 * File and Version Information:
 * $Id: 
 **********************************************************************************/
#ifndef TRIGEVENTATHENAPOOL_TRIGROIDESCRIPTORCOLLECTIONCNV_H
#define TRIGEVENTATHENAPOOL_TRIGROIDESCRIPTORCOLLECTIONCNV_H


#include "TrigSteeringEvent/TrigRoiDescriptorCollection.h"
#include "AthenaPoolCnvSvc/T_AthenaPoolCustomCnv.h"
class TrigRoiDescriptorCollection_p3;


// typedef to the latest persistent version
typedef TrigRoiDescriptorCollection_p3  TrigRoiDescriptorCollection_PERS;

class MsgStream;

struct TrigRoiDescriptorCollectionCnv_impl;
class TrigRoiDescriptorCollectionCnv : public T_AthenaPoolCustomCnv<TrigRoiDescriptorCollection, TrigRoiDescriptorCollection_PERS > 
{

  friend class CnvFactory<TrigRoiDescriptorCollectionCnv>;

 protected:
  
  TrigRoiDescriptorCollectionCnv(ISvcLocator* svcloc);
  ~TrigRoiDescriptorCollectionCnv();

  TrigRoiDescriptorCollection_PERS*  createPersistent(TrigRoiDescriptorCollection* transObj);

  TrigRoiDescriptorCollection*       createTransient ();

 private:
  TrigRoiDescriptorCollectionCnv_impl* m_impl;
};


#endif
