///////////////////////// -*- C++ -*- /////////////////////////////

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// ElectronCnv_p1.h 
// Header file for class ElectronCnv_p1
// Author: K.Cranmer<cranmer@cern.ch>
// Author: S.Binet<binet@cern.ch>
/////////////////////////////////////////////////////////////////// 
#ifndef EGAMMAEVENTTPCNV_ELECTRONCNV_P1_H 
#define EGAMMAEVENTTPCNV_ELECTRONCNV_P1_H 

// STL includes

// AthenaPoolCnvSvc includes
#include "AthenaPoolCnvSvc/T_AthenaPoolTPConverter.h"

// egammaEventTPCnv includes
#include "egammaEventTPCnv/egammaCnv_p1.h"
#include "egammaEventTPCnv/Electron_p1.h"

// Forward declaration
class MsgStream;
namespace Analysis { class Electron; }

class ElectronCnv_p1 : public T_AthenaPoolTPCnvBase<
                                 Analysis::Electron, 
		                 Electron_p1
		              >
{ 

  /////////////////////////////////////////////////////////////////// 
  // Public methods: 
  /////////////////////////////////////////////////////////////////// 
 public: 

  /** Default constructor: 
   */
  ElectronCnv_p1();

  /////////////////////////////////////////////////////////////////// 
  // Const methods: 
  ///////////////////////////////////////////////////////////////////

  /** Method creating the transient representation of @c Analysis::Electron
   *  from its persistent representation @c Electron_p1
   */
  virtual void persToTrans( const Electron_p1* persObj, 
                            Analysis::Electron* transObj, 
                            MsgStream& msg ) override final;

  /** Method creating the persistent representation @c Electron_p1
   *  from its transient representation @c Analysis::Electron
   */
  virtual void transToPers( const Analysis::Electron* transObj, 
                            Electron_p1* persObj, 
                            MsgStream& msg ) override final;

  /////////////////////////////////////////////////////////////////// 
  // Protected method: 
  /////////////////////////////////////////////////////////////////// 
 protected: 
 private:
   egammaCnv_p1   m_egammaCnv;

}; 

/////////////////////////////////////////////////////////////////// 
// Inline methods: 
/////////////////////////////////////////////////////////////////// 

inline ElectronCnv_p1::ElectronCnv_p1()
{}

#endif //> EGAMMAEVENTTPCNV_ELECTRONCNV_P1_H
