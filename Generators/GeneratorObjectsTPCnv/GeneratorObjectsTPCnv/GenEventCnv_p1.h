///////////////////////// -*- C++ -*- /////////////////////////////

/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

// GenEventCnv_p1.h 
// Header file for class GenEventCnv_p1
// Author: S.Binet<binet@cern.ch>
/////////////////////////////////////////////////////////////////// 
#ifndef GENERATOROBJECTSTPCNV_GENEVENTCNV_P1_H 
#define GENERATOROBJECTSTPCNV_GENEVENTCNV_P1_H 

// STL includes
#include <unordered_map>

// AthenaPoolCnvSvc includes
#include "AthenaPoolCnvSvc/T_AthenaPoolTPConverter.h"

// HepMC includes
#ifdef __clang__
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wkeyword-macro"
#endif
#define private public
#define protected public
#include "AtlasHepMC/GenEvent.h"
#include "AtlasHepMC/GenVertex.h"
#include "AtlasHepMC/GenParticle.h"
#undef private
#undef protected
#ifdef __clang__
#pragma clang diagnostic pop
#endif

// GeneratorObjectsTPCnv includes
#include "GeneratorObjectsTPCnv/GenEvent_p1.h"

// Forward declaration
class MsgStream;
namespace HepMC { struct DataPool; }

class GenEventCnv_p1 : public T_AthenaPoolTPCnvBase<HepMC::GenEvent, 
                                                    GenEvent_p1>  
{
  /////////////////////////////////////////////////////////////////// 
  // Public methods: 
  /////////////////////////////////////////////////////////////////// 
 public: 

  /** constructor: 
   */
  GenEventCnv_p1( HepMC::DataPool* pool = 0 );

  /** Destructor: 
   */
  //virtual ~GenEventCnv_p1(); 

  /////////////////////////////////////////////////////////////////// 
  // Const methods: 
  /////////////////////////////////////////////////////////////////// 

  /** Method creating the transient representation of @c HepMC::GenEvent
   *  from its persistent representation @c GenEvent_p1
   */
  virtual void persToTrans( const GenEvent_p1* persObj, 
			    HepMC::GenEvent* transObj, 
			    MsgStream& msg );

  /** Method creating the persistent representation @c GenEvent_p1
   *  from its transient representation @c HepMC::GenEvent
   */
  virtual void transToPers( const HepMC::GenEvent* transObj, 
			    GenEvent_p1* persObj, 
			    MsgStream& msg );

  /////////////////////////////////////////////////////////////////// 
  // Non-const methods: 
  /////////////////////////////////////////////////////////////////// 
  
  /// reset the @ HepMC::DataPool pointer
  void setDataPool( HepMC::DataPool* pool );

  /////////////////////////////////////////////////////////////////// 
  // Protected methods: 
  /////////////////////////////////////////////////////////////////// 
 protected: 

  typedef std::unordered_map<HepMC::GenParticlePtr,int> ParticlesMap_t;

  /** @brief Create a transient @c GenVertex from a persistent one (version 1)
   *  It returns the new @c GenVertex. 
   *  This method calls @c createGenParticle for each of the out-going
   *  particles and only for the in-going particle which are orphans (no
   *  production vertex): for optimisation purposes.
   *  Note that the map being passed as an argument is to hold the association 
   *  of barcodes to particle so that we can reconnect all the (non-orphan)
   *  particles to their decay vertex (if any).
   */
  HepMC::GenVertexPtr 
  createGenVertex( const GenEvent_p1& persEvt,
		   const GenVertex_p1& vtx,
  		   ParticlesMap_t& bcToPart ) const;

  /** @brief Create a transient @c GenParticle from a persistent one (vers.1)
   *  It returns the new @c GenParticle. Note that the map being passed as an
   *  argument is to hold the association of barcodes to particle so that
   *  we can reconnect all the particles to their decay vertex (if any).
   */
  HepMC::GenParticlePtr 
  createGenParticle( const GenParticle_p1& p,
		     ParticlesMap_t& partToEndVtx ) const;

  /////////////////////////////////////////////////////////////////// 
  // Protected data: 
  /////////////////////////////////////////////////////////////////// 
 protected: 

  /// a PIMPL idiom to hide the DataPools (and their specialized destructors)
  /// from the outside world
  HepMC::DataPool* m_pool;
}; 

/////////////////////////////////////////////////////////////////// 
/// Inline methods: 
/////////////////////////////////////////////////////////////////// 

#endif //> GENERATOROBJECTSTPCNV_GENEVENTCNV_P1_H
