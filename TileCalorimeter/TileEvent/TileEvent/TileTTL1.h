/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

//***************************************************************************
// Filename : TileTTL1.h
// Author   : Frank Merritt
// Created  : March 2003
//
// DESCRIPTION:
//    TTL1 is the class for raw Tile Level 1 Trigger Towers.  Each tower is
//    contains a hardware sum of Tile channels (about 4 each), read out in
//    N time slices.  Each TTl1 object also contains a special identifier.
//
// HISTORY:
//    14Sep09 TileDigits changed from double to float (M.Soares)
//
// ***************************************************************************

#ifndef TILEEVENT_TILETTL1_H
#define TILEEVENT_TILETTL1_H

#include "Identifier/Identifier.h"
#include <vector>

class TileTTL1 {

public:
   
    /* Constructors */

    TileTTL1() {}

    TileTTL1(const Identifier& id, const std::vector<double>& digits );

    TileTTL1(const Identifier& id, const std::vector<float>& digits );

    TileTTL1(const Identifier& id, std::vector<float>&& digits );

    ~TileTTL1() {}

    /* Access methods */

    /* Return Identifier. */
    const Identifier & TTL1_ID() const;
    inline Identifier identify(void) const { return m_ID; }
 
    /* return number of time samples */
    short nsamples() const;

    /* Return vector of double digits (will be removed soon) */
    const std::vector<double>   samples() const;

    /* Return reference to the vector of float digits. */
    const std::vector<float>&   fsamples() const;


    std::string whoami   (void) const { return "TileTTL1"; }
    void        print    (void) const;
    // Convertion operator to a std::string 
    // Can be used in a cast operation : (std::string) TileTTL1
    operator std::string() const;

private:

  /* Member variables: */
  Identifier m_ID;
  std::vector<float> m_TTL1digits;
};

#endif  //TILEEVENT_TILETTL1_H

