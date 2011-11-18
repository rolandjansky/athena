///////////////////////// -*- C++ -*- /////////////////////////////

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// TruthEtIsolationsCnv_p1.h 
// Header file for class TruthEtIsolationsCnv_p1
// Author: S.Binet<binet@cern.ch>
/////////////////////////////////////////////////////////////////// 
#ifndef MCPARTICLEEVENTTPCNV_TRUTHETISOLATIONSCNV_P1_H 
#define MCPARTICLEEVENTTPCNV_TRUTHETISOLATIONSCNV_P1_H 

// STL includes

// DataModel converters
#include "DataModelAthenaPool/ElementLinkCnv_p1.h"

// AthenaPoolCnvSvc includes
#include "AthenaPoolCnvSvc/T_AthenaPoolTPConverter.h"

// McParticleEventTPCnv includes
#include "McParticleEventTPCnv/TruthEtIsolations_p1.h"

// Forward declaration
class MsgStream;
class TruthEtIsolations;

class TruthEtIsolationsCnv_p1 : public T_AthenaPoolTPCnvBase<TruthEtIsolations, TruthEtIsolations_p1>
{ 

  /////////////////////////////////////////////////////////////////// 
  // Public methods: 
  /////////////////////////////////////////////////////////////////// 
 public: 

  /** Default constructor: 
   */
  TruthEtIsolationsCnv_p1();

  /////////////////////////////////////////////////////////////////// 
  // Const methods: 
  ///////////////////////////////////////////////////////////////////

  /** Method creating the transient representation of @c TruthEtIsolations
   *  from its persistent representation @c TruthEtIsolations_p1
   */
  virtual void persToTrans( const TruthEtIsolations_p1* persObj, 
                            TruthEtIsolations* transObj, 
                            MsgStream& msg );

  /** Method creating the persistent representation @c TruthEtIsolations_p1
   *  from its transient representation @c TruthEtIsolations
   */
  virtual void transToPers( const TruthEtIsolations* transObj, 
                            TruthEtIsolations_p1* persObj, 
                            MsgStream& msg );

  /////////////////////////////////////////////////////////////////// 
  // Protected method: 
  /////////////////////////////////////////////////////////////////// 
 protected: 

  /** Converter for the ElementLink<McEventCollection> data member
   */
  ElementLinkCnv_p1<ElementLink<McEventCollection> > m_genEvtCnv;

}; 

/////////////////////////////////////////////////////////////////// 
// Inline methods: 
/////////////////////////////////////////////////////////////////// 

inline TruthEtIsolationsCnv_p1::TruthEtIsolationsCnv_p1() :
  m_genEvtCnv (   )
{}

#endif //> MCPARTICLEEVENTTPCNV_TRUTHETISOLATIONSCNV_P1_H
