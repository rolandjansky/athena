/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

//********************************************************************
// Filename : TileHit.h
// Author   : Zhifang
// Created  : April, 2002
//
// DESCRIPTION:
//    A TileHit represents the total energy deposited in scintillator
//    which is visible by one PMT
//
// HISTORY:
//    
// BUGS:
//
// *******************************************************************

#ifndef TILESIMEVENT_TILEHIT_H
#define TILESIMEVENT_TILEHIT_H

#include "Identifier/HWIdentifier.h"

#include <string>
#include <vector>
class TileSimHit;

class TileHit
{
public:

    /** @brief Default constructor needed for POOL */
    TileHit() { }

    /** @brief Constructor which copies everything from TileSimHit */
    TileHit( const TileSimHit * hitInp );

    /** @brief Constructor for single hit without time binning */
    TileHit(const Identifier & id, float energy=0.0, float time=0.0); 

    /** @brief Constructor with time rounding to the center of nearest deltaT bin */
    TileHit(const Identifier & id, float energy, float time, float deltaT); 

    /** @brief Default destructor */
    ~TileHit() { }
    
    /** @brief Add sub-hit to a given hit */
    int add  (float energy, float time);
    /** @brief Add sub-hit to a given hit with time rounding to the center of nearest deltaT bin */
    int add  (float energy, float time, float deltaT);
    /** @brief Addi all sub-hits from another hit to a given hit */
    int add  (const TileHit *hitInp, float deltaT);
    
    /** @brief Scale energy of all sub-hits in a hit  */
    void scale(float coeff);

    /* Inline set methods */

    /** @brief Add energy to the first sub-hit in a hit  */
    inline void         addEnergy (float e)               { m_energy[0]  += e;  }
    /** @brief Set energy of ind-th sub-hit in a hit  */
    inline void         setEnergy (float e, int ind=0)    { m_energy[ind] = e;  }
    /** @brief Set time of ind-th sub-hit in a hit  */
    inline void         setTime   (float t, int ind=0)    { m_time[ind]   = t;  }
    /** @brief Resize energy and time vectors in a hit  */
    inline void         resize    (int len) { m_energy.resize(len); m_time.resize(len); }
    /** @brief Reserve length of energy and time vectors in a hit  */
    inline void         reserve   (int len) { m_energy.reserve(len); m_time.reserve(len); }
    /** @brief Resize energy/time vectors in a hit to one and set energy/time to zero  */
    inline void         setZero   () { m_energy.resize(1); m_time.resize(1);
                                       m_energy[0] = 0.0; m_time[0] = 0.0;}
    /* Inline access methods */

    /** @brief Return logical ID of the pmt */
    inline Identifier   identify(void)  const   { return m_pmt_id; }
    /** @brief Return logical ID of the pmt */
    inline Identifier   pmt_ID  (void)  const   { return m_pmt_id; }

    /** @brief Return pmt hardware ID (== channel ID)  */
    HWIdentifier        pmt_HWID(void)  const;
    /** @brief Return trigger tower ID  */
    Identifier          tt_ID   (void)  const;
    /** @brief Return muon trigger ID  */
    Identifier          mt_ID   (void)  const;
    /** @brief Return frag ID (like in BS file)  */
    int                 frag_ID (void)  const;

    /** @brief Return energy of ind-th sub-hit  */
    inline float       energy   (int ind=0)  const  { return m_energy[ind]; }
    /** @brief Return time of ind-th sub-hit  */
    inline float       time     (int ind=0)  const  { return m_time[ind];   }
    /** @brief Return length of energy/time vectors  */
    inline int          size     (void)       const  { return m_energy.size(); }
    
    /** @brief Return name of this object  */
    std::string whoami   (void) const { return "TileHit"; }
    /** @brief Dump contents of the hit to std::out  */
    void        print    (void) const;
    /** @brief Return contents of the hit in a form of string. 
               Can be used in a cast operation : (std::string) TileHit */
    operator std::string() const;

    /** @brief Print contents of one vector to output stream */
    static void print_to_stream ( const std::vector<float>& val,
                                  const std::string & label, 
                                  std::ostringstream & text);

private:

    /** @brief Logical ID of the pmt  */
    Identifier m_pmt_id;
    /** @brief Energy deposited in scint, as seen by pmt in sepatate time bins  */
    std::vector<float> m_energy;         //  
    /** @brief Time of the energy deposition relative to BC  */
    std::vector<float> m_time;
};

#endif //TILESIMEVENT_TILEHIT_H

