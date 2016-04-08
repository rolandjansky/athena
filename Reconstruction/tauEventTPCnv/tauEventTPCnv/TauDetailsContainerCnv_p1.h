/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/// @file TauDetailsContainerCnv_p1.h
///
/// Definition of transient<->persistent converter
/// for Analysis::TauDetailsContainer
///
/// Package: Reconstruction/tauEventTPCnv
///
/// @author Thomas Burgess
/// @author Lukasz Janyst
///
/// Created by Thomas Burgess on 2007-07-06
///
/// $Id: TauDetailsContainerCnv_p1.h,v 1.3 2009-01-20 17:19:02 tburgess Exp $

#ifndef TAUEVENTTPCNV_TAUDETAILSCONTAINERCNV_P1_H
#define TAUEVENTTPCNV_TAUDETAILSCONTAINERCNV_P1_H

#include "tauEvent/TauDetailsContainer.h"
#include "AthenaPoolCnvSvc/T_AthenaPoolTPConverter.h"
#include "tauEventTPCnv/TauDetailsContainer_p1.h"


/// Transient<->persistent converter for Analysis::TauDetailsContainer
class TauDetailsContainerCnv_p1: 
    public T_AthenaPoolTPCnvBase<
    Analysis::TauDetailsContainer, 
    TauDetailsContainer_p1>
{
public:
    ///Constructor
    TauDetailsContainerCnv_p1();
    
    ///Destructor
    virtual ~TauDetailsContainerCnv_p1();

    ///Creates persistent object from transient object
    ///
    ///@param transVect Transient object
    ///@param persVect Persistent object
    ///@param msg Message stream
    virtual void transToPers( 
	const Analysis::TauDetailsContainer *transVect, 
	TauDetailsContainer_p1 *persVect,
	MsgStream &msg );
    
    ///Creates transient object from persistent object
    ///
    ///@param trans Transient object
    ///@param pers Persistent object
    ///@param msg Message stream
    virtual void persToTrans( 
	const TauDetailsContainer_p1 *persVect, 
	Analysis::TauDetailsContainer *transVect, 
	MsgStream &msg );
};

#endif // TAUEVENTTPCNV_TAUDETAILSCONTAINERCNV_P1_H
