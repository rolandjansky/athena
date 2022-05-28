/*
  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
*/

#ifndef PANTAUALGS_PANTAUSEED_H
#define PANTAUALGS_PANTAUSEED_H


#include <vector>
#include <string>

#include "xAODBase/IParticle.h"

#include "PanTauAlgs/TauConstituent.h"
#include "PanTauAlgs/TauFeature.h"

 
#include "xAODTau/TauJet.h"

namespace PanTau {



  class PanTauSeed : public xAOD::IParticle {
    // IParticle implementation as in 
    // http://acode-browser.usatlas.bnl.gov/lxr/source/atlas/Event/xAOD/xAODEgamma/xAODEgamma/versions/Egamma_v1.h
    // http://acode-browser.usatlas.bnl.gov/lxr/source/atlas/Event/xAOD/xAODEgamma/Root/Egamma_v1.cxx

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
    
    // enumeration to describe extended PanTau decay mode (number of tracks, pi0 tagged neutrals and other neutrals)
    enum DecayModeExtended {
      t_ExtMode100 = 1,
      t_ExtMode10X = 2, 
      t_ExtMode110 = 3,
      t_ExtMode11X = 4,
      t_ExtMode1XX = 5,
      t_ExtMode300 = 6,
      t_ExtMode30X = 7,
      t_ExtMode3XX = 8,
      t_ExtModeOther = 9,
      t_ExtModeNotSet = 10,
      t_ExtModeError = 11,
      t_ExtMode11shots = 12 // for taus which have been classified as 1p1n but have >2 shots
    };

    static int          getDecayMode(int nCharged, int nNeutral);
    static std::string  getDecayModeName(int decayMode);
        
    PanTauSeed();
    ~PanTauSeed();
    PanTauSeed(const PanTauSeed& seed);
    PanTauSeed& operator=(const PanTauSeed& seed);


    /// @name xAOD::IParticle functions
    /// These are already virtual due to IParticle
    /// @{
     
    /// @brief The transverse momentum (\f$p_T\f$) of the particle
    virtual double           pt() const;
 
    /// @brief The pseudorapidity (\f$\eta\f$) of the particle
    virtual double           eta() const;
 
    /// @brief The azimuthal angle (\f$\phi\f$) of the particle
    virtual double           phi() const;
 
    /// @brief The invariant mass of the particle
    virtual double           m() const;
 
    /// The total energy of the particle
    virtual double           e() const;
 
    /// @brief The true rapidity (y) of the particle
    virtual double           rapidity() const;
     
    /// @brief Definition of the 4-momentum type
    typedef IParticle::FourMom_t FourMom_t;
 
    /// @brief The full 4-momentum of the particle as a TLoretzVector
    virtual FourMom_t p4() const;

    /// @brief The type of the object as a simple enumeration, remains pure virtual in e/gamma.
    virtual xAOD::Type::ObjectType type() const; 


    /// @brief set the 4-vec
    void setP4(float pt, float eta, float phi, float m);
 
    /// @brief set the Pt
    void setPt(float pt);
 
    ///  @brief set the eta
    void setEta(float eta);
 
    /// @brief set the phi
    void setPhi(float phi);
 
    /// @brief set the Mass
    void setM(float m);



    /** Main constructor to be used */
    PanTauSeed( std::string                          nameInputAlgorithm,
		xAOD::TauJet*                        tauJet,
		std::vector<PanTau::TauConstituent*> tauConstituents,
		std::vector<PanTau::TauConstituent*> tauConstituentsWithUnselected,
		std::vector<int>                     pantauSeed_TechnicalQuality
		);


    /** Constructor for invalid seeds */
    PanTauSeed( std::string                          nameInputAlgorithm,
		 xAOD::TauJet*                        tauJet,
		 std::vector<int>                     pantauSeed_TechnicalQuality
		 );

    std::string                                         getNameInputAlgorithm() const;
    const xAOD::TauJet*                                 getTauJet() const;
    xAOD::TauJet*                                       getTauJet();
    const PanTau::TauFeature*                           getFeatures() const;
    PanTau::TauFeature*                                 getFeatures();
    TLorentzVector                                      getProtoMomentumCore() const;
    TLorentzVector                                      getProtoMomentumWide() const;

    bool                                                getIsValidSeed() const;
    std::vector<int>                                    getTechnicalQuality() const;
    bool                                                isOfTechnicalQuality(int pantauSeed_TechnicalQuality) const;
    std::vector< std::vector<PanTau::TauConstituent*> > getConstituents() const;
    std::vector<PanTau::TauConstituent*>                getConstituentsAsList_Core() const;
    std::vector<PanTau::TauConstituent*>                getConstituentsAsList_Wide() const;
    std::vector<PanTau::TauConstituent*>                getConstituentsAsList_All() const;

    std::vector<PanTau::TauConstituent*>                getConstituentsOfType(int tauConstituent_Type, bool& foundit);
    int                                                 getNumberOfConstituentsOfType(int tauConstituent_Type);
    TLorentzVector                                      getSubsystemHLV(int tauConstituent_Type, bool& foundit);

