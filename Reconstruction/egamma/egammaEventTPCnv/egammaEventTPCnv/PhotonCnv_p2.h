///////////////////////// -*- C++ -*- /////////////////////////////

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// PhotonCnv_p2.h 
// Header file for class PhotonCnv_p2
/////////////////////////////////////////////////////////////////// 
#ifndef EGAMMAEVENTTPCNV_PHOTONCNV_P2_H 
#define EGAMMAEVENTTPCNV_PHOTONCNV_P2_H 

// STL includes

// AthenaPoolCnvSvc includes
#include "AthenaPoolCnvSvc/T_AthenaPoolTPConverter.h"

// egammaEventTPCnv includes
#include "egammaEventTPCnv/egammaCnv_p2.h"
#include "egammaEventTPCnv/Photon_p2.h"

// Forward declaration
class MsgStream;
namespace Analysis { class Photon; }

class PhotonCnv_p2 : public T_AthenaPoolTPCnvBase<
                                 Analysis::Photon, 
		                 Photon_p2
		              >
{ 

  /////////////////////////////////////////////////////////////////// 
  // Public methods: 
  /////////////////////////////////////////////////////////////////// 
 public: 

  /** Default constructor: 
   */
  PhotonCnv_p2();

  /////////////////////////////////////////////////////////////////// 
  // Const methods: 
  ///////////////////////////////////////////////////////////////////

  /** Method creating the transient representation of @c Analysis::Photon
   *  from its persistent representation @c Photon_p2
   */
  virtual void persToTrans( const Photon_p2* persObj, 
                            Analysis::Photon* transObj, 
                            MsgStream& msg ) override final;

  /** Method creating the persistent representation @c Photon_p2
   *  from its transient representation @c Analysis::Photon
   */
  virtual void transToPers( const Analysis::Photon* transObj, 
                            Photon_p2* persObj, 
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

inline PhotonCnv_p2::PhotonCnv_p2()
{}

#endif //> EGAMMAEVENTTPCNV_PHOTONCNV_P2_H
