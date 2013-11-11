/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/// @file TauJetCnv_p5.h
/// 
/// Implementation of transient<->persistent converter 
/// for Analysis::TauJet v5
/// 
/// Package: Reconstruction/tauEventTPCnv
/// 
/// @author Thomas Burgess  
/// @author Lukasz Janyst
/// 
/// Created by Lukasz Janyst in May 2008
///
/// $Id: TauJetCnv_p4.h,v 1.2 2009-01-27 17:38:43 tburgess Exp $


#ifndef tauEventTPCnv_TAUJETCNV_P5_H
#define tauEventTPCnv_TAUJETCNV_P5_H

#include "AthenaPoolCnvSvc/T_AthenaPoolTPConverter.h"
#include "tauEventTPCnv/TauJet_p5.h"

class MsgStream;

/// Transient<->persistent converter for Analysis::TauJet v5
class TauJetCnv_p5: 
    public T_AthenaPoolTPCnvBase<Analysis::TauJet, TauJet_p5>
{
public:
    ///Constructor
    TauJetCnv_p5() {}

    ///Destructor
    virtual ~TauJetCnv_p5() {}

    ///Fills a transient object from a persistent object
    ///
    ///@param pers Persistent object
    ///@param trans Transient object
    ///@param msg Message stream
    virtual void persToTrans( 
	const TauJet_p5 *pers,
	Analysis::TauJet *trans,
	MsgStream &msg );

    ///Fills a persistent object from a transient object   
    ///
    ///@param trans Transient object
    ///@param pers Persistent object
    ///@param msg Message stream
    virtual void transToPers( 
	const Analysis::TauJet *trans,
	TauJet_p5 *pers,
	MsgStream &msg );
private:
};

#endif // tauEventTPCnv_TAUJETCNV_P5_H
