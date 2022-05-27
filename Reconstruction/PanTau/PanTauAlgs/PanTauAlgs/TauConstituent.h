/*
  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
*/

#ifndef PANTAUALGS_TAUCONSTITUENT_H
#define PANTAUALGS_TAUCONSTITUENT_H

#include <string>
#include <vector>

#include "xAODBase/IParticle.h"
#include "xAODPFlow/PFO.h"
#include <TLorentzVector.h>


namespace PanTau {

/** @class TauConstituent
    Input object for PanTau.
    Mainly a 4-vector with additional information about its type (charged, neutral, pi0 tagged)
    
    @author limbach@physik.uni-bonn.de
    */

  class TauConstituent : public xAOD::IParticle {

  public:
    
    /**
     * Type enumeration to specify type of this tau constituent
     * 
     * t_NoType:    This is used in default constructors when no type is specified
     * t_Charged:   Used to represent objects considered as 'charged' by input substructure reco
     * t_Neutral:   Use for objects considered neutral by input substructure algorithm
     * t_Pi0Neut:   If a particle passes pi0 ID, use this flag
     * t_OutChrg:   Charged objects in 0.2 to 0.4
     * t_OutNeut:   Neutral objects in 0.2 to 0.4
     * t_NeutLowA:  (Core) Neutrals with lower Et cut
     * t_NeutLowB:  (Core) Neutrals with even lower Et cut
     * t_nTypes:    Dont use this, just tell number of types
     */
    enum Type {
        t_NoType    = 0,
        t_Charged   = 1,
        t_Neutral   = 2,
        t_Pi0Neut   = 3,
        t_OutChrg   = 4,
        t_OutNeut   = 5,
        t_NeutLowA  = 6,
        t_NeutLowB  = 7,
        t_nTypes    = 8
    };
    
    static double       DefaultBDTValue()       {return -4711.1337;}
    static double       DefaultCharge()         {return -47111337;}
    static std::string  AllConstituentsName()   {return "All";}
    
    static std::string  getTypeName(PanTau::TauConstituent::Type aType);
    static bool         isNeutralType(int tauConstituentType);
    static bool         isCoreType(int tauConstituentType);    

      
    /** Default Constructor for POOL. Do not use! */
    TauConstituent();
    
    /**
     * @param itsMomentum The P4EEtaPhiM of this particle
     * @param itsType The type of this particle using this class' enumeration 
     * @param itsBDTValue If a BDT was used ot ID this particle, provide the BDT response
     */
    TauConstituent(TLorentzVector   itsMomentum,
                   int              itsCharge,
                   std::vector<int> itsType,
                   double           itsBDTValue,
                   xAOD::PFO*       itsPFOLink);
    
    
    /** Destructor */
    virtual ~TauConstituent();
    
    
    /** Copy Constructor */
    TauConstituent(const TauConstituent& tauConst);

    /** Assignment operator */
    TauConstituent& operator=(const TauConstituent& tauConst);

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
 
     /// @brief The full 4-momentum of the particle as a TLorentzVector
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


    //no setter functions needed as all properties are set in constructor
    void                                    removeTypeFlag(TauConstituent::Type aType);
    
    //Getter functions
    std::vector<std::string>                getTypeName() const;
    std::string                             getTypeNameString() const;
    std::vector<int>                        getTypeFlags() const;
    bool                                    isOfType(TauConstituent::Type aType) const;
    double                                  getBDTValue() const;
    int                                     getCharge() const;
    const xAOD::PFO*                        getPFO() const;
    
    void                                    addShot(TauConstituent* shot);
    std::vector<TauConstituent*>            getShots() const;
    unsigned int                            getNShots() const;
    
    void                                    setNPhotonsInShot(int nPhotons);
    int                                     getNPhotonsInShot() const;

  private:

     /// 4-momentum object
     FourMom_t m_p4;


  protected:
    
    std::vector<int>                m_TypeFlags;
    double                          m_BDTValue;
    int                             m_Charge;
    
    xAOD::PFO*                      m_PFOLink;
    
    // the constituents in there are owned by this! delete them!
    std::vector<TauConstituent*>    m_Shots;
    int                             m_nPhotonsInShot;    

  };


} //end namespace
#endif // PANTAUALGS_TAUCONSTITUENT_H
