///////////////////////// -*- C++ -*- /////////////////////////////

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// PhotonCnv_p3.h 
// Header file for class PhotonCnv_p3
/////////////////////////////////////////////////////////////////// 
#ifndef EGAMMAEVENTTPCNV_PHOTONCNV_P3_H 
#define EGAMMAEVENTTPCNV_PHOTONCNV_P3_H 1

// STL includes

// AthenaPoolCnvSvc includes
#include "AthenaPoolCnvSvc/T_AthenaPoolTPConverter.h"

// egammaEventTPCnv includes
#include "egammaEventTPCnv/egammaCnv_p3.h"
#include "egammaEventTPCnv/Photon_p3.h"

// Forward declaration
class MsgStream;
namespace Analysis { class Photon; }

class PhotonCnv_p3 : public T_AthenaPoolTPCnvBase<
                                 Analysis::Photon, 
		                 Photon_p3
		              >
{ 

  /////////////////////////////////////////////////////////////////// 
  // Public methods: 
  /////////////////////////////////////////////////////////////////// 
 public: 

  /** Default constructor: 
   */
  PhotonCnv_p3();

  /////////////////////////////////////////////////////////////////// 
  // Const methods: 
  ///////////////////////////////////////////////////////////////////

  /** Method creating the transient representation of @c Analysis::Photon
   *  from its persistent representation @c Photon_p3
   */
  virtual void persToTrans( const Photon_p3* persObj, 
                            Analysis::Photon* transObj, 
                            MsgStream& msg ) override final;

  /** Method creating the persistent representation @c Photon_p3
   *  from its transient representation @c Analysis::Photon
   */
  virtual void transToPers( const Analysis::Photon* transObj, 
                            Photon_p3* persObj, 
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

inline PhotonCnv_p3::PhotonCnv_p3()
{}

#endif //> EGAMMAEVENTTPCNV_PHOTONCNV_P3_H
