// Dear emacs, this is -*- c++ -*-

/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/

// $Id: PFO_v1.h 783497 2016-11-10 15:00:53Z mhodgkin $
#ifndef XAODPFLOW_VERSIONS_PFO_V1_H
#define XAODPFLOW_VERSIONS_PFO_V1_H

//Author Mark Hodgkinson and Michel Janus

// Core include(s):
#include "AthLinks/ElementLink.h"

// xAOD include(s):
#include "xAODBase/IParticle.h"
#include "xAODBase/IParticleContainer.h"
#include "xAODPFlow/PFODefs.h"
#include "xAODCaloEvent/CaloCluster.h"
#include "xAODCaloEvent/CaloClusterContainer.h"
#include "xAODTracking/VertexContainer.h"
#include "xAODTracking/TrackParticle.h"
#include "xAODTracking/TrackParticleContainer.h"

//ROOT includes
#include "TVector3.h"
#include "Math/Vector4D.h"

namespace xAOD {


   /// Class describing a particle flow object
   ///
  class PFO_v1 : public IParticle {

  public:
    /// Default constructor
    PFO_v1();
    /** Copy Constructor */
    PFO_v1(const PFO_v1& other);

    PFO_v1& operator=(const PFO_v1& other) = default;

    
    /// @name xAOD::IParticle functions
    /// @{
    
    /// The transverse momentum (\f$p_T\f$) of the particle
    virtual double           pt() const;
    /// The pseudorapidity (\f$\eta\f$) of the particle
    virtual double           eta() const;
    /// The azimuthal angle (\f$\phi\f$) of the particle
    virtual double           phi() const;
    /// The invariant mass of the particle
    virtual double           m() const;
    /// The total energy of the particle
    virtual double           e() const;
    /// The true rapidity (y) of the particle
    virtual double           rapidity() const;
    
    /// The full 4-momentum of the particle
    virtual FourMom_t        p4() const;
    
    /// The type of the object as a simple enumeration
    virtual Type::ObjectType type() const;
    
    /// @}

    /// Base 4 Momentum type (GenVector version)
    typedef ROOT::Math::LorentzVector<ROOT::Math::PtEtaPhiM4D<double> > GenVecFourMom_t;

    ///  The full 4-momentum of the particle : GenVector version.
    GenVecFourMom_t genvecP4() const; 

    /// set the 4-vec
    void setP4(const FourMom_t& vec);
    
    /// set the 4-vec
    void setP4(float pt, float eta, float phi, float m=0.0);

    /** get EM scale 4-vector */
    FourMom_t p4EM() const;
    /** get EM scale 4-vector */
    GenVecFourMom_t genvecP4EM() const;
    /** set EM scale 4-vector */
    void setP4EM(const FourMom_t& p4EM);
    /** set EM scale 4-vector */
    void setP4EM(float pt, float eta, float phi, float m);

    /** get EM scale pt */
    virtual double           ptEM() const;
    /** get EM scale eta */
    virtual double           etaEM() const;
    /** get EM scale phi */
    virtual double           phiEM() const;
    /** get EM scale mass */
    virtual double           mEM() const;
    /** get EM scale energy */
    virtual double           eEM() const;



    /** get BDT Score used to classify clusters as Pi0 like or not */
    float bdtPi0Score() const;
    /** set BDT Score used to classify clusters as Pi0 like or not */
    void setBDTPi0Score(float BDTPi0Score);

    /** get CenterMag moment needed for vertex correction */
    float centerMag() const;
    /** set CenterMag moment needed for vertex correction */
    void setCenterMag(float CenterMag);
    
    /** is a charged PFO */
    bool isCharged() const;
    /** get charge of PFO */
    float charge() const;
    /** set charge of PFO */
    void setCharge(float charge);

    /** Set a PFO Variable via enum - overwrite is allowed */
    template<class T> void setAttribute(PFODetails::PFOAttributes AttributeType, const T& anAttribute) ;
    /** get a PFO Variable via enum */
    template<class T> bool attribute(PFODetails::PFOAttributes AttributeType, T& anAttribute) const;

    /** Set a PFO Variable via string - overwrite is allowed */
    template<class T> void setAttribute(const std::string& AttributeType, const T& anAttribute) ;
    /** Get a PFO Variable via string */
    template<class T> bool attribute(const std::string& AttributeType, T& anAttribute) const;

    /** Accessor for cluster moments */
    bool getClusterMoment(float& theMoment, xAOD::CaloCluster::MomentType momentType) const;

