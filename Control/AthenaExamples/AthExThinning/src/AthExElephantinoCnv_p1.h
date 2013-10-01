///////////////////////// -*- C++ -*- /////////////////////////////

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// AthExElephantinoCnv_p1.h 
// Header file for class AthExElephantinoCnv_p1
// Author: S.Binet<binet@cern.ch>
/////////////////////////////////////////////////////////////////// 
#ifndef ATHEXTHINNING_AthExELEPHANTINOCNV_P1_H 
#define ATHEXTHINNING_AthExELEPHANTINOCNV_P1_H 

// STL includes

// HepMC / CLHEP includes

// Gaudi includes

// AthenaPoolCnvSvc includes
#include "AthenaPoolCnvSvc/T_AthenaPoolTPConverter.h"

// DataModel converters
#include "DataModelAthenaPool/ElementLinkVectorCnv_p1.h"

// AthExThinning includes
#include "AthExThinning/AthExElephantino.h"
#include "AthExThinning/AthExIParticles.h"

// AthExThinning includes
#include "AthExThinning/AthExElephantino_p1.h"

// Forward declaration
class MsgStream;

class AthExElephantinoCnv_p1 : public T_AthenaPoolTPCnvBase<
                                              AthExElephantino, 
				              AthExElephantino_p1
                                              >  
{ 

  /////////////////////////////////////////////////////////////////// 
  // Public methods: 
  /////////////////////////////////////////////////////////////////// 
 public: 

  /** Default constructor: 
   */
  AthExElephantinoCnv_p1();

  /////////////////////////////////////////////////////////////////// 
  // Const methods: 
  ///////////////////////////////////////////////////////////////////

  /** Method creating the transient representation of @c Elephantino
   *  from its persistent representation @c Elephantino_p1
   */
  virtual void persToTrans( const AthExElephantino_p1* persObj, 
			    AthExElephantino* transObj, 
			    MsgStream& msg );

  /** Method creating the persistent representation @c Elephantino_p1
   *  from its transient representation @c Elephantino
   */
  virtual void transToPers( const AthExElephantino* transObj, 
			    AthExElephantino_p1* persObj, 
			    MsgStream& msg );

  /////////////////////////////////////////////////////////////////// 
  // Protected method: 
  /////////////////////////////////////////////////////////////////// 
 protected: 

  /////////////////////////////////////////////////////////////////// 
  // Protected data: 
  /////////////////////////////////////////////////////////////////// 
 protected: 

  ElementLinkVectorCnv_p1<ElementLinkVector<AthExIParticles> > m_cnv;
}; 

/////////////////////////////////////////////////////////////////// 
/// Inline methods: 
/////////////////////////////////////////////////////////////////// 

inline AthExElephantinoCnv_p1::AthExElephantinoCnv_p1() :
  m_cnv( )
{}

#endif //> ATHEXTHINNING_ELEPHANTINOCNV_P1_H
