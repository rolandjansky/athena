/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/// @file Tau1P3PExtraDetailsCnv_p1.h
///
/// Declaration of persistent Tau1P3PExtraDetails to transient
/// Tau1P3PExtraDetails converter
///
/// Package: Reconstruction/tauEventTPCnv
///
/// @author Thomas Burgess
/// @author Lukasz Janyst
///
/// Created on 2007-07-07.
///
/// $Id: Tau1P3PExtraDetailsCnv_p1.h,v 1.3 2009-01-20 17:19:02 tburgess Exp $

#ifndef TAUEVENTTPCNV_TAU1P3PEXTRADETAILSCNV_P1_H
#define TAUEVENTTPCNV_TAU1P3PEXTRADETAILSCNV_P1_H

#include "AthenaPoolCnvSvc/T_AthenaPoolTPConverter.h"
#include "DataModelAthenaPool/ElementLinkCnv_p2.h"
#include "tauEventTPCnv/Tau1P3PExtraDetails_p1.h"
#include "tauEvent/TauDetails.h"
#include "tauEvent/Tau1P3PExtraDetails.h"
class MsgStream;

///Legacy converter
///
///Converts a persistent Tau1P3PExtraDetails instance to a
///transient Tau1P3PExtraDetails instance
class Tau1P3PExtraDetailsCnv_p1:
    public T_AthenaPoolTPPolyCnvBase<
    Analysis::TauDetails,
    Analysis::Tau1P3PExtraDetails,
    Tau1P3PExtraDetails_p1 >
{
public:
    ///Constructor
    Tau1P3PExtraDetailsCnv_p1();

    ///Destructor
    virtual ~Tau1P3PExtraDetailsCnv_p1();

    ///Fills transient Tau1P3PExtraDetails instance from a
    ///persistent tau1P3PExtraDetails instance
    ///
    ///@param pers Persistent object
    ///@param trans Transient object
    ///@param msg Message stream
    virtual void persToTrans(
	const Tau1P3PExtraDetails_p1 *pers,
	Analysis::Tau1P3PExtraDetails *trans,
	MsgStream &msg );

    ///DEPRECATED DO NOT USE!
    ///
    ///Fills persistent Tau1P3PDetails instance from a
    ///transient tau1P3PDetails instance
    ///
    ///@param trans Transient object
    ///@param pers Persistent object
    ///@param msg Message stream
    virtual void transToPers(
	const Analysis::Tau1P3PExtraDetails *trans,
	Tau1P3PExtraDetails_p1 *pers,
	MsgStream &msg );

private:
    ///Calorimeter cell cluster converter
    ElementLinkCnv_p2<ElementLink<CaloCellContainer> > m_cellCnv;
};

#endif // TAUEVENTTPCNV_TAU1P3PEXTRADETAILSCNV_P1_H
