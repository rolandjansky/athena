///////////////////////// -*- C++ -*- /////////////////////////////

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// ElectronCnv_p5.h 
// Header file for class ElectronCnv_p5
/////////////////////////////////////////////////////////////////// 
#ifndef EGAMMAEVENTTPCNV_ELECTRONCNV_P5_H 
#define EGAMMAEVENTTPCNV_ELECTRONCNV_P5_H 1

// STL includes

// AthenaPoolCnvSvc includes
#include "AthenaPoolCnvSvc/T_AthenaPoolTPConverter.h"

// egammaEventTPCnv includes
#include "egammaEventTPCnv/egammaCnv_p5.h"
#include "egammaEventTPCnv/Electron_p5.h"

// Forward declaration
class MsgStream;
namespace Analysis { class Electron; }

class ElectronCnv_p5 : public T_AthenaPoolTPCnvBase<
                                 Analysis::Electron, 
		                 Electron_p5
		              >
{ 

  /////////////////////////////////////////////////////////////////// 
  // Public methods: 
  /////////////////////////////////////////////////////////////////// 
 public: 

  /** Default constructor: 
   */
  ElectronCnv_p5();

  /////////////////////////////////////////////////////////////////// 
  // Const methods: 
  ///////////////////////////////////////////////////////////////////

  /** Method creating the transient representation of @c Analysis::Electron
   *  from its persistent representation @c Electron_p5
   */
  virtual void persToTrans( const Electron_p5* persObj, 
                            Analysis::Electron* transObj, 
                            MsgStream& msg ) override final;

  /** Method creating the persistent representation @c Electron_p5
   *  from its transient representation @c Analysis::Electron
   */
  virtual void transToPers( const Analysis::Electron* transObj, 
                            Electron_p5* persObj, 
                            MsgStream& msg ) override final;

  /////////////////////////////////////////////////////////////////// 
  // Protected method: 
  /////////////////////////////////////////////////////////////////// 
 protected: 
 private:
   egammaCnv_p5   m_egammaCnv;

}; 

/////////////////////////////////////////////////////////////////// 
// Inline methods: 
/////////////////////////////////////////////////////////////////// 

inline ElectronCnv_p5::ElectronCnv_p5()
{}

#endif //> EGAMMAEVENTTPCNV_ELECTRONCNV_P5_H
