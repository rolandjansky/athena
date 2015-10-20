/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

//***************************************************************************
// Filename : ZdcDigits.h
// Author   : Peter Steinberg
// Created  : March 2009
//
// DESCRIPTION:
//    A ZdcDigits is the data class corresponding to the amplitude information
//    transmitted from the digitizers to the ROD's.  It contains a hardware
//    identifier (including pmt and adc info) and nsample amplitudes corresponding
//    to the time slices read out.  Optimal filtering must be applied to these
//    amplitudes (including pedestal subtractioin) to produce a ZdcRawChannel.
//
// HISTORY:
//
// ***************************************************************************

#ifndef ZDCEVENT_ZDCDIGITS_H
#define ZDCEVENT_ZDCDIGITS_H

#include "ZdcEvent/ZdcRawData.h"
#include <stdint.h>

class ZdcDigits : public ZdcRawData
{
public:
   
    /* Constructors */

    ZdcDigits() {
      m_nSamples = 0;
    }

    ZdcDigits(const Identifier& id);

    /* Destructor */

    virtual ~ZdcDigits() { }

    /* Inline access methods */

    inline int NtimeSamples(void) const { return m_digits_gain0_delay0.size(); }

    unsigned int m_nSamples;

    void set_digits_gain0_delay0(const std::vector<int>& v);
    void set_digits_gain0_delay1(const std::vector<int>& v);
    void set_digits_gain1_delay0(const std::vector<int>& v);
    void set_digits_gain1_delay1(const std::vector<int>& v);
    void set_digits_gain0_delay0(const std::vector<uint16_t>& v);
    void set_digits_gain0_delay1(const std::vector<uint16_t>& v);
    void set_digits_gain1_delay0(const std::vector<uint16_t>& v);
    void set_digits_gain1_delay1(const std::vector<uint16_t>& v);

    inline const std::vector<int>&  get_digits_gain0_delay0(void)  const { return m_digits_gain0_delay0; }
    inline const std::vector<int>&  get_digits_gain0_delay1(void)  const { return m_digits_gain0_delay1; }
    inline const std::vector<int>&  get_digits_gain1_delay0(void)  const { return m_digits_gain1_delay0; }
    inline const std::vector<int>&  get_digits_gain1_delay1(void)  const { return m_digits_gain1_delay1; }

    std::string whoami   (void) const { return "ZdcDigits"; }
    void        print    (void) const;
    // Convertion operator to a std::string 
    // Can be used in a cast operation : (std::string) ZdcRawChannel
    operator std::string() const;

private:

    /* Member variables: */

    std::vector<int> m_digits_gain0_delay0;
    std::vector<int> m_digits_gain0_delay1;
    std::vector<int> m_digits_gain1_delay0;
    std::vector<int> m_digits_gain1_delay1;
};

#endif  //ZDCEVENT_ZDCDIGITS_H

