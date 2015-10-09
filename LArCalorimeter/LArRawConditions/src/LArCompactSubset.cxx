/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id$
/**
 * @file LArRawConditions/src/LArCompactSubset.cxx
 * @author scott snyder <snyder@bnl.gov>
 * @date Apr, 2011
 * @brief A more compact way of storing shape/ofc data.
 */


#include "LArRawConditions/LArCompactSubset.h"
#include "LArRawConditions/LAr2DWaveBase.h"
#include <cstdlib>
#include <cassert>


//=========================================================================


/**
 * @brief Initialize the referenced data from a standalone object.
 * @param other The object from which to initialize.
 */
void LArCompactSubsetChannelProxy::assign (const LAr2DWaveBase& other)
{
  size_t chanSize = other.waveSize(0);
  m_subset->setTimings (m_chan, other.timeOffset(), other.timeBinWidth());
  for (unsigned int which = 0; which < LAr2DWaveBase::nWaves; ++which) {
    assert (chanSize == other.waveSize(which));
    for (unsigned int tbin = 0; tbin < chanSize; tbin++)
      m_subset->setData (which, m_chan, tbin, chanSize,
                         other.wave(which, tbin));
  }
}


//==========================================================================


/**
 * @brief Constructor.
 * @param nfeb Initial number of FEBs to allocate.
 */
LArCompactSubsetVector::LArCompactSubsetVector (size_t nfeb /*= 0*/)
  : m_chanSize (0),
    m_nSamples (0),
    m_timeOffset (0),
    m_timeBinWidth (0),
    m_febids (nfeb),
    m_indices (nfeb+1)
{
}


/**
 * @brief Change the size of the vector.
 * @param sz New size.
 *
 * Restriction: The size can only be increased.
 */
void LArCompactSubsetVector::resize (size_t sz)
{
  size_t oldsz = m_febids.size();
  m_febids.resize (sz);
  m_indices.resize (sz+1);
  
  if (sz >= oldsz) {
    for (size_t j = oldsz+1; j < sz+1; j++)
      m_indices[j] = m_indices[oldsz];
  }
  else {
    abort();
    // Unimplemented.
  }
}


/**
 * @brief Release any allocated but unused storage.
 * Called by the P->T converter after conversion is complete.
 */
void LArCompactSubsetVector::trim()
{
  std::vector<float> tmp_data (m_data);
  m_data.swap (tmp_data);

  std::vector<int> tmp_slots (m_slots);
  m_slots.swap (tmp_slots);
}


/**
 * @brief Resize one channel vector.
 * @param febIndex The index of the vector to resize.
 * @param sz The new size of the vector.
 *
 * Restriction: Only the last index may be resized.
 */
void LArCompactSubsetVector::resizeChanVec (size_t febIndex, size_t sz)
{
  assert (m_indices[febIndex+1] == m_indices.back());
  size_t cursz = m_indices[febIndex+1] - m_indices[febIndex];
  if (sz == cursz) return;
  int newend = m_indices[febIndex] + sz;
  for (size_t j = febIndex+1; j < m_indices.size(); j++)
    m_indices[j] = newend;
  m_slots.resize (m_slots.size() + (sz - cursz) * m_chanSize, -1);
}



/**
 * @brief Retrieve wave data.
 * @param which The wave type to retrieve.
 * @param chan The index of the channel to retrieve.
 * @param tbin The time bin index to retrieve.
 */
LArVectorProxy
LArCompactSubsetVector::getData (unsigned int which,
                                 unsigned int chan,
                                 size_t tbin) const
{
  assert (which < nWaves);
  int slot = m_slots[chan * m_chanSize + tbin];
  const float* beg = 0;
  const float* end = 0;
  if (slot >= 0) {
    size_t nsamp = m_nSamples;
    const float* chanstart = &m_data[slot * nWaves * nsamp];
    beg = chanstart + which * nsamp;
    end = beg + nsamp;
  }
  return LArVectorProxy (beg, end);
}


/**
 * @brief Set timing information for a channel.
 * @param chan Channel index to set.
 * @param timeOffset Time offset to set.
 * @param timeBinWidth Time bin width to set.
 *
 * We currently assume that all non-empty channels within a subset
 * have the same values for these parameters.  We'll trip
 * an assertion if that's not the case.
 */
void LArCompactSubsetVector::setTimings (unsigned int /*chan*/,
                                         float timeOffset,
                                         float timeBinWidth)
{
  if (m_timeOffset == 0 && m_timeBinWidth == 0) {
    m_timeOffset = timeOffset;
    m_timeBinWidth = timeBinWidth;
  }
  else {
    assert (m_timeOffset == timeOffset);
    assert (m_timeBinWidth == timeBinWidth);
  }
}


/**
 * @brief Set wave data for a channel.
 * @param which Wave type to set.
 * @param chan Channel index to set.
 * @param tbin Time bin index to set.
 * @param chanSize Number of time bins for this channel.
 * @param from Wave data.
 *
 * We currently assume that all non-empty channels within a subset
 * have the same number of time bins and samples.  We'll trip
 * an assertion if that's not the case.
 */
void LArCompactSubsetVector::setData (unsigned int which,
                                      unsigned int chan,
                                      size_t tbin,
                                      size_t chanSize,
                                      const LArVectorProxy& from)
{
  assert (which < nWaves);

  if (m_chanSize == 0 &&
      m_nSamples == 0)
  {
    m_chanSize = chanSize;
    m_nSamples = from.size();
    m_slots.resize (m_indices.back() * m_chanSize, -1);
  }
  else {
    assert (m_chanSize == chanSize);
    assert (m_nSamples == from.size());
  }

  int slot = m_slots[chan * m_chanSize + tbin];
  if (slot < 0) {
    slot = m_data.size() / (nWaves * m_nSamples);
    m_data.resize ((slot+1) * (nWaves * m_nSamples));
    m_slots[chan * m_chanSize + tbin] = slot;
  }

  std::copy (from.begin(), from.end(),
             &m_data[(slot * nWaves + which) * m_nSamples]);
}


