/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef CALOEVENT_CALOBINDESCRIPTOR_H
#define CALOEVENT_CALOBINDESCRIPTOR_H

#include <vector>

template<typename T>
class CaloBinDescriptor
{
 public:

  /*! \brief Default constructor generates empty bin descriptor */  
  CaloBinDescriptor() { m_bins.clear(); };
  /*! \brief Explicit copy constructor */
  explicit CaloBinDescriptor(const CaloBinDescriptor& theBins);
  /// Assignment.
  CaloBinDescriptor& operator=(const CaloBinDescriptor& theBins);
  /*! \brief Construct with variable sized bins
   *
   *  \param theBins  reference to vector of bin boundaries with n+1 
   *                  entries for n bins;
   *
   *  \note There is no consistency check performed on the bin delimiters.
   *        Any invalid sequence of delimiters, i.e. not ordered increasing
   *        in value, will lead to unexpected index mapping. 
   */
  CaloBinDescriptor(const std::vector<T>& theBins);
  /*! \brief Construct with regular bins 
   *
   *  \param nBins       number of bins
   *  \param lowerLimit  lower limit of value range
   *  \param upperLimit  upper limit of value range
   * 
   *  \note Invalid input (\a nbins = 0, \a lowerLimit >= \a upperLimit) will
   *        lead to empty (unusable) bin descriptor.
   *
   *  The input parameters are internally converted into a vector of lower
   *  bin boundaries (first n entries) and the upper limit of the value
   *  range (n+1 entry).
   */
  CaloBinDescriptor(size_t nBins, const T& lowerLimit, const T& upperLimit);
  /*! \brief Default constructor */
  ~CaloBinDescriptor() { }; 

  /*! \brief Retrieve the number of bins 
   *
   *  Returns 0 for empty bin descriptor.
   */
  size_t getNumberOfBins() const;
  /*! \brief Retrieve the lower value range boundary */
  T lowerBound()           const;
  /*! \brief Retrieve the upper value range boundary */
  T upperBound()           const;

  /*! \brief Retrieve the lower bin edge of a given bin 
   *
   *  \param theIndex  bin index \f$ i \in [1,n] \f$ for \f$ n \f$ bins.
   *
   *  Returns 0 if \a theIndex out of range.
   *
   *  \warning 0 is a valid return as well. 
   */
  T lowerBinEdge(size_t theIndex) const;
  /*! \brief Retrieve the upper bin edge of a given bin 
   *
   *  \param theIndex  bin index \f$ i \in [1,n] \f$ for \f$ n \f$ bins.
   *
   *  Returns 0 if \a theIndex out of range.
   *
   * \warning 0 is a valid return as well.  
   */
  T upperBinEdge(size_t theIndex) const;
  /*! \brief Retrieve the bin width of a given bin 
   *
   *  \param theIndex  bin index \f$ i \in [1,n] \f$ for \f$ n \f$ bins.
   *
   *  Returns 0 if \a theIndex out of range. A valid return is positive
   *  definit.
   *
   */
  T binWidth(size_t theIndex)     const;

  /*! \brief Direct Access to the vector describing the binning */
  const std::vector<T>& getBinning() const;

  /*! \brief Get the index for a iven value
   *
   *  \param theData input value to be tested
   *
   *  Returns 0 if \a theData is smaller than the lower value range boundary,
   *  and \a outOfRangeMarker if \a theData is bigger than the upper
   *  value range boundary.
   */
  size_t getIndex(const T& theData) const;
  
  /*! \brief Tests if given index is out of valid range
   *
   *  \param theIndex  input index to be tested
   *
   *  Returns <tt>true</tt> if \a theIndex within range. 
   */
  bool   outOfRange(size_t theIndex) const;
  /*! \brief Returns the value of the out-of-range marker */
  size_t outOfRangeMarker()          const;

 private:

  /*! \brief Bin store
   *
   *  n+1 entries for n bins. 
   */
  std::vector<T> m_bins;

