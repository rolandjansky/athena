/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// TauSeed.h, (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////

#ifndef PANTAUEVENTTAUCONSTITUENT_H
#define PANTAUEVENTTAUCONSTITUENT_H

#include <string>
#include <vector>

#include "FourMom/P4EEtaPhiM.h"
#include "xAODPFlow/PFO.h"

class MsgStream;


namespace PanTau {

/** @class TauConstituent
    Input object for PanTau.
    Mainly a 4-vector with additional information about its type (charged, neutral, pi0 tagged)
    
    @author limbach@physik.uni-bonn.de
    */


class TauConstituent : virtual public P4EEtaPhiM {

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
    
protected:
    
    std::vector<int>                m_TypeFlags;
    double                          m_BDTValue;
    int                             m_Charge;
    
    xAOD::PFO*                      m_PFOLink;
    
    // the constituents in there are owned by this! delete them!
    std::vector<TauConstituent*>    m_Shots;
    int                             m_nPhotonsInShot;
    
    
public:
    
    
    /** Default Constructor for POOL. Do not use! */
    TauConstituent();
    
    /** Copy Constructor */
    TauConstituent(const TauConstituent& tauConst);
    
    /** Assignment operator */
    TauConstituent& operator=(const TauConstituent& tauConst);
    
    /**
     * @param itsMomentum The P4EEtaPhiM of this particle
     * @param itsType The type of this particle using this class' enumeration 
     * @param itsBDTValue If a BDT was used ot ID this particle, provide the BDT response
     */
    TauConstituent(P4EEtaPhiM                               itsMomentum,
                   int                                      itsCharge,
                   std::vector<int>                         itsType,
                   double                                   itsBDTValue,
                   xAOD::PFO*                               itsPFOLink);
    
    
    /** Destructor */
    virtual ~TauConstituent();
    
    
    
    //no setter functions needed as all properties are set in constructor
    void                                    removeTypeFlag(TauConstituent::Type aType);
    
    //Getter functions
    std::vector<std::string>                getTypeName() const;
    std::string                             getTypeNameString() const;
    std::vector<int>                        getTypeFlags() const;
    bool                                    isOfType(TauConstituent::Type aType) const;
    double                                  getBDTValue() const;
    int                                     getCharge() const;
    xAOD::PFO*                              getPFO() const;
    
    void                                    addShot(TauConstituent* shot);
    std::vector<TauConstituent*>            getShots();
    unsigned int                            getNShots();
    
    void                                    setNPhotonsInShot(int nPhotons);
    int                                     getNPhotonsInShot();
};




} //end namespace
#endif // PANTAUEVENTTAUCONSTITUENT_H
