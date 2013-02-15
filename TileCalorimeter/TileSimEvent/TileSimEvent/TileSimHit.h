/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

//********************************************************************
// Filename : TileHit.h
// Author   : Zhifang
// Created  : April, 2002
//
// DESCRIPTION:
//    A TileHit represents the total energy deposited in scintillator
//    which is visible by one PMT
//    Energy and time is kept as double 
//
// HISTORY:
//    04Jun09: Copied from TileHit class
//    
// BUGS:
//
// *******************************************************************

#ifndef TILESIMEVENT_TILESIMHIT_H
#define TILESIMEVENT_TILESIMHIT_H

#include "Identifier/Identifier.h"

#include <string>
#include <vector>

class TileSimHit
{
public:

    /** @brief Default constructor needed for POOL */
    TileSimHit() { }

    /** @brief Constructor with time rounding to the center of nearest deltaT bin */
    TileSimHit(Identifier & id, double energy, double time, double deltaT); 

    /** @brief Default destructor */
    ~TileSimHit() { }
    
    /** @brief Add sub-hit to a given hit with time rounding to the center of nearest deltaT bin */
    int add  (double energy, double time, double deltaT);

    /* Inline access methods */

    /** @brief Return logical ID of the pmt */
    inline Identifier   identify(void)  const   { return m_pmt_id; }
    /** @brief Return energy of ind-th sub-hit  */
    inline double       energy   (int ind=0)  const  { return m_energy[ind]; }
    /** @brief Return time of ind-th sub-hit  */
    inline double       time     (int ind=0)  const  { return m_time[ind];   }
    /** @brief Return length of energy/time vectors  */
    inline int          size     (void)       const  { return m_energy.size(); }
    
private:

    /** @brief Logical ID of the pmt  */
    Identifier m_pmt_id;
    /** @brief Energy deposited in scint, as seen by pmt in sepatate time bins  */
    std::vector<double> m_energy;         //  
    /** @brief Time of the energy deposition relative to BC  */
    std::vector<double> m_time;
};

#endif //TILESIMEVENT_TILESIMHIT_H

