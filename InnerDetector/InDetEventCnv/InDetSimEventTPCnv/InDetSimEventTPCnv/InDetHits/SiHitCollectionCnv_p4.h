/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#ifndef SIHITCOLLECTIONCNV_P4_H
#define SIHITCOLLECTIONCNV_P4_H

// SiHitCollectionCnv_p4, T/P separation of Si Hits
// author D.Costanzo <davide.costanzo@cern.ch>
// author O.Arnaez <olivier.arnaez@cern.ch>

#include "AthenaPoolCnvSvc/T_AthenaPoolTPConverter.h"
#include "InDetSimEvent/SiHitCollection.h"
#include "SiHitCollection_p4.h"


class SiHitCollectionCnv_p4 : public T_AthenaPoolTPCnvBase<SiHitCollection, SiHitCollection_p4>
{
 public:

  SiHitCollectionCnv_p4()  {};

  virtual SiHitCollection* createTransient(const SiHitCollection_p4* persObj, MsgStream &log);

  virtual void	persToTrans(const SiHitCollection_p4* persCont,
                            SiHitCollection* transCont,
                            MsgStream &log) ;
  virtual void	transToPers(const SiHitCollection* transCont,
                            SiHitCollection_p4* persCont,
                            MsgStream &log) ;

 private:

  static const double m_persEneUnit;
  static const double m_persLenUnit;
  static const double m_persAngUnit;
  static const double m_2bHalfMaximum;
  static const int m_2bMaximum;
};

#endif
