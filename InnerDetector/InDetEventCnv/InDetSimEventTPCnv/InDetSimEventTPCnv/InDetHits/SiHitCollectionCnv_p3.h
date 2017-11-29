/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef SIHITCOLLECTIONCNV_P3_H
#define SIHITCOLLECTIONCNV_P3_H

// SiHitCollectionCnv_p3, T/P separation of Si Hits
// author D.Costanzo <davide.costanzo@cern.ch>
// author O.Arnaez <olivier.arnaez@cern.ch>

#include "AthenaPoolCnvSvc/T_AthenaPoolTPConverter.h"
#include "InDetSimEvent/SiHitCollection.h"
#include "SiHitCollection_p3.h"


class SiHitCollectionCnv_p3 : public T_AthenaPoolTPCnvBase<SiHitCollection, SiHitCollection_p3>
{
 public:

  SiHitCollectionCnv_p3()  {};

  virtual SiHitCollection* createTransient(const SiHitCollection_p3* persObj, MsgStream &log);

  virtual void	persToTrans(const SiHitCollection_p3* persCont,
                            SiHitCollection* transCont,
                            MsgStream &log) ;
  virtual void	transToPers(const SiHitCollection* transCont,
                            SiHitCollection_p3* persCont,
                            MsgStream &log) ;

 private:

  static const double m_persEneUnit;
  static const double m_persLenUnit;
  static const double m_persAngUnit;
  static const double m_2bHalfMaximum;
  static const int m_2bMaximum;
};

#endif
