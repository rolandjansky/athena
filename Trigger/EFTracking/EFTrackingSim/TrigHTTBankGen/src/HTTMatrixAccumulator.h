// Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration

#ifndef HTTMATRIXACCUMULATOR_H
#define HTTMATRIXACCUMULATOR_H


/**
 * @file HTTMatrixAccumulator.h
 * @author Riley Xu - riley.xu@cern.ch
 * @date May 8th, 2020
 * @brief Helper struct for accumulating sector information for matrix generation.
 *
 * This file declares a helper struct that stores all the information needed by
 * the matrix file, for a single sector. It also declares several helper functions
 * and typedefs for reading and writing this struct from ROOT files.
 */

#include <vector>
#include <unordered_map>
#include <algorithm>
#include <boost/functional/hash.hpp>

#include "TTree.h"

#include "TrigHTTObjects/HTTTypes.h"
#include "TrigHTTObjects/HTTTrackPars.h"


/*
 * This struct stores all the information in the matrix file for a single sector.
 * It is an accumulation of relevant track properties from training truth tracks
 * used to create the sector. These are later used to generate the fit constants.
 * See comments in HTTConstGenAlgo.h.
 */
struct HTTMatrixAccumulator
{
  // nCoords is the number of hit coordinates for a full 13/13 track
  // (i.e. +2 per pixel layer, +1 per strip layer)
HTTMatrixAccumulator(unsigned nLayers, unsigned nCoords) :
  FTK_modules(nLayers),
    coords_usable(nCoords),
    hit_coords(nCoords),
    hit_coordsG(nCoords),
    hit_x_QoP(nCoords),
    hit_xG_HIP(nCoords),
    hit_x_d0(nCoords),
    hit_x_z0(nCoords),
    hit_x_eta(nCoords),
    hit_xG_eta(nCoords),
    hit_x_phi(nCoords),
    covariance(nCoords * nCoords),
    covarianceG(nCoords * nCoords)
  {
  }

  std::vector<module_t> FTK_modules; // Old FTK-style module ids
  std::vector<bool> coords_usable; // size ncoords, determine whether this is a usable coordinate or from a WC, only filled as we normali
  size_t nusable() const { 
    size_t n = 0; 
    for (auto coord : coords_usable) {
      if (coord) n++; 
    }
    return n;
  }

  ///////////////////////////////////////////////////////////////////////////
  // For each track in this sector, add up the following variables.
  ///////////////////////////////////////////////////////////////////////////

  HTTTrackPars pars;

  // The hit_* vectors have size nCoords. For example, hit_x_phi stores the hit
  // coordinates times phi for each track, so after accumulation
  //      hit_x_phi[i] = sum_tracks(track.hit_coords[i] * track.phi)
  std::vector<double> hit_coords; // hit coordinates
  std::vector<double> hit_coordsG; // hit global coordinates
  std::vector<double> hit_x_QoP;  // hit coordinate * half inverse pt
  std::vector<double> hit_xG_HIP;  // global hit coordinate * half inverse pt
  std::vector<double> hit_x_d0;   // hit coordinate * d0
  std::vector<double> hit_x_z0;   // hit coordinate * z0
  std::vector<double> hit_x_eta;  // hit coordinate * eta
  std::vector<double> hit_xG_eta;  // global hit coordinate * eta
  std::vector<double> hit_x_phi;  // hit coordinate * phi
  std::vector<double> covariance; // Size nCoords * nCoords, hit_coord[i] * hit_coord[j]
  std::vector<double> covarianceG; // Size nCoords * nCoords, hit_coordG[i] * hit_coordG[j]

  ///////////////////////////////////////////////////////////////////////////
  // For each track in this sector, append to these variables.
  ///////////////////////////////////////////////////////////////////////////

  std::vector<HTTTrackParsI> track_bins; // Size # tracks (coverage).
  // Stores the binned track parameters of each track.
  // Note track_bins.size() conveniently also stores the coverage of this sector.
};


// A hash customization for std containers, to be used in unordered_map
template <typename Container>
struct container_hash
{
  std::size_t operator()(Container const& c) const
  { return boost::hash_range(c.begin(), c.end()); }
};


// A useful typedef for the matrix algorithms.
// Maps the sector definition (list of module identifier hashes) to an accumulator.
typedef std::unordered_map<std::vector<module_t>, HTTMatrixAccumulator, container_hash<std::vector<module_t>>> AccumulateMap;


/**
 * Helper function for adding a track/accumulator to an accumulate map.
 *
 * @param map - map to be added to
 * @param modules - sector definition of the accumulator to be added
 * @param acc - accumulator to be added
 *
 * @return true if a new sector was just added, and false if acc was accumulated into an existing item.
 */
bool accumulate(AccumulateMap & map, std::vector<module_t> const & modules, HTTMatrixAccumulator const & acc);


#endif
