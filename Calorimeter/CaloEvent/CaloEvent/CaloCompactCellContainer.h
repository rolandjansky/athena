///////////////////////// -*- C++ -*- /////////////////////////////

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef CALOEVENT_CALOCOMPACTCELLCONTAINER_H
#define CALOEVENT_CALOCOMPACTCELLCONTAINER_H

/**
 * @class CaloCompactCellContainer
 * @version \$Id: CaloCompactCellContainer.h,v 1.7 2008-11-20 21:35:56 ssnyder Exp $
 * @author Sven Menke <menke@mppmu.mpg.de>
 * @date 03-August-2004
 * @brief container class for @c CaloCompactCell objects
 *
 * Container class to contain compactified @c CaloCell objects
 * from a complete @c CaloCellContainer. The completeness is
 * required in the sense that for every Calorimeter present
 * in the @c CaloCellContainer all possible cells have to be
 * filled. This allows for not storing the @c Identifier for
 * the cells. The compact cells (and instances of this class)
 * are produced by the @c CaloCompactCellTool in @c CaloTools.
 */

#include "CLIDSvc/CLASS_DEF.h"
#include "CaloEvent/CaloCompactCell.h"
#include <vector>

class CaloCompactCellContainer
{

 public:

  /** @brief value type for the internal data
   *
   *  The internal data format has a length of 32bit. Therefore this
   *  type must have the length of 32 bits.
   */
  typedef int	value_type;

 private:

  /**
   * @brief vector containing header and compact cell data.
   *
   * the first int in the vector must contain the header length
   * (in number of 32bit ints) including the first int
   */
  std::vector<value_type> m_compactData;
 public:
  /** @brief default Constructor.*/
  CaloCompactCellContainer()
  {
    m_compactData.resize(0);
  };

  /**
   * @brief Constructor.
   * @param theCompactData The vector holding the header and
   * @c CaloCompactCell data
   */
  CaloCompactCellContainer(const std::vector<value_type> & theCompactData) :
    m_compactData(theCompactData)
  { };

  virtual ~CaloCompactCellContainer() { };

  /**
   *  to speed things up ivukotic@cern.ch added this inline function
   *  to be used instead of the two bellow.
   *  It uses no vectors since it takes time to make them.
   *  nwords and a loop over them is removed since nwords is always=1
   *  as nBytesPerCell is = sizeof(CaloCompactCell::value_type)
   *  Its inlined and calculation is simplifed
   */
    void getCompactCellDataTime(const int & iCell,  CaloCompactCell::value_type & dataTime) const;


  /**
   * @brief returns the compactified @c CaloCell data
   * @param iCell the index of the cell in the @c CaloCompactCellContainer
   * @param nBytesPerCell the size of each @c CaloCompactCell in bytes
   * @return a @c CaloCompactCell
   *
   * returns the compactified @c CaloCell information in form of a
   * @c CaloCompactCell which is needed to restore the original
   * @c CaloCell object.
   */
  CaloCompactCell getCompactCell(const int & iCell,
				 const int & nBytesPerCell) const;

  /**
   * @brief returns the entire compact cell container (includes header)
   * @return a const reference to the vector holding the compact data
   */
  inline const std::vector<value_type> & getData() const {
    return m_compactData;
  };

  /**
   * @brief returns header portion of the compact cell container
   * @return a const vector holding the header data
   */
  const std::vector<value_type> getHeader() const;

  /**
   * @brief resize the data vector to totalSize (includes Header)
   * @param totalSize the total size in unites of @c value_type
   */
  void resize (const unsigned int & totalSize);

  /**
   * @brief replaces the current header (if there was one)
   * @param theHeader the new header
   *
   * used by the compactifier tool on creation of the container
   */
  void setHeader(const std::vector<value_type> & theHeader);

