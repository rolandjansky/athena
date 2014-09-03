/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/


#include "CaloEvent/CaloSamplingData.h"

#include <iterator>
#include <algorithm>

/*! 
 *  Generates an empty but complete store by default (all values are indexed
 *  and defaulted, i.e. = 0.).
 */
CaloSamplingData::CaloSamplingData()
  : m_varTypePattern(0)
{
  this->setupStores();
}

CaloSamplingData::CaloSamplingData(unsigned int varTypePattern)
  : m_varTypePattern(CaloVariableType::setConstMarker(varTypePattern))
{
  this->setupStores();
}

CaloSamplingData::CaloSamplingData(const CaloSamplingData& rData)
  : m_dataStore (rData.m_dataStore),
    m_varTypePattern (rData.m_varTypePattern)
{
}

CaloSamplingData::CaloSamplingData(const CaloSamplingData* pData)
  : m_dataStore (pData->m_dataStore),
    m_varTypePattern (pData->m_varTypePattern)
{
}

CaloSamplingData& CaloSamplingData::operator= (const CaloSamplingData& other)
{
  if (this != &other) {
    m_dataStore = other.m_dataStore;
    m_varTypePattern = other.m_varTypePattern;
  }
  return *this;
}

CaloSamplingData::~CaloSamplingData()
{ }


void CaloSamplingData::swap (CaloSamplingData& other)
{
  m_dataStore.swap (other.m_dataStore);
  std::swap (m_varTypePattern, other.m_varTypePattern);
}


/*! 
 * \overload
 *
 * \param theVariable   reference to variable key, non-modifiable
 * \param theData       vector of data words
 *
 * Retrieves all data for a given variable type into a vector. This vector is
 * referenced on input, and is cleared in any case. Returns <tt>true</tt> if
 * vector is not empty on output.
 */
bool
CaloSamplingData::retrieveData(variable_key_type theVariable,
			       vector_type& theData) const
{
  variable_store_type::const_iterator beg =
    m_dataStore.begin() + index (theVariable, 0);
  theData.assign (beg, beg + fastNsamp());
  return !theData.empty();
}

/*! 
 * \overload
 *
 * \param theVariable reference to a non-modifiable variable type indicator
 * \param theData     vector of data words
 *
 * Returns <tt>true</tt> only if input data vector is empty.
 * Note that the vector index is translated into a calo sampling indicator.
 * Only up to \a theData.size() words are overwritten in the store. If this
 * number is smaller than the store size, the original data beyond the
 * corresponding index is not changed.
 * 
 * If \a theData.size() is larger than the store size, only the first 
 * store size words are copied from \a theData. If \a theData.size() is 0,
 * the store is unchanged.
 */
bool CaloSamplingData::storeData(variable_key_type theVariable,
				 const vector_type& theData)
{
  return storeData (theVariable, theData.begin(), theData.end());
}

/*!
 * \param theVariable reference to non-modifiable variable type indicator
 * \param theSampling reference to non-modifiable calo sampling indicator
 * \param theData     reference to non-modifiable data word 
 *
 * Returns <tt>true</tt> if data is succesfully stored.
 * Note that previously stored data with the same indices are overwritten
 * without warning.
 */
bool CaloSamplingData::storeData(variable_key_type theVariable,
				 sampling_key_type theSampling,
				 value_type        theData)
{
  bool testWrite = this->checkBitPattern(theVariable);
  if ( testWrite ) 
    { 
      m_dataStore[index(theVariable,theSampling)] = theData;
    }
  return testWrite;
}

/*!
 * \overload
 *
 * \param theVariable  reference to non-modifiable variable type
 *
 * This is a run time calculation. It returns the number of samplings with
 * data content for a given variable type. Note the difference to the
 * \a getNumberOfSamplings() method.
 */
size_t 
CaloSamplingData::getNumberOfSamplings(variable_key_type theVariable) const
{
  variable_store_type::const_iterator beg =
    m_dataStore.begin() + index (theVariable, 0);
  return size_t(std::count_if(beg, beg + fastNsamp(),
			      IsValidData())); 
}

bool
CaloSamplingData::setupStores()
{
  //size_t samplingSize(this->getNumberOfSamplings());
  size_t variableSize(this->getNumberOfVariableTypes());
  // preset store with default values (empty variable lists)
  m_dataStore.resize(index(variableSize, 0));
  return !m_dataStore.empty();
}

bool 
CaloSamplingData::clearBitPattern(variable_key_type theVariable)
{
  unsigned int extractBit = this->getBitPattern(theVariable); 
  m_varTypePattern = m_varTypePattern & ~extractBit ;
  return ( m_varTypePattern & extractBit ) == 0;
}

bool 
CaloSamplingData::clearBitPattern()
{
  bool testConst = this->testConstPattern();
  m_varTypePattern = m_varTypePattern & ~m_varTypePattern;
  if ( testConst ) 
    m_varTypePattern = CaloVariableType::setConstMarker(m_varTypePattern);
  return true;
}

bool
CaloSamplingData::checkBitPattern(variable_key_type theVariable)
{
  if ( this->testConstPattern() ) return this->testBitPattern(theVariable);
  m_varTypePattern = m_varTypePattern | this->getBitPattern(theVariable);
  return true;
}
