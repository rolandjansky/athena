///////////////////////// -*- C++ -*- /////////////////////////////

/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

// NeutrinoCnv_p2.h 
// Header file for class NeutrinoCnv_p2
// Author: S.Binet<binet@cern.ch>
/////////////////////////////////////////////////////////////////// 
#ifndef PARTICLEEVENTTPCNV_NEUTRINOCNV_P2_H 
#define PARTICLEEVENTTPCNV_NEUTRINOCNV_P2_H 1

// STL includes

// Gaudi includes

// AthenaPoolCnvSvc includes
#include "AthenaPoolCnvSvc/T_AthenaPoolTPConverter.h"

// ParticleEventTPCnv includes
#include "ParticleEventTPCnv/Neutrino_p2.h"

// Forward declaration
class MsgStream;
class Neutrino;

class NeutrinoCnv_p2 : public T_AthenaPoolTPCnvConstBase<
                                          Neutrino, 
                                          Neutrino_p2
                                       >  
{ 
 public: 
  using base_class::transToPers;
  using base_class::persToTrans;


  /** Default constructor: 
   */
  NeutrinoCnv_p2();


  /** Method creating the transient representation of @c Neutrino
   *  from its persistent representation @c Neutrino_p2
   */
  virtual void persToTrans( const Neutrino_p2* pers, 
                            Neutrino* trans, 
                            MsgStream& msg ) const override;

  /** Method creating the persistent representation @c Neutrino_p2
   *  from its transient representation @c Neutrino
   */
  virtual void transToPers( const Neutrino* trans, 
                            Neutrino_p2* pers, 
                            MsgStream& msg ) const override;
}; 


inline NeutrinoCnv_p2::NeutrinoCnv_p2()
{}

#endif //> PARTICLEEVENTTPCNV_NEUTRINOCNV_P2_H
