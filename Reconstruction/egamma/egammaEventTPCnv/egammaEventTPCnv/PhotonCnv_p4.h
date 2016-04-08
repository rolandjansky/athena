///////////////////////// -*- C++ -*- /////////////////////////////

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// PhotonCnv_p4.h 
// Header file for class PhotonCnv_p4
/////////////////////////////////////////////////////////////////// 
#ifndef EGAMMAEVENTTPCNV_PHOTONCNV_P4_H 
#define EGAMMAEVENTTPCNV_PHOTONCNV_P4_H 1

// STL includes

// AthenaPoolCnvSvc includes
#include "AthenaPoolCnvSvc/T_AthenaPoolTPConverter.h"

// egammaEventTPCnv includes
#include "egammaEventTPCnv/egammaCnv_p4.h"
#include "egammaEventTPCnv/Photon_p4.h"

// Forward declaration
class MsgStream;
namespace Analysis { class Photon; }

class PhotonCnv_p4 : public T_AthenaPoolTPCnvBase<
                                 Analysis::Photon, 
		                 Photon_p4
		              >
{ 

  /////////////////////////////////////////////////////////////////// 
  // Public methods: 
  /////////////////////////////////////////////////////////////////// 
 public: 

  /** Default constructor: 
   */
  PhotonCnv_p4();

  /////////////////////////////////////////////////////////////////// 
  // Const methods: 
  ///////////////////////////////////////////////////////////////////

  /** Method creating the transient representation of @c Analysis::Photon
   *  from its persistent representation @c Photon_p4
   */
  virtual void persToTrans( const Photon_p4* persObj, 
                            Analysis::Photon* transObj, 
                            MsgStream& msg ) override final;

  /** Method creating the persistent representation @c Photon_p4
   *  from its transient representation @c Analysis::Photon
   */
  virtual void transToPers( const Analysis::Photon* transObj, 
                            Photon_p4* persObj, 
                            MsgStream& msg ) override final;

  /////////////////////////////////////////////////////////////////// 
  // Protected method: 
  /////////////////////////////////////////////////////////////////// 
 protected: 
 private:
   egammaCnv_p4   m_egammaCnv;

}; 

/////////////////////////////////////////////////////////////////// 
// Inline methods: 
/////////////////////////////////////////////////////////////////// 

inline PhotonCnv_p4::PhotonCnv_p4()
{}

#endif //> EGAMMAEVENTTPCNV_PHOTONCNV_P4_H
