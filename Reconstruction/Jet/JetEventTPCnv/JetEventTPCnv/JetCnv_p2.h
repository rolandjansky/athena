///////////////////////// -*- C++ -*- /////////////////////////////

/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

// JetCnv_p2.h 
// Header file for class JetCnv_p2
// Author: S.Binet<binet@cern.ch>
/////////////////////////////////////////////////////////////////// 
#ifndef JETEVENTTPCNV_JETCNV_P2_H 
#define JETEVENTTPCNV_JETCNV_P2_H 

// STL includes

// AthenaPoolCnvSvc includes
#include "AthenaPoolCnvSvc/T_AthenaPoolTPConverter.h"

// JetEventTPCnv includes
#include "JetEventTPCnv/Jet_p2.h"

// Forward declaration
class MsgStream;
class Jet;

class JetCnv_p2 : public T_AthenaPoolTPCnvConstBase<Jet, Jet_p2>
{ 
 public: 
  using base_class::transToPers;
  using base_class::persToTrans;


  /** Default constructor: 
   */
  JetCnv_p2();


  /** Method creating the transient representation of @c Jet
   *  from its persistent representation @c Jet_p2
   */
  virtual void persToTrans( const Jet_p2* persObj, 
                            Jet* transObj, 
                            MsgStream& msg ) const override;

  /** Method creating the persistent representation @c Jet_p2
   *  from its transient representation @c Jet
   */
  virtual void transToPers( const Jet* transObj, 
                            Jet_p2* persObj, 
                            MsgStream& msg ) const override;
}; 


inline JetCnv_p2::JetCnv_p2()
{}

#endif //> JETEVENTTPCNV_JETCNV_P2_H
