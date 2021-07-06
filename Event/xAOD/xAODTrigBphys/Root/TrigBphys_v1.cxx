/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/


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
        setPt   (0.);
        setMass (0.);
        setLevel       (UNKOWNLEVEL);
        setParticleType(UNKNOWNPTYPE);
        // set defaults
        setFitmass(-99.);
        setFitchi2(-99.);
        setFitndof(-99.);
        setFitx   (-99.);
        setFity   (-99.);
        setFitz   (-99.);
        
        setFitmassError( -99.);
        setLxy     ( -99.);
        setLxyError( -99.);
        setTau     ( -99.);
        setTauError( -99.);

        
        // ensure that we clear unused variables
        //secondaryDecay.clear();
        //trackVector.clear();

    
    } // initialise
  
    
    
    void TrigBphys_v1::clone(const TrigBphys_v1 & rhs) {
        if (this == &rhs) {
            // if attempting to clone oneself, dont
            return;
        }
        initialise(rhs.roiId(), rhs.eta(), rhs.phi(), rhs.pt(),
                   rhs.particleType(),
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
    void TrigBphys_v1::initialise(uint32_t roi, float eta, float phi, float pt,
                                  pType particleType,
                                  float mass, levelType level) {
        setRoiId(roi);
        setEta  (eta);
        setPhi  (phi);
        setPt   (pt);
        setMass (mass);
        setLevel(level);
        setParticleType(particleType);
        // set defaults
        setFitmass(-99.);
        setFitchi2(-99.);
        setFitndof(-99.);
        setFitx   (-99.);
        setFity   (-99.);
        setFitz   (-99.);
        
        setFitmassError( -99.);
        setLxy     ( -99.);
        setLxyError( -99.);
        setTau     ( -99.);
        setTauError( -99.);

        // ensure that we clear unused variables
        //secondaryDecay.clear();
        //trackVector.clear(); #FIXME
        
    } //initialise

    
    //** initialise with default parameters */
    void TrigBphys_v1::initialise(uint32_t roi, float eta, float phi, pType particleType,
                                  float mass, levelType level) {
        // deprecated method, as pt is initialised to 0. here.
        // use the alternative initialise method
        setRoiId(roi);
        setEta  (eta);
        setPhi  (phi);
        setPt   (0.0);
        setMass (mass);
        setLevel(level);
        setParticleType(particleType);
        // set defaults
        setFitmass(-99.);
        setFitchi2(-99.);
        setFitndof(-99.);
        setFitx   (-99.);
        setFity   (-99.);
        setFitz   (-99.);
        
        setFitmassError( -99.);
        setLxy     ( -99.);
        setLxyError( -99.);
        setTau     ( -99.);
        setTauError( -99.);

        // ensure that we clear unused variables
        //secondaryDecay.clear();
        //trackVector.clear(); #FIXME
        
    } //initialise
    
    /** constructor replacement for particle of type "particleType" with mass "mass"
     and a secondary decay (given as ElementLink to another TrigEFBphys) */
    void TrigBphys_v1::initialise(uint32_t roi, float eta, float phi, pType particleType,
                                  float mass, const ElementLink<xAOD::TrigBphysContainer_v1 >& sDecay,
                                  levelType level) {
        // deprecated method, as pt is initialised to 0. here.
        // use the alternative initialise method

        // call this method first, as it clears the trackVector and
        initialise(roi,eta,phi,particleType,mass,level);
        setSecondaryDecayLink(sDecay);
    } // initialise

    /** constructor replacement for particle of type "particleType" with mass "mass"
     and a secondary decay (given as ElementLink to another TrigEFBphys) */
    void TrigBphys_v1::initialise(uint32_t roi, float eta, float phi, float pt,
                                  pType particleType,
                                  float mass, const ElementLink<xAOD::TrigBphysContainer_v1 >& sDecay,
                                  levelType level) {
        // call this method first, as it clears the trackVector and
        initialise(roi,eta,phi,pt,particleType,mass,level);
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
    // no default pt method, as need to check if it exists for older file compatibility
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
        static const Accessor< ElementLink< TrigBphysContainer_v1 > > acc( "secondaryDecayLink" );
        if( ! acc.isAvailable( *this ) ) {
           return nullptr;
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
    static const SG::AuxElement::Accessor< std::vector< ElementLink< TrackParticleContainer > > >
       trackAcc( "trackParticleLinks" );

    size_t TrigBphys_v1::nTrackParticles() const {

       if( ! trackAcc.isAvailable( *this ) ) {
          return 0;
       }
       return trackAcc( *this ).size();
    }

    const TrackParticle* TrigBphys_v1::trackParticle( size_t i ) const {

       if( ! trackAcc.isAvailable( *this ) ) {
          return nullptr;
       }
       if( i >= nTrackParticles() ) {
          return nullptr;
       }
       if( ! trackAcc( *this )[ i ].isValid() ) {
          return nullptr;
       }
       return *( trackAcc( *this )[ i ] );
    }

    /** add a track to the vector of tracks */
    void TrigBphys_v1::addTrackParticleLink(const ElementLink<xAOD::TrackParticleContainer>& track) {
       trackAcc( *this ).push_back( track );
       return;
    } //addTrack

    bool TrigBphys_v1::operator==(const TrigBphys_v1& rhs) const {
        const double epsilon = 1e-5;

        int aHasSD = (secondaryDecay()    !=nullptr)?1:0;
        int bHasSD = (rhs.secondaryDecay()!=nullptr)?1:0;
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
        if(fabs(pt()*0.001-rhs.pt()*0.001)>epsilon)return false; // internal conversion to GeV for more sensible comparison.
        if(fabs(mass()*0.001-rhs.mass()*0.001)>epsilon)       return false;
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
        static const Accessor< ElementLink< TrigBphysContainer_v1 > > acc( "lowerChainLink" );
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
    static const SG::AuxElement::Accessor< std::vector< ElementLink< IParticleContainer > > >
    particleAcc( "particleLinks" );
    
    size_t TrigBphys_v1::nParticles() const {
        
        if( ! particleAcc.isAvailable( *this ) ) {
            return 0;
        }
        return particleAcc( *this ).size();
    }

    const IParticle * TrigBphys_v1::particle( size_t i ) const {
        
        if( ! particleAcc.isAvailable( *this ) ) {
            return nullptr;
        }
        if( i >= nParticles() ) {
            return nullptr;
        }
        if( ! particleAcc( *this )[ i ].isValid() ) {
            return nullptr;
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
    static const SG::AuxElement::Accessor< std::vector< uint32_t > >
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

    // get pT, but check if is in the object
    float TrigBphys_v1::pt() const {
        // #FIXME - why doesn't this work as expected?
        static const SG::AuxElement::Accessor< float > ptAcc( "pt" );
        if( ! ptAcc.isAvailable( *this ) ) {
            return 0.;
        } 
        return ptAcc( *this );
    }
    
    void TrigBphys_v1::setPt(float pt) {
        static const SG::AuxElement::Accessor< float > ptAcc( "pt" );
        ptAcc(*this) = pt;
        return;
    }

    static const SG::AuxElement::Accessor< float > fitmassErrorAcc( "fitmassError" );
    static const SG::AuxElement::Accessor< float > lxyAcc         ( "lxy"          );
    static const SG::AuxElement::Accessor< float > lxyErrorAcc    ( "lxyError"     );
    static const SG::AuxElement::Accessor< float > tauAcc         ( "tau"          );
    static const SG::AuxElement::Accessor< float > tauErrorAcc    ( "tauError"     );

    
    float TrigBphys_v1::fitmassError() const {
        return fitmassErrorAcc.isAvailable(*this)? fitmassErrorAcc(*this) : -99.;
    }
    float TrigBphys_v1::lxy() const {
        return lxyAcc.isAvailable(*this)         ? lxyAcc(*this)      : -99.;
    }
    float TrigBphys_v1::lxyError() const {
        return lxyErrorAcc.isAvailable(*this)    ? lxyErrorAcc(*this) : -99.;
    }
    float TrigBphys_v1::tau() const {
        return tauAcc.isAvailable(*this)         ? tauAcc(*this)      : -99.;
    }
    float TrigBphys_v1::tauError() const {
        return tauErrorAcc.isAvailable(*this)    ? tauErrorAcc(*this) : -99.;
    }

    
    void TrigBphys_v1::setFitmassError(float x) {
        fitmassErrorAcc(*this) = x;
        return;
    }
    void TrigBphys_v1::setLxy(float x) {
        lxyAcc(*this) = x;
        return;
    }
    void TrigBphys_v1::setLxyError(float x) {
        lxyErrorAcc(*this) = x;
        return;
    }
    void TrigBphys_v1::setTau(float x) {
        tauAcc(*this) = x;
        return;
    }
    void TrigBphys_v1::setTauError(float x) {
        tauErrorAcc(*this) = x;
        return;
    }

    
    
} // namespace xAOD
