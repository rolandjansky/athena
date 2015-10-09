// This file's extension implies that it's C, but it's really -*- C++ -*-.

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id$
/**
 * @file LArRawConditions/LArCompactSubset.h
 * @author scott snyder <snyder@bnl.gov>
 * @date Apr, 2011
 * @brief A more compact way of storing shape/ofc data.
 *
 * Conditions data are divided into `subsets'.
 * Each subset acts as a vector of <FebId, ChannelVector> pairs,
 * where ChannelVector acts as a vector of channel data.
 *
 * For LArShapeP2 and LArOFCP1, each channel object holds two types
 * of arrays of waves, indexed by time bin.  This used to be represented
 * as two vector<vector<float> >'s.  This, however, results in a large
 * overhead.  So, instead, we combine the data for all channels
 * within a subset into a single object.  To preserve the existing
 * interfaces, we need to define a set of proxy objects for accessing
 * the data.
 *
 * Note that some channels may be missing, so we need to allow for that
 * without allocating the full data for that event.
 *
 * As part of this we assume that within a subset:
 *   - All waves have the same number of samples.
 *   - All channels have the same number of time bins.
 *   - All channels have the same time offset and bin width.
 *
 * We organize the data like this.
 * We have two floats, giving the time offset and bin width.
 * We have two integers, giving the number of time bins (chanSize)
 * and the number of samples (nSamples).
 *
 * We have vector of FEB IDs.  The length of this vector corresponds
 * to the number of pairs in the subset.  We have a corresponding array
 * indices.  The channels corresponding to FEB index i have indices
 * indices[i] through indices[i+1] (exclusive).  (So there is one more
 * element in indices than there is in feb ids.)  Another array
 * of integers, slots, is indexed by the channel index and time bin.
 * This is -1 if no data are present here; otherwise, it gives
 * a `slot' number.  Finally, the data themselves are in m_data.
 * This is structured as fixed-size blocks of nWaves * nSamples floats,
 * indexed by the slot number.   Currently, nWaves is a compile-time
 * constant of 2.
 *
 * Here's a brief rundown of the classes here.
 *
 * LArCompactSubsetVector acts as the vector of pairs.
 * It is also what holds all the data; to that end, it has some extra
 * methods called by the other classes here to access the data.
 * If you access an `element' of this class, you get a...
 *
 * LArCompactSubsetFebPair or LArCompactSubsetConstFebPair.
 * These act like a pair; with `first' being a reference to a FebId (non-const
 * for the first variant and const for the second variant).
 * `second' is ...
 *
 * LArCompactSubsetChannelVector, which acts like a vector of
 * channel objects.  This holds a pointer to the subset, as well
 * as the feb index.  For technical reasons, we also have
 * a LArCompactSubsetChannelVectorPointer, which acts like a pointer
 * to the previous class.  If we dereference an element here, we get a...
 *
 * LArCompactSubsetChannelProxy.  This holds a subset pointer and
 * a channel index.  This is a proxy to the data for a single proxy.
 * We want to be able to use the class we get here as either
 * a LArShapeP2 or a LArOFCP1.  To really make that work, we'd need
 * to duplicate all the code for the two cases (either manually
 * or via templates).  Since the two classes are really just the same
 * except for the method names, we take the lazy way out here,
 * and just give LArCompactSubsetChannelProxy the methods for
 * both the Shape and OFC interfaces.  We also turn out to need
 * a LArCompactSubsetChannelPointer, which acts like a pointer
 * to channel data.
 */


#ifndef LARRAWCONDITIONS_LARCOMPACTSUBSET_H
#define LARRAWCONDITIONS_LARCOMPACTSUBSET_H

#include "LArElecCalib/ILArShape.h"
#include "LArElecCalib/ILArOFC.h"
#include <cstddef>


class LArCompactSubsetVector;
class LAr2DWaveBase;


//==========================================================================


/**
 * @brief A more compact way of storing shape/ofc data.
 */
class LArCompactSubsetChannelProxy
{
public:
  /**
   * @brief Default constructor.  Makes an invalid proxy.
   */
  LArCompactSubsetChannelProxy();


  /**
   * @brief Constructor.
   * @brief chan The channel index we reference.
   * @brief subset The subset object within which we live.
   */
  LArCompactSubsetChannelProxy (unsigned int chan,
                                LArCompactSubsetVector& subset);


  /**
   * @brief Test to see if the proxy is valid.
   */
  bool isValid() const;