  /**
   * @brief store the compact cell with index iCell
   * @param theCompactCell the compact @c CaloCell data
   * @param iCell the index in the @c CaloCompactCellContainer
   * @param headerLength the space reserved for the header information
   */
  void setCompactCell(const CaloCompactCell & theCompactCell,
		      const int & iCell, const int &headerLength);
  /**
   *  to speed things up ivukotic@cern.ch added this inline function
   *  to be used instead of the one above.
   *  It uses no vectors since it takes time to make them.
   *  nwords and a loop over them is removed since nwords is always=1
   *  as nBytesPerCell is = sizeof(CaloCompactCell::value_type)
   *  Its inlined and calculation is simplifed
   */
  void setCompactCellDataTime(CaloCompactCell::value_type &theCompactCell, const int & iCell,const int & headerLength);


  /**
   * @brief Simple iterator-like object for reading from the container.
   *
   * This allows reading one @c CaloCompactCell::value_type object
   * at a time from the container.  There is only one method, @c next(),
   * which returns the next value (and advances the iterator).
   */
  class compact_input_iterator
  {
  public:
    /**
     * @brief Constructor.
     * @param it value_type iterator at the start of the data
     *           (after the header).
     */
    compact_input_iterator (std::vector<value_type>::const_iterator it);


    /**
     * @brief Return the current value and advance to the next one.
     */
    CaloCompactCell::value_type next();


    /**
     * @brief Return the underlying container iterator.
     */
    std::vector<value_type>::const_iterator base() const;


  private:
    /// The iterator in the underlying container type.
    std::vector<value_type>::const_iterator m_it;

    /// The current word.  This gets shifted to the right
    /// as we consume values from it.
    value_type m_word;

    /// Count; used to tell how many more times we need to shift @c m_word.
    int m_count;
  };


  /**
   * @brief Return a new @c compact_input_iterator pointing at the start
   * of the data.
   */
  compact_input_iterator compact_begin_input() const;

  /**
   * @brief Return a new @c compact_input_iterator pointing at position
   * given as the parameter BUT from the end of the data.
   */
  compact_input_iterator compact_begin_input_from (int) const;

  /**
   * @brief Simple iterator-like object for writing to the container.
   *
   * This allows writing one @c CaloCompactCell::value_type object
   * at a time to the container.  There is a @c set method which
   * writes one value to the container (and advances the iterator).
   */
  class compact_output_iterator
  {
  public:
    /**
     * @brief Default constructor.
     *
     * This makes an invalid iterator.
     */
    compact_output_iterator ();


    /**
     * @brief Constructor.
     * @param it value_type iterator at the start of the data
     *           (after the header).
     */
    compact_output_iterator (std::vector<value_type>::iterator it);


    /**
     * @brief Set the next value, and advance the iterator.
     * @param x The value to copy to the container.
     */
    void set (CaloCompactCell::value_type x);


    /**
     * @brief Advance the iterator.
     */
    compact_output_iterator& operator++ ();


    /**
     * @brief Return the number of underlying @c value_type words used.
     */
    int used () const;


  private:
    /// The iterator in the underlying container type.
    std::vector<value_type>::iterator m_it;

    /// Count; used to tell how many more times we need to shift @c m_word.
    int m_count;
  };


  /**
   * @brief Return a new @c compact_output_iterator pointing at the start
   *        of the data.
   * @param headerLength The length of the header in front of the data.
   */
  compact_output_iterator compact_begin_output(int headerLength);
};

inline  void CaloCompactCellContainer::getCompactCellDataTime(const int & iCell,  CaloCompactCell::value_type & dataTime) const{
  	static const int nRatio = sizeof(value_type)/sizeof(CaloCompactCell::value_type);
	int iPos = m_compactData[0] + (int)(iCell/nRatio);
  	int iMod = (iCell%nRatio)<<(8/nRatio);
  	dataTime=(CaloCompactCell::value_type) (((m_compactData[iPos])>>iMod) & CaloCompactCell::WORDMASK);
    }

