/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef PANTAUEVENT_PANTAUSEED_H
#define PANTAUEVENT_PANTAUSEED_H


#include <vector>
#include <string>

#include "FourMom/P4EEtaPhiM.h"

#include "PanTauEvent/TauConstituent.h"
#include "PanTauEvent/TauFeature.h"

#include "GaudiKernel/ToolHandle.h"

// namespace Analysis {
//     class TauJet;
// }
#include "xAODTau/TauJet.h"

namespace PanTau {
    
    
    class PanTauSeed : virtual public P4EEtaPhiM {
        
        
    public:
        
        //enumeration to describe technical status of the seed
        // i.e. numTrack ok? pT ok? etc...
        enum TechnicalQuality {
            t_NoValidInputTau, //use this when there's no substructure info available
            t_NoConstituentsAtAll, //use when no constituent was found
            t_NoSelectedConstituents, //use when no constituent passes the cuts (all below pT, none in cone...)
            t_BadPtValue, //for cases when pT is not in the range of any BDT PT bin
            t_GoodSeed,
            t_nTechnicalQualities
        };
        
        enum DecayMode {
            Mode_1p0n,
            Mode_1p1n,
            Mode_1pXn,
            Mode_3p0n,
            Mode_3pXn,
            Mode_Other,   // for 2p, 4p, 5p taus
            Mode_NotSet,  // for 0p, >= 6p
            Mode_Error    // use this as initialisation
        };
        
        static int          getDecayMode(int nCharged, int nNeutral);
        static std::string  getDecayModeName(int decayMode);
        
        PanTauSeed();
        ~PanTauSeed();
        PanTauSeed(const PanTauSeed& seed);
        PanTauSeed& operator=(const PanTauSeed& seed);
        
        
        /** Main constructor to be used */
        PanTauSeed( std::string                          nameInputAlgorithm,
                    const xAOD::TauJet*                  tauJet,
                    std::vector<PanTau::TauConstituent*> tauConstituents,
                    std::vector<PanTau::TauConstituent*> tauConstituentsWithUnselected,
                    std::vector<int>                     pantauSeed_TechnicalQuality
                  );
        
        
        /** Constructor for invalid seeds */
        PanTauSeed( std::string                          nameInputAlgorithm,
                    const xAOD::TauJet*                  tauJet,
                    std::vector<int>                     pantauSeed_TechnicalQuality
                  );
        
        std::string                                         getNameInputAlgorithm() const;
        const xAOD::TauJet*                                 getTauJet() const;
        PanTau::TauFeature*                                 getFeatures() const;
        CLHEP::HepLorentzVector                             getProtoMomentumCore() const;
        CLHEP::HepLorentzVector                             getProtoMomentumWide() const;
        
        bool                                                getIsValidSeed() const;
        std::vector<int>                                    getTechnicalQuality() const;
        bool                                                isOfTechnicalQuality(int pantauSeed_TechnicalQuality) const;
        std::vector< std::vector<PanTau::TauConstituent*> > getConstituents() const;
        std::vector<PanTau::TauConstituent*>                getConstituentsAsList_Core() const;
        std::vector<PanTau::TauConstituent*>                getConstituentsAsList_Wide() const;
        std::vector<PanTau::TauConstituent*>                getConstituentsAsList_All() const;
        
        std::vector<PanTau::TauConstituent*>                getConstituentsOfType(int tauConstituent_Type, bool& foundit);
        int                                                 getNumberOfConstituentsOfType(int tauConstituent_Type);
        CLHEP::HepLorentzVector                             getSubsystemHLV(int tauConstituent_Type, bool& foundit);
        
        CLHEP::HepLorentzVector                             getFinalMomentum() const;
        int                                                 getDecayModeBySubAlg() const;
        int                                                 getDecayModeByPanTau() const;
        
        void                                                setFinalMomentum(CLHEP::HepLorentzVector finalMom);
        void                                                setDecayModeByPanTau(int decayModePanTau);
        
        void                                                setModeHackedByCellBased(bool f) {m_decayModeHack_CellBasedShots = f;}
        bool                                                getModeHackedByCellBased() {return m_decayModeHack_CellBasedShots;}
        
    protected:
        
        //flag that tells whether the seed is valid
        bool                                                m_IsValidSeed;
        
        std::vector<int>                                    m_TechnicalQuality;
        
        //place to store which input alg created this pantauseed: eflowRec, CellBased, ClusterBased..
        std::string                                         m_NameInputAlgorithm;
        
