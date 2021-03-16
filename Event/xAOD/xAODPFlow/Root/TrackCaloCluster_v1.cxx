/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/
// $Id: TrackCaloCluster_v1.cxx $

// Misc includes
#include <bitset>
#include <vector>

// EDM include(s):
#include "xAODCore/AuxStoreAccessorMacros.h"

// Local include(s):
#include "xAODPFlow/versions/TrackCaloCluster_v1.h"

namespace xAOD {

  // *************************************************
  // Helper functions. We put them in an anonymous namespace to avoid any potential conflicts.
  namespace {

    // Accessor to internal links to IParticles
    static const SG::AuxElement::Accessor< std::vector<ElementLink< IParticleContainer > > > ipartAccessor("iparticleLinks");


    /// Converts a vector<ElementLink<Container>> to a vector<ElementLink<IParticleContainer>>
    //  output is filled ONLY with valid element links.
    template<typename T>
    std::vector<ElementLink<IParticleContainer> > toVectELIParticle( const std::vector< ElementLink< T > >& input){
      
      std::vector<ElementLink<IParticleContainer> > ipartVec;      
      ipartVec.reserve(input.size());
      for( auto & elInput : input) {
        const T * cont = elInput.getStorableObjectPointer();
        if(elInput.isValid()) ipartVec.push_back( ElementLink<IParticleContainer>(*cont, elInput.index() ) ) ;        
      }
      return ipartVec;
    }

    
    /// Retrieves EL to IParticles stored in a TrackCaloCluster and returns them as a given type T
    ///  The returned vector is filled ONLY with valid EL which can be dynamic_casted to type T
    template<typename T>
    std::vector<const T * > retrieveFromIParticle(const TrackCaloCluster_v1* tcc)  {
      
      // prepare output vector :
      std::vector<const T*> outVec;

      if( !ipartAccessor.isAvailable(*tcc) ) return outVec;

      // retrieve the vector of iparticle from ourselves
      const std::vector< ElementLink<IParticleContainer> > & ipartVec = ipartAccessor( *tcc );
      outVec.reserve(ipartVec.size() );

      // convert iparticle to clusters and fill the output vector
      for(auto & el : ipartVec ){
        const T *cl = nullptr;
        if( el.isValid() && (cl=dynamic_cast<const T*>(*el)) ) outVec.push_back(cl);       
      }
      
      return outVec;
    }

    
  } // end helper namespace
  // *************************************************


  // ---------------------------  
  TrackCaloCluster_v1::TrackCaloCluster_v1() : IParticle(), m_p4(), m_p4Cached( false ) {}

  TrackCaloCluster_v1::~TrackCaloCluster_v1() {}

  // ---------------------------  
  double TrackCaloCluster_v1::e() const {
    return p4().E();
  }
 
  double TrackCaloCluster_v1::rapidity() const {
    return p4().Rapidity();
  }
   
  AUXSTORE_PRIMITIVE_GETTER_WITH_CAST(TrackCaloCluster_v1, float, double, pt)
  AUXSTORE_PRIMITIVE_GETTER_WITH_CAST(TrackCaloCluster_v1, float, double, eta)
  AUXSTORE_PRIMITIVE_GETTER_WITH_CAST(TrackCaloCluster_v1, float, double, phi)
  AUXSTORE_PRIMITIVE_GETTER_WITH_CAST(TrackCaloCluster_v1, float, double, m)
  AUXSTORE_PRIMITIVE_GETTER(TrackCaloCluster_v1,    int, taste)
  
  const TrackCaloCluster_v1::FourMom_t& TrackCaloCluster_v1::p4() const {
    if( ! m_p4Cached ) {
      m_p4.SetPtEtaPhiM( pt(), eta(), phi(), m() );
      m_p4Cached = true;
    }
    return m_p4;
  }

  TrackCaloCluster_v1::GenVecFourMom_t TrackCaloCluster_v1::genvecP4() const {
    return GenVecFourMom_t( pt(), eta(), phi(),m()); 
  }

  Type::ObjectType TrackCaloCluster_v1::type() const { 
     return Type::TrackCaloCluster;
  }


  // ---------------------------
  // set and get TrackParticleContainer 
  
  AUXSTORE_OBJECT_SETTER_AND_GETTER( TrackCaloCluster_v1,
				     ElementLink< TrackParticleContainer >,
				     trackParticleLink, 
				     setTrackParticleLink)
  
  const TrackParticle* TrackCaloCluster_v1::trackParticle() const {
    // The accessor:       
    static const SG::AuxElement::Accessor< ElementLink< TrackParticleContainer > >  acc( "trackParticleLink" );
    if( !acc.isAvailable( *this ) ) {
      return 0;       
    }
    if( !acc( *this ).isValid() ) {
      return 0;
    }
    return *(acc( *this ));
  }
  


  // ---------------------------
  // set and get CaloCluster 
  std::vector<const CaloCluster* > TrackCaloCluster_v1::caloClusters() const {
    return retrieveFromIParticle<CaloCluster>(this) ;
  }

  
  void TrackCaloCluster_v1::setCaloClusterLinks(const std::vector< ElementLink< CaloClusterContainer > >& caloClusterLinks){
    setIParticleLinks( toVectELIParticle(caloClusterLinks) );
  }


  // ---------------------------
  // set and get PFO 
  std::vector<const PFO* > TrackCaloCluster_v1::pFlowObjects() const {
    return retrieveFromIParticle<PFO>(this) ;    
  }
  
  void TrackCaloCluster_v1::setPFlowObjects(const std::vector< ElementLink< PFOContainer > >& pfoLinks){
    setIParticleLinks( toVectELIParticle(pfoLinks) );
  }

  
  // ---------------------------
  // set and get IParticle 
  AUXSTORE_OBJECT_SETTER_AND_GETTER( TrackCaloCluster_v1, 
                                     std::vector< ElementLink< IParticleContainer > >, 
                                     iparticleLinks, 
                                     setIParticleLinks)



  // ---------------------------------
  
  void TrackCaloCluster_v1::setParameters(float pt, float eta, float phi, float m, TrackCaloCluster_v1::Taste taste,
                                          const ElementLink<TrackParticleContainer> particle, 
                                          const std::vector< ElementLink< CaloClusterContainer > >& clusters) {
    setParameters(pt, eta, phi, m, taste, particle, toVectELIParticle(clusters));
  }
  void TrackCaloCluster_v1::setParameters(float pt, float eta, float phi, float m, TrackCaloCluster_v1::Taste taste, 
                                          const ElementLink<TrackParticleContainer> particle, 
                                          const std::vector< ElementLink< PFOContainer > >& clusters){
    setParameters(pt, eta, phi, m, taste, particle, toVectELIParticle(clusters));
  }

  //
  void TrackCaloCluster_v1::setParameters(float pt, float eta, float phi, float m, TrackCaloCluster_v1::Taste taste, 
                                          const ElementLink<TrackParticleContainer> track, 
                                          const std::vector< ElementLink< IParticleContainer > >& iparticles){
    
    static const Accessor< float > acc1( "pt" );     
    acc1( *this ) = pt;     
    static const  Accessor< float > acc2( "eta" );     
    acc2( *this ) = eta;     
    static const  Accessor< float > acc3( "phi" );     
    acc3( *this ) = phi;     
    static const  Accessor< float > acc4( "m" );     
    acc4( *this ) = m;     

    m_p4Cached = false; 
    
    static const Accessor< int > acc( "taste" );     
    acc( *this ) = (int)taste;
    
    setTrackParticleLink(track);
    setIParticleLinks(iparticles);
    
  }
  


} // namespace xAOD
