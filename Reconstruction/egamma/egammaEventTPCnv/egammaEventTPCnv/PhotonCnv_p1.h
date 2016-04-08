///////////////////////// -*- C++ -*- /////////////////////////////

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// PhotonCnv_p1.h 
// Header file for class PhotonCnv_p1
// Author: K.Cranmer<cranmer@cern.ch>
// Author: S.Binet<binet@cern.ch>
/////////////////////////////////////////////////////////////////// 
#ifndef EGAMMAEVENTTPCNV_PHOTONCNV_P1_H 
#define EGAMMAEVENTTPCNV_PHOTONCNV_P1_H 

// STL includes

// AthenaPoolCnvSvc includes
#include "AthenaPoolCnvSvc/T_AthenaPoolTPConverter.h"

// egammaEventTPCnv includes
#include "egammaEventTPCnv/egammaCnv_p1.h"
#include "egammaEventTPCnv/Photon_p1.h"

// Forward declaration
class MsgStream;
namespace Analysis { class Photon; }

class PhotonCnv_p1 : public T_AthenaPoolTPCnvBase<
                                 Analysis::Photon, 
		                 Photon_p1
		              >
{ 

  /////////////////////////////////////////////////////////////////// 
  // Public methods: 
  /////////////////////////////////////////////////////////////////// 
 public: 

  /** Default constructor: 
   */
  PhotonCnv_p1();

  /////////////////////////////////////////////////////////////////// 
  // Const methods: 
  ///////////////////////////////////////////////////////////////////

  /** Method creating the transient representation of @c Analysis::Photon
   *  from its persistent representation @c Photon_p1
   */
  virtual void persToTrans( const Photon_p1* persObj, 
                            Analysis::Photon* transObj, 
                            MsgStream& msg ) override final;

  /** Method creating the persistent representation @c Photon_p1
   *  from its transient representation @c Analysis::Photon
   */
  virtual void transToPers( const Analysis::Photon* transObj, 
                            Photon_p1* persObj, 
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

inline PhotonCnv_p1::PhotonCnv_p1()
{}

#endif //> EGAMMAEVENTTPCNV_PHOTONCNV_P1_H
