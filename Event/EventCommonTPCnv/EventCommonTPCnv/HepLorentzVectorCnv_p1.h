///////////////////////// -*- C++ -*- /////////////////////////////

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// HepLorentzVectorCnv_p1.h 
// Header file for class HepLorentzVectorCnv_p1
// Author: S.Binet<binet@cern.ch>
/////////////////////////////////////////////////////////////////// 
#ifndef EVENTCOMMONTPCNV_HEPLORENTZVECTORCNV_P1_H 
#define EVENTCOMMONTPCNV_HEPLORENTZVECTORCNV_P1_H 

// STL includes

// HepMC / CLHEP includes
#include "CLHEP/Vector/LorentzVector.h"

// Gaudi includes

// AthenaPoolCnvSvc includes
#include "AthenaPoolCnvSvc/T_AthenaPoolTPConverter.h"

// EventCommonTPCnv includes
#include "EventCommonTPCnv/HepLorentzVector_p1.h"

// Forward declaration
class MsgStream;

class HepLorentzVectorCnv_p1 : public T_AthenaPoolTPCnvBase<
                                              CLHEP::HepLorentzVector, 
                                              HepLorentzVector_p1
                                            >  
{ 

  /////////////////////////////////////////////////////////////////// 
  // Public methods: 
  /////////////////////////////////////////////////////////////////// 
 public: 

  /** Default constructor: 
   */
  HepLorentzVectorCnv_p1();

  /** Destructor: 
   */
  virtual ~HepLorentzVectorCnv_p1();

  /////////////////////////////////////////////////////////////////// 
  // Const methods: 
  ///////////////////////////////////////////////////////////////////

  /** Method creating the transient representation of @c HepLorentzVector
   *  from its persistent representation @c HepLorentzVector_p1
   */
  virtual void persToTrans( const HepLorentzVector_p1* persObj, 
                            CLHEP::HepLorentzVector* transObj, 
                            MsgStream &log );

  /** Method creating the persistent representation @c HepLorentzVector_p1
   *  from its transient representation @c HepLorentzVector
   */
  virtual void transToPers( const CLHEP::HepLorentzVector* transObj, 
                            HepLorentzVector_p1* persObj, 
                            MsgStream &log );

  /////////////////////////////////////////////////////////////////// 
  // Protected method: 
  /////////////////////////////////////////////////////////////////// 
 protected: 

  /////////////////////////////////////////////////////////////////// 
  // Protected data: 
  /////////////////////////////////////////////////////////////////// 
 protected: 

}; 

/////////////////////////////////////////////////////////////////// 
/// Inline methods: 
/////////////////////////////////////////////////////////////////// 

inline HepLorentzVectorCnv_p1::HepLorentzVectorCnv_p1() 
{}

inline HepLorentzVectorCnv_p1::~HepLorentzVectorCnv_p1()
{}

#endif //> EVENTCOMMONTPCNV_HEPLORENTZVECTORCNV_P1_H
