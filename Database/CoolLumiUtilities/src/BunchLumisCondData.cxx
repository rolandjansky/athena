/*
 * Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration.
 */
/**
 * @file CoolLumiUtilities/src/BunchLumisCondData.cxx
 * @author scott snyder <snyder@bnl.gov>
 * @date May, 2019
 * @brief Holds raw luminosity data, per channel.
 */


#include "CoolLumiUtilities/BunchLumisCondData.h"


/**
 * @brief Return vector of per-BCID raw luminosity for a given channel.
 * @param Channel to retrieve.
 *
 * Will return an empty vector if the channel doesn't exist.
 */
const std::vector<float>&
BunchLumisCondData::rawLuminosity (unsigned int channel) const
{
  auto it =  m_rawLumi.find (channel);
  if (it != m_rawLumi.end()) {
    return it->second;
  }
  static const std::vector<float> dum;
  return dum;
}


/**
 * @brief Add raw luminisity data for a channel.
 * @param channel Channel to add.
 * @param rawLumi Vector of per-BCID raw luminosities.
 */
void BunchLumisCondData::addChannel (unsigned int channel,
                                     std::vector<float>&& rawLumi)
{
  m_rawLumi[channel] = std::move (rawLumi);
}

