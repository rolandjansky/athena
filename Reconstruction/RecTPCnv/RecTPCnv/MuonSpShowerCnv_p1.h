///////////////////////// -*- C++ -*- /////////////////////////////

/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

// MuonSpShowerCnv_p1.h 
// Header file for class MuonSpShowerCnv_p1
// Author: K.Cranmer<cranmer@cern.ch>
// Author: S.Binet<binet@cern.ch>
/////////////////////////////////////////////////////////////////// 
#ifndef RECTPCNV_MUONSPSHOWERCNV_P1_H 
#define RECTPCNV_MUONSPSHOWERCNV_P1_H 

// STL includes

// AthenaPoolCnvSvc includes
#include "AthenaPoolCnvSvc/T_AthenaPoolTPConverter.h"

// RecTPCnv includes
#include "RecTPCnv/MuonSpShower_p1.h"


// Forward declaration
class MsgStream;

namespace Rec { 
    class MuonSpShower; 
}

class MuonSpShowerCnv_p1 : public T_AthenaPoolTPCnvConstBase<Rec::MuonSpShower, MuonSpShower_p1>
{ 
 public: 
  using base_class::transToPers;
  using base_class::persToTrans;


  /** Default constructor: 
   */
  MuonSpShowerCnv_p1();


  /** Method creating the transient representation of @c Rec::MuonSpShower
   *  from its persistent representation @c Muon_p1
   */
  virtual void persToTrans( const MuonSpShower_p1* persObj, 
                            Rec::MuonSpShower* transObj, 
                            MsgStream& msg ) const override;

  /** Method creating the persistent representation @c MuonSpShower_p1
   *  from its transient representation @c Rec::MuonSpShower
   */
  virtual void transToPers( const Rec::MuonSpShower* transObj, 
                            MuonSpShower_p1* persObj, 
                            MsgStream& msg ) const override;
}; 


inline MuonSpShowerCnv_p1::MuonSpShowerCnv_p1()
{}

#endif //> RECTPCNV_MUONCNV_P1_H
