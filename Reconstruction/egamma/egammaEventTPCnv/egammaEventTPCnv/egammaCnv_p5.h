///////////////////////// -*- C++ -*- /////////////////////////////

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// egammaCnv_p5.h 
// Header file for class egammaCnv_p5
// New cnv to include Ringer: W.S.Freund<wsfreund@mail.cern.ch>
/////////////////////////////////////////////////////////////////// 
#ifndef EGAMMAEVENTTPCNV_EGAMMACNV_P5_H 
#define EGAMMAEVENTTPCNV_EGAMMACNV_P5_H 1

// STL includes

// AthenaPoolCnvSvc includes
#include "AthenaPoolCnvSvc/T_AthenaPoolTPConverter.h"

// egammaEventTPCnv includes
#include "egammaEventTPCnv/egamma_p5.h"

// Forward declaration
class MsgStream;
class egamma;

class egammaCnv_p5 : public T_AthenaPoolTPCnvBase<egamma, egamma_p5>
{ 

  /////////////////////////////////////////////////////////////////// 
  // Public methods: 
  /////////////////////////////////////////////////////////////////// 
 public: 

  /** Default constructor: 
   */
  egammaCnv_p5();

  /////////////////////////////////////////////////////////////////// 
  // Const methods: 
  ///////////////////////////////////////////////////////////////////

  /** Method creating the transient representation of @c egamma
   *  from its persistent representation @c egamma_p5
   */
  virtual void persToTrans( const egamma_p5* persObj, 
                            egamma* transObj, 
                            MsgStream& msg );

  /** Method creating the persistent representation @c egamma_p5
   *  from its transient representation @c egamma
   */
  virtual void transToPers( const egamma* transObj, 
                            egamma_p5* persObj, 
                            MsgStream& msg );

  /////////////////////////////////////////////////////////////////// 
  // Protected method: 
  /////////////////////////////////////////////////////////////////// 
 protected: 

}; 

/////////////////////////////////////////////////////////////////// 
// Inline methods: 
/////////////////////////////////////////////////////////////////// 

inline egammaCnv_p5::egammaCnv_p5()
{}

#endif //> EGAMMAEVENTTPCNV_EGAMMACNV_P5_H
