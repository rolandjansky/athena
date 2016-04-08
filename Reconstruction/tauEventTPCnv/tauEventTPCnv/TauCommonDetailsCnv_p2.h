/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/// @file TauCommonDetails_p1.h
/// 
/// Declaration of transient<->persistent converter 
/// for Analysis::TauCommonDetails
/// 
/// Package: Reconstruction/tauEventTPCnv
/// 
/// @author Thomas Burgess  
/// @author Ilija Vukotic

#ifndef TAUEVENTTPCNV_TAUCOMMONDETAILSCNV_P2_H
#define TAUEVENTTPCNV_TAUCOMMONDETAILSCNV_P2_H

#include "AthenaPoolCnvSvc/T_AthenaPoolTPConverter.h"
#include "tauEvent/TauCommonDetails.h"
#include "tauEventTPCnv/TauCommonDetails_p2.h"

class RecVertexCnv_p1;
class MsgStream;

///Transient Persistent class converter for Analysis::TauCommonDetails
class TauCommonDetailsCnv_p2 : public T_AthenaPoolTPPolyCnvBase< 
    Analysis::TauDetails, Analysis::TauCommonDetails, TauCommonDetails_p2>{
public:
    ///Constructor
    TauCommonDetailsCnv_p2();
    
    ///Destructor
    virtual ~TauCommonDetailsCnv_p2();

    ///Fills a transient object from a persistent object
    ///
    ///@param pers Persistent object
    ///@param trans Transient object
    ///@param msg Message stream
    virtual void persToTrans( 
	const TauCommonDetails_p2 *pers, 
	Analysis::TauCommonDetails *trans, 
	MsgStream &msg );


    ///Fills a persistent object from a transient object   
    ///
    ///@param trans Transient object
    ///@param pers Persistent object
    ///@param msg Message stream
    virtual void transToPers( 
	const Analysis::TauCommonDetails *trans, 
	TauCommonDetails_p2 *pers,
	MsgStream &msg );

private:
    ///Reconstruction vertex converter
    RecVertexCnv_p1 *m_recVertexCnv;
};

#endif // TAUEVENTTPCNV_TAUCOMMONDETAILSCNV_P2_H
