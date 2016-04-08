/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

//***************************************************************************
// Filename : TileBeamElem.h
// Author   : AS
// Created  : Oct 29, 2003
//
// DESCRIPTION:
//    A TileBeamElem is the data class corresponding to the amplitude information
//    obtained in various beam elements (scintillators, wire chambers etc).
//    It contains a hardware identifier ("ADC" identifier)
//    and one or many uint32 amplitudes stored in a vector
//
// HISTORY:
//    29Oct03  Created
//
// ***************************************************************************

#ifndef TILEEVENT_TILEBEAMELEM_H
#define TILEEVENT_TILEBEAMELEM_H

#include "TileEvent/TileRawData.h"

class TileBeamElem : public TileRawData
{
public:
   
    /* Constructors */

    TileBeamElem() { }

    TileBeamElem(const Identifier& id, const std::vector<uint32_t>& digits );

    TileBeamElem(const HWIdentifier& HWid, const std::vector<uint32_t>& digits );
    TileBeamElem(const HWIdentifier& HWid, std::vector<uint32_t>&& digits );
  
    TileBeamElem(const HWIdentifier& HWid, uint32_t digit );
  
    /* Destructor */

    ~TileBeamElem() {}

    /* Inline access methods */

    inline int size(void) const { return m_digits.size(); }
  
    inline const std::vector<uint32_t>&  get_digits(void)  const { return m_digits; }

    std::string whoami   (void) const { return "TileBeamElem"; }
    void        print    (void) const;
    // Convertion operator to a std::string 
    // Can be used in a cast operation : (std::string) TileBeamElem
    operator std::string() const;

private:

  /* Member variables: */
  std::vector<uint32_t> m_digits;
};

#endif  //TILEEVENT_TILEBEAMELEM_H

