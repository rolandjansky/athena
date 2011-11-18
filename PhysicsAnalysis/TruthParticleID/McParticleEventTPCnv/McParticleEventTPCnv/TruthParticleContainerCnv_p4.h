///////////////////////// -*- C++ -*- /////////////////////////////

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// TruthParticleContainerCnv_p4.h 
// Header file for class TruthParticleContainerCnv_p4
// Author: S.Binet<binet@cern.ch>
/////////////////////////////////////////////////////////////////// 
#ifndef MCPARTICLEEVENTTPCNV_TRUTHPARTICLECONTAINERCNV_P4_H 
#define MCPARTICLEEVENTTPCNV_TRUTHPARTICLECONTAINERCNV_P4_H 

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

class TruthParticleContainerCnv_p4 : public T_AthenaPoolTPCnvBase<
                                              TruthParticleContainer, 
				              TruthParticleContainer_p4
                                            >  
{ 

  /////////////////////////////////////////////////////////////////// 
  // Public methods: 
  /////////////////////////////////////////////////////////////////// 
 public: 

  /** Default constructor: 
   */
  TruthParticleContainerCnv_p4();

  /** Constructor with parameters: 
   */
  TruthParticleContainerCnv_p4( StoreGateSvc* storeGate,
				ITruthParticleCnvTool* cnvTool );

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

  /////////////////////////////////////////////////////////////////// 
  // Protected method: 
  /////////////////////////////////////////////////////////////////// 
 protected: 

  /////////////////////////////////////////////////////////////////// 
  // Protected data: 
  /////////////////////////////////////////////////////////////////// 
 protected: 

  /** Pointer to the StoreGateSvc: we need this as we'll have to record
   *  the @c TruthEtIsolations container.
   */
  StoreGateSvc* m_storeGate;

  /** Pointer to the IAlgTool providing the conversion from a 
   *  @c McEventCollection to a @c TruthParticleContainer
   */
  ITruthParticleCnvTool* m_cnvTool;

  /** Converter for the ElementLink<McEventCollection> data member
   */
  ElementLinkCnv_p1<ElementLink<McEventCollection> > m_genEvtCnv;

}; 

/////////////////////////////////////////////////////////////////// 
/// Inline methods: 
/////////////////////////////////////////////////////////////////// 

inline TruthParticleContainerCnv_p4::TruthParticleContainerCnv_p4() :
  m_storeGate ( 0 ),
  m_cnvTool   ( 0 ),
  m_genEvtCnv (   )
{}

inline 
TruthParticleContainerCnv_p4::TruthParticleContainerCnv_p4( StoreGateSvc* storeGate,
							    ITruthParticleCnvTool* cnvTool ) :
  m_storeGate ( storeGate ),
  m_cnvTool   ( cnvTool ),
  m_genEvtCnv (   )
{}

#endif //> MCPARTICLEEVENTTPCNV_TRUTHPARTICLECONTAINERCNV_P4_H
