/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/// @file TauRecExtraDetailsCnv_p1.h
///
/// Declaration of persistent/transient TauRecExtraDetails converter
///
/// Package: Reconstruction/tauEventTPCnv
///
/// @author Thomas Burgess
/// @author Lukasz Janyst
///
/// Created on 2007-07-07.
///
/// $Id: TauRecExtraDetailsCnv_p1.h,v 1.5 2009-01-20 17:19:02 tburgess Exp $

#ifndef TAUEVENTTPCNV_TAURECEXTRADETAILSCNV_P1_H
#define TAUEVENTTPCNV_TAURECEXTRADETAILSCNV_P1_H

#include "AthenaPoolCnvSvc/T_AthenaPoolTPConverter.h"
#include "tauEvent/TauDetails.h"
#include "tauEvent/TauRecExtraDetails.h"
#include "tauEventTPCnv/TauRecExtraDetails_p1.h"
#include "tauEventTPCnv/tauAnalysisHelperObjectCnv_p1.h"
class MsgStream;

///Legacy converter
///
///Converts between persistent and transient TauRecExtraDetails 
class TauRecExtraDetailsCnv_p1:
    public T_AthenaPoolTPPolyCnvBase<
    Analysis::TauDetails,
    Analysis::TauRecExtraDetails,
    TauRecExtraDetails_p1 >
{
public:
    ///Constructor
    TauRecExtraDetailsCnv_p1();

    ///Destructor
    virtual ~TauRecExtraDetailsCnv_p1();

    ///Fills persistent TauRecExtraDetails instance from a
    ///transient tauRecExtraDetails instance
    ///
    ///@param pers Persistent object
    ///@param trans Transient object
    ///@param msg Message stream
    void persToTrans(
	const TauRecExtraDetails_p1 *pers,
	Analysis::TauRecExtraDetails *trans,
	MsgStream &msg );

    ///DEPRECATED DO NOT USE!
    ///
    ///Fills transient TauRecDetails instance from a
    ///persistent tauRecDetails instance
    ///
    ///@param trans Transient object
    ///@param pers Persistent object
    ///@param msg Message stream
    virtual void transToPers(
	const Analysis::TauRecExtraDetails *trans,
	TauRecExtraDetails_p1 *pers,
	MsgStream &msg );
    
private:
    tauAnalysisHelperObjectCnv_p1 *m_tauAnalysisHelperObjectCnv;
};

#endif // TAUEVENTTPCNV_TAURECEXTRADETAILSCNV_P1_H
