/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// TauSeed.h, (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////

#ifndef PANTAUEVENTTAUSEED_H
#define PANTAUEVENTTAUSEED_H

#include "AthContainers/DataVector.h"
#include "AthContainers/ConstDataVector.h"

#include "tauEvent/TauJet.h"

#include <iostream>
#include <utility>
#include <bitset>
#include "FourMom/P4EEtaPhiM.h"

#include "PanTauEvent/TauClassificationTypes.h"

class MsgStream;
class Jet;
//class eflowObject;

// namespace CLHEP {
// class HepLorentzVector;
// }

namespace PanTau {

class TauFeature;
class CorrectedEflowMomentum;

/** @class TauSeed
    Seed for Pan Tau.
    @author Sebastian.Fleischmann@cern.ch
    @author Robindra.Prabhu@cern.ch (extended 08.07.08 )
    @author limbach@physik.uni-bonn.de (modified October 2012 )
    */

class TauSeed : virtual public P4EEtaPhiM {

public:

    /** Default Constructor for POOL. Do not use! */
    TauSeed();
    /** Copy Constructor */
    TauSeed(const TauSeed& seed);
    /** Assignment operator */
    TauSeed& operator=(const TauSeed& seed);

    /**
    * Full constructor. 
    *
    * The jet object passed in does NOT belong to the this object!
    *
    * (in short delete it yourself) 
    *
    * @param[in] jet pointer to jet built of eflowObjects during seed building (TauSeed takes ownership)
    * @param[in] type the type of the seed
    * @param[in] momentum 4-momentum of the seed (may differ from 4-momentum of the jet)
    * @param[in] features pointer to TauFeature (TauSeed takes ownership!)
    * @param[in] correctedEflowMomenta pointer to DataVector of CorrectedEflowMomentum holding the corrected 4-momenta of eflowObjects (TauSeed takes ownership!).
    */
    
    //! climbach: October 2012
    //! Trying two new constructors:
    //! A: For tauRec seeding
    //! B: For eflowJet seeding
    //! Also see new set- function to set the seed type
    
    //!A:
    TauSeed(
        const Analysis::TauJet*                     tauJet,
        const Jet*                                  eflowJet,
        const I4Momentum&                           fourMom_core,
        const PanTau::TauFeature*                   features,
        const DataVector<CorrectedEflowMomentum>*   corEfoMomenta
           );
    
    
    //!B:
    TauSeed(
        const Jet*                                  eflowJet,
        const I4Momentum&                           fourMom_core,
        const PanTau::TauFeature*                   features,
        const DataVector<CorrectedEflowMomentum>*   corEfoMomenta
           );
    
    
    /** Destructor */
    virtual ~TauSeed();
    
    
    /** returns Jet
     *  This will return the jet built of eflowObjects that is assigned to this tau
     */
    const Jet* jet() const;
    
    
    /** returns Analysis::TauJet* */
    const Analysis::TauJet* tauJet() const;
    
    
    /** returns PanTau::TauFeature*/
    const PanTau::TauFeature* features() const;
    
    /** returns the list of correctedEfoMomenta for this tau */
    const DataVector<CorrectedEflowMomentum>& correctedEflowMomenta() const;
    const DataVector<CorrectedEflowMomentum>& chargedEflowMomenta() const;
    const DataVector<CorrectedEflowMomentum>& emNeutralEflowMomenta() const;
    
    /** sets the reco mode of this tau to the provided value from TauClassificationTypes::PanTauRecoMode */
    void setRecoModeLoose(TauClassificationTypes::PanTauRecoMode recoMode);
    void setRecoModeMedium(TauClassificationTypes::PanTauRecoMode recoMode);
    void setRecoModeTight(TauClassificationTypes::PanTauRecoMode recoMode);
    
    /** returns the unique reco mode identifier for this tau as integer 
     * (as in TauClassificationTypes::PanTauRecoMode)
     */
    TauClassificationTypes::PanTauRecoMode getRecoModeLoose() const;
    TauClassificationTypes::PanTauRecoMode getRecoModeMedium() const;
    TauClassificationTypes::PanTauRecoMode getRecoModeTight() const;
    
     /** returns the unique reco mode name for this tau */
     std::string  getRecoModeLooseName() const;
     std::string  getRecoModeMediumName() const;
     std::string  getRecoModeTightName() const;
     
     
     std::string dumpType() const;

protected:

    /** The jet 
     * See above:
     * This is the jet built during seed building (constituents are EFOs). TauSeed owns that object
     */
    const Jet*  m_jet;
    
    
    /** 
     * climbach @ PanTauIntegration:
     * Need to add a pointer to the tauRec seed that was used to build this pantau seed
     * NOTE: This is not 0 when using TauJets as seed
     * NOTE: This is 0 when using eflowJets as seed
     */
    //     const PanTau::PanTauDetails* m_Details; //not needed since PanTau::TauSeed is only temporarilicity
    const Analysis::TauJet* m_TauJet;
    
    /**
     * The unique RecoMode identifier set for this seed by the JetBuilderAlg
     */
    TauClassificationTypes::PanTauRecoMode  m_RecoModeLoose;
    TauClassificationTypes::PanTauRecoMode  m_RecoModeMedium;
    TauClassificationTypes::PanTauRecoMode  m_RecoModeTight;
    
    
    /** the features of the seed */
    const TauFeature* m_features;
    
    mutable const DataVector<CorrectedEflowMomentum>* m_correctedEflowMomenta;
    mutable const ConstDataVector<DataVector<CorrectedEflowMomentum> >* m_chargedEflowMomenta;
    mutable const ConstDataVector<DataVector<CorrectedEflowMomentum> >* m_emNeutralEflowMomenta;
    
};


inline const Jet*       PanTau::TauSeed::jet() const
{ 
    return m_jet;
}

inline const PanTau::TauFeature*        PanTau::TauSeed::features() const
{ 
    return m_features;
}

inline const Analysis::TauJet*      PanTau::TauSeed::tauJet() const
{
    return m_TauJet;
}

inline void         PanTau::TauSeed::setRecoModeLoose(TauClassificationTypes::PanTauRecoMode recoMode) {
    m_RecoModeLoose = recoMode;
}

inline void         PanTau::TauSeed::setRecoModeMedium(TauClassificationTypes::PanTauRecoMode recoMode) {
    m_RecoModeMedium = recoMode;
}

inline void         PanTau::TauSeed::setRecoModeTight(TauClassificationTypes::PanTauRecoMode recoMode) {
    m_RecoModeTight = recoMode;
}

inline TauClassificationTypes::PanTauRecoMode   PanTau::TauSeed::getRecoModeLoose() const {
    return m_RecoModeLoose;
}

inline TauClassificationTypes::PanTauRecoMode   PanTau::TauSeed::getRecoModeMedium() const {
    return m_RecoModeMedium;
}

inline TauClassificationTypes::PanTauRecoMode   PanTau::TauSeed::getRecoModeTight() const {
    return m_RecoModeTight;
}
    
inline std::string      PanTau::TauSeed::getRecoModeLooseName() const {
    return TauClassificationTypes::getRecoModeName(m_RecoModeLoose);
}

inline std::string      PanTau::TauSeed::getRecoModeMediumName() const {
    return TauClassificationTypes::getRecoModeName(m_RecoModeMedium);
}

inline std::string      PanTau::TauSeed::getRecoModeTightName() const {
    return TauClassificationTypes::getRecoModeName(m_RecoModeTight);
}


}
#endif // PANTAUEVENTTAUSEED_H