  /**
   * @brief Initialize the referenced data from a standalone object.
   * @param other The object from which to initialize.
   */
  LArCompactSubsetChannelProxy& operator= (const LAr2DWaveBase& other);


  /**
   * @brief Initialize the referenced data from a standalone object.
   * @param other The object from which to initialize.
   */
  void assign (const LAr2DWaveBase& other);


  /**
   * @brief Return the time offset for this channel.
   */
  float timeOffset() const;


  /**
   * @brief Return the time bin width for this channel.
   */
  float timeBinWidth() const;


  /**
   * @brief Return the number of time bins for this channel.
   */
  size_t getSize () const;


  /**
   * @brief Return one set of wave data from this channel.
   * @param which Index of the wave type to return.
   * @param tbin Time bin index.
   */
  LArVectorProxy getData (unsigned int which, size_t tbin) const;


  //=== LArShapeP2 interface.  The two wave types are shape and derivative.

  size_t shapeSize() const;
  size_t shapeDerSize() const;
  ILArShape::ShapeRef_t shape (size_t tbin) const;
  ILArShape::ShapeRef_t shapeDer (size_t tbin) const;

  //=== LArOFCP1 interface.  The two wave types are OFC A and B coeffiecients.

  size_t OFC_aSize() const;
  size_t OFC_bSize() const;
  ILArOFC::OFCRef_t OFC_a (size_t tbin) const;
  ILArOFC::OFCRef_t OFC_b (size_t tbin) const;


private:
  /// Channel index to which we're referring.
  unsigned int m_chan;

  /// Subset within which we live.  Null for an invalid proxy.
  LArCompactSubsetVector* m_subset;
};


//==========================================================================


/**
 * @brief This acts as a pointer to a LArCompactSubsetChannelProxy.
 *
 * Actually, since LArCompactSubsetChannelProxy is itself a reference,
 * and we'd need to hold the reference anyway,
 * we just extend LArCompactSubsetChannelProxy with a pointer dereference.
 */
class LArCompactSubsetChannelPointer
  : public LArCompactSubsetChannelProxy
{
public:
  /**
   * @brief Constructor.
   * @param p Pointer to a channel proxy.
   */
  LArCompactSubsetChannelPointer (const LArCompactSubsetChannelProxy* p);


  /**
   * @brief `Dereference' the pointer.
   */
  LArCompactSubsetChannelProxy operator*() const;


  /**
   * @brief Test for a valid pointer.
   */
  operator bool() const;
};


//==========================================================================


/**
 * @brief This acts like a vector of channel objects.
 *
 * We don't implement the complete vector interface, though;
 * just what's actually used.
 */
class LArCompactSubsetChannelVector
{
public:
  /// Iterator for LArCompactSubsetChannelVector.
  /// Iteration variable is the channel index.
  class const_iterator
  {
  public:
    /**
     * @brief Default constructor.
     */
    const_iterator();


    /**
     * @brief Constructor from channel index and subset reference.
     */
    const_iterator (unsigned int chan, LArCompactSubsetVector& subset);


    /**
     * @brief Iterator dereference.
     * Yields a channel proxy.
     */
    LArCompactSubsetChannelProxy operator*() const;


    /**
     * @brief Iterator equality test.
     */
    bool operator== (const const_iterator& other) const;


    /**
     * @brief Iterator inequality test.
     */
    bool operator!= (const const_iterator& other) const;


    /**
     * @brief Iterator increment.
     */
    const_iterator& operator++();


    /**
     * @brief Adjust iterator backwards.
     * @param delta Amount by which to adjust.
     */
    const_iterator operator- (size_t delta) const;


    /**
     * @brief Iterator difference.
     * @param other Other iterator for difference.
     */
    size_t operator- (const const_iterator& other) const;


  private:
    /// Channel index at which we're currently pointing.
    unsigned int m_chan;

    /// Subset within which we live.
    LArCompactSubsetVector* m_subset;
  };


  /// No separate non-const iterator.
  typedef const_iterator iterator;


  /**
   * @brief Constructor.
   * @param febIndex FEB index with which we're associated.
   * @param subset Subset within which we live.
   */
  LArCompactSubsetChannelVector (size_t febIndex,
                                 LArCompactSubsetVector* subset);


  /**
   * @brief Return the size of this vector (number of channels for this FEB).
   */
  size_t size() const;


