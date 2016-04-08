/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/// @file TauRecDetailsCnv_p1.h
///
/// Declaration of persistent to transient TauRecDetails converter
///
/// Package: Reconstruction/tauEventTPCnv
///
/// @author Thomas Burgess
/// @author Lukasz Janyst
///
/// Created on 2007-07-07.
///
/// $Id: TauRecDetailsCnv_p1.h,v 1.3 2009-01-20 17:19:02 tburgess Exp $

#ifndef TAUEVENTTPCNV_TAURECDETAILSCNV_P1_H
#define TAUEVENTTPCNV_TAURECDETAILSCNV_P1_H

#include "AthenaPoolCnvSvc/T_AthenaPoolTPConverter.h"
#include "TrkEventTPCnv/VxVertex/RecVertexCnv_p1.h"
#include "tauEvent/TauDetails.h"
#include "tauEvent/TauRecDetails.h"
#include "tauEventTPCnv/TauRecDetails_p1.h"
class RecVertexCnv_p1;
class MsgStream;

///Legacy TauRecDetails converter
///
///Converts a persistent TauRecDetails instance to a
///transient TauRecDetails isntance
class TauRecDetailsCnv_p1:
    public T_AthenaPoolTPPolyCnvBase<
    Analysis::TauDetails,
    Analysis::TauRecDetails,
    TauRecDetails_p1 >
{
public:
    ///Default constructor
    TauRecDetailsCnv_p1();

    ///Destructor
    virtual ~TauRecDetailsCnv_p1();

    ///Fills transient TauRecDetails instance from a
    ///persistent tauRecDetails instance
    ///
    ///@param trans Transient object
    ///@param pers Persistent object
    ///@param msg Message stream
    virtual void persToTrans(
	const TauRecDetails_p1 *pers,
	Analysis::TauRecDetails *trans,
	MsgStream &msg );

    ///DEPRECATED DO NOT USE!
    ///
    ///Fills persistent TauRecDetails instance from a
    ///transient tauRecDetails instance
    ///
    ///@param trans Transient object
    ///@param pers Persistent object
    ///@param msg Message stream
    virtual void transToPers(
	const Analysis::TauRecDetails *trans,
	TauRecDetails_p1 *pers,
	MsgStream &msg );

private:
    ///Reconstruction vertex converter
    RecVertexCnv_p1 *m_recVertexCnv;
};

#endif // TAUEVENTTPCNV_TAURECDETAILSCNV_P1_H
