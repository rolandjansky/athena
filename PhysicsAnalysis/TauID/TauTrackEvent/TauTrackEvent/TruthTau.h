/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// TauSeed.h, (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////

#ifndef TAUTRACKEVENT_TRUTHTAU_H
#define TAUTRACKEVENT_TRUTHTAU_H


#include <bitset>
#include <string>
#include "McParticleEvent/TruthParticle.h"

//class MsgStream;
//class HepLorentzVector;

// namespace HepMC {
//    class GenParticle;
// }

namespace TauID {


/** @class TruthTau
    Helper class to collect truth tau properties.
    @author Sebastian.Fleischmann@cern.ch
    */

class TruthTau  : virtual public TruthParticle {

public:
    /**
    * This enum describes the type of the tau seed.
    */
    enum TruthTauType {
        /**
        * Leptonic decay (electron)
        */
        Electron=0,
        /**
        * Leptonic decay (muon)
        */
        Muon=1,
        /**
        * Hadronic decay
        */
        Hadronic=2,
        /**
        * 1 prong
        */
        OneProng=3,
        /**
        * 3 prong
        */
        ThreeProng=4,
        /**
        * 5 prong
        */
        FiveProng=5,
        /**
        * Kaon
        */
        Kaon=6,
        /**
        * Has additional neutrals
        */
        Neutrals=7,
        /**
        * Has photons from pi0 decays, which converted into e+ e-
        */
        Conversions=8,
        /**
        * Has pions which had hadronic interactions in the Inner Detector
        */
        HadronicInteractions=9,
        NumberOfTruthTauTypes=10
    };

    /** Default Constructor for POOL. Do not use! */
    TruthTau();
    /** Copy Constructor */
    TruthTau(const TruthTau& truthtau);

    TruthTau& operator= (const TruthTau& truthtau) = delete;

//    /**
//    * Full constructor. 
//    *
//    * @param[in] truthParticle truthParticle will be cloned (the tau which decays, not one of its products)
//    * @param[in] typePattern The pattern of 'types' which correspond to this
//    *            tau. You create the bitset as follows: 
//    *     std::bitset<TruthTau::NumberOfTruthTauTypes> typePattern(0);
//    *     typePattern.set(TruthTau::ThreeProng);
//    */
//    TruthTau(
//        const TruthParticle&                                    truthParticle,
//        const std::bitset<TruthTau::NumberOfTruthTauTypes>      typePattern
//        );
    /**
    * Full constructor. 
    *
    * construct a TruthParticle from a HepMC::GenParticle and the TruthParticleContainer holding this TruthParticle.
    * @param[in] genParticle the GenParticle the TruthParticle refers to (the tau which decays, not one of its products)
    * @param[in] typePattern The pattern of 'types' which correspond to this
    *            tau. You create the bitset as follows: 
    *     std::bitset<TruthTau::NumberOfTruthTauTypes> typePattern(0);
    *     typePattern.set(TruthTau::ThreeProng);
    * @param[in] hlvVis the CLHEP::HepLorentzVector of the visible component of the decay products. The
    *            TruthTau object takes the ownership! Do not delete this object yourself!
    * @param[in] resonance Pointer to the TruthParticle which is the first resonance in the decay chain.
                 The TruthTau object DOES NOT take the ownership!
    * @param[in] container pointer to the TruthParticleContainer which will hold the new TruthParticle and all its daugthers
    * @param[in] convertedPhotons pointer to vector of TruthParticle which contains pointers to photons from pi0 decays,
    *            which converted. The TruthTau object takes the ownership! Do not delete this object yourself!
    *            However the TruthParticles inside the vector are not owned and will not be deleted by TruthTau!
    * @param[in] chargedProducts pointer to vector of TruthParticle which contains pointers to charged particles from
    *            the decay. The TruthTau object takes the ownership! Do not delete this object yourself!
    *            However the TruthParticles inside the vector are not owned and will not be deleted by TruthTau!
    * @param[in] neutralProducts pointer to vector of TruthParticle which contains pointers to relevant neutral particles
    *            (neutral pions, neutral Kaons, etc) from
    *            the decay. The TruthTau object takes the ownership! Do not delete this object yourself!
    *            However the TruthParticles inside the vector are not owned and will not be deleted by TruthTau!
    */
    TruthTau(
        const HepMC::GenParticle*                               genParticle,
        const std::bitset<TruthTau::NumberOfTruthTauTypes>      typePattern,
        const CLHEP::HepLorentzVector*                          hlvVis,
        unsigned int                                            nPi0 = 0,
        const TruthParticle*                                    resonance = 0,
        const TruthParticleContainer *                          container = 0,
        const std::vector<const TruthParticle*>*                convertedPhotons = 0,
        const std::vector<const TruthParticle*>*                chargedProducts = 0,
        const std::vector<unsigned int>*                        chargedProductsWithHadronicInteraction = 0,
        const std::vector<const TruthParticle*>*                neutralProducts = 0
        );

