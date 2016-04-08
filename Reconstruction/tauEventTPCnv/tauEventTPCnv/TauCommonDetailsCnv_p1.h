/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/// @file TauCommonDetailsCnv_p1.h
/// 
/// Implementation of transient<->persistent converter 
/// for Analysis::TauCommonDetails
/// 
/// Package: Reconstruction/tauEventTPCnv
/// 
/// @author Thomas Burgess  
/// 
/// Created by Thomas Burgess on 2008-11-04.
///
/// $Id: TauCommonDetailsCnv_p1.h,v 1.4 2009-01-27 17:38:43 tburgess Exp $
///

#ifndef TAUEVENTTPCNV_TAUCOMMONDETAILSCNV_P1_H
#define TAUEVENTTPCNV_TAUCOMMONDETAILSCNV_P1_H

#include "AthenaPoolCnvSvc/T_AthenaPoolTPConverter.h"
#include "tauEvent/TauCommonDetails.h"
#include "tauEventTPCnv/TauCommonDetails_p1.h"

class RecVertexCnv_p1;
class MsgStream;

///Transient Persistent class converter for Analysis::TauCommonDetails
class TauCommonDetailsCnv_p1 : 
    public T_AthenaPoolTPPolyCnvBase<
    Analysis::TauDetails,
    Analysis::TauCommonDetails,
    TauCommonDetails_p1>
{
public:
    ///Constructor
    TauCommonDetailsCnv_p1();
    
    ///Destructor
    virtual ~TauCommonDetailsCnv_p1();

    ///Fills a transient object from a persistent object
    ///
    ///@param pers Persistent object
    ///@param trans Transient object
    ///@param msg Message stream
    virtual void persToTrans( 
	const TauCommonDetails_p1 *pers,
	Analysis::TauCommonDetails *trans,
	MsgStream &msg );


    ///Fills a persistent object from a transient object   
    ///
    ///@param trans Transient object
    ///@param pers Persistent object
    ///@param msg Message stream
    virtual void transToPers( 
	const Analysis::TauCommonDetails *trans,
	TauCommonDetails_p1 *pers,
	MsgStream &msg );

private:
    ///Reconstruction vertex converter
    RecVertexCnv_p1 *m_recVertexCnv;
};

#endif // TAUEVENTTPCNV_TAUCOMMONDETAILSCNV_P1_H
