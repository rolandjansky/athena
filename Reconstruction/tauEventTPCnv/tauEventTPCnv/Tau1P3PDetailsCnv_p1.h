/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/// @file Tau1P3PDetailsCnv_p1.h
///
/// Declaration of persistent to transient Tau1P3PDetails converter
///
/// Package: Reconstruction/tauEventTPCnv
///
/// @author Thomas Burgess
/// @author Lukasz Janyst
///
/// Created on 2007-07-07.
///
/// $Id: Tau1P3PDetailsCnv_p1.h,v 1.3 2009-01-20 17:19:02 tburgess Exp $

#ifndef TAUEVENTTPCNV_TAU1P3PDETAILSCNV_P1_H
#define TAUEVENTTPCNV_TAU1P3PDETAILSCNV_P1_H

#include "AthenaPoolCnvSvc/T_AthenaPoolTPConverter.h"
#include "tauEvent/TauDetails.h"
#include "tauEvent/Tau1P3PDetails.h"
#include "tauEventTPCnv/Tau1P3PDetails_p1.h"
class RecVertexCnv_p1;
class MsgStream;

///Legacy converter
///
///Converts a persistent Tau1P3PDetails instance to a
///transient Tau1P3PDetails instance
class Tau1P3PDetailsCnv_p1:
    public T_AthenaPoolTPPolyCnvBase<
    Analysis::TauDetails,
    Analysis::Tau1P3PDetails,
    Tau1P3PDetails_p1 >
{
public:
    ///Default constructor
    Tau1P3PDetailsCnv_p1();

    ///Destructor
    virtual ~Tau1P3PDetailsCnv_p1();

    ///Fills transient Tau1P3PDetails instance from a
    ///transient tau1P3PDetails instance
    ///
    ///@param pers Persistent object
    ///@param trans Transient object
    ///@param msg Message stream
    virtual void persToTrans(
	const Tau1P3PDetails_p1 *pers,
	Analysis::Tau1P3PDetails *trans,
	MsgStream &msg );

    ///DEPRECATED DO NOT USE!
    ///
    ///Fills transient Tau1P3PDetails instance from a
    ///transient tau1P3PDetails instance
    ///
    ///@param tau1P3PDetails_t Transient object
    ///@param tau1P3PDetails_p Persistent object
    ///@param msg Message stream
    virtual void transToPers(
	const Analysis::Tau1P3PDetails *trans,
	Tau1P3PDetails_p1 *pers,
	MsgStream &msg );

private:
    ///Converter for rec vertex
    RecVertexCnv_p1 *m_recVertexCnv;
};

#endif // TAUEVENTTPCNV_TAU1P3PDETAILSCNV_P1_H
