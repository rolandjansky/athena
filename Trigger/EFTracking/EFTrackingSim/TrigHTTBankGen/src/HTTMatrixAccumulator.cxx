// Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration

/**
 * @file HTTMatrixAccumulator.cxx
 * @author Riley Xu - riley.xu@cern.ch
 * @date May 8th, 2020
 * @brief Helper struct and functions for accumulating sector information for matrix generation.
 */

#include "HTTMatrixAccumulator.h"


/**
 * Accumulates an accumulator (e.g. a single track) to an accumulate map.
 *
 * @param map - map to be added to
 * @param modules - sector definition of the accumulator to be added
 * @param acc - accumulator to be added
 *
 * @return true if a new sector was just added, and false if acc was accumulated into an existing item.
 */
bool accumulate(AccumulateMap & map, std::vector<module_t> const & modules, HTTMatrixAccumulator const & acc)
{
  auto it = map.find(modules);
  if (it == map.end()) // Create new entry
    {
      map.insert({ modules, acc });
      return true;
    }
  else // Add/append entries
    {

      for (unsigned i = 0; i < HTTTrackPars::NPARS; i++)
	it->second.pars[i] += acc.pars[i];

      size_t nDim = acc.hit_coords.size();
      for (unsigned i = 0; i < nDim; i++)
        {
	  it->second.hit_coords[i] += acc.hit_coords[i];
	  it->second.hit_x_QoP[i]  += acc.hit_x_QoP[i];
	  it->second.hit_xG_HIP[i]  += acc.hit_xG_HIP[i];
	  it->second.hit_x_d0[i]   += acc.hit_x_d0[i];
	  it->second.hit_x_z0[i]   += acc.hit_x_z0[i];
	  it->second.hit_x_eta[i]  += acc.hit_x_eta[i];
	  it->second.hit_xG_eta[i]  += acc.hit_xG_eta[i];
	  it->second.hit_x_phi[i]  += acc.hit_x_phi[i];

	  for (unsigned j = i; j < nDim; j++)
	    it->second.covariance[i * nDim + j] += acc.covariance[i * nDim + j];
        }

      it->second.track_bins.insert(it->second.track_bins.end(), acc.track_bins.begin(), acc.track_bins.end());
      return false;
    }
}

