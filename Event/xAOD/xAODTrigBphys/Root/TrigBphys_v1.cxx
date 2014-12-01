/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: TrigBphys_v1.cxx 631136 2014-11-26 11:22:46Z gwatts $

// xAOD include(s):
#include "xAODCore/AuxStoreAccessorMacros.h"

// Local include(s):
#include "xAODTrigBphys/versions/TrigBphys_v1.h"
#include "xAODTrigBphys/TrigBphysContainer.h"
#include "xAODTrigBphys/versions/TrigBphysContainer_v1.h"

namespace xAOD {

    TrigBphys_v1::TrigBphys_v1()
    : SG::AuxElement() {
    } // TrigBphys_v1
    

    //** initialise with defaults. Attempt to call this after creating a particle */
    void TrigBphys_v1::initialise() {
        setRoiId(0);
        setEta  (0.);
        setPhi  (0.);
        setMass (0.);
        setLevel       (UNKOWNLEVEL);
        setParticleType(UNKNOWNPTYPE);
        // set defaults
        setFitmass(-99.);
        setFitchi2(-99.);
        setFitndof(-99);
        setFitx   (-99);
        setFity   (-99);
        setFitz   (-99);
        // ensure that we clear unused variables
        //secondaryDecay.clear();
        //trackVector.clear();

    
    } // initialise
  
    
    
    void TrigBphys_v1::clone(const TrigBphys_v1 & rhs) {
        if (this == &rhs) {
            // if attempting to clone oneself, dont
            return;
        }
        initialise(rhs.roiId(), rhs.eta(), rhs.phi(), rhs.particleType(),
                   rhs.mass(), rhs.level());
        setFitmass(rhs.fitmass());
        setFitchi2(rhs.fitchi2());
        setFitndof(rhs.fitndof());
        setFitx   (rhs.fitx());
        setFity   (rhs.fity());
        setFitz   (rhs.fitz());

        setSecondaryDecayLink(rhs.secondaryDecayLink());
        setTrackParticleLinks(rhs.trackParticleLinks());
        
        setVecRoiIds(rhs.vecRoiIds());
        setLowerChainLink(rhs.lowerChainLink());
        setParticleLinks(rhs.particleLinks());
    }


    //** initialise with default parameters */
    void TrigBphys_v1::initialise(uint32_t roi, float eta, float phi, pType particleType,
                                  float mass, levelType level) {
        
        setRoiId(roi);
        setEta  (eta);
        setPhi  (phi);
        setMass (mass);
        setLevel(level);
        setParticleType(particleType);
        // set defaults
        setFitmass(-99);
        setFitchi2(-99.);
        setFitndof(-99);
        setFitx(-99);
        setFity(-99);
        setFitz(-99);
        // ensure that we clear unused variables
        //secondaryDecay.clear();
        //trackVector.clear(); #FIXME
        
    } //initialise
    
    /** constructor replacement for particle of type "particleType" with mass "mass"
     and a secondary decay (given as ElementLink to another TrigEFBphys) */
    void TrigBphys_v1::initialise(uint32_t roi, float eta, float phi, pType particleType,
                                  float mass, const ElementLink<xAOD::TrigBphysContainer_v1 >& sDecay,
                                  levelType level) {

        // call this method first, as it clears the trackVector and
        initialise(roi,eta,phi,particleType,mass,level);
        setSecondaryDecayLink(sDecay);
    } // initialise

    
    
