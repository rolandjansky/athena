///////////////////////// -*- C++ -*- /////////////////////////////

/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

// McEventCollectionCnv_p3.h
// Header file for class McEventCollectionCnv_p3
// Author: S.Binet<binet@cern.ch>
///////////////////////////////////////////////////////////////////
#ifndef GENERATOROBJECTSTPCNV_MCEVENTCOLLECTIONCNV_P3_H
#define GENERATOROBJECTSTPCNV_MCEVENTCOLLECTIONCNV_P3_H

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
#include "GeneratorObjectsTPCnv/McEventCollection_p3.h"

// Forward declaration
class MsgStream;
namespace HepMC { struct DataPool; }

class McEventCollectionCnv_p3 : public T_AthenaPoolTPCnvBase<
                                          McEventCollection,
                                          McEventCollection_p3
                                       >
{

  typedef T_AthenaPoolTPCnvBase<McEventCollection,
                                McEventCollection_p3> Base_t;

  ///////////////////////////////////////////////////////////////////
  // Public methods:
  ///////////////////////////////////////////////////////////////////
 public:

  /** Default constructor:
   */
  McEventCollectionCnv_p3();

  /** Copy constructor
   */
  McEventCollectionCnv_p3( const McEventCollectionCnv_p3& rhs );

  /** Assignement operator
   */
  McEventCollectionCnv_p3& operator=( const McEventCollectionCnv_p3& rhs );

  /** Destructor
   */
  virtual ~McEventCollectionCnv_p3();

  ///////////////////////////////////////////////////////////////////
  // Const methods:
  ///////////////////////////////////////////////////////////////////

  /** Method creating the transient representation of @c McEventCollection
   *  from its persistent representation @c McEventCollection_p3
   */
  virtual void persToTrans( const McEventCollection_p3* persObj,
                            McEventCollection* transObj,
                            MsgStream &log ) ;

  /** Method creating the persistent representation @c McEventCollection_p3
   *  from its transient representation @c McEventCollection
   */
  virtual void transToPers( const McEventCollection* transObj,
                            McEventCollection_p3* persObj,
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
  createGenVertex( const McEventCollection_p3& persEvts,
                   const GenVertex_p3& vtx,
                   ParticlesMap_t& bcToPart,
                   HepMC::DataPool* datapools ) const;

  /** @brief Create a transient @c GenParticle from a persistent one (vers.1)
   *  It returns the new @c GenParticle. Note that the map being passed as an
   *  argument is to hold the association of barcodes to particle so that
   *  we can reconnect all the particles to their decay vertex (if any).
   */
  HepMC::GenParticlePtr
  createGenParticle( const GenParticle_p3& p,
                     ParticlesMap_t& partToEndVtx,
                     HepMC::DataPool* datapools ) const;

};

///////////////////////////////////////////////////////////////////
/// Inline methods:
///////////////////////////////////////////////////////////////////

#endif //> GENERATOROBJECTSTPCNV_MCEVENTCOLLECTIONCNV_P3_H