  /**
   * @brief Vector indexing.  Returns a channel proxy.
   * @param i Channel index within the vector.
   */
  LArCompactSubsetChannelProxy operator[] (size_t i) const;


  /// Begin iterator.
  const_iterator begin() const;


  /// End iterator.
  const_iterator end() const;


  /**
   * @brief Resize this vector.
   * @param sz The desired new size.
   *
   * Restriction: Only the vector associated with the last FEB index
   * in the subset may be resized.
   */
  void resize (size_t sz);


protected:
  /// The index of the FEB with which this vector is associated.
  size_t m_febIndex;

  /// The subset within which we live.
  LArCompactSubsetVector* m_subset;
};



//==========================================================================


/**
 * @brief This acts as a pointer to a LArCompactSubsetChannelVector.
 *
 * Actually, since LArCompactSubsetChannelVector is itself a reference,
 * and we'd need to hold the reference anyway,
 * we just extend LArCompactSubsetChannelVector with a pointer dereference.
 */
class LArCompactSubsetChannelVectorPointer
  : public LArCompactSubsetChannelVector
{
public:
  /**
   * @brief Default constructor.  Makes an invalid `pointer'.
   */
  LArCompactSubsetChannelVectorPointer();


  /**
   * @brief Constructor.
   * @param p Pointer to a channel vector proxy.
   */
  LArCompactSubsetChannelVectorPointer(const LArCompactSubsetChannelVector* p);


  /**
   * @brief `Dereference' the pointer.
   */
  LArCompactSubsetChannelVector operator*() const;


  /**
   * @brief `Dereference' the pointer.
   */
  const LArCompactSubsetChannelVector* operator->() const;
};


//==========================================================================


/**
 * @brief This acts like a FEB ID/channel vector pair.
 */
class LArCompactSubsetFebPair
{
public:
  typedef unsigned int FebId;


  /**
   * @brief Constructor.
   * @param febIndex The index of the pair.
   * @param subset The subset within which we live.
   */
  LArCompactSubsetFebPair (size_t febIndex,
                           LArCompactSubsetVector& subset);


  FebId& first;
  LArCompactSubsetChannelVector second;
};


/**
 * @brief This acts like a FEB ID/channel vector pair.
 */
class LArCompactSubsetConstFebPair
{
public:
  typedef unsigned int FebId;


  /**
   * @brief Constructor.
   * @param febIndex The index of the pair.
   * @param subset The subset within which we live.
   */
  LArCompactSubsetConstFebPair (size_t febIndex,
                                LArCompactSubsetVector& subset);


  const FebId& first;
  LArCompactSubsetChannelVector second;
};


//==========================================================================


/**
 * @brief This acts like a vector of FEB ID / channel vector pairs.
 *
 * This is also the class which contains all the data.
 */
class LArCompactSubsetVector
{
public:
  /// Number of wave types we store.
  static const unsigned int nWaves = 2;

  /// Type for FEB IDs.  Would be better to get this from somewhere common...
  typedef unsigned int FebId;


  /**
   * @brief Iterator class.
   * This dereferences into LArCompactSubsetFebPair.
   * The iteration variable is the FEB index.
   */
  class iterator
  {
  public:
    /**
     * @brief Constructor.
     * @param febIndex The FEB index to which we point.
     * @param subset The subset within which we live.
     */
    iterator (size_t febIndex,
              LArCompactSubsetVector& subset);


    /**
     * @brief `Dereference' the iterator.
     */
    LArCompactSubsetFebPair operator*();


    /**
     * @brief Compare iterators for equality.
     */
    bool operator== (const iterator& other) const;


    /**
     * @brief Compare iterators for inequality.
     */
    bool operator!= (const iterator& other) const;


    /**
     * @brief Advance iterator.
     */
    iterator& operator++();


    /**
     * @brief Adjust iterator.
     * @param delta Amount by which to advance the iterator.
     */
    iterator operator+ (size_t delta) const;


  private:
    /// Index we're currently referencing.
    size_t m_febIndex;

    /// The subset within which we live.
    LArCompactSubsetVector& m_subset;
  };


  /// Need this type defined, but we don't do anything with it.
  class const_iterator {};


  /**
   * @brief Constructor.
   * @param nfeb Initial number of FEBs to allocate.
   */
  LArCompactSubsetVector (size_t nfeb = 0);



  /**
   * @brief Return the size of this vector.
   */
  size_t size() const;


