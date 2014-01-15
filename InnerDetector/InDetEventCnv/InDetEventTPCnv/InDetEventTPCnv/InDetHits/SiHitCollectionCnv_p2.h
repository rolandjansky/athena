/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef SIHITCOLLECTIONCNV_P2_H
#define SIHITCOLLECTIONCNV_P2_H

// SiHitCollectionCnv_p2, T/P separation of Si Hits
// author D.Costanzo <davide.costanzo@cern.ch>

#include "AthenaPoolCnvSvc/T_AthenaPoolTPConverter.h"
#include "InDetSimEvent/SiHitCollection.h"
#include "SiHitCollection_p2.h"


class SiHitCollectionCnv_p2 : public T_AthenaPoolTPCnvBase<SiHitCollection, SiHitCollection_p2>
{
 public:
  
  SiHitCollectionCnv_p2()  {};
  
  virtual SiHitCollection* createTransient(const SiHitCollection_p2* persObj, MsgStream &log);

  virtual void	persToTrans(const SiHitCollection_p2* persCont,
			    SiHitCollection* transCont,
			    MsgStream &log) ;
  virtual void	transToPers(const SiHitCollection* transCont,
			    SiHitCollection_p2* persCont,
			    MsgStream &log) ;

 private:

  static const double m_persEneUnit;
  static const double m_persLenUnit;
  static const double m_persAngUnit;
  static const double m_2bHalfMaximum;
  static const int m_2bMaximum;
};

#endif
