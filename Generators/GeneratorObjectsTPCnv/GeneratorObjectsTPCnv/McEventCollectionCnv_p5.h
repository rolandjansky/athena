///////////////////////// -*- C++ -*- /////////////////////////////

/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

// McEventCollectionCnv_p5.h
// Header file for class McEventCollectionCnv_p5
// Author: S.Binet<binet@cern.ch>
///////////////////////////////////////////////////////////////////
#ifndef GENERATOROBJECTSTPCNV_MCEVENTCOLLECTIONCNV_P5_H
#define GENERATOROBJECTSTPCNV_MCEVENTCOLLECTIONCNV_P5_H

// STL includes
#include <unordered_map>

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
#include "GeneratorObjects/McEventCollection.h"

// AthenaPoolCnvSvc includes
#include "AthenaPoolCnvSvc/T_AthenaPoolTPConverter.h"

// GeneratorObjectsTPCnv includes
#include "GeneratorObjectsTPCnv/McEventCollection_p5.h"

#include "GaudiKernel/ServiceHandle.h"

class IHepMCWeightSvc;

// Forward declaration
class MsgStream;
namespace HepMC { struct DataPool; }

class McEventCollectionCnv_p5 : public T_AthenaPoolTPCnvBase<
                                          McEventCollection,
                                          McEventCollection_p5
                                       >
{

  typedef T_AthenaPoolTPCnvBase<McEventCollection,
                                McEventCollection_p5> Base_t;

  ///////////////////////////////////////////////////////////////////
  // Public methods:
  ///////////////////////////////////////////////////////////////////
 public:

  /** Default constructor:
   */
  McEventCollectionCnv_p5();

  /** Copy constructor
   */
  McEventCollectionCnv_p5( const McEventCollectionCnv_p5& rhs );

  /** Assignement operator
   */
  McEventCollectionCnv_p5& operator=( const McEventCollectionCnv_p5& rhs );

  /** Destructor
   */
  virtual ~McEventCollectionCnv_p5();

  void setPileup();

  ///////////////////////////////////////////////////////////////////
  // Const methods:
  ///////////////////////////////////////////////////////////////////

  /** Method creating the transient representation of @c McEventCollection
   *  from its persistent representation @c McEventCollection_p5
   */
  virtual void persToTrans( const McEventCollection_p5* persObj,
                            McEventCollection* transObj,
                            MsgStream &log ) ;

  /** Method creating the persistent representation @c McEventCollection_p5
   *  from its transient representation @c McEventCollection
   */
  virtual void transToPers( const McEventCollection* transObj,
                            McEventCollection_p5* persObj,
                            MsgStream &log ) ;

  ///////////////////////////////////////////////////////////////////
  // Protected method:
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
  createGenVertex( const McEventCollection_p5& persEvts,
                   const GenVertex_p5& vtx,
                   ParticlesMap_t& bcToPart,
                   HepMC::DataPool* datapools ) const;

  /** @brief Create a transient @c GenParticle from a persistent one (vers.1)
   *  It returns the new @c GenParticle. Note that the map being passed as an
   *  argument is to hold the association of barcodes to particle so that
   *  we can reconnect all the particles to their decay vertex (if any).
   */
  HepMC::GenParticlePtr
  createGenParticle( const GenParticle_p5& p,
                     ParticlesMap_t& partToEndVtx,
                     HepMC::DataPool* datapools ) const;

  /** @brief Method to write a persistent @c GenVertex object. The persistent
   *  vertex is added to the persistent is added to the persistent
   *  @c GenEvent.
   */
  void writeGenVertex( const HepMC::GenVertex& vtx,
                       McEventCollection_p5& persEvt ) const;

  /** @brief Method to write a persistent @c GenParticle object
   *  It returns the index of the persistent @c GenParticle into the
   *  collection of persistent of @c GenParticles from the
   *  persistent @c GenEvent
   */
  int writeGenParticle( const HepMC::GenParticle& p,
                        McEventCollection_p5& persEvt ) const;

  ///////////////////////////////////////////////////////////////////
  // Protected data:
  ///////////////////////////////////////////////////////////////////
 protected:

  bool m_isPileup;
    ServiceHandle<IHepMCWeightSvc> m_hepMCWeightSvc;
};

///////////////////////////////////////////////////////////////////
/// Inline methods:
///////////////////////////////////////////////////////////////////

#endif //> GENERATOROBJECTSTPCNV_MCEVENTCOLLECTIONCNV_P5_H
