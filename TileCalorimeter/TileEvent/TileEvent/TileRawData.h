/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

//***************************************************************************
// Filename : TileRawData.h
// Author   : Alexandre Solodkov
// Created  : Oct, 2002
//
// DESCRIPTION:
//    A TileRawData is the base class for raw data classes,
//    such as TileDigits and TileRawChannels
//    It has only one member element - HWIdentifier
//
// HISTORY:
//    02Oct02     
//    14Sep09 changed to print_to_stream to accomodate float TileDigits (M.Soares)
//
// BUGS:
//
// ***************************************************************************

#ifndef TILEEVENT_TILERAWDATA_H
#define TILEEVENT_TILERAWDATA_H

#include "Identifier/HWIdentifier.h"

#include <string>
#include <vector>

#include <inttypes.h>

class TileRawData
{
public:

    /* Constructor: */
    TileRawData(){}
    TileRawData(const Identifier& id);
    TileRawData(const HWIdentifier& HWid)
      : m_adc_hwid (HWid)
    {
    }

    /* Destructor */
    virtual ~TileRawData() {}

    /*  Inline accessor methods: */
    inline Identifier   identify(void)  const   { return adc_ID();   }
    inline HWIdentifier adc_HWID(void)  const   { return m_adc_hwid; }

    Identifier          adc_ID  (void)  const;
    Identifier          pmt_ID  (void)  const;
    Identifier          cell_ID (void)  const;
    Identifier          cell_ID_index (int & index, int & pmt)  const;
    Identifier          tt_ID   (void)  const;
    Identifier          mt_ID   (void)  const;
    int                 frag_ID (void)  const;

    virtual std::string whoami   (void) const   { return "TileRawData"; }
    virtual void        print    (void) const;
    // Convertion operator to a std::string 
    // Can be used in a cast operation : (std::string) TileRawData
    virtual operator std::string() const;

    static void print_to_stream ( const std::vector<float> val,
                                  const std::string & label, 
                                  std::ostringstream & text);
  
    static void print_to_stream ( const std::vector<double> val,
                                  const std::string & label, 
                                  std::ostringstream & text);

    static void print_to_stream ( const std::vector<uint32_t> val,
                                  const std::string & label, 
                                  std::ostringstream & text);
private:

    HWIdentifier m_adc_hwid; // Hardware (online) ID of the adc
};

#endif  //TILEEVENT_TILERAWDATA_H

