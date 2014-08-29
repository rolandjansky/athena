// -*- C++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef XAODTRUTH_VERSIONS_TRUTHEVENTBASE_V1_H
#define XAODTRUTH_VERSIONS_TRUTHEVENTBASE_V1_H

#include <utility>

#include "AthContainers/AuxElement.h"
#include "AthLinks/ElementLink.h"
#include "xAODBase/ObjectType.h"

#include "xAODTruth/TruthParticleContainer.h"
#include "xAODTruth/TruthVertexContainer.h"

namespace xAOD {


  /// @brief Basic class describing a pile-up truth event in the MC record
  ///
  /// Provides access to particles and vertices, but no extra event info since
  /// this is different for signal and pile-up events.
  ///
  /// @author Andy Buckley <Andy.Buckley@cern.ch>
  /// @author Jovan Mitrevski <Jovan.Mitrevski@cern.ch>
  ///
  class TruthEventBase_v1 : public SG::AuxElement {
  public:

    /// Default constructor
    TruthEventBase_v1();

    /// Virtual destructor since this is a base class
    virtual ~TruthEventBase_v1() {}


    /// @name Access to the vertices and particles
    //@{

    /// @todo Add nicer accessors and deprecate the truth* ones.

    /// Get all the truth particles
    const std::vector< ElementLink<TruthParticleContainer> >& truthParticleLinks() const;
    const std::vector< ElementLink<TruthParticleContainer> >& particleLinks() const { return truthParticleLinks(); }
    /// Set all the truth particles
    void setTruthParticleLinks(const std::vector< ElementLink<TruthParticleContainer> >& plinks);
    void setParticleLinks(const std::vector< ElementLink<TruthParticleContainer> >& plinks) { setTruthParticleLinks(plinks); }
    /// Get the number of truth particles
    size_t nTruthParticles() const;
    size_t nParticles() const { return nTruthParticles(); }
    /// Get one of the truth particles
    const ElementLink<TruthParticleContainer>& truthParticleLink(size_t index) const;
    const TruthParticle* truthParticle(size_t index) const;
    const TruthParticle* particle(size_t index) const { return truthParticle(index); }
    /// Add one truth particle
    void addTruthParticleLink(const ElementLink<TruthParticleContainer>& plink);
    void addParticleLink(const ElementLink<TruthParticleContainer>& plink) { addTruthParticleLink(plink); }
    /// Remove all truth particles
    void clearTruthParticleLinks();
    void clearParticleLinks() { clearTruthParticleLinks(); }

    /// Get all the truth vertices
    const std::vector< ElementLink<TruthVertexContainer> >& truthVertexLinks() const;
    const std::vector< ElementLink<TruthVertexContainer> >& vertexLinks() const { return truthVertexLinks(); }
    /// Set all the truth vertices
    void setTruthVertexLinks(const std::vector< ElementLink<TruthVertexContainer> >&);
    void setVertexLinks(const std::vector< ElementLink<TruthVertexContainer> >& vlinks) { setTruthVertexLinks(vlinks); }
    /// Get the number of truth vertices
    size_t nTruthVertices() const;
    size_t nVertices() const { return nTruthVertices(); }
    /// Get one of the truth vertices
    const ElementLink<TruthVertexContainer>& truthVertexLink(size_t index) const;
    const TruthVertex* truthVertex(size_t index) const;
    const TruthVertex* vertex(size_t index) const { return truthVertex(index); }
    /// Add one truth vertex
    void addTruthVertexLink( const ElementLink<TruthVertexContainer>& vlink );
    void addVertexLink( const ElementLink<TruthVertexContainer>& vlink ) { addTruthVertexLink(vlink); }
    /// Remove all truth vertices
    void clearTruthVertexLinks();
    void clearVertexLinks() { clearTruthVertexLinks(); }

    //@}


    /// The type of the object as a simple enumeration
    virtual Type::ObjectType type() const = 0;

    /// Function making sure that the object is ready for persistification
    virtual void toPersistent();

  };


}

#include "xAODCore/BaseInfo.h"
SG_BASE (xAOD::TruthEventBase_v1, SG::AuxElement);

#endif
