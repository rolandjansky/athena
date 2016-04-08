///////////////////////// -*- C++ -*- /////////////////////////////

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// ElectronCnv_p3.h 
// Header file for class ElectronCnv_p3
/////////////////////////////////////////////////////////////////// 
#ifndef EGAMMAEVENTTPCNV_ELECTRONCNV_P3_H 
#define EGAMMAEVENTTPCNV_ELECTRONCNV_P3_H 1

// STL includes

// AthenaPoolCnvSvc includes
#include "AthenaPoolCnvSvc/T_AthenaPoolTPConverter.h"

// egammaEventTPCnv includes
#include "egammaEventTPCnv/egammaCnv_p3.h"
#include "egammaEventTPCnv/Electron_p3.h"

// Forward declaration
class MsgStream;
namespace Analysis { class Electron; }

class ElectronCnv_p3 : public T_AthenaPoolTPCnvBase<
                                 Analysis::Electron, 
		                 Electron_p3
		              >
{ 

  /////////////////////////////////////////////////////////////////// 
  // Public methods: 
  /////////////////////////////////////////////////////////////////// 
 public: 

  /** Default constructor: 
   */
  ElectronCnv_p3();

  /////////////////////////////////////////////////////////////////// 
  // Const methods: 
  ///////////////////////////////////////////////////////////////////

  /** Method creating the transient representation of @c Analysis::Electron
   *  from its persistent representation @c Electron_p3
   */
  virtual void persToTrans( const Electron_p3* persObj, 
                            Analysis::Electron* transObj, 
                            MsgStream& msg ) override final;

  /** Method creating the persistent representation @c Electron_p3
   *  from its transient representation @c Analysis::Electron
   */
  virtual void transToPers( const Analysis::Electron* transObj, 
                            Electron_p3* persObj, 
                            MsgStream& msg ) override final;

  /////////////////////////////////////////////////////////////////// 
  // Protected method: 
  /////////////////////////////////////////////////////////////////// 
 protected: 
 private:
   egammaCnv_p3   m_egammaCnv;

}; 

/////////////////////////////////////////////////////////////////// 
// Inline methods: 
/////////////////////////////////////////////////////////////////// 

inline ElectronCnv_p3::ElectronCnv_p3()
{}

#endif //> EGAMMAEVENTTPCNV_ELECTRONCNV_P3_H
