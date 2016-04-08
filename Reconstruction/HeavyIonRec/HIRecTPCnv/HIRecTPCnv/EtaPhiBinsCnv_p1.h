/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef RECTPCNV_ETAPHIBINSCNV_P1_H
#define RECTPCNV_ETAPHIBINSCNV_P1_H

/********************************************************************

NAME:     MissingEtCnv_p1.h
PACKAGE:  offline/Reconstruction/RecTPCnv

AUTHORS:  S. Resconi
CREATED:  Jul 2007

PURPOSE:  Transient/Persisten converter for EtaPhiBins class
********************************************************************/

#include "AthenaPoolCnvSvc/T_AthenaPoolTPConverter.h"
#include "HIRecTPCnv/EtaPhiBins_p1.h"
#include "HIGlobal/EtaPhiBins.h"

#include "TBuffer.h"

class MsgStream;

class EtaPhiBinsCnv_p1 : public T_AthenaPoolTPCnvBase<EtaPhiBins, EtaPhiBins_p1>
{
    public:
        EtaPhiBinsCnv_p1() {};
        virtual void persToTrans( const EtaPhiBins_p1    *persObj,
                                  EtaPhiBins             *transObj,
                                  MsgStream             &msg );

        virtual void transToPers( const EtaPhiBins       *transObj,
                                  EtaPhiBins_p1          *persObj,
                                  MsgStream             &msg );
};

template <>
class T_TPCnv<EtaPhiBins, EtaPhiBins_p1 >
  : public EtaPhiBinsCnv_p1
{
public:
};



#endif // RECTPCNV_ETAPHIBINSCNV_P1_H
