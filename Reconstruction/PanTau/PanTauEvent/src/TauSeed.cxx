/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "PanTauEvent/TauSeed.h"
#include "PanTauEvent/TauFeature.h"
#include "PanTauEvent/CorrectedEflowMomentum.h"

//#include "GaudiKernel/MsgStream.h"
#include "JetEvent/Jet.h"
#include <string>

bool sortCorrectedEflowEt(const PanTau::CorrectedEflowMomentum* u, const PanTau::CorrectedEflowMomentum* v) {
    return u->correctedMomentum()->et() > v->correctedMomentum()->et();    //This is bad! Find appropriate location!
}



/// //////////////////////////////////////////////////////////
/// Default constructor
/// //////////////////////////////////////////////////////////
PanTau::TauSeed::TauSeed()
    :
    P4EEtaPhiM(0., 0., 0., 0.),
    m_jet(0),
    m_TauJet(0),
    m_RecoModeLoose(),
    m_RecoModeMedium(),
    m_RecoModeTight(),
    m_features(0),
    m_correctedEflowMomenta(0),
    m_chargedEflowMomenta(nullptr),
    m_emNeutralEflowMomenta(nullptr)
{
}



/// //////////////////////////////////////////////////////////
/// Constructor for TauRec-Seeding
/// //////////////////////////////////////////////////////////
PanTau::TauSeed::TauSeed(
                        const Analysis::TauJet*                     tauJet,
                        const Jet*                                  tauJet_Jet,
                        const I4Momentum&                           fourMom_core,
                        const PanTau::TauFeature*                   features,
                        const DataVector<CorrectedEflowMomentum>*   corEfoMomenta
                        )
                    :
                        P4EEtaPhiM(fourMom_core),
                        m_jet(tauJet_Jet),
                        m_TauJet(tauJet),
                        m_RecoModeLoose(),
                        m_RecoModeMedium(),
                        m_RecoModeTight(),
                        m_features(features),
                        m_correctedEflowMomenta(corEfoMomenta),
                        m_chargedEflowMomenta(nullptr),
                        m_emNeutralEflowMomenta(nullptr)
{
}
    
    
    
/// //////////////////////////////////////////////////////////
/// Constructor for eflowJet-Seeding
/// //////////////////////////////////////////////////////////
PanTau::TauSeed::TauSeed(
                        const Jet*                                  eflowJet,
                        const I4Momentum&                           fourMom_core,
                        const PanTau::TauFeature*                   features,
                        const DataVector<CorrectedEflowMomentum>*   corEfoMomenta
                        )
                    :
                        P4EEtaPhiM(fourMom_core),
                        m_jet(eflowJet),
                        m_TauJet(0),
                        m_RecoModeLoose(),
                        m_RecoModeMedium(),
                        m_RecoModeTight(),
                        m_features(features),
                        m_correctedEflowMomenta(corEfoMomenta),
                        m_chargedEflowMomenta(nullptr),
                        m_emNeutralEflowMomenta(nullptr)
{
}



/// //////////////////////////////////////////////////////////
/// Copy constructor
/// //////////////////////////////////////////////////////////
PanTau::TauSeed::TauSeed(
    const PanTau::TauSeed& rhs
    ):
    I4Momentum(),
    P4EEtaPhiMBase(),
    P4EEtaPhiM(rhs),
    m_jet(rhs.m_jet),
    m_TauJet(rhs.m_TauJet),
    m_RecoModeLoose(rhs.m_RecoModeLoose),
    m_RecoModeMedium(rhs.m_RecoModeMedium),
    m_RecoModeTight(rhs.m_RecoModeTight),
    m_features(rhs.m_features ? new PanTau::TauFeature(*rhs.m_features) : 0),
    m_correctedEflowMomenta(rhs.m_correctedEflowMomenta ? new DataVector<CorrectedEflowMomentum>(*rhs.m_correctedEflowMomenta) : 0),
    // FIXME: Check again that this works: The copy constructor of DataVector is a `shallow' copy; the new container
    // does not own its elements. Do we get problems here, if the copied TauSeed is deleted? Maybe! We have to clone
    // all the CorrectedEflowMomentum for those DataVectors which own their elements
    m_chargedEflowMomenta(rhs.m_chargedEflowMomenta ? new ConstDataVector<DataVector<CorrectedEflowMomentum> >(*rhs.m_chargedEflowMomenta) : 0),
    m_emNeutralEflowMomenta(rhs.m_emNeutralEflowMomenta ? new ConstDataVector<DataVector<CorrectedEflowMomentum> >(*rhs.m_emNeutralEflowMomenta) : 0)
{
}



