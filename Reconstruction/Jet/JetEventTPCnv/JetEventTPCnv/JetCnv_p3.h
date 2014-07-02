///////////////////////// -*- C++ -*- /////////////////////////////

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// JetCnv_p3.h 
// Header file for class JetCnv_p3
// Author: S.Binet<binet@cern.ch>
/////////////////////////////////////////////////////////////////// 
#ifndef JETEVENTTPCNV_JETCNV_P3_H 
#define JETEVENTTPCNV_JETCNV_P3_H 

// STL includes

// AthenaPoolCnvSvc includes
#include "AthenaPoolCnvSvc/T_AthenaPoolTPConverter.h"

// JetEventTPCnv includes
#include "JetEventTPCnv/Jet_p3.h"

// Forward declaration
class MsgStream;
class Jet;

class JetCnv_p3 : public T_AthenaPoolTPCnvBase<Jet, Jet_p3>
{ 

  /////////////////////////////////////////////////////////////////// 
  // Public methods: 
  /////////////////////////////////////////////////////////////////// 
 public: 

  /** Default constructor: 
   */
  JetCnv_p3();

  /////////////////////////////////////////////////////////////////// 
  // Const methods: 
  ///////////////////////////////////////////////////////////////////

  /** Method creating the transient representation of @c Jet
   *  from its persistent representation @c Jet_p3
   */
  virtual void persToTrans( const Jet_p3* persObj, 
                            Jet* transObj, 
                            MsgStream& msg );

  /** Method creating the persistent representation @c Jet_p3
   *  from its transient representation @c Jet
   */
  virtual void transToPers( const Jet* transObj, 
                            Jet_p3* persObj, 
                            MsgStream& msg );

  /////////////////////////////////////////////////////////////////// 
  // Protected method: 
  /////////////////////////////////////////////////////////////////// 
 protected: 


}; 

/////////////////////////////////////////////////////////////////// 
// Inline methods: 
/////////////////////////////////////////////////////////////////// 

inline JetCnv_p3::JetCnv_p3()
{
}

#endif //> JETEVENTTPCNV_JETCNV_P3_H