    // use macros to create the getter and setter class methods
    AUXSTORE_PRIMITIVE_SETTER_AND_GETTER( xAOD::TrigBphys_v1, uint32_t, roiId, setRoiId )
    AUXSTORE_PRIMITIVE_GETTER_WITH_CAST( xAOD::TrigBphys_v1, uint16_t, TrigBphys_v1::pType, particleType )
    AUXSTORE_PRIMITIVE_SETTER_WITH_CAST( xAOD::TrigBphys_v1, uint16_t, TrigBphys_v1::pType, particleType, setParticleType )
    AUXSTORE_PRIMITIVE_GETTER_WITH_CAST( xAOD::TrigBphys_v1, uint16_t, TrigBphys_v1::levelType, level )
    AUXSTORE_PRIMITIVE_SETTER_WITH_CAST( xAOD::TrigBphys_v1, uint16_t, TrigBphys_v1::levelType, level, setLevel )
    AUXSTORE_PRIMITIVE_SETTER_AND_GETTER( xAOD::TrigBphys_v1, float, eta,     setEta )
    AUXSTORE_PRIMITIVE_SETTER_AND_GETTER( xAOD::TrigBphys_v1, float, phi,     setPhi )
    AUXSTORE_PRIMITIVE_SETTER_AND_GETTER( xAOD::TrigBphys_v1, float, mass,    setMass )
    AUXSTORE_PRIMITIVE_SETTER_AND_GETTER( xAOD::TrigBphys_v1, float, fitmass, setFitmass )
    AUXSTORE_PRIMITIVE_SETTER_AND_GETTER( xAOD::TrigBphys_v1, float, fitchi2, setFitchi2 )
    AUXSTORE_PRIMITIVE_SETTER_AND_GETTER( xAOD::TrigBphys_v1, int,   fitndof, setFitndof )
    AUXSTORE_PRIMITIVE_SETTER_AND_GETTER( xAOD::TrigBphys_v1, float, fitx,    setFitx )
    AUXSTORE_PRIMITIVE_SETTER_AND_GETTER( xAOD::TrigBphys_v1, float, fity,    setFity )
    AUXSTORE_PRIMITIVE_SETTER_AND_GETTER( xAOD::TrigBphys_v1, float, fitz,    setFitz )

    AUXSTORE_OBJECT_SETTER_AND_GETTER( TrigBphys_v1, ElementLink< TrigBphysContainer_v1 >,
                                       secondaryDecayLink, setSecondaryDecayLink )

    /** accessor method: secondary decay particle */
    const TrigBphys_v1* TrigBphys_v1::secondaryDecay() const {
        static Accessor< ElementLink< TrigBphysContainer_v1 > > acc( "secondaryDecayLink" );
        if( ! acc.isAvailable( *this ) ) {
           return 0;
        }
        const ElementLink< TrigBphysContainer_v1 > & sd = acc( *this );
        if (sd.isValid()) {
            return *sd;
        } else {
            return nullptr;
        }
    } //pSecondDecay

   AUXSTORE_OBJECT_SETTER_AND_GETTER( TrigBphys_v1,
                                      std::vector< ElementLink< TrackParticleContainer > >,
                                      trackParticleLinks, setTrackParticleLinks )

    /// Accessor for the track particle links variable
    static SG::AuxElement::Accessor< std::vector< ElementLink< TrackParticleContainer > > >
       trackAcc( "trackParticleLinks" );

    size_t TrigBphys_v1::nTrackParticles() const {

       if( ! trackAcc.isAvailable( *this ) ) {
          return 0;
       }
       return trackAcc( *this ).size();
    }

    const TrackParticle* TrigBphys_v1::trackParticle( size_t i ) const {

       if( ! trackAcc.isAvailable( *this ) ) {
          return 0;
       }
       if( i >= nTrackParticles() ) {
          return 0;
       }
       if( ! trackAcc( *this )[ i ].isValid() ) {
          return 0;
       }
       return *( trackAcc( *this )[ i ] );
    }

    /** add a track to the vector of tracks */
    void TrigBphys_v1::addTrackParticleLink(const ElementLink<xAOD::TrackParticleContainer>& track) {
       trackAcc( *this ).push_back( track );
       return;
    } //addTrack

