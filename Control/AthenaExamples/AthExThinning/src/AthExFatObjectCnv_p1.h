///////////////////////// -*- C++ -*- /////////////////////////////

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// AthExFatObjectCnv_p1.h 
// Header file for class AthExFatObjectCnv_p1
// Author: S.Binet<binet@cern.ch>
/////////////////////////////////////////////////////////////////// 
#ifndef ATHEXTHINNING_AthExFATOBJECTCNV_P1_H 
#define ATHEXTHINNING_AthExFATOBJECTCNV_P1_H 

// STL includes

// HepMC / CLHEP includes

// Gaudi includes

// AthenaPoolCnvSvc includes
#include "AthenaPoolCnvSvc/T_AthenaPoolTPConverter.h"

// DataModel converters
#include "DataModelAthenaPool/ElementLinkCnv_p1.h"

// AthExThinning includes
#include "AthExThinning/AthExFatObject.h"
#include "AthExThinning/AthExParticles.h"

// AthExThinning includes
#include "AthExThinning/AthExFatObject_p1.h"

// Forward declaration
class MsgStream;

class AthExFatObjectCnv_p1 : public T_AthenaPoolTPCnvBase<
                                              AthExFatObject, 
				              AthExFatObject_p1
                                              >  
{ 

  /////////////////////////////////////////////////////////////////// 
  // Public methods: 
  /////////////////////////////////////////////////////////////////// 
 public: 

  /** Default constructor: 
   */
  AthExFatObjectCnv_p1();

  /////////////////////////////////////////////////////////////////// 
  // Const methods: 
  ///////////////////////////////////////////////////////////////////

  /** Method creating the transient representation of @c FatObject
   *  from its persistent representation @c FatObject_p1
   */
  virtual void persToTrans( const AthExFatObject_p1* persObj, 
			    AthExFatObject* transObj, 
			    MsgStream& msg );

  /** Method creating the persistent representation @c FatObject_p1
   *  from its transient representation @c FatObject
   */
  virtual void transToPers( const AthExFatObject* transObj, 
			    AthExFatObject_p1* persObj, 
			    MsgStream& msg );

  /////////////////////////////////////////////////////////////////// 
  // Protected method: 
  /////////////////////////////////////////////////////////////////// 
 protected: 

  /////////////////////////////////////////////////////////////////// 
  // Protected data: 
  /////////////////////////////////////////////////////////////////// 
 protected: 

  ElementLinkCnv_p1<ElementLink<AthExParticles> > m_cnv;
}; 

/////////////////////////////////////////////////////////////////// 
/// Inline methods: 
/////////////////////////////////////////////////////////////////// 

inline AthExFatObjectCnv_p1::AthExFatObjectCnv_p1() :
  m_cnv( )
{}

#endif //> ATHEXTHINNING_FATOBJECTCNV_P1_H
