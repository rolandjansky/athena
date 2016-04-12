///////////////////////// -*- C++ -*- /////////////////////////////

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef CALOEVENT_CALOSAMPLINGDATA_H
#define CALOEVENT_CALOSAMPLINGDATA_H

#include "CaloGeoHelpers/CaloSampling.h"
#include "CaloEvent/CaloVariableType.h" 
#include "CxxUtils/copy_bounded.h"

#include <vector>
#include <algorithm>

class CaloSamplingData
{
 public: 

  /*! \brief Sampling look-up key type */ 
  typedef CaloSampling::CaloSample                sampling_key_type;
  /*! \brief Variable look-up type */ 
  typedef CaloVariableType::VariableType          variable_key_type;

  /*! \brief General data type */ 
  typedef double                                  value_type;    
  /*! \brief Sampling look-up table type, internal use only */
  typedef std::vector<value_type>                 variable_store_type;

  /*! \brief Vector type for data exchange (read only) */
  typedef std::vector<value_type>                 const_vector_type;
  /*! \brief Vector type for data exchange (read/write) */
  typedef std::vector<value_type>                 vector_type;

  /*! \brief Constructor */
  CaloSamplingData();
  /*! \brief Alternative constructor with variable type pattern */
  CaloSamplingData(unsigned int varTypePattern);
  /*! \brief Copy constructor */
  explicit CaloSamplingData(const CaloSamplingData& rData);
  explicit CaloSamplingData(const CaloSamplingData* pData);
  CaloSamplingData& operator= (const CaloSamplingData&);
  /*! \brief Destructor */
  virtual ~CaloSamplingData();

  void swap (CaloSamplingData& other);

  /*! \brief Set variable type pattern for this store */
////  bool setVarTypePattern(const unsigned int& thePattern);

  /*! \brief Return number of samplings stored */
  size_t getNumberOfSamplings() const;
  /*! \brief Return number of samplings stored for a given variable type */
  size_t getNumberOfSamplings(variable_key_type theVariable) const; 

  /*! \brief Return number of variable types */
  size_t getNumberOfVariableTypes() const;

  /*! \brief Retrieve const reference to individual variable */
  value_type retrieveData(variable_key_type theVariable,
                          sampling_key_type theSampling) const;
  /*! \brief Retrieve non-cont reference to individual variable */
  value_type& retrieveData(variable_key_type theVariable,
			   sampling_key_type theSampling);
  /*! \brief Retrieve set of variables into a vector */
  bool retrieveData(variable_key_type theVariable,
		    vector_type& theData) const;

  /*! \brief Remove a certain variable */
  bool removeVariable(variable_key_type theVariable, 
		      sampling_key_type theSampling);
  /*! \brief Remove all variables of the same type */
  bool removeVariable(variable_key_type theVariable);
  /*! \brief Remove all variables from a certain sampling */
  bool removeVariable(sampling_key_type theSampling);
  /*{! \brief Clear the store */
  bool removeVariable(); 
  bool clearStore();
  /*!}*/
  
  /*! \brief Store individual data word */ 
  bool storeData(variable_key_type theVariable,
		 sampling_key_type theSampling,
		 value_type theData);
  /*! \brief Store vector of data */
  bool storeData(variable_key_type theVariable,
		 const vector_type& theData);
  /*! \brief Store range of data */
  template <class Iterator>
  bool storeData(variable_key_type theVariable, Iterator beg, Iterator end);

  /*! \brief Check on available variable types */
  bool contains(variable_key_type theVariable) const;

 protected: 

  class IsValidData
  {
  public:    
    bool operator()(value_type theData)
    { return theData != value_type(); }
  };

 private:
  friend class CaloClusterContainerCnv_p4;
  friend class CaloClusterContainerCnv_p5;
  friend class CaloClusterContainerCnv_p6;
  friend class CaloClusterContainerCnv_p7;
  friend class CaloClusterContainerCnvTest_p6;
  friend class CaloClusterContainerCnvTest_p7;

  variable_store_type m_dataStore;

  unsigned int  m_varTypePattern;

  bool setupStores();

  unsigned int getBitPattern(variable_key_type theVariable) const;

  bool checkBitPattern(variable_key_type theVariable);
  bool testBitPattern(variable_key_type theVariable) const;
  bool clearBitPattern(variable_key_type theVariable);
  bool clearBitPattern();
  bool testConstPattern() const;

  size_t index (size_t theVariable, size_t theSampling) const;
  size_t fastNsamp() const;
}; 
/*! \class CaloSamplingData 
 *  \brief Simple data object to store all variables in calorimeter samplings
 *
 *  Simple data object to store all variables in calorimeter samplings. The
 *  underlying store is compressed in that only variables indexed by the
 *  sampling indicator and the variable type are actually stored. The store
 *  is dynamic as it adds the appropriate data words for a given samping and
 *  variable type, if those are not existing yet. Already existing data words
 *  are replaced (overwritten). Note that whenever an interface involving
 *  a vector of data words is used to set the variables, the store for the
 *  given variable type is completely cleared before insertion of new data
 *  words from the vector (all 0's are ignored in this process, and the 
 *  vector index is translated into a sampling indicator).   
 *
 *  This was orginally a vector<vector<double> >.  But it was always
 *  strictly a rectangular array, so it was rewritten to store the data
 *  as a flat vector to avoid multiple memory allocations.
 */


/*! \class CaloSamplingData::IsValidData 
 *  \brief Functor to check if data is valid (non-zero)
 */

