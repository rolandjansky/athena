// this files is  -*- c++ -*-
/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/
#ifndef XAODPFLOW_VERSIONS_FLOWELEMENT_V1_H
#define XAODPFLOW_VERSIONS_FLOWELEMENT_V1_H

// Core include(s):
#include "AthLinks/ElementLink.h"

// xAOD include(s):
#include "xAODBase/IParticle.h"
#include "xAODBase/IParticleContainer.h"

namespace xAOD {

  /////////////////////////////////////////////////////////
  /// A detector object made of other lower level object(s) 
  ///
  /// This class is intended to describe reconstruction objects coumpound of lower level detector objects.
  /// Typically it provides links to tracks and calo clusters such as is needed for particle flow objects.
  ///
  /// The class is kept minimal and very generic so it can cover many use cases, including a generic type
  /// to describe jet constituents.
  class FlowElement_v1 : public IParticle {
  public:

    using IParticle::IParticle;
    
    typedef unsigned long  signal_t; // 32-bit minimum
    typedef short vertex_t;
    using IParticle::FourMom_t ;
    
    /// Enum to encode the nature of the object this FlowElement represents
    enum SignalType { 
		     // global characteristics
		     Neutral      = 0x1000,
		     Charged      = 0x2000,
		     Combined     = 0x4000, //needed??

		     // detector level signals and flow objects
		     CaloCluster  = Neutral | 0x0100,
		     Track        = Charged | 0x0200,
		     Muon         = Charged | 0x0400,
		     PFlow        = 0x0010,
		     NeutralPFlow = Neutral | PFlow,
		     ChargedPFlow = Charged | PFlow,

		     // higher level flow objects
		     TCC          = 0x0020,
		     NeutralTCC   = Neutral | TCC,
		     ChargedTCC   = Charged | TCC,
		     UFO          = 0x0001,
		     NeutralUFO   = Neutral | UFO,
		     ChargedUFO   = Charged | UFO,

		     // unknown
		     Unknown      = 0x0000
    };

    /// Enum to encode high-level information on the vertex associated to this FlowElement
    enum MatchedPVType {
			Undefined = 0x00, HardScatter = 0x10, Pileup = 0x20, PileupSideBand = 0x21
    };

    // *************************************************
    ///@{
    /// kinematics (IParticle interface)
    virtual double pt()    const override;
    virtual double eta()   const override;
    virtual double phi()   const override;
    virtual double m()     const override;
    virtual double e()     const override;
    virtual double rapidity()     const override;
    virtual FourMom_t p4() const override;
    virtual Type::ObjectType type() const override ;
    
    void setP4(float pt, float eta, float phi, float m) ;
    ///@}

    // *************************************************
    ///@{
    ///Access signal type
    signal_t signalType() const;
    void setSignalType(signal_t t);
    ///@}
    
    // *************************************************
    ///@{
    /// Acess  vertex types
    bool     isMatchedToPV(MatchedPVType vxtype=HardScatter) const;

    vertex_t vertexType() const;
    void     setVertexType(vertex_t t);
    ///@}
    
    // *************************************************
    ///@{
    ///Access charge
    bool  isCharged() const;
    float charge() const;

    void  setCharge(float c);
    ///@}

    // *************************************************

    ///@{    
    /// Access directly the charged underlying IParticle (typically : TrackParticle)
    /// nullptr are returned if the ElementLink to the IParticle are invalid (ex: after thinning)
    std::vector<const xAOD::IParticle*> chargedObjects() const ;
    std::vector<std::pair<const xAOD::IParticle*,float> > chargedObjectsAndWeights() const ;

    std::size_t nChargedObjects() const ; 
    const xAOD::IParticle* chargedObject( std::size_t i ) const;
    std::pair< const xAOD::IParticle*, float > chargedObjectAndWeight( std::size_t i ) const;
    ///@}
    

    ///@{    
    /** Access to the EL.*/
    const std::vector<ElementLink<IParticleContainer>> & chargedObjectLinks() const ;
    const std::vector<float> & chargedObjectWeights() const ;
    void setChargedObjectLinks(const std::vector<ElementLink<IParticleContainer>> & elV);
    void setChargedObjectLinks(const std::vector<ElementLink<IParticleContainer>> & elV, const std::vector<float> & wV);
    ///@}

    // *************************************************
    ///@{
    /// Access directly the 'other' underlying IParticle (typically neutral CaloCluster)
    /// nullptr are returned if the ElementLink to the IParticle are invalid (ex: after thinning)
    std::vector<const xAOD::IParticle*> otherObjects() const ;
    std::vector<std::pair<const xAOD::IParticle*,float> > otherObjectsAndWeights() const ;

    std::size_t nOtherObjects() const ; 
    const xAOD::IParticle* otherObject( std::size_t i ) const;
    std::pair< const xAOD::IParticle*, float > otherObjectAndWeight( std::size_t i ) const;
    ///@}
    
    ///@{
    /// Access to the EL
    const std::vector<ElementLink<IParticleContainer>>& otherObjectLinks() const ;
    const std::vector<float>& otherObjectWeights() const ;

    void setOtherObjectLinks(const std::vector<ElementLink<IParticleContainer>> & elV);
    void setOtherObjectLinks(const std::vector<ElementLink<IParticleContainer>> & elV, const std::vector<float> & wV);
    ///@}
    
  };

}
// Declare IParticle as a base class of FlowElement_v1:
#include "AthContainers/DataVector.h"
DATAVECTOR_BASE( xAOD::FlowElement_v1, xAOD::IParticle );


#endif
