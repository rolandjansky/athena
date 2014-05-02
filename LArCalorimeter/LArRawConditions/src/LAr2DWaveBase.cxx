/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id$
/**
 * @file LArRawConditions/src/LAr2DWaveBase.cxx
 * @author scott snyder <snyder@bnl.gov>
 * @date Apr, 2011
 * @brief Per-channel class holding wave information in time bins.
 */

#include "LArRawConditions/LAr2DWaveBase.h"
#include "LArRawConditions/LArCompactSubset.h"
#include <cassert>


/**
 * @brief Constructor from wave data directly.
 * @param timeOffset Time offset value.
 * @param timeBinWidth Time bin width value.
 * @param wave0 Data for first wave, as [tbin][sample].
 * @param wave0 Data for second wave, as [tbin][sample].
 */
LAr2DWaveBase::LAr2DWaveBase(float timeOffset,
                             float timeBinWidth,
                             const std::vector<std::vector<float> >& wave0,
                             const std::vector<std::vector<float> >& wave1)
  : m_timeOffset(timeOffset),
    m_timeBinWidth(timeBinWidth)
{
  m_waves[0] = wave0;
  m_waves[1] = wave1;
} 


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
LAr2DWaveBase::LAr2DWaveBase(float timeOffset,
                             float timeBinWidth,
                             unsigned int nPhases,
                             unsigned int nSamples,
                             const std::vector<float>& wave0,
                             const std::vector<float>& wave1,
                             unsigned int index)
 : m_timeOffset(timeOffset),
   m_timeBinWidth(timeBinWidth)
{
  assert (index + nPhases*nSamples <= wave0.size());
  assert (index + nPhases*nSamples <= wave1.size());

  m_waves[0].resize (nPhases);
  m_waves[1].resize (nPhases);

  for (unsigned int i = 0; i < nPhases; i++) {
    m_waves[0][i].assign (wave0.begin() + index,
                          wave0.begin() + index + nSamples);
    m_waves[1][i].assign (wave1.begin() + index,
                          wave1.begin() + index + nSamples);
    index += nSamples;
  }
}


/**
 * @brief Constructor for channel data from a compact subset.
 * @param other Reference to the source channel data.
 */
LAr2DWaveBase::LAr2DWaveBase (const LArCompactSubsetChannelProxy& other)
{
  if (!other.isValid()) {
    // Invalid source --- do the same as the default ctor.
    m_timeOffset = 0;
    m_timeBinWidth = 25./24.;
  }
  else {
    m_timeOffset = other.timeOffset();
    m_timeBinWidth = other.timeBinWidth();
    size_t chanSize = other.getSize();
    for (unsigned int which = 0; which < nWaves; ++which) {
      m_waves[which].resize (chanSize);
      for (unsigned int i = 0; i < chanSize; i++) {
        LArVectorProxy wave = other.getData (which, i);
        m_waves[which][i].assign (wave.begin(), wave.end());
      }
    }
  }
}


/**
 * @brief Assign from another wave object.
 * @param other The source object.
 *              NOTE: We use move semantics; OTHER is left with no data.
 *
 * Could write with an rvalue reference in the future?
 */
void LAr2DWaveBase::setFrom (LAr2DWaveBase& other)
{
  m_timeOffset = other.m_timeOffset;
  m_timeBinWidth = other.m_timeBinWidth;
  for (unsigned int which = 0; which < nWaves; ++which)
    m_waves[which].swap (other.m_waves[which]);
}