/*! \class CaloSamplingData::AddValidData
 *  \brief Functor to add Valid data 
 */

/*! \class CaloSamplingData::IsValidStore
 *  \brief Functor to check if Store is valid
 */

/*! \class CaloSamplingData::RemoveData
 *  \brief Functor to remove a sampling from the Store
 */

/*! \class CaloSamplingData::RemoveSampling
 *  \brief Functor to remove all sampling in a store
 */


///////////////////////////////////////////////////////////////////////////////

///////////////////////////
// Retrieve Data Helpers //
///////////////////////////

/*!
 *
 *  Returns the maximum number of possible calo samplings, which also delimits
 *  the numerical range for the calo sampling indicator to 
 *  <tt>[0,getNumberOfSamples()-1]</tt>. Useful for sizing the data vectors
 *  holding sampling variables.
 */
inline size_t CaloSamplingData::getNumberOfSamplings() const
{ return size_t(CaloSampling::getNumberOfSamplings()); }

inline size_t CaloSamplingData::getNumberOfVariableTypes() const
{ return size_t(CaloVariableType::getNumberOfVariables()); }

/*!
 *
 * \param theVariable   reference to variable key, non-modifiable
 * \param theSampling   reference to sampling key, non-modifiable
 *
 * Retrieval of a modifiable indexed data word.
 *
 * The enumerator ranges prevent illegal data access out of the valid
 * index space. The internal data matrix is dimensioned as 
 * ([valid_number_of_variables]+1) * ([valid_number_of_samplings]+1). This
 * allows valid response to the \a CaloSampling::Unknown and 
 * \a CaloVariableType::NO_OF_TYPES enumerators. 
 */
inline CaloSamplingData::value_type&
CaloSamplingData::retrieveData(variable_key_type theVariable,
			       sampling_key_type theSampling)
{ 
  m_varTypePattern = m_varTypePattern | this->getBitPattern(theVariable);
  return m_dataStore[index(theVariable, theSampling)];
}

/*!
 *
 * \overload
 *
 * \param theVariable   reference to variable key, non-modifiable
 * \param theSampling   reference to sampling key, non-modifiable
 *
 * \overload
 *
 * Retrieval of a non-modifiable indexed data word.
 */
inline CaloSamplingData::value_type
CaloSamplingData::retrieveData(variable_key_type theVariable,
			       sampling_key_type theSampling) const
{
  return m_dataStore[index(theVariable, theSampling)];
}

/////////////////////////////////
// Store And Erase Data Helper //
/////////////////////////////////


/*!{
 * \param theVariable reference to a non-modifiable variable type
 * \param theSampling reference to a non-modifiable sampling indicator
 * 
 * Removing a variable in this implementation means to set it to the
 * default value generated by its constructor, e.g. \a double() generates
 * 0. The data word is left in store. This is an artifact of the underlying
 * storage model, which cannot use maps. Several interfaces are available
 * for data removal, starting from the removal of individual data words to
 * the removal of all words of a given variable type or sampling. 
 */
inline bool
CaloSamplingData::removeVariable(variable_key_type theVariable,
				 sampling_key_type theSampling)
{
  m_dataStore[index(theVariable,theSampling)] = value_type();
  return true;
}
inline bool
CaloSamplingData::removeVariable(sampling_key_type theSampling)
{
  variable_store_type::iterator beg =
    m_dataStore.begin() + index (0, theSampling);
  variable_store_type::iterator end = m_dataStore.end();
  int nsamp = fastNsamp();
  for (; beg < end; beg += nsamp)
    *beg = value_type();
  return true;
}
inline bool
CaloSamplingData::removeVariable(variable_key_type theVariable)
{
  size_t idx = index (theVariable, 0);
  std::fill_n (m_dataStore.begin() + idx, fastNsamp(), value_type());
  return true;
}
inline bool
CaloSamplingData::removeVariable()
{
  std::fill (m_dataStore.begin(), m_dataStore.end(), value_type());
  return this->clearBitPattern();
}
inline bool CaloSamplingData::clearStore() { return this->removeVariable(); }
/*!}*/

inline bool
CaloSamplingData::contains(variable_key_type theVariable) const
{ return this->testBitPattern(theVariable); }

inline unsigned int
CaloSamplingData::getBitPattern(variable_key_type theVariable) const
{ return CaloVariableType::getVariableBit(theVariable); }

inline bool
CaloSamplingData::testBitPattern(variable_key_type theVariable) const
{
  unsigned int testBit(this->getBitPattern(theVariable));
  if (testBit == 0) return false;
  return ( m_varTypePattern & testBit ) == testBit;
}

inline bool CaloSamplingData::testConstPattern() const
{ return CaloVariableType::checkConst(m_varTypePattern); }

inline
size_t CaloSamplingData::index (size_t theVariable, size_t theSampling) const
{
  return theVariable * fastNsamp() + theSampling;
}

inline
size_t CaloSamplingData::fastNsamp() const
{
  return (size_t)(CaloSampling::Unknown);
}


template <class Iterator>
bool CaloSamplingData::storeData (variable_key_type theVariable,
                                  Iterator beg, Iterator end)
{
  //Check const bit and set m_varTypePattern 
  if (!this->checkBitPattern (theVariable))
    return false;
  if (beg == end)
    return false;
  variable_store_type::iterator output =
    m_dataStore.begin() + index(theVariable, 0);
  CxxUtils::copy_bounded (beg, end, output, output + fastNsamp());
  return true;
  
}

#endif
