/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/**********************************************************************************
 * @Project: Trigger
 * @Package: TrigEventAthenaPool
 * @class  : TrigCompositeCnv
 *
 * @brief transient persistent converter for TrigComposite
 *
 * @author Camille B.-Champagne  <camille.belanger-champagne@cern.ch> - McGill U.
 **********************************************************************************/
#ifndef TRIGEVENTATHENAPOOL_TRIGCOMBINEDEVENTCNV_H
#define TRIGEVENTATHENAPOOL_TRIGCOMBINEDEVENTCNV_H

#include "TrigCombinedEvent/TrigComposite.h"
#include "AthenaPoolCnvSvc/T_AthenaPoolCustomCnv.h"

// typedef to the latest persistent version
class TrigComposite_p1;
typedef TrigComposite_p1  TrigComposite_PERS;

class MsgStream;

struct TrigCompositeCnv_impl;
class TrigCompositeCnv  : public T_AthenaPoolCustomCnv<TrigComposite, TrigComposite_PERS > 
{

  friend class CnvFactory<TrigCompositeCnv>;

public:
  TrigCompositeCnv(ISvcLocator* svcloc);
  ~TrigCompositeCnv();

protected:
  TrigComposite_PERS*  createPersistent(TrigComposite* transCont);

  TrigComposite*       createTransient ();

 private:
  TrigCompositeCnv_impl* m_impl;
};


#endif
