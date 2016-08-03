/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/// ////////////////////////////////////////////////////////////
/// 
/// Header for conversion of PanTauDetails class
/// 
/// ////////////////////////////////////////////////////////////

/********************************************************************

NAME:     PanTauDetails_p1.h
PACKAGE:  offline/Reconstruction/PanTau

AUTHORS:  Christian Limbach
CREATED:  29.November.2011

PURPOSE:  T/P Converter for the PanTau::PanTauDetails persistent data class

********************************************************************/

#ifndef TAUEVENTTPCNV_PANTAUDETAILSCNV_P1_H
#define TAUEVENTTPCNV_PANTAUDETAILSCNV_P1_H

//!Conversion classes
#include "AthenaPoolCnvSvc/T_AthenaPoolTPConverter.h"

//!Transient and persistent versions
#include "PanTauEvent/PanTauDetails.h"
#include "tauEventTPCnv/PanTauDetails_p1.h"


class MsgStream;

class PanTauDetailsCnv_p1 : public T_AthenaPoolTPPolyCnvBase<Analysis::TauDetails, PanTau::PanTauDetails, PanTauDetails_p1 >
{
    public:

        PanTauDetailsCnv_p1 () {};
        
        virtual void persToTrans( const PanTauDetails_p1      *persObj,   PanTau::PanTauDetails *transObj,  MsgStream &log );
        virtual void transToPers( const PanTau::PanTauDetails *transObj,  PanTauDetails_p1      *persObj,   MsgStream &log );
        

};
#endif