  /**
   * @brief Vector indexing.
   * @param i Index to look up.
   */
  LArCompactSubsetFebPair operator[] (size_t i);


  /**
   * @brief Vector indexing.
   * @param i Index to look up.
   */
  const LArCompactSubsetConstFebPair operator[] (size_t i) const;


  /// Begin iterator.
  iterator begin();


  /// End iterator.
  iterator end();


  /**
   * @brief Change the size of the vector.
   * @param sz New size.
   *
   * Restriction: The size can only be increased.
   */
  void resize (size_t sz);


  //======
  // Functions used by P->T conversion.

  /**
   * @brief Release any allocated but unused storage.
   * Called by the P->T converter after conversion is complete.
   */
  void trim();


  //======
  // Functions used by LArCompactSubsetFebPair

  /**
   * @brief Return a reference to a FEB ID variable.
   * @param febIndex Index of the desired FEB ID.
   */
  FebId& febIdRef (size_t febIndex);


  //======
  // Functions used by LArCompactSubsetChannelVector

  /**
   * @brief Return the number of channels for a given FEB index.
   * @param febIndex The index of the FEB to query.
   */
  size_t febSize (unsigned int febIndex) const;


  /**
   * @brief Return the index of the first channel for a FEB index.
   * @param febIndex The index of the FEB to query.
   *
   * If febIndex is the number of FEBs, then this returns
   * the total number of channels.
   */
  unsigned int chanIndex (unsigned int febIndex) const;


  /**
   * @brief Resize one channel vector.
   * @param febIndex The index of the vector to resize.
   * @param sz The new size of the vector.
   *
   * Restriction: Only the last index may be resized.
   */
  void resizeChanVec (size_t febIndex, size_t sz);


  //======
  // Functions used by LArCompactSubsetChannelProxy.


  /**
   * @brief Return the number of time bins used in this subset.
   */
  size_t getChanSize() const;


  /**
   * @brief Return the number of time bins for channel index CHAN.
   * @param chan The channel index to query.
   *
   * This is either the same as getChanSize(), or 0 if this channel
   * is empty.
   */
  size_t getSize (unsigned int chan) const;


  /**
   * @brief Return the time offset for channel index CHAN.
   * @param chan The channel index to query.
   *
   * We currently assume that this is the same for all channels,
   * so CHAN is ignored.
   */
  float timeOffset (unsigned int chan) const;


  /**
   * @brief Return the time bin width for channel index CHAN.
   * @param chan The channel index to query.
   *
   * We currently assume that this is the same for all channels,
   * so CHAN is ignored.
   */
  float timeBinWidth (unsigned int chan) const;


  /**
   * @brief Retrieve wave data.
   * @param which The wave type to retrieve.
   * @param chan The index of the channel to retrieve.
   * @param tbin The time bin index to retrieve.
   */
  LArVectorProxy getData (unsigned int which, unsigned int chan, size_t tbin)
    const;


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
  void setTimings (unsigned int chan, float timeOffset, float timeBinWidth);


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
  void setData (unsigned int which,
                unsigned int chan,
                size_t tbin,
                size_t chanSize,
                const LArVectorProxy& from);
    


private:
  /// Number of time bins per channels.  0 before any waves have been added.
  unsigned int m_chanSize;

  /// Number of samples per wave.  0 before any waves have been added.
  unsigned int m_nSamples;

  /// Time parameters.  We assume that these are the same for all
  /// non-empty channels within a subset.
  float m_timeOffset;
  float m_timeBinWidth;

  /// Vector of stored FebIds.
  // The length of this gives the length of the subset `vector'.
  // Indexed by febIndex.
  std::vector<FebId> m_febids;

  /// Map from FEB indices to channel indices.
  // Channel index of the start of data for the corresponding FebId.
  // An extra entry at the end gives the total channel count.
  std::vector<unsigned int> m_indices;

  /// Map from channel indices to wave slots.
  // Indexed by channel and time bin numbers, chan*m_chanSize + tbin.
  // Gives the slot corresponding to this channel/time bin, or -1 if
  // it's empty.
  std::vector<int> m_slots;

  /// The wave data.
  // This is divided into slots, each containing nWaves * m_nSamples words.
  // (All wave types for one time bin for one channel.)
  std::vector<float> m_data;
};


#include "LArRawConditions/LArCompactSubset.icc"


#endif // not LARRAWCONDITIONS_LARCOMPACTSUBSET_H
