/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/**********************************************************************************
 * @Project: Trigger
 * @Package: TrigEventAthenaPool
 * @Class  : TrigRoiDescriptorCnv
 *
 * @brief transient persistent converter for TrigRoiDescriptor
 *
 * @author Andrew Hamilton  <Andrew.Hamilton@cern.ch>  - U. Geneva
 * @author Francesca Bucci  <f.bucci@cern.ch>          - U. Geneva
 *
 * File and Version Information:
 * $Id: 
 **********************************************************************************/
#ifndef TRIGEVENTATHENAPOOL_TRIGROIDESCRIPTORCNV_H
#define TRIGEVENTATHENAPOOL_TRIGROIDESCRIPTORCNV_H


#include "TrigSteeringEvent/TrigRoiDescriptor.h"
#include "AthenaPoolCnvSvc/T_AthenaPoolCustomCnv.h"
class TrigRoiDescriptor_p3;

// typedef to the latest persistent version
typedef TrigRoiDescriptor_p3  TrigRoiDescriptor_PERS;

class MsgStream;

struct TrigRoiDescriptorCnv_tp;
class TrigRoiDescriptorCnv : public T_AthenaPoolCustomCnv<TrigRoiDescriptor, TrigRoiDescriptor_PERS > 
{

  friend class CnvFactory<TrigRoiDescriptorCnv>;

 protected:
  
  TrigRoiDescriptorCnv(ISvcLocator* svcloc);
  ~TrigRoiDescriptorCnv();

  TrigRoiDescriptor_PERS*  createPersistent(TrigRoiDescriptor* transObj);

  TrigRoiDescriptor*       createTransient ();

 private:

  TrigRoiDescriptorCnv_tp *m_TPConverters;

};


#endif
