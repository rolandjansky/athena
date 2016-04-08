///////////////////////// -*- C++ -*- /////////////////////////////

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// ElectronCnv_p2.h 
// Header file for class ElectronCnv_p2
/////////////////////////////////////////////////////////////////// 
#ifndef EGAMMAEVENTTPCNV_ELECTRONCNV_P2_H 
#define EGAMMAEVENTTPCNV_ELECTRONCNV_P2_H 1

// STL includes

// AthenaPoolCnvSvc includes
#include "AthenaPoolCnvSvc/T_AthenaPoolTPConverter.h"

// egammaEventTPCnv includes
#include "egammaEventTPCnv/egammaCnv_p2.h"
#include "egammaEventTPCnv/Electron_p2.h"

// Forward declaration
class MsgStream;
namespace Analysis { class Electron; }

class ElectronCnv_p2 : public T_AthenaPoolTPCnvBase<
                                 Analysis::Electron, 
		                 Electron_p2
		              >
{ 

  /////////////////////////////////////////////////////////////////// 
  // Public methods: 
  /////////////////////////////////////////////////////////////////// 
 public: 

  /** Default constructor: 
   */
  ElectronCnv_p2();

  /////////////////////////////////////////////////////////////////// 
  // Const methods: 
  ///////////////////////////////////////////////////////////////////

  /** Method creating the transient representation of @c Analysis::Electron
   *  from its persistent representation @c Electron_p2
   */
  virtual void persToTrans( const Electron_p2* persObj, 
                            Analysis::Electron* transObj, 
                            MsgStream& msg ) override final;

  /** Method creating the persistent representation @c Electron_p2
   *  from its transient representation @c Analysis::Electron
   */
  virtual void transToPers( const Analysis::Electron* transObj, 
                            Electron_p2* persObj, 
                            MsgStream& msg ) override final;

  /////////////////////////////////////////////////////////////////// 
  // Protected method: 
  /////////////////////////////////////////////////////////////////// 
 protected:
 private:
   egammaCnv_p2   m_egammaCnv;


}; 

/////////////////////////////////////////////////////////////////// 
// Inline methods: 
/////////////////////////////////////////////////////////////////// 

inline ElectronCnv_p2::ElectronCnv_p2()
{}

#endif //> EGAMMAEVENTTPCNV_ELECTRONCNV_P2_H
