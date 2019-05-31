// This file's extension implies that it's C, but it's really -*- C++ -*-.
/*
 * Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration.
 */
/**
 * @file CoolLumiUtilities/BunchLumisCondData.h
 * @author scott snyder <snyder@bnl.gov>
 * @date May, 2019
 * @brief Holds raw luminosity data, per channel.
 */


#ifndef COOLLUMIUTILITIES_BUNCHLUMISCONDDATA_H
#define COOLLUMIUTILITIES_BUNCHLUMISCONDDATA_H


#include "AthenaKernel/CondCont.h"
#include "AthenaKernel/CLASS_DEF.h"
#include <vector>
#include <unordered_map>


/**
 * @brief Holds raw luminosity data, per channel.
 */
class BunchLumisCondData
{
public:
  /**
   * @brief Return vector of per-BCID raw luminosity for a given channel.
   * @param Channel to retrieve.
   *
   * Will return an empty vector if the channel doesn't exist.
   */
  const std::vector<float>& rawLuminosity (unsigned int channel) const;


  /**
   * @brief Add raw luminisity data for a channel.
   * @param channel Channel to add.
   * @param rawLumi Vector of per-BCID raw luminosities.
   */
  void addChannel (unsigned int channel, std::vector<float>&& rawLumi);
  

private:
  /// Raw luminosity, per channel.
  std::unordered_map<unsigned int, std::vector<float> > m_rawLumi;
};


CLASS_DEF (BunchLumisCondData, 140920238, 0)
CONDCONT_DEF (BunchLumisCondData, 134532794);


#endif // not COOLLUMIUTILITIES_BUNCHLUMISCONDDATA_H
