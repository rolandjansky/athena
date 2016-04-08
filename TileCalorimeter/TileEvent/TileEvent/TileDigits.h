/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

//***************************************************************************
// Filename : TileDigits.h
// Author   : Frank Merritt
// Created  : May 16, 2002
//
// DESCRIPTION:
//    A TileDigits is the data class corresponding to the amplitude information
//    transmitted from the digitizers to the ROD's.  It contains a hardware
//    identifier (including pmt and adc info) and nsample amplitudes corresponding
//    to the time slices read out.  Optimal filtering must be applied to these
//    amplitudes (including pedestal subtractioin) to produce a TileRawChannel.
//
// HISTORY:
//    14May02  First created for DC1 phase-1 and HLT studies.
//    02Oct02  Now sub-class of TileRawData (A.Solodkov)
//    14Sep09  TileDigits changed from double to float (M.Soares)
//
// ***************************************************************************

#ifndef TILEEVENT_TILEDIGITS_H
#define TILEEVENT_TILEDIGITS_H

#include "TileEvent/TileRawData.h"

class TileDigits : public TileRawData
{
public:
   
    /* Constructors */

    TileDigits() { }

    TileDigits(const Identifier& id, const std::vector<double>& digits );

    TileDigits(const HWIdentifier& HWid, const std::vector<double>& digits );
  
    TileDigits(const HWIdentifier& HWid, const std::vector<short>& digits );

    TileDigits(const Identifier& id, const std::vector<float>& digits );

    TileDigits(const HWIdentifier& HWid, const std::vector<float>& digits );

    TileDigits(const HWIdentifier& HWid, std::vector<float>&& digits );


    /** @return number of samples */
    int nsamples() const { return m_digits.size(); }

    /** @return a reference to a stl vector containing the sample values */
    const std::vector < float > & samples() const { return m_digits; } 
  
    /* Destructor */

    ~TileDigits() { }

    /* Inline access methods */

    inline int NtimeSamples(void) const { return m_digits.size(); }
  
    // inline std::vector<double>  TileDigits::get_digits(void)  const 
    std::vector<double>  get_digits(void) const  
    {
	 std::vector<double> ddigits(m_digits.size());
	 for (unsigned int i=0; i<m_digits.size(); ++i) ddigits[i] = m_digits[i];
	 return ddigits;
    } 

    // inline std::vector<int>  get_Idigits(void) const { return m_digits; }
    // inline const std::vector<double>  * get_digits_ptr(void)  const { return &m_digits; }

    std::string whoami   (void) const { return "TileDigits"; }
    void        print    (void) const;
    // Convertion operator to a std::string 
    // Can be used in a cast operation : (std::string) TileRawChannel
    operator std::string() const;

private:

  /* Member variables: */
  std::vector<float> m_digits;
};

#endif  //TILEEVENT_TILEDIGITS_H

