///////////////////////// -*- C++ -*- /////////////////////////////

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// AthExDecayCnv_p1.h 
// Header file for class AthExDecayCnv_p1
// Author: S.Binet<binet@cern.ch>
/////////////////////////////////////////////////////////////////// 
#ifndef ATHEXTHINNING_AthExDECAYCNV_P1_H 
#define ATHEXTHINNING_AthExDECAYCNV_P1_H 

// STL includes

// HepMC / CLHEP includes

// Gaudi includes

// AthenaPoolCnvSvc includes
#include "AthenaPoolCnvSvc/T_AthenaPoolTPConverter.h"

// DataModel converters
#include "DataModelAthenaPool/ElementLinkCnv_p1.h"

// AthExThinning includes
#include "AthExThinning/AthExDecay.h"

// AthExThinning includes
#include "AthExThinning/AthExDecay_p1.h"

// Forward declaration
class MsgStream;

class AthExDecayCnv_p1 : public T_AthenaPoolTPCnvBase<
                                              AthExDecay, 
				              AthExDecay_p1
                                              >  
{ 

  /////////////////////////////////////////////////////////////////// 
  // Public methods: 
  /////////////////////////////////////////////////////////////////// 
 public: 

  /** Default constructor: 
   */
  AthExDecayCnv_p1();

  /////////////////////////////////////////////////////////////////// 
  // Const methods: 
  ///////////////////////////////////////////////////////////////////

  /** Method creating the transient representation of @c Decay
   *  from its persistent representation @c Decay_p1
   */
  virtual void persToTrans( const AthExDecay_p1* persObj, 
			    AthExDecay* transObj, 
			    MsgStream &msg );

  /** Method creating the persistent representation @c Decay_p1
   *  from its transient representation @c Decay
   */
  virtual void transToPers( const AthExDecay* transObj, 
			    AthExDecay_p1* persObj, 
			    MsgStream &msg );

  /////////////////////////////////////////////////////////////////// 
  // Protected method: 
  /////////////////////////////////////////////////////////////////// 
 protected: 

  /////////////////////////////////////////////////////////////////// 
  // Protected data: 
  /////////////////////////////////////////////////////////////////// 
 protected: 

  ElementLinkCnv_p1<ElementLink<AthExParticles> > m_partLinkCnv;
}; 

/////////////////////////////////////////////////////////////////// 
/// Inline methods: 
/////////////////////////////////////////////////////////////////// 

inline AthExDecayCnv_p1::AthExDecayCnv_p1() :
  m_partLinkCnv( )
{}

#endif //> ATHEXTHINNING_DECAYCNV_P1_H