        //pointer to the TauJet this PanTauSeed was build from (pointer not owned by PanTauSeed)
        const xAOD::TauJet*                                 m_TauJet;
        
        //for each type of tauConstituent, a list of constituents (of that type)
        // the TauConstituent objects are owned by PanTauSeed (this class), so they need to be deleted in the destructor
        std::vector< std::vector<PanTau::TauConstituent*> > m_Constituents;
        
        //the momentum as calculated by using all constituents added to this seed
        CLHEP::HepLorentzVector                             m_ProtoMomentum_Wide;
        
        //the momentum as calculated by using only core constituents
        CLHEP::HepLorentzVector                             m_ProtoMomentum_Core;
        
        //the momentum as calculated by using info about decay mode
        CLHEP::HepLorentzVector                             m_FinalMomentum;
        
        //a list of hlvs for the different subsystems (charged, neutral, pi0Neut...)
        std::vector< CLHEP::HepLorentzVector >              m_TypeHLVs;
        
        //also store constituents as flat list for easier access later on
        std::vector<PanTau::TauConstituent*>                m_ConstituentsList_Core; //only objects in core region
        std::vector<PanTau::TauConstituent*>                m_ConstituentsList_Wide; //objects out of core region up to 0.4
        std::vector<PanTau::TauConstituent*>                m_ConstituentsList_AllSelected;  //all selected objects for this seed
        
        //for memory reasons:
        // pass list of selected AND unselected pfos to seed, which will take ownership
        std::vector<PanTau::TauConstituent*>                m_ConstituentsList_All; //all objects for this seed, selected and unselected 
        
        //the decay mode
        int                                                 m_DecayMode_BySubAlg;
        int                                                 m_DecayMode_ByPanTau;
        
        bool                                                m_decayModeHack_CellBasedShots; //! flag to indicate whether the decay mode by cellbased was 
                                                                                            //! 1p1n and hits in EM1 > 2 (because then 1p1n -> 1pX
                                                                                            //! is set)
        
        //the features of this pantauseed
        PanTau::TauFeature*                                 m_Features;
        
    };
    
    
} //end name space pantau


inline std::string                                          PanTau::PanTauSeed::getNameInputAlgorithm() const       {return m_NameInputAlgorithm;}
inline const xAOD::TauJet*                                  PanTau::PanTauSeed::getTauJet() const                   {return m_TauJet;}
inline PanTau::TauFeature*                                  PanTau::PanTauSeed::getFeatures() const                 {return m_Features;}
inline CLHEP::HepLorentzVector                              PanTau::PanTauSeed::getProtoMomentumWide() const        {return m_ProtoMomentum_Wide;}
inline CLHEP::HepLorentzVector                              PanTau::PanTauSeed::getProtoMomentumCore() const        {return m_ProtoMomentum_Core;}
inline std::vector< std::vector<PanTau::TauConstituent*> >  PanTau::PanTauSeed::getConstituents() const             {return m_Constituents;}
inline std::vector<PanTau::TauConstituent*>                 PanTau::PanTauSeed::getConstituentsAsList_Core() const  {return m_ConstituentsList_Core;}
inline std::vector<PanTau::TauConstituent*>                 PanTau::PanTauSeed::getConstituentsAsList_Wide() const  {return m_ConstituentsList_Wide;}
inline std::vector<PanTau::TauConstituent*>                 PanTau::PanTauSeed::getConstituentsAsList_All() const   {return m_ConstituentsList_All;}
inline CLHEP::HepLorentzVector                              PanTau::PanTauSeed::getFinalMomentum() const            {return m_FinalMomentum;}
inline int                                                  PanTau::PanTauSeed::getDecayModeBySubAlg() const        {return m_DecayMode_BySubAlg;}
inline int                                                  PanTau::PanTauSeed::getDecayModeByPanTau() const        {return m_DecayMode_ByPanTau;}
inline bool                                                 PanTau::PanTauSeed::getIsValidSeed() const              {return m_IsValidSeed;}
inline std::vector<int>                                     PanTau::PanTauSeed::getTechnicalQuality() const         {return m_TechnicalQuality;}

inline void                                                 PanTau::PanTauSeed::setFinalMomentum(CLHEP::HepLorentzVector finalMom)  {m_FinalMomentum = finalMom;}
inline void                                                 PanTau::PanTauSeed::setDecayModeByPanTau(int decayModePanTau)           {m_DecayMode_ByPanTau = decayModePanTau;}



#endif // PANTAUEVENT_PANTAUSEED_H


