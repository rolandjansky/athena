/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

//***************************************************************************
// Filename : TileSimData.h
// Author   : Alexandre Solodkov
// Created  : Feb, 2003
//
// DESCRIPTION:
//    A TileSimData is the base class for sim data classes,
//    such as TileHit
//    It has only one member element - Identifier
//
// HISTORY:
//    02Feb03
//
// BUGS:
//
// ***************************************************************************

#ifndef TILEEVENT_TILESIMDATA_H
#define TILEEVENT_TILESIMDATA_H

#include "Identifier/HWIdentifier.h"

#include <string>
#include <vector>

class TileSimData
{
public:

    /* Constructor: */
    TileSimData(){}
    TileSimData(const Identifier& id);
    TileSimData(const HWIdentifier& HWid);

    /* Destructor */
    virtual ~TileSimData() {}

    /*  Inline accessor methods: */
    inline Identifier   identify(void)  const   { return m_pmt_id; }
    inline Identifier   pmt_ID  (void)  const   { return m_pmt_id; }

    HWIdentifier        pmt_HWID(void)  const;
    Identifier          tt_ID   (void)  const;
    Identifier          mt_ID   (void)  const;
    int                 frag_ID (void)  const;

    virtual std::string whoami   (void) const   { return "TileSimData"; }
    virtual void        print    (void) const;
    // Convertion operator to a std::string 
    // Can be used in a cast operation : (std::string) TileSimData
    virtual operator std::string() const;
  
private:

    Identifier m_pmt_id; // logical ID of the pmt
};

#endif  //TILEEVENT_TILESIMDATA_H