    /** Destructor */
    virtual ~TruthTau();

    /**
    * Use this method to find out if the TauSeed is of a certain type:
    * i.e. if ( truthtau->type(TruthTau::ThreeProng) { //etc }
    *
    * @return true if the TruthTau is of this type
    */
    bool type( const TruthTauType& type ) const;

    /** returns a string with the type of the object*/
    std::string dumpType() const;

//    /** returns HepMC::GenParticle*/
//    const HepMC::GenParticle* genParticle() const;

    /** returns the visible 4-momentum of the tau*/
    const CLHEP::HepLorentzVector& hlvVis() const;

    /** returns the PDG ID of the resonance*/
    PDG::pidType resonancePdgId() const;

    /** returns the resonance*/
    const TruthParticle* resonance() const;

    /** returns the number of neutral Pions in the decay*/
    unsigned int nPi0() const;

    /** returns the number of photons from pi0 decays which converted*/
    unsigned int nPhotonConversions() const;

    /** returns the photons from pi0 decays which converted*/
    const std::vector<const TruthParticle*>& convertedPhotons() const;

    /** returns the charged particles created in the decay */
    const std::vector<const TruthParticle*>& chargedProducts() const;

    /** did the charged product have hadronic interactions inside the Inner Detector? */
    bool chargedProductHadHadronicInteraction(unsigned int index) const;

    /** returns the relevant neutral particles (neutral pions, neutral Kaons, etc) created in the decay */
    const std::vector<const TruthParticle*>& neutralProducts() const;

//    /**Overload of << operator for MsgStream for debug output*/ 
//    MsgStream& operator << ( MsgStream& sl, const TauSeed& seed);
    
//    /**Overload of << operator for std::ostream for debug output*/ 
//    std::ostream& operator << ( std::ostream& sl, const TauSeed& seed);


protected:

//    /** The gen particle */
//    const HepMC::GenParticle*   m_genPart;
    /** The first resonance */
    const TruthParticle*        m_truthPartResonance;
    /** flags for the types */
    long                        m_typeFlags;
    /** 4-momentum of the visible tau decay products */
    const CLHEP::HepLorentzVector*     m_hlvVis;
    /** number of neutral Pions in decay */
    unsigned int                m_nPi0;
    /** The photons from pi0 decays which converted */
    mutable const std::vector<const TruthParticle*>*    m_convertedPhotons;
    /** The charged decay products */
    mutable const std::vector<const TruthParticle*>*    m_chargedProducts;
    /** Indices of charged decay products which had hadronic interactions*/
    mutable const std::vector<unsigned int>*            m_chargedProductsWithHadronicInteraction;
    /** The neutral decay products */
    mutable const std::vector<const TruthParticle*>*    m_neutralProducts;

};

inline bool TauID::TruthTau::type( const TruthTauType& type ) const
{
    if ((1<<static_cast<int>(type))&m_typeFlags) return true;
    return false;
}

inline const CLHEP::HepLorentzVector& TauID::TruthTau::hlvVis() const
{ 
    return *m_hlvVis;
}

inline unsigned int TauID::TruthTau::nPi0() const {
    return m_nPi0;
}

inline unsigned int TauID::TruthTau::nPhotonConversions() const {
    return (m_convertedPhotons ? m_convertedPhotons->size() : 0);
}


inline const std::vector<const TruthParticle*>& TauID::TruthTau::convertedPhotons() const {
    if (!m_convertedPhotons) {
        m_convertedPhotons = new std::vector<const TruthParticle*>(0);
    }
    return *m_convertedPhotons;
}

inline const TruthParticle* TauID::TruthTau::resonance() const {
    return m_truthPartResonance;
}

inline const std::vector<const TruthParticle*>& TauID::TruthTau::chargedProducts() const {
    if (!m_chargedProducts) {
        m_chargedProducts = new std::vector<const TruthParticle*>(0);
    }
    return *m_chargedProducts;
}

inline const std::vector<const TruthParticle*>& TauID::TruthTau::neutralProducts() const {
    if (!m_neutralProducts) {
        m_neutralProducts = new std::vector<const TruthParticle*>(0);
    }
    return *m_neutralProducts;
}

}

#endif // TAUTRACKEVENT_TRUTHTAU_H
