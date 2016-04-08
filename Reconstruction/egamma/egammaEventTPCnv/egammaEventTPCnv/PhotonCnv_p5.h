///////////////////////// -*- C++ -*- /////////////////////////////

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// PhotonCnv_p5.h 
// Header file for class PhotonCnv_p5
/////////////////////////////////////////////////////////////////// 
#ifndef EGAMMAEVENTTPCNV_PHOTONCNV_P5_H 
#define EGAMMAEVENTTPCNV_PHOTONCNV_P5_H 1

// STL includes

// AthenaPoolCnvSvc includes
#include "AthenaPoolCnvSvc/T_AthenaPoolTPConverter.h"

// egammaEventTPCnv includes
#include "egammaEventTPCnv/Photon_p5.h"
#include "egammaEventTPCnv/egammaCnv_p5.h"

// Forward declaration
class MsgStream;
namespace Analysis { class Photon; }

class PhotonCnv_p5 : public T_AthenaPoolTPCnvBase<
                                 Analysis::Photon, 
		                 Photon_p5
		              >
{ 

  /////////////////////////////////////////////////////////////////// 
  // Public methods: 
  /////////////////////////////////////////////////////////////////// 
 public: 

  /** Default constructor: 
   */
  PhotonCnv_p5();

  /////////////////////////////////////////////////////////////////// 
  // Const methods: 
  ///////////////////////////////////////////////////////////////////

  /** Method creating the transient representation of @c Analysis::Photon
   *  from its persistent representation @c Photon_p5
   */
  virtual void persToTrans( const Photon_p5* persObj, 
                            Analysis::Photon* transObj, 
                            MsgStream& msg ) override final;

  /** Method creating the persistent representation @c Photon_p5
   *  from its transient representation @c Analysis::Photon
   */
  virtual void transToPers( const Analysis::Photon* transObj, 
                            Photon_p5* persObj, 
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

inline PhotonCnv_p5::PhotonCnv_p5()
{}

#endif //> EGAMMAEVENTTPCNV_PHOTONCNV_P5_H