/// //////////////////////////////////////////////////////////
/// Destructor
/// //////////////////////////////////////////////////////////
PanTau::TauSeed::~TauSeed()
{
    delete m_jet;
    delete m_features;
    delete m_correctedEflowMomenta;
    delete m_chargedEflowMomenta;
    delete m_emNeutralEflowMomenta;
}



/// //////////////////////////////////////////////////////////
/// Assignment operator
/// //////////////////////////////////////////////////////////
PanTau::TauSeed& PanTau::TauSeed::operator=(const PanTau::TauSeed& seed)
{
    if (this!=&seed){
        this->set4Mom(seed);
        m_jet = seed.m_jet;
        m_TauJet = seed.m_TauJet;
        delete m_features;
        m_features = (seed.m_features ? new PanTau::TauFeature(*seed.m_features) : 0);
        delete m_correctedEflowMomenta;
        m_correctedEflowMomenta = (seed.m_correctedEflowMomenta ? new DataVector<CorrectedEflowMomentum>(*seed.m_correctedEflowMomenta) : 0);
        // FIXME: Check again that this works: The copy constructor of DataVector is a `shallow' copy; the new container
        // does not own its elements. Do we get problems here, if the assigned TauSeed is deleted?
        m_chargedEflowMomenta = (seed.m_chargedEflowMomenta ? new ConstDataVector<DataVector<CorrectedEflowMomentum> >(*seed.m_chargedEflowMomenta) : 0);
        m_emNeutralEflowMomenta = (seed.m_emNeutralEflowMomenta ? new ConstDataVector<DataVector<CorrectedEflowMomentum> >(*seed.m_emNeutralEflowMomenta) : 0);

    }
    return *this;
}



/// //////////////////////////////////////////////////////////
/// getter function for correctedEflowMomenta in this seed
/// //////////////////////////////////////////////////////////
const DataVector<PanTau::CorrectedEflowMomentum>& PanTau::TauSeed::correctedEflowMomenta() const {
    if (!m_correctedEflowMomenta) {
        m_correctedEflowMomenta = new DataVector<CorrectedEflowMomentum>(0);
    }
    return *m_correctedEflowMomenta;
}



const DataVector<PanTau::CorrectedEflowMomentum>& PanTau::TauSeed::chargedEflowMomenta() const {
    if (!m_chargedEflowMomenta) {
        // create a view collection based on the given m_correctedEflowMomenta
        ConstDataVector<DataVector<CorrectedEflowMomentum> >* chargedMomenta = new ConstDataVector<DataVector<CorrectedEflowMomentum> >(SG::VIEW_ELEMENTS);
        m_chargedEflowMomenta = chargedMomenta;
        if (!m_correctedEflowMomenta) return *m_chargedEflowMomenta->asDataVector();
        // loop over corrected eflow momenta to get the charged ones
        DataVector<CorrectedEflowMomentum>::const_iterator  efoMomIter = m_correctedEflowMomenta->begin();
        for ( ; efoMomIter!=m_correctedEflowMomenta->end(); efoMomIter++) {
            if ((*efoMomIter)->type(CorrectedEflowMomentum::Charged)) {
                chargedMomenta->push_back(*efoMomIter);
            }
        }
        chargedMomenta->sort( sortCorrectedEflowEt );
    }
    return *m_chargedEflowMomenta->asDataVector();
}

const DataVector<PanTau::CorrectedEflowMomentum>& PanTau::TauSeed::emNeutralEflowMomenta() const {
    if (!m_emNeutralEflowMomenta) {
        // create a view collection based on the given m_correctedEflowMomenta
        ConstDataVector<DataVector<CorrectedEflowMomentum> >* emNeutralMomenta = new ConstDataVector<DataVector<CorrectedEflowMomentum> >(SG::VIEW_ELEMENTS);
        m_emNeutralEflowMomenta = emNeutralMomenta;
        if (!m_correctedEflowMomenta) return *m_emNeutralEflowMomenta->asDataVector();
        // loop over corrected eflow momenta to get the em neutral ones
        ConstDataVector<DataVector<CorrectedEflowMomentum> >::const_iterator  efoMomIter = m_correctedEflowMomenta->begin();
        for ( ; efoMomIter!=m_correctedEflowMomenta->end(); efoMomIter++) {
            if ((*efoMomIter)->type(CorrectedEflowMomentum::EmNeutral)) {
                emNeutralMomenta->push_back(*efoMomIter);
            }
        }
        emNeutralMomenta->sort( sortCorrectedEflowEt );
    }
    return *m_emNeutralEflowMomenta->asDataVector();
}


    
std::string PanTau::TauSeed::dumpType() const {
    std::string type = this->getRecoModeLooseName();
    return type;
}

