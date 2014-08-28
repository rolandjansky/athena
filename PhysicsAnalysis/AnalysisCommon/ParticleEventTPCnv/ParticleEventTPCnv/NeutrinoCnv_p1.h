///////////////////////// -*- C++ -*- /////////////////////////////

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// NeutrinoCnv_p1.h 
// Header file for class NeutrinoCnv_p1
// Author: S.Binet<binet@cern.ch>
/////////////////////////////////////////////////////////////////// 
#ifndef PARTICLEEVENTTPCNV_NEUTRINOCNV_P1_H 
#define PARTICLEEVENTTPCNV_NEUTRINOCNV_P1_H 1

// STL includes

// Gaudi includes

// AthenaPoolCnvSvc includes
#include "AthenaPoolCnvSvc/T_AthenaPoolTPConverter.h"

// ParticleEventTPCnv includes
#include "ParticleEventTPCnv/Neutrino_p1.h"

// Forward declaration
class MsgStream;
class Neutrino;

class NeutrinoCnv_p1 : public T_AthenaPoolTPCnvBase<
                                          Neutrino, 
                                          Neutrino_p1
                                       >  
{ 

  /////////////////////////////////////////////////////////////////// 
  // Public methods: 
  /////////////////////////////////////////////////////////////////// 
 public: 

  /** Default constructor: 
   */
  NeutrinoCnv_p1();

  /////////////////////////////////////////////////////////////////// 
  // Const methods: 
  ///////////////////////////////////////////////////////////////////

  /** Method creating the transient representation of @c Neutrino
   *  from its persistent representation @c Neutrino_p1
   */
  virtual void persToTrans( const Neutrino_p1* pers, 
                            Neutrino* trans, 
                            MsgStream& msg ) ;

  /** Method creating the persistent representation @c Neutrino_p1
   *  from its transient representation @c Neutrino
   */
  virtual void transToPers( const Neutrino* trans, 
                            Neutrino_p1* pers, 
                            MsgStream& msg ) ;

  /////////////////////////////////////////////////////////////////// 
  // Protected method: 
  /////////////////////////////////////////////////////////////////// 
 protected: 

}; 

/////////////////////////////////////////////////////////////////// 
/// Inline methods: 
/////////////////////////////////////////////////////////////////// 

inline NeutrinoCnv_p1::NeutrinoCnv_p1()
{}

#endif //> PARTICLEEVENTTPCNV_NEUTRINOCNV_P1_H