    /** Retrieve a const pointer to a CaloCluster */
    const CaloCluster* cluster(unsigned int index) const;
    /** Retrieve a const pointer to a Rec::TrackParticle */
    const TrackParticle* track(unsigned int index) const;
    /** Retrieve a const pointer to the xAOD::Vertex a charged PFO is associated to */
    const xAOD::Vertex* vertex() const;

    /** Set a vertex link */
    bool setVertexLink(const ElementLink< xAOD::VertexContainer>& theVertexLink);

    /** Convert a generic link to an IParticleLink - perhaps this will go in a dedicated helper tool? */
    template<class T> void convertLink(ElementLink<IParticleContainer>& IParticleLink, const ElementLink<T>& templateLink);
    
    /** Set a track constituent - does NOT append to existing container  */
    bool setTrackLink(const ElementLink<xAOD::TrackParticleContainer>& theTrack);
    /** Set a cluster constituent - does NOT append to existing container  */
    bool setClusterLink(const ElementLink<xAOD::CaloClusterContainer>& theCluster);
    /** Add a cluster constituent  appends to existing container */
    bool addClusterLink(const ElementLink<xAOD::CaloClusterContainer>& theCluster);
    /** Set an IParticle constituent via enum - does NOT append to existing container */
    bool setAssociatedParticleLink(PFODetails::PFOParticleType ParticleType, const ElementLink<IParticleContainer>& theParticle);
    /** Set an IParticle constituent via string  - does NOT append to existing container */
    void setAssociatedParticleLink(const std::string& ParticleType, const ElementLink<IParticleContainer>& theParticle);
    /** Add an IParticle constituent via enum - appends to existing container */
    bool addAssociatedParticleLink(PFODetails::PFOParticleType ParticleType,  const ElementLink<IParticleContainer>& theParticle);
     /** Add an IParticle constituent via enum - appends to existing container */
    void addAssociatedParticleLink(const std::string& ParticleType,  const ElementLink<IParticleContainer>& theParticle);
    
    //switch to elementlinkvector
    /** Set a vector of PFO constituent particle types via enum - overwrite is allowed */
    bool setAssociatedParticleLinks(PFODetails::PFOParticleType ParticleType,  const std::vector<ElementLink<IParticleContainer> >& theParticles) ;
    /** get a vector of PFO constituent particle types via enum */
    bool associatedParticles(PFODetails::PFOParticleType ParticleType, std::vector<const IParticle*>& theParticles ) const;
    /** Set a vector of PFO constituent particle types via string - overwrite is allowed */
    void setAssociatedParticleLinks(const std::string& ParticleType,  const std::vector<ElementLink<IParticleContainer> >& theParticles) ;
    /** get a vector of PFO constituent particle types via string */
    bool associatedParticles(const std::string& ParticleType, std::vector<const IParticle*>& theParticles ) const;

    /** Correct 4-vector to point at a vertex */
    TLorentzVector GetVertexCorrectedFourVec(const xAOD::Vertex& vertexToCorrectTo) const;
    /** Correct 4-vector to point at a vertex */
    TLorentzVector GetVertexCorrectedFourVec(const TVector3& vertexToCorrectTo) const;

    /** Correct EM scale 4-vector to point at a vertex */
    TLorentzVector GetVertexCorrectedEMFourVec(const xAOD::Vertex& vertexToCorrectTo) const;
    /** Correct EM scale 4-vector to point at a vertex */
    TLorentzVector GetVertexCorrectedEMFourVec(const TVector3& vertexToCorrectTo) const;

    /** prepare all links for persistification */
    void toPersistent();

   private:

    /** Gives access to cluster moments direct from the PFO - getClusterMoment calls this, if needed */
    bool getClusterMomentFromPFO(float& theMoment, xAOD::CaloCluster::MomentType momentType) const;
    
    /** This does the vertex correction of neutral PFO */
    void VertexCorrectTheFourVector(const TVector3& vertexToCorrectTo, TLorentzVector& theFourVector) const;

    /** Map from cluster moment name onto PFOAttribute name */
    bool getAttributeName_FromClusterMoment(xAOD::PFODetails::PFOAttributes& myAttribute, xAOD::CaloCluster::MomentType& momentType) const;

    /** Performs a check as to whether a variable should be compressed */
    bool isJetETMissFloatForCompression(xAOD::PFODetails::PFOAttributes AttributeType) const;

    /** this defines the factor to compress floats by */
    const static int s_floatCompressionFactor = 1000;

  }; // class PFO

} // namespace xAOD

// Declare IParticle as a base class of PFO_v1:
#include "AthContainers/DataVector.h"
DATAVECTOR_BASE( xAOD::PFO_v1, xAOD::IParticle );

#include "xAODPFlow/versions/PFO_v1.icc"

#endif // XAODPFLOW_VERSIONS_PFO_V1_H
