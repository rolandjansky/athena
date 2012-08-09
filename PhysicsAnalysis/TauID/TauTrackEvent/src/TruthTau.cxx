/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "TauTrackEvent/TruthTau.h"
#include <sstream>
//#include <iostream>

//#include "GaudiKernel/MsgStream.h"
//#include <string>

TauID::TruthTau::TruthTau()
    :
    m_truthPartResonance(0),
    m_typeFlags(0),
    m_hlvVis(0),
    m_nPi0(0),
    m_convertedPhotons(0),
    m_chargedProducts(0),
    m_chargedProductsWithHadronicInteraction(0),
    m_neutralProducts(0)
{
}


TauID::TruthTau::TruthTau(
        const HepMC::GenParticle*                               genParticle,
        const std::bitset<TruthTau::NumberOfTruthTauTypes>      typePattern,
        const CLHEP::HepLorentzVector*                          hlvVis,
        unsigned int                                            nPi0,
        const TruthParticle*                                    resonance,
        const TruthParticleContainer *                          container,
        const std::vector<const TruthParticle*>*                convertedPhotons,
        const std::vector<const TruthParticle*>*                chargedProducts,
        const std::vector<unsigned int>*                        chargedProductsWithHadronicInteraction,
        const std::vector<const TruthParticle*>*                neutralProducts )
    :
    TruthParticle(genParticle, container),
    m_truthPartResonance(resonance),
    m_typeFlags( typePattern.to_ulong() ),
    m_hlvVis(hlvVis),
    m_nPi0(nPi0),
    m_convertedPhotons(convertedPhotons),
    m_chargedProducts(chargedProducts),
    m_chargedProductsWithHadronicInteraction(chargedProductsWithHadronicInteraction),
    m_neutralProducts(neutralProducts)
{
}


TauID::TruthTau::TruthTau(
    const TauID::TruthTau& truthtau
    ):
    IAthenaBarCode(truthtau),
    INavigable(truthtau),
    I4Momentum(truthtau),
    INavigable4Momentum(truthtau),
    TruthParticle(truthtau),
    m_truthPartResonance(truthtau.m_truthPartResonance),
    m_typeFlags(truthtau.m_typeFlags),
    m_hlvVis(truthtau.m_hlvVis ? new CLHEP::HepLorentzVector(*truthtau.m_hlvVis) : 0),
    m_nPi0(truthtau.m_nPi0),
    m_convertedPhotons(truthtau.m_convertedPhotons ? new std::vector<const TruthParticle*>(*truthtau.m_convertedPhotons) : 0),
    m_chargedProducts(truthtau.m_chargedProducts ? new std::vector<const TruthParticle*>(*truthtau.m_chargedProducts) : 0),
    m_chargedProductsWithHadronicInteraction(truthtau.m_chargedProductsWithHadronicInteraction ? new std::vector<unsigned int>(*truthtau.m_chargedProductsWithHadronicInteraction) : 0),
    m_neutralProducts(truthtau.m_neutralProducts ? new std::vector<const TruthParticle*>(*truthtau.m_neutralProducts) : 0)
{
}

TauID::TruthTau::~TruthTau()
{
    delete m_hlvVis;
    delete m_convertedPhotons;
    delete m_chargedProducts;
    delete m_chargedProductsWithHadronicInteraction;
    delete m_neutralProducts;
}

std::string TauID::TruthTau::dumpType() const{
    std::string type;
    std::stringstream out;
  for (int i=0; i<NumberOfTruthTauTypes; ++i)
  {
    if ( (1<<i)&m_typeFlags )
    {
      switch ( i ) {
      case TauID::TruthTau::Electron:
        type+="leptonic decay (electron)";
        break;
      case TauID::TruthTau::Muon:
        type+="leptonic decay (muon)";
        break;
      case TauID::TruthTau::Hadronic:
        type+="hadronic decay ";
        break;
      case TauID::TruthTau::Kaon:
        type+="Kaon ";
        break;
      case TauID::TruthTau::OneProng:
        type+="1 Prong ";
        break;
      case TauID::TruthTau::ThreeProng:
        type+="3 Prong ";
        break;
      case TauID::TruthTau::FiveProng:
        type+="5 Prong ";
        break;
      case TauID::TruthTau::Neutrals:
        out << "+ " << m_nPi0 << " Pi0 ";
        break;
      case TauID::TruthTau::Conversions:
        out << "with " << this->nPhotonConversions() << " conversions ";
        break;
      case TauID::TruthTau::HadronicInteractions:
        out << "with " << (m_chargedProductsWithHadronicInteraction ? m_chargedProductsWithHadronicInteraction->size() : 0) << " hadronic interactions ";
        break;
      default:
        type+="Other ";
        break;
      }
    } 
  } 
    type+=out.str();

    return type;
}

PDG::pidType TauID::TruthTau::resonancePdgId() const {
    return (m_truthPartResonance ? m_truthPartResonance->pdgId() : PDG::null);
}

bool TauID::TruthTau::chargedProductHadHadronicInteraction(unsigned int index) const {
    if (!m_chargedProductsWithHadronicInteraction) {
        return false;
    } else {
        std::vector<unsigned int>::const_iterator iter = m_chargedProductsWithHadronicInteraction->begin();
        for (; iter != m_chargedProductsWithHadronicInteraction->end(); iter++) {
            if ((*iter) == index) {
                return true;
            }
        }
    }
    return false;
}


// MsgStream& operator << ( MsgStream& sl, const TauID::TauSeed& seed)
// { 
//     std::string name("TauSeed: ");
//     sl <<name<<"\t of type : "<<seed.dumpType()<<endreq;
//     return sl; 
// }
// 
// std::ostream& operator << ( std::ostream& sl, const TauID::TauSeed& seed)
// { 
//     std::string name("TauSeed: ");
//     sl <<name<<"\t of type : "<<seed.dumpType()<<endreq;
//     return sl; 
// }
