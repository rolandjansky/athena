/*
  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
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


#ifndef tauEventTPCnv_TAUJETCNV_P5_H
#define tauEventTPCnv_TAUJETCNV_P5_H

#include "tauEventTPCnv/TauJet_p5.h"

class MsgStream;

/// Transient<->persistent converter for Analysis::TauJet v5
class TauJetCnv_p5
{
public:
    ///Constructor
    TauJetCnv_p5() {}

    ///Fills a transient object from a persistent object
    ///
    ///@param pers Persistent object
    ///@param trans Transient object
    ///@param msg Message stream
    void persToTrans( 
	const TauJet_p5 *pers,
	Analysis::TauJet *trans,
	MsgStream &msg ) const;

    ///Fills a persistent object from a transient object   
    ///
    ///@param trans Transient object
    ///@param pers Persistent object
    ///@param msg Message stream
    void transToPers( 
	const Analysis::TauJet *trans,
	TauJet_p5 *pers,
	MsgStream &msg ) const;
};

#endif // tauEventTPCnv_TAUJETCNV_P5_H
