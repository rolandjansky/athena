///////////////////////// -*- C++ -*- /////////////////////////////

/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

// JetCnv_p1.h 
// Header file for class JetCnv_p1
// Author: S.Binet<binet@cern.ch>
/////////////////////////////////////////////////////////////////// 
#ifndef RECTPCNV_JETCNV_P1_H 
#define RECTPCNV_JETCNV_P1_H 

// STL includes

// AthenaPoolCnvSvc includes
#include "AthenaPoolCnvSvc/T_AthenaPoolTPConverter.h"

// JetEventTPCnv includes
#include "JetEventTPCnv/Jet_p1.h"

// Forward declaration
class MsgStream;
class Jet;

class JetCnv_p1 : public T_AthenaPoolTPCnvConstBase<Jet, Jet_p1>
{ 
 public: 
  using base_class::transToPers;
  using base_class::persToTrans;


  /** Default constructor: 
   */
  JetCnv_p1();


  /** Method creating the transient representation of @c Jet
   *  from its persistent representation @c Jet_p1
   */
  virtual void persToTrans( const Jet_p1* persObj, 
                            Jet* transObj, 
                            MsgStream& msg ) const override;

  /** Method creating the persistent representation @c Jet_p1
   *  from its transient representation @c Jet
   */
  virtual void transToPers( const Jet* transObj, 
                            Jet_p1* persObj, 
                            MsgStream& msg ) const override;

}; 


inline JetCnv_p1::JetCnv_p1()
{}

#endif //> RECTPCNV_JETCNV_P1_H
