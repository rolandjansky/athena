/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#ifndef LArRawChannel_H
#define LArRawChannel_H

#include "Identifier/HWIdentifier.h"
#include "CaloIdentifier/CaloGain.h"
#include <stdint.h>

// Gaudi Class ID

// static const CLID CLID_LArRawChannel = 2722 ;

/**
@class LArRawChannel
@brief Liquid Argon ROD output object base class

This class holds the output of a liquid argon channel when it is 
processed through the RODS  
* 
* Objects of this type are readonly <br>
* @author Kin Yip
*
* ----------------------------
* move to DataVector 
*  Hong Ma 
* @version 00-00-02  
*
* -----------------------------
* Add raw channel gain information   25/09/2003
*  G.Unal
*
* ----------------------------
* Move to HWIdentifier 10/2/2004 
*  W. Lampl
*
*/
class LArRawChannel {

public:

    /** @brief default constructor for persistenty */
    LArRawChannel();

    /** @brief useful constructor 
        @param[in] chan_id   Online identifier of channel
        @param[in] energy    Energy in MeV
        @param[in] time      time in ps
        @param[in] quality   Quality from pulse reconstruction
        @param[in] provenance Provenance of pulse reconstruction
        @param[in] gain      Gain 
    */
    LArRawChannel( HWIdentifier chan_id, 
                   int energy, 
                   int time, 
                   uint16_t quality, 
                   uint16_t provenance,
                   CaloGain::CaloGain gain);
    

    /** @brief destructor  */
    virtual ~LArRawChannel()                    { };

    /** @return online identifier */
    HWIdentifier        identify() const;

    /** @return online identifier */
    HWIdentifier        channelID() const; 
    const HWIdentifier& hardwareID() const;
  
    /** @return energy in MeV (rounded to integer) */
    int                 energy() const;

    /** @return time in ps (rounded to integer) */
    int                 time() const;

    /** @return quality from pulse reconstruction */
    uint16_t            quality() const;

    /** @return Provenance of pulse reconstruction */
    uint16_t            provenance() const;

    /** @return gain */
    CaloGain::CaloGain  gain() const;

private:

    /** online identifier */
    HWIdentifier        m_channelID;
    /** energy in MeV */
    int                 m_energy;
    /** time in ps */
    int                 m_time;
    /** quality/provenance from pulse reconstruction */

// Reflex doesn't handle unions completely correctly.
// If you have a union as a data member, it will print a warning
// at runtime.  So, hide the union from reflex.  (Only m_quality is used
// in inlined code.)
#ifdef __REFLEX__
    int  m_quality ; 
#   define m_qualProv ((uint16_t*)&m_quality)
#else
    union {
     int  m_quality ; 
     uint16_t m_qualProv[2];
    };
#endif

    /** gain information */
    CaloGain::CaloGain  m_gain;

};

  
///  inlines

inline
LArRawChannel::LArRawChannel() 
        : 
        m_channelID(0), 
        m_energy(0), 
        m_time(0), 
        m_quality(0), 
        m_gain(CaloGain::CaloGain(4)) 
{ }

inline
LArRawChannel::LArRawChannel( HWIdentifier chan_id, 
                              int energy, 
                              int time, 
                              uint16_t quality, 
                              uint16_t provenance,
                              CaloGain::CaloGain gain)
    :
        m_channelID(chan_id), 
        m_energy(energy), 
        m_time(time), 
        m_gain(gain)
{ m_qualProv[0] = quality;
  // cppcheck-suppress objectIndex
  m_qualProv[1] = provenance;
} 

inline
HWIdentifier 
LArRawChannel::identify() const { return m_channelID ; }

inline
HWIdentifier 
LArRawChannel::channelID() const { return m_channelID; }  

inline
const HWIdentifier & 
LArRawChannel::hardwareID() const {return m_channelID; }

inline
int 
LArRawChannel::energy() const { return m_energy; } 

inline
int 
LArRawChannel::time() const { return m_time; }

inline
uint16_t 
LArRawChannel::quality() const { return m_qualProv[0]; }

inline
uint16_t
LArRawChannel::provenance() const
{
  // cppcheck-suppress objectIndex
  return m_qualProv[1];
}

inline
CaloGain::CaloGain 
LArRawChannel::gain() const { return m_gain;}

#endif