    TLorentzVector                                      getFinalMomentum() const;
    int                                                 getDecayModeBySubAlg() const;
    int                                                 getDecayModeByPanTau() const;

    void                                                setFinalMomentum(TLorentzVector finalMom);
    void                                                setDecayModeByPanTau(int decayModePanTau);

    void                                                setModeHackedByCellBased(bool f) {m_decayModeHack_CellBasedShots = f;}
    bool                                                getModeHackedByCellBased() {return m_decayModeHack_CellBasedShots;}



  private:

    /// 4-momentum object
    FourMom_t m_p4;

  protected:

    //flag that tells whether the seed is valid                                                                                                                                                                                                
    bool                                                m_IsValidSeed;

    std::vector<int>                                    m_TechnicalQuality;

    //place to store which input alg created this pantauseed: eflowRec, CellBased, ClusterBased..                                                                                                                                              
    std::string                                         m_NameInputAlgorithm;

    //pointer to the TauJet this PanTauSeed was build from (pointer not owned by PanTauSeed)                                                                                                                                                   
    xAOD::TauJet*                                       m_TauJet;

    //for each type of tauConstituent, a list of constituents (of that type)                                                                                                                                                                   
    // the TauConstituent objects are owned by PanTauSeed (this class), so they need to be deleted in the destructor                                                                                                                           
    std::vector< std::vector<PanTau::TauConstituent*> > m_Constituents;

    //the momentum as calculated by using all constituents added to this seed                                                                                                                                                                  
    TLorentzVector                                      m_ProtoMomentum_Wide;

    //the momentum as calculated by using only core constituents                                                                                                                                                                               
    TLorentzVector                                      m_ProtoMomentum_Core;

    //the momentum as calculated by using info about decay mode                                                                                                                                                                                
    TLorentzVector                                      m_FinalMomentum;

    //a list of hlvs for the different subsystems (charged, neutral, pi0Neut...)                                                                                                                                                               
    std::vector< TLorentzVector >                       m_TypeHLVs;

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


  static void                                            SetP4EEtaPhiM(TLorentzVector& hlv, double e, double eta, double phi, double m );


} //end name space pantau


inline std::string                                          PanTau::PanTauSeed::getNameInputAlgorithm() const       {return m_NameInputAlgorithm;}
inline const xAOD::TauJet*                                  PanTau::PanTauSeed::getTauJet() const                   {return m_TauJet;}
inline xAOD::TauJet*                                        PanTau::PanTauSeed::getTauJet()                         {return m_TauJet;}
inline const PanTau::TauFeature*                            PanTau::PanTauSeed::getFeatures() const                 {return m_Features;}
inline PanTau::TauFeature*                                  PanTau::PanTauSeed::getFeatures()                       {return m_Features;}
inline TLorentzVector                                       PanTau::PanTauSeed::getProtoMomentumWide() const        {return m_ProtoMomentum_Wide;}
inline TLorentzVector                                       PanTau::PanTauSeed::getProtoMomentumCore() const        {return m_ProtoMomentum_Core;}
inline std::vector< std::vector<PanTau::TauConstituent*> >  PanTau::PanTauSeed::getConstituents() const             {return m_Constituents;}
inline std::vector<PanTau::TauConstituent*>                 PanTau::PanTauSeed::getConstituentsAsList_Core() const  {return m_ConstituentsList_Core;}
inline std::vector<PanTau::TauConstituent*>                 PanTau::PanTauSeed::getConstituentsAsList_Wide() const  {return m_ConstituentsList_Wide;}
inline std::vector<PanTau::TauConstituent*>                 PanTau::PanTauSeed::getConstituentsAsList_All() const   {return m_ConstituentsList_All;}
inline TLorentzVector                                       PanTau::PanTauSeed::getFinalMomentum() const            {return m_FinalMomentum;}
inline int                                                  PanTau::PanTauSeed::getDecayModeBySubAlg() const        {return m_DecayMode_BySubAlg;}
inline int                                                  PanTau::PanTauSeed::getDecayModeByPanTau() const        {return m_DecayMode_ByPanTau;}
inline bool                                                 PanTau::PanTauSeed::getIsValidSeed() const              {return m_IsValidSeed;}
inline std::vector<int>                                     PanTau::PanTauSeed::getTechnicalQuality() const         {return m_TechnicalQuality;}

inline void                                                 PanTau::PanTauSeed::setFinalMomentum(TLorentzVector finalMom)  {m_FinalMomentum = finalMom;}
inline void                                                 PanTau::PanTauSeed::setDecayModeByPanTau(int decayModePanTau)           {m_DecayMode_ByPanTau = decayModePanTau;}


inline static void                                          PanTau::SetP4EEtaPhiM(TLorentzVector& hlv, double e, double eta, double phi, double m ){
  // FIXME: what is wrong with eta=0?
  if( eta == 0. ) return;
  double pt = std::sqrt( e*e - m*m) / std::cosh( eta ); // not really safe.
  hlv.SetPtEtaPhiE(pt, eta, phi, e);
  return;
}


#endif // PANTAUALGS_PANTAUSEED_H
