///////////////////////// -*- C++ -*- /////////////////////////////

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// RootTruthParticleContainerCnv_p4.h 
// Header file for root converters
// Author: S.Binet<binet@cern.ch>
/////////////////////////////////////////////////////////////////// 
#ifndef MCPARTICLEEVENTTPCNV_ROOTTRUTHPARTICLECONTAINERCNV_P4_H 
#define MCPARTICLEEVENTTPCNV_ROOTTRUTHPARTICLECONTAINERCNV_P4_H 

// STL includes

// HepMC / CLHEP includes

// Gaudi includes

// AthenaPoolCnvSvc includes
#include "AthenaPoolCnvSvc/T_AthenaPoolTPConverter.h"

// DataModel converters
#include "DataModelAthenaPool/ElementLinkCnv_p1.h"

// McParticleEvent includes
#define private public
#define protected public
#include "McParticleEvent/TruthParticleContainer.h"
#undef private
#undef protected

// McParticleEventTPCnv includes
#include "McParticleEventTPCnv/TruthParticleContainer_p4.h"

// Forward declaration
class MsgStream;
class StoreGateSvc;
class ITruthParticleCnvTool;

template <>
class T_TPCnv<TruthParticleContainer, TruthParticleContainer_p4>
  : public T_AthenaPoolTPCnvBase<TruthParticleContainer, 
				 TruthParticleContainer_p4>
{ 

  /////////////////////////////////////////////////////////////////// 
  // Public methods: 
  /////////////////////////////////////////////////////////////////// 
 public: 

  /** Constructor.
   */
  T_TPCnv();
  ~T_TPCnv();

  T_TPCnv(const T_TPCnv&) = delete;
  T_TPCnv& operator= (const T_TPCnv&) = delete;

  /////////////////////////////////////////////////////////////////// 
  // Const methods: 
  ///////////////////////////////////////////////////////////////////

  /** Method creating the transient representation of @c TruthParticleContainer
   *  from its persistent representation @c TruthParticleContainer_p4
   */
  virtual void persToTrans( const TruthParticleContainer_p4* persObj, 
			    TruthParticleContainer* transObj, 
			    MsgStream &log ) ;

  /** Method creating the persistent representation @c TruthParticleContainer_p4
   *  from its transient representation @c TruthParticleContainer
   */
  virtual void transToPers( const TruthParticleContainer* transObj, 
			    TruthParticleContainer_p4* persObj, 
			    MsgStream &log ) ;

 protected: 
  /** Pointer to the IAlgTool providing the conversion from a 
   *  @c McEventCollection to a @c TruthParticleContainer
   */
  ITruthParticleCnvTool* m_cnvTool;

  /** Converter for the ElementLink<McEventCollection> data member
   */
  ElementLinkCnv_p1<ElementLink<McEventCollection> > m_genEvtCnv;
}; 

#endif //> MCPARTICLEEVENTTPCNV_ROOTTRUTHPARTICLECONTAINERCNV_P4_H
