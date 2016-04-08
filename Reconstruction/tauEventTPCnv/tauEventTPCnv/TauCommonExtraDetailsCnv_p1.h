/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/// @file TauCommonExtraDetailsCnv_p1.h
/// 
/// Implementation of transient<->persistent converter 
/// for Analysis::TauCommonExtraDetails
/// 
/// Package: Reconstruction/tauEventTPCnv
/// 
/// @author Thomas Burgess  
/// 
/// Created by Thomas Burgess on 2008-11-04.
///
/// $Id: TauCommonExtraDetailsCnv_p1.h,v 1.3 2009-01-20 17:19:02 tburgess Exp $

#ifndef TAUEVENTTPCNV_TAUCOMMONEXTRADETAILSCNV_P1_H
#define TAUEVENTTPCNV_TAUCOMMONEXTRADETAILSCNV_P1_H

#include "AthenaPoolCnvSvc/T_AthenaPoolTPConverter.h"
#include "DataModelAthenaPool/ElementLinkCnv_p2.h"
#include "tauEvent/TauCommonExtraDetails.h"
#include "tauEventTPCnv/TauCommonExtraDetails_p1.h"

class MsgStream;

///Transient Persistent class converter for Analysis::TauCommonExtraDetails
class TauCommonExtraDetailsCnv_p1 : 
    public T_AthenaPoolTPPolyCnvBase<Analysis::TauDetails, Analysis::TauCommonExtraDetails, TauCommonExtraDetails_p1>
{
public:
    ///Constructor
    TauCommonExtraDetailsCnv_p1();
    
    ///Destructor
    virtual ~TauCommonExtraDetailsCnv_p1();

    ///Creates persistent object from transient object
    ///
    ///@param transObj Transient object
    ///@param persObj Persistent object
    ///@param msg Message stream
    virtual void persToTrans( 
	const TauCommonExtraDetails_p1 *pers,
	Analysis::TauCommonExtraDetails *trans,
	MsgStream &msg );

    ///Creates transient object from persistent object
    ///
    ///@param transObj Transient object
    ///@param persObj Persistent object
    ///@param msg Message stream
    virtual void transToPers( 
	const Analysis::TauCommonExtraDetails *trans,
	TauCommonExtraDetails_p1 *pers,
	MsgStream &msg );

private:
    ElementLinkCnv_p2<ElementLink<CaloCellContainer> > m_cellCnv;
};

#endif // TAUEVENTTPCNV_TAUCOMMONEXTRADETAILSCNV_P1_H