  /*! \brief Out-of-range marker (convention) */
  static size_t m_outOfRange;  
};

template<typename T>
CaloBinDescriptor<T>::CaloBinDescriptor(const CaloBinDescriptor& theBins)
  : m_bins (theBins.m_bins)
{
}

template<typename T>
CaloBinDescriptor<T>&
CaloBinDescriptor<T>::operator=(const CaloBinDescriptor& theBins)
{
  if (this != &theBins)
    m_bins = theBins.m_bins;
  return *this;
}

template<typename T>
size_t CaloBinDescriptor<T>::m_outOfRange = size_t(-1);

template<typename T>
CaloBinDescriptor<T>::CaloBinDescriptor(const std::vector<T>& theBins)
  : m_bins (theBins)
{
}

template<typename T>
CaloBinDescriptor<T>::CaloBinDescriptor(size_t nBins, const T& lowerEdge,
					const T& upperEdge)
{
  if ( lowerEdge < upperEdge && nBins > 0 )
    {
      double fBin =lowerEdge;
      double dBin = ( upperEdge - lowerEdge ) / (double)nBins;
      while ( fBin <= ( upperEdge + dBin/2. ) )
	{
	  m_bins.push_back(fBin);
	  fBin += dBin;
	}
    }
  else
    {
      m_bins.clear();
    }
}

template<typename T>
inline size_t CaloBinDescriptor<T>::getNumberOfBins() const
{
  return m_bins.size()-1;
}

template<typename T>
inline T CaloBinDescriptor<T>::lowerBound() const
{
  return m_bins.size() > 0 ? *(m_bins.begin()) : 0.;
}

template<typename T>
inline T CaloBinDescriptor<T>::upperBound() const
{
  return m_bins.size() > 0 ? *(--(m_bins.end())) : 0.;
}

template<typename T>
inline T CaloBinDescriptor<T>::lowerBinEdge(size_t theIndex) const
{
  return theIndex < m_bins.size()-2 ? m_bins[theIndex] : 0.;
}

template<typename T>
inline T CaloBinDescriptor<T>::upperBinEdge(size_t theIndex) const
{
  return theIndex < m_bins.size()-1 ? m_bins[theIndex+1] : 0;
}

template<typename T>
inline T CaloBinDescriptor<T>::binWidth(size_t theIndex) const
{
  return theIndex < m_bins.size()-2 
    ? m_bins[theIndex+1] - m_bins[theIndex]
    : 0.;
}

template<typename T>
inline const std::vector<T>& CaloBinDescriptor<T>::getBinning() const
{
  return m_bins;
}

template<typename T>
size_t CaloBinDescriptor<T>::getIndex(const T& theData) const
{
  size_t theIndex = 0;
  while ( theData > m_bins[theIndex] && theIndex < this->getNumberOfBins() )
    {
      theIndex++;
    }
  return theIndex < this->getNumberOfBins() ? theIndex : m_outOfRange;
}

template<typename T>
inline bool CaloBinDescriptor<T>::outOfRange(size_t theIndex) const
{
  return theIndex != m_outOfRange
    ? theIndex < this->getNumberOfBins()
    : true;
}

template<typename T>
inline size_t CaloBinDescriptor<T>::outOfRangeMarker() const
{
  return m_outOfRange;
}

/*! \class CaloBinDescriptor
 *
 *  \brief Data object containing bin descriptions
 *
 *  \author  Peter Loch <loch@physics.arizona.edu>
 *  \date    April 1, 2005
 *  \version 0
 *
 *  This data object stores the description of bins for any data type
 *  \a T with support for simple arithmatic 
 *  (operators <tt>+</tt>,<tt>-</tt>,<tt>*</tt>,<tt>/</tt>)
 *  and valid comparators
 *  (operators <tt>></tt>,<tt>=></tt>,<tt><</tt>,<tt><=</tt>). The bins
 *  can be equidistant or variable.
 */ 
#endif