inline void CaloCompactCellContainer::setCompactCellDataTime(CaloCompactCell::value_type &theCompactCell, const int & iCell,const int & headerLength) {

  static const int nRatio = sizeof(value_type)/sizeof(CaloCompactCell::value_type);

  // the compace data for one cell
  //CaloCompactCell::value_type cData(theCompactCell);
    int iPos = headerLength + (int)(iCell/nRatio);
    int iMod = (iCell%nRatio)<<(8/nRatio);
    if ( iMod == 0 ) m_compactData[iPos] = 0;
    m_compactData[iPos] = m_compactData[iPos] | (theCompactCell<<iMod);

}


inline
CaloCompactCellContainer::compact_input_iterator::compact_input_iterator
   (std::vector<value_type>::const_iterator it)
  : m_it (it),
    m_word(0),
    m_count (0)
{
}


inline
CaloCompactCell::value_type
CaloCompactCellContainer::compact_input_iterator::next()
{
  // Constants.
  const int mask = static_cast<CaloCompactCell::value_type>(-1);
  const int shift = 8*sizeof (CaloCompactCell::value_type);
  const int ratio =
    sizeof(value_type) / sizeof(CaloCompactCell::value_type);

  // Fetch the next @c value_type, if needed.
  if (m_count % ratio == 0)
    m_word = *m_it;

  // Mask off the current value.
  CaloCompactCell::value_type ret = m_word & mask;

  // Bump the count.
  if (((++m_count)%ratio) == 0) {
    // We wrapped to a new @c value_type.  Bump the underlying iterator.
    ++m_it;
  }
  else {
    // Same @c value_type --- shift the word over.
    m_word >>= shift;
  }

  return ret;
}


inline
std::vector<CaloCompactCellContainer::value_type>::const_iterator
CaloCompactCellContainer::compact_input_iterator::base() const
{
  return m_it;
}


inline
CaloCompactCellContainer::compact_input_iterator
CaloCompactCellContainer::compact_begin_input() const
{
  return compact_input_iterator (m_compactData.begin() + m_compactData[0]);
}

inline
CaloCompactCellContainer::compact_input_iterator
CaloCompactCellContainer::compact_begin_input_from(int pos) const{
	return compact_input_iterator (m_compactData.end() - pos);
}

inline
CaloCompactCellContainer::compact_output_iterator::compact_output_iterator ()
{
}


inline
CaloCompactCellContainer::compact_output_iterator::compact_output_iterator
    (std::vector<value_type>::iterator it)
  : m_it (it),
    m_count (0)
{
}

inline
void CaloCompactCellContainer::compact_output_iterator::set
  (CaloCompactCell::value_type x)
{
  // Constants.
  const int mask = static_cast<CaloCompactCell::value_type>(-1);
  const int ratio =
    sizeof(value_type) / sizeof(CaloCompactCell::value_type);
  const int shift = 8*sizeof (CaloCompactCell::value_type) * (m_count%ratio);

  // Mask the value into the current word.
  *m_it = (*m_it & ~(mask<<shift)) | (x << shift);

  // Advance the iterator.
  if (((++m_count)%ratio) == 0)
    ++m_it;
}


inline
CaloCompactCellContainer::compact_output_iterator&
CaloCompactCellContainer::compact_output_iterator::operator++ ()
{
  const int ratio =
    sizeof(value_type) / sizeof(CaloCompactCell::value_type);
  if (((++m_count)%ratio) == 0)
    ++m_it;
  return *this;
}


inline
int CaloCompactCellContainer::compact_output_iterator::used () const
{
  const int ratio =
    sizeof(value_type) / sizeof(CaloCompactCell::value_type);
  return (m_count + ratio - 1) / ratio;
}


inline
CaloCompactCellContainer::compact_output_iterator
CaloCompactCellContainer::compact_begin_output (int headerLength)
{
  return compact_output_iterator (m_compactData.begin() + headerLength);
}


CLASS_DEF(CaloCompactCellContainer, 1106685019, 1)

#endif
