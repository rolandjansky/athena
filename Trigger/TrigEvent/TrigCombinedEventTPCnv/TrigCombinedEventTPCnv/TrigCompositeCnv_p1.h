/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/**********************************************************************************
 * @Project: Trigger
 * @Package: TrigCombinedEventTPCnv
 * @Class  : TrigCompositeCnv_p1
 *
 * @brief transient persistent converter for TrigComposites
 *
 * @author Camille B.-Champagne  <camille.belanger-champagne@cern.ch> 
 **********************************************************************************/
#ifndef TRIGCOMBINEDEVENTTPCNV_TRIGCOMBINEDEVENTCNV_P1_H
#define TRIGCOMBINEDEVENTTPCNV_TRIGCOMBINEDEVENTCNV_P1_H

#include "AthenaPoolCnvSvc/T_AthenaPoolTPConverter.h"
	
#include "TrigCombinedEvent/TrigComposite.h"
#include "TrigCombinedEventTPCnv/TrigComposite_p1.h"
#include "TrigNavigation/TrigFeatureLink.h"

class MsgStream;

class TrigCompositeCnv_p1 : public T_AthenaPoolTPCnvBase<TrigComposite, TrigComposite_p1>  {
	   
 public:
  
  TrigCompositeCnv_p1() { }
  
  virtual void persToTrans(const TrigComposite_p1* persObj,  TrigComposite* transObj,   MsgStream& log);
  virtual void transToPers(const TrigComposite* transObj, TrigComposite_p1* persObj, MsgStream& log);

};
	
#endif
