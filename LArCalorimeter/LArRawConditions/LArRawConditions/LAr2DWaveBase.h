// This file's extension implies that it's C, but it's really -*- C++ -*-.

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id$
/**
 * @file LArRawConditions/LAr2DWaveBase.h
 * @author scott snyder <snyder@bnl.gov>
 * @date Apr, 2011
 * @brief Per-channel class holding wave information in time bins.
 *
 * In several cases, we need to hold a set of wave data per channel.
 * The waves are binned in the starting time for the wave (tbin),
 * and each wave consists of a fixed number of samples.
 * We also record the time offset and bin width.
 *
 * This is used by both LArShapeP2 and LArOFCP1, both of which store
 * two types of wave per channel: the shape and its derivative
 * for LArShapeP2, and the A and B OFCs for LArOFCP1.  We use this
 * class to factor out common code between them.
 *
 * Presently, the number of types of wave stored is given as a
 * compile-time constant (nWaves).
 *
 * This representation is used when we need to store data for a single
 * channel on its own.  When the channel is part of a subset,
 * then for LArShapeP2 and LArOFCP1, we use LArCompactSubset,
 * and the data for all channels live within the subset, and we
 * access the data for a channel via LArCompactSubsetChannelProxy.
 */


#ifndef LARRAWCONDITIONS_LAR2DWAVEBASE_H 
#define LARRAWCONDITIONS_LAR2DWAVEBASE_H


#include "LArElecCalib/LArVectorProxy.h"
#include <vector>
class LArCompactSubsetChannelProxy;


/**
 * @brief Per-channel class holding wave information in time bins.
 */
class LAr2DWaveBase
{
public:      
  /// The number of different types of waves we store.
  static const unsigned int nWaves = 2;


  /**
   * @brief Default constructor.
   */
  LAr2DWaveBase();


  /**
   * @brief Constructor from wave data directly.
   * @param timeOffset Time offset value.
   * @param timeBinWidth Time bin width value.
   * @param wave0 Data for first wave, as [tbin][sample].
   * @param wave0 Data for second wave, as [tbin][sample].
   */
  LAr2DWaveBase(float timeOffset,
                float timeBinWidth,
                const std::vector<std::vector<float> >& wave0,
                const std::vector<std::vector<float> >& wave1);


  /**
   * @brief Constructor from persistent data.
   * @param timeOffset Time offset value.
   * @param timeBinWidth Time bin width value.
   * @param nPhases Number of time bins in the data.
   * @param nSamples Number of samples in the data.
   * @param wave0 Packed data for first wave.
   * @param wave1 Packed data for second wave.
   * @param index Start of the desired data in the packed data.
   *
   * The packed data are formatted in blocks of nPhases * nSamples
   * floats.  The first word we should use is given by index.
   */
  LAr2DWaveBase(float timeOffset,
                float timeBinWidth,
                unsigned int nPhases,
                unsigned int nSamples,
                const std::vector<float>& wave0,
                const std::vector<float>& wave1,
                unsigned int index);


  /**
   * @brief Constructor for channel data from a compact subset.
   * @param other Reference to the source channel data.
   */
  LAr2DWaveBase(const LArCompactSubsetChannelProxy& other);


  /**
   * @brief Assign from another wave object.
   * @param other The source object.
   *              NOTE: We use move semantics; OTHER is left with no data.
   *
   * Could write with an rvalue reference in the future?
   */
  void setFrom (LAr2DWaveBase& other);


  /**
   * @brief Is there any data in this channel?
   */
  bool isEmpty() const;


  /**
   * @brief Return the time offset for this channel.
   */
  float timeOffset() const;


  /**
   * @brief Return the time bin width for this channel.
   */
  float timeBinWidth() const;


  /**
   * @brief Return the number of time bins for a wave.
   * @param which The type of wave to query.
   */
  size_t waveSize(unsigned int which) const;


  /**
   * @brief Return wave data.
   * @param which The type of wave to return.
   * @param tbin The time bin to return.
   */
  LArVectorProxy wave (unsigned int which, size_t tbin) const;

private:
  /// The time offset for this channel.
  float        m_timeOffset;

  /// The time bin width for this channel.
  float        m_timeBinWidth;

  /// The wave data.
  std::vector<std::vector<float> > m_waves[nWaves];
};


#include "LArRawConditions/LAr2DWaveBase.icc"


#endif // not LARRAWCONDITIONS_LAR2DWAVEBASE_H