    bool TrigBphys_v1::operator==(const TrigBphys_v1& rhs) const {
        const double epsilon = 1e-8;

        int aHasSD = (secondaryDecay()!=NULL)?1:0;
        int bHasSD = (rhs.secondaryDecay()!=NULL)?1:0;
        if((aHasSD+bHasSD)==1)
            return false;

        if(trackParticleLinks().size()!=rhs.trackParticleLinks().size())
            return false;

        if(roiId()        != rhs.roiId())         return false;
        if(particleType() != rhs.particleType())  return false;
        if(level()        != rhs.level())     return false;
        if(fitndof()      != rhs.fitndof())       return false;

        if(fabs(eta()-rhs.eta())>epsilon)         return false;
        if(fabs(phi()-rhs.phi())>epsilon)         return false;
        if(fabs(mass()-rhs.mass())>epsilon)       return false;
        if(fabs(fitmass()-rhs.fitmass())>epsilon) return false;
        if(fabs(fitchi2()-rhs.fitchi2())>epsilon) return false;
        if(fabs(fitx()-rhs.fitx())>epsilon)       return false;
        if(fabs(fity()-rhs.fity())>epsilon)       return false;
        if(fabs(fitz()-rhs.fitz())>epsilon)       return false;

        if((aHasSD+bHasSD)==2)
        {
            if ( secondaryDecay() != rhs.secondaryDecay() )
                return false;
        }

        return true;
    }


    
    AUXSTORE_OBJECT_SETTER_AND_GETTER( TrigBphys_v1, ElementLink< TrigBphysContainer_v1 >,
                                       lowerChainLink, setLowerChainLink )

    /** accessor method: lower chain particle */
    const TrigBphys_v1* TrigBphys_v1::lowerChain() const {
        static Accessor< ElementLink< TrigBphysContainer_v1 > > acc( "lowerChainLink" );
        if( ! acc.isAvailable( *this ) ) {
           return nullptr;
        }
        const ElementLink< TrigBphysContainer_v1 > & sd = acc( *this );
        if (sd.isValid()) {
            return *sd;
        } else {
            return nullptr;
        }
    } //pLowerChain

   AUXSTORE_OBJECT_SETTER_AND_GETTER( TrigBphys_v1,
                                      std::vector< ElementLink< IParticleContainer > >,
                                      particleLinks, setParticleLinks )
    
    /// Accessor for the track particle links variable
    static SG::AuxElement::Accessor< std::vector< ElementLink< IParticleContainer > > >
    particleAcc( "particleLinks" );
    
    size_t TrigBphys_v1::nParticles() const {
        
        if( ! particleAcc.isAvailable( *this ) ) {
            return 0;
        }
        return particleAcc( *this ).size();
    }

    const IParticle * TrigBphys_v1::particle( size_t i ) const {
        
        if( ! particleAcc.isAvailable( *this ) ) {
            return 0;
        }
        if( i >= nParticles() ) {
            return 0;
        }
        if( ! particleAcc( *this )[ i ].isValid() ) {
            return 0;
        }
        return *( particleAcc( *this )[ i ] );
    }
    
    /** add a track to the vector of particles */
    void TrigBphys_v1::addParticleLink(const ElementLink<xAOD::IParticleContainer>& particle) {
        particleAcc( *this ).push_back( particle );
        return;
    } //addTrack


    AUXSTORE_OBJECT_SETTER_AND_GETTER( TrigBphys_v1,
                                      std::vector< uint32_t >,
                                      vecRoiIds, setVecRoiIds )

    /// Accessor for the vecRoiId variable
    static SG::AuxElement::Accessor< std::vector< uint32_t > >
    vRoiAcc( "vecRoiIds" );
    
    size_t TrigBphys_v1::nVecRoiIds() const {
        
        if( ! vRoiAcc.isAvailable( *this ) ) {
            return 0;
        }
        return vRoiAcc( *this ).size();
    }
    
    uint32_t TrigBphys_v1::vecRoiId( size_t i ) const {
        
        if( ! vRoiAcc.isAvailable( *this ) ) {
            return ~0;
        }
        if( i >= nVecRoiIds() ) {
            return ~0;
        }
        return vRoiAcc( *this )[ i ];
    }
    
    /** add a roiId to the vector */
    void TrigBphys_v1::addVecRoiId(uint32_t roiId) {
        vRoiAcc( *this ).push_back( roiId );
        return;
    } //addTrack

    
} // namespace xAOD
