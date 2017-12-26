/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// CorrectedEflowMomentum.h, (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////

#ifndef PANTAUEVENT_CORRECTEDEFLOWMOMENTUM_H
#define PANTAUEVENT_CORRECTEDEFLOWMOMENTUM_H

#include <bitset>
//#include "CLHEP/Vector/LorentzVector.h"

class eflowObject;
namespace CLHEP {
    class HepLorentzVector;
}

namespace PanTau {


/** @class CorrectedEflowMomentum
    Helper class to store massive and fitted 4-momenta of eflowObjects
    @author Sebastian.Fleischmann@cern.ch
    */

class CorrectedEflowMomentum {

public:
    /**
    * This enum describes the type of the eflowObject.
    */
    enum CorrectedEflowMomentumType {
        /**
        * charged
        */
        Charged=0,
        /**
        * charged with qualified track
        */
        QualifiedTrack=1,
        /**
        * EM neutral
        */
        EmNeutral=2, 
        /**
        * Pi0 Tagged
        */
        Pi0=3, 
        /**
        * HAD neutral
        */
        HadNeutral=4,
        /**
         * Neutral (not further classified)
         */
        Neutral=5,
        
        /**
         * New Types for optimization tests
         * 
         * Neutrals above 1 GeV without further requirements
         */
        Neutral1GeV=6,
        /**
         * Neutrals above 2 GeV without further requirements
         */
        Neutral2GeV=7,
        /**
         * Neutrals above 1 GeV from core region (deltaR < 0.2)
         */
        NeutralCore1GeV=8,
        /**
         * Neutrals above 2 GeV from core region (deltaR < 0.2)
         */
        NeutralCore2GeV=9,
        /**
         * Qualified Charged over 2 GeV
         */
        QualifiedTrack2GeV=10,
        /**
         * Qualified Neutral (this will be the final neutral ones to be used)
         */
        QualifiedNeutral=11,

        
        NumberOfCorrectedEflowMomentumTypes=12
    };

    /** Default Constructor for POOL. Do not use! */
    CorrectedEflowMomentum();
    /** Copy Constructor */
    CorrectedEflowMomentum(const CorrectedEflowMomentum& rhs);
    /** Assignment operator */
    CorrectedEflowMomentum& operator=(const CorrectedEflowMomentum& rhs);

    /**
    * Full constructor. 
    *
    * The eflowObject passed in does NOT belong to the this object!
    *
    * (in short delete it yourself) 
    *
    * @param[in] eflow pointer to eflowObject
    * @param[in] typePattern The pattern of 'types' which correspond to this
    *            seed. You create the bitset as follows: 
    *     std::bitset<CorrectedEflowMomentum::NumberOfCorrectedEflowMomentumTypes> typePattern(0);
    *     typePattern.set(CorrectedEflowMomentum::Charged);
    * @param[in] correctedMomentum corrected 4-momentum (HepLorentzVector) of the eflowObject (CorrectedEflowMomentum takes ownership)
    * @param[in] fittedMomentum 4-momentum (HepLorentzVector) of the eflowObject from kinematic fit (CorrectedEflowMomentum takes ownership)
    */
    CorrectedEflowMomentum(
        const eflowObject                                   *eflow,
        const std::bitset<CorrectedEflowMomentum::NumberOfCorrectedEflowMomentumTypes>    typePattern,
        const CLHEP::HepLorentzVector                       *correctedMomentum = 0,
        const CLHEP::HepLorentzVector                       *fittedMomentum = 0
        );

    /** Destructor */
    virtual ~CorrectedEflowMomentum();

    /**
    * Use this method to find out if the CorrectedEflowMomentum is of a certain type:
    * i.e. if ( seed->type(CorrectedEflowMomentum::Charged) { //etc }
    *
    * @return true if the CorrectedEflowMomentum is of this type
    */
    bool type( const CorrectedEflowMomentumType& type ) const;

    /** returns the eflowObject*/
    const eflowObject* originalEflowObject() const;

    /** returns typeFlags*/
    long  typeFlags() const;
    
    /** return string based on type flag for feature storage */
    static std::string typeName(CorrectedEflowMomentumType theType);

    /** returns the corrected 4-momentum (HepLorentzVector) of the eflowObject*/
    const CLHEP::HepLorentzVector* correctedMomentum() const;
    /** returns the 4-momentum (HepLorentzVector) of the eflowObject from kinematic fit*/
    const CLHEP::HepLorentzVector* fittedMomentum() const;

    /** add the 4-momentum (HepLorentzVector) of the eflowObject from kinematic fit*/
    void addFittedMomentum(const CLHEP::HepLorentzVector* hlv);

protected:

    /** The jet */
    const eflowObject*  m_eflowObject;
    /** flags for the types */
    long                m_typeFlags;
    const CLHEP::HepLorentzVector*  m_correctedMomentum;
    const CLHEP::HepLorentzVector*  m_fittedMomentum;

};

inline bool PanTau::CorrectedEflowMomentum::type( const CorrectedEflowMomentumType& type ) const
{
    if ( ( 1<<static_cast<int>(type) ) &m_typeFlags ) return true;
    return false;
}

inline const eflowObject* PanTau::CorrectedEflowMomentum::originalEflowObject() const
{ 
    return m_eflowObject;
}

inline long PanTau::CorrectedEflowMomentum::typeFlags() const
{
    return m_typeFlags;
}

inline const CLHEP::HepLorentzVector* PanTau::CorrectedEflowMomentum::correctedMomentum() const
{ 
    return m_correctedMomentum;
}

inline const CLHEP::HepLorentzVector* PanTau::CorrectedEflowMomentum::fittedMomentum() const
{ 
    return m_fittedMomentum;
}

inline void PanTau::CorrectedEflowMomentum::addFittedMomentum(const CLHEP::HepLorentzVector* hlv)
{
    m_fittedMomentum = hlv;
}

}

#endif // PANTAUEVENT_CORRECTEDEFLOWMOMENTUM_H
