/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "PanTauEvent/CorrectedEflowMomentum.h"
#include "CLHEP/Vector/LorentzVector.h"

PanTau::CorrectedEflowMomentum::CorrectedEflowMomentum()
    :
    m_eflowObject(0),
    m_typeFlags(0),
    m_correctedMomentum(0),
    m_fittedMomentum(0)
{
}


PanTau::CorrectedEflowMomentum::CorrectedEflowMomentum(
        const eflowObject                                   *eflow,
        const std::bitset<CorrectedEflowMomentum::NumberOfCorrectedEflowMomentumTypes>    typePattern,
        const CLHEP::HepLorentzVector                       *correctedMomentum,
        const CLHEP::HepLorentzVector                       *fittedMomentum
        ) :
    m_eflowObject(eflow),
    m_typeFlags( typePattern.to_ulong() ),
    m_correctedMomentum(correctedMomentum),
    m_fittedMomentum(fittedMomentum)
{
}


PanTau::CorrectedEflowMomentum::CorrectedEflowMomentum(
    const PanTau::CorrectedEflowMomentum& rhs
    ):
    m_eflowObject(rhs.m_eflowObject),
    m_typeFlags(rhs.m_typeFlags),
    m_correctedMomentum(rhs.m_correctedMomentum ? new CLHEP::HepLorentzVector(*rhs.m_correctedMomentum) : 0),
    m_fittedMomentum(rhs.m_fittedMomentum ? new CLHEP::HepLorentzVector(*rhs.m_fittedMomentum) : 0)
{
}

PanTau::CorrectedEflowMomentum::~CorrectedEflowMomentum()
{
    delete m_correctedMomentum;
    delete m_fittedMomentum;
}

PanTau::CorrectedEflowMomentum& PanTau::CorrectedEflowMomentum::operator=(const PanTau::CorrectedEflowMomentum& rhs)
{
    if (this!=&rhs){
        m_eflowObject   = rhs.m_eflowObject;
        m_typeFlags     = rhs.m_typeFlags;
        delete m_correctedMomentum;
        m_correctedMomentum = (rhs.m_correctedMomentum ? new CLHEP::HepLorentzVector(*rhs.m_correctedMomentum) : 0);
        delete m_fittedMomentum;
        m_fittedMomentum    = (rhs.m_fittedMomentum ? new CLHEP::HepLorentzVector(*rhs.m_fittedMomentum) : 0);
    }
    return *this;
}

std::string PanTau::CorrectedEflowMomentum::typeName(CorrectedEflowMomentumType theType) {
    
    switch(theType) {
        case CorrectedEflowMomentum::Charged:               return "Charged";
        case CorrectedEflowMomentum::QualifiedTrack:        return "QualChrg";
        case CorrectedEflowMomentum::QualifiedTrack2GeV:    return "QualChrg2";
        case CorrectedEflowMomentum::EmNeutral:             return "EmNeut";
        case CorrectedEflowMomentum::Pi0:                   return "Pi0Neut";
        case CorrectedEflowMomentum::HadNeutral:            return "HadNeut";
        case CorrectedEflowMomentum::Neutral:               return "Neutral";
        case CorrectedEflowMomentum::Neutral1GeV:           return "Neutral1";
        case CorrectedEflowMomentum::Neutral2GeV:           return "Neutral2";
        case CorrectedEflowMomentum::NeutralCore1GeV:       return "NeutCore1";
        case CorrectedEflowMomentum::NeutralCore2GeV:       return "NeutCore2";
        case CorrectedEflowMomentum::QualifiedNeutral:      return "QualNeut";
        case CorrectedEflowMomentum::NumberOfCorrectedEflowMomentumTypes: return "All";
        default:                                            return "BadType";
        
    }
}
