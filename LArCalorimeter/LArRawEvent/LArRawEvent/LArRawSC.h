/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#ifndef LARRAWSC_H
#define LARRAWSC_H
#include <vector>

#include "Identifier/HWIdentifier.h"

/**
   @class LArRawSC
   @brief Liquid Argon SuperCell raw data

   represents the output of the LATOME
*/

class LArRawSC  {
  
 private:
  
  /** @brief Online Identifier */
  HWIdentifier m_hardwareID;
  
  /** @brief Latome channel */
  short m_chan;
  
  /** @brief LATOME source Id*/
  unsigned int m_sourceId;

  /** @brief vector of energies */
  std::vector < int > m_energies;
  
  /** @brief vector of bcids */
  std::vector < unsigned short > m_BCId;
  
  /** @brief vector of saturation flags */
  std::vector < bool > m_satur;
  
 public:
  /** @brief constructor 
      @param[in] channel_value  Online identifier
      @param[in] gain_value  Gain
      @param[in] sample_value Reference of vector with ADC samples
  */
  LArRawSC(const HWIdentifier & channel_value, const short chan, const unsigned int sourceId, const std::vector < int > & energies, const std::vector<unsigned short> bcids, std::vector<bool> satur):
    m_hardwareID(channel_value), m_chan(chan),m_sourceId(sourceId), m_energies(energies), m_BCId(bcids), m_satur(satur) {}

  LArRawSC(const HWIdentifier & channel_value,
           const short chan,
           const unsigned int sourceId,
           std::vector < int > && energies,
           std::vector < unsigned short > && bcids,
           std::vector < bool > && satur)
    : m_hardwareID(channel_value),
      m_chan(chan),
      m_sourceId(sourceId),
      m_energies(std::move(energies)),
      m_BCId(std::move(bcids)),
      m_satur(std::move(satur))
  {}
    
    /** @return HWIdentifier */
    const HWIdentifier & hardwareID() const {return m_hardwareID; }

    /** @return channel */
    short chan() const { return m_chan; }

    /** @return source Id value */
    unsigned int SourceId() const {return m_sourceId;}

    /** @return number of samples */
    int nsamples() const { return m_energies.size(); }

    /** @return a reference to a stl vector containing the energies values */
    const std::vector < int > & energies() const { return m_energies; }

    /** @return a reference to a stl vector containing the bcid values */
    const std::vector < unsigned short > & bcids() const { return m_BCId; }

    /** @return a reference to a stl vector containing the saturation flags */
    const std::vector < bool > & satur() const { return m_satur; }

    /** @brief Conversion operator to a std::string <br> Can be used in a cast operation : (std::string) digit <br> */
    virtual operator std::string() const;
    
    /** @brief Set energies .
        @param[in] samples  vector of energies
    */
    void setEnergies( std::vector < int > energies);

    /** @brief Set bcids.
        @param[in] samples  vector of bcids
    */
    void setBCIds( std::vector < unsigned short > bcids);

    /** @brief Destructor */
    virtual ~LArRawSC() { }

    /** @brief default constructor for persistency */
    LArRawSC();
};

#endif //LArRawSC_H

