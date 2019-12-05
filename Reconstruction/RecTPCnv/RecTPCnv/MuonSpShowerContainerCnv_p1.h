///////////////////////// -*- C++ -*- /////////////////////////////

/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

// MuonSpShowerContainerCnv_p1.h 
// Header file for class MuonSpShowerContainerCnv_p1
// Author: Edward.Moyse@cern.ch
/////////////////////////////////////////////////////////////////// 
#ifndef RECTPCNV_MUONSPSHOWERCONTAINERCNV_P1_H 
#define RECTPCNV_MUONSPSHOWERCONTAINERCNV_P1_H 

// STL includes

// Gaudi includes

// muonEvent includes
#include "muonEvent/MuonSpShowerContainer.h"

// AthenaPoolCnvSvc includes
#include "AthenaPoolCnvSvc/T_AthenaPoolTPConverter.h"

// RecTPCnv includes
#include "RecTPCnv/MuonSpShowerContainer_p1.h"
#include "RecTPCnv/MuonSpShowerCnv_p1.h"

class MuonSpShowerContainerCnv_p1
  : public   T_AthenaPoolTPCnvConstBase< 
                Rec::MuonSpShowerContainer, 
                MuonSpShowerContainer_p1 >
{
public:
    using base_class::transToPers;
    using base_class::persToTrans;


    /** Method creating the transient representation of @c MuonSpShowerContainer
     *  from its persistent representation @c MuonSpShowerContainer_p1
     */
    virtual void persToTrans( const MuonSpShowerContainer_p1* pers, 
                              Rec::MuonSpShowerContainer* trans, 
                              MsgStream& msg ) const override;

    /** Method creating the persistent representation @c MuonSpShowerContainer_p1
     *  from its transient representation @c MuonSpShowerContainer
     */
    virtual void transToPers( const Rec::MuonSpShowerContainer* trans, 
                              MuonSpShowerContainer_p1* pers, 
                              MsgStream& msg ) const override;
private:
    MuonSpShowerCnv_p1          m_cnv;
};


#endif //> RECTPCNV_MUONSPSHOWERCONTAINERCNV_P1_H
