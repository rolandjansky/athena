/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef CALOEVENT_CALOSUM_H
#define CALOEVENT_CALOSUM_H

#include "CaloEvent/CaloBinDescriptor.h"
#include "CaloGeoHelpers/CaloSampling.h"

#include <utility>
#include <vector>
#include <map>

class CaloCell;

class CaloSum
{
 public:

  /*! \brief Region indicator implementation */
  enum CaloRegion 
    {
      Central       = 0x01, /*!< Central region indicator */
      EndCap        = 0x02, /*!< Central region indicator */
      Forward       = 0x04, /*!< Forward region indicator */
      Em            = 0x08, /*!< Electromagnetic calorimeter indicator */
      Had           = 0x10, /*!< Hadronic calorimeter indicator */
      UnknownRegion = 0xff  /*!< Unknown region */
    };

  /*! \brief Binned variable type */
  enum CaloVarType
    {
      E           = 0x01,   /*!< Energy amplitude */
      EDensity    = 0x02,   /*!< Energy density */
      EFraction   = 0x04,   /*!< Energy fraction */
      Et          = 0x08,   /*!< Transverse energy amplitude */
      EtFraction  = 0x10,   /*!< Transverse energy fraction */      
      UnknownType = 0xff    /*!< Unknown variable type */
    };

  /*! \brief Key type for compressed storage */
  typedef std::pair<CaloRegion,CaloSampling::CaloSample>            key_t;

  /*! \brief Data type for parallel running sums in a bin */
  typedef std::vector<double>                                      data_t;
  /*! \brief Data iterator type */
  typedef data_t::iterator                                    data_iter_t;
  /*! \brief Data const iterator type */
  typedef data_t::const_iterator                        data_const_iter_t;

  /*! \brief Bin type */
  typedef std::vector<data_t>                                       bin_t;
  /*! \brief Bin iterator type */
  typedef bin_t::iterator                                      bin_iter_t;
  /*! \brief Bin const iterator type */
  typedef bin_t::const_iterator                          bin_const_iter_t;

  /*! \brief Store data type */
  typedef std::map<key_t,bin_t>                                   store_t;
  /*! \brief Store iterator type */
  typedef store_t::iterator                                  store_iter_t;
  /*! \brief Store const iterator type */
  typedef store_t::const_iterator                      store_const_iter_t;

  /*! \brief Range type */
  typedef std::pair<double,double>                                range_t;
  /*! \brief Range container type */
  typedef std::vector<range_t>                               range_cont_t;
  /*! \brief Range map type */
  typedef std::map<CaloRegion,range_cont_t>                   range_map_t;

  //
  ///////////////////////
  // Embedded Iterator //
  ///////////////////////
    //
    //  class CaloSumIterator
    //    {
    //    public:
    //      CaloSumIterator(store_t& theStore) 
    //	: m_store(&theStore)
    //	, m_actual(theStore.begin())
    //	{ };
    //
    //      ~CaloSumIterator() { };
    //
      // positioning iterator
    //      CaloSumIterator begin() { m_actual = m_store->begin(); return *this; }
    //      CaloSumIterator end()   { m_actual = m_store->end();   return *this; }
    //      CaloSumIterator next()  { m_actual++; return *this; }
    //      CaloSumIterator prev()  { m_actual--; return *this; }
    //
      // bi-directional iterator advancement
    //      CaloSumIterator operator++()    { return this->next(); }
    //      CaloSumIterator operator++(int) { return this->next(); }
    //      CaloSumIterator operator--()    { return this->prev(); }
    //      CaloSumIterator operator--(int) { return this->prev(); }
    //      
      // comparators
    //      bool operator==(const CaloSumIterator& anOtherIter) const
    //	{ return m_actual == anOtherIter.m_actual; }
    //      bool operator==(CaloSumIterator& anOtherIter)
    //	{ return m_actual == anOtherIter.m_actual; }
    //      bool operator!=(const CaloSumIterator& anOtherIter ) const
    //	{ return m_actual != anOtherIter.m_actual; }
    //      bool operator!=(CaloSumIterator& anOtherIter )
    //	{ return m_actual != anOtherIter.m_actual; }
    //
      // data access
    //      store_const_iter_t& operator*()  const { return m_actual; }
    //      store_const_iter_t& operator->() const { return m_actual; } 
    //
    //    private:
    //      
    //      const store_t*     m_store;
    //      store_const_iter_t m_actual;
    //    };

  /*! \brief Store iterator type */
    //  typedef CaloSumIterator   const_iterator;
  typedef store_const_iter_t        const_iterator;

  /*! \brief Default constructor
   *
   *  Usefulness limited, allows only default configuration, but has no
   *  bin descriptor, i.e. no sums!
   */
  CaloSum();

  /*! \brief Useful constructor
   *
   *  Accepts a variable type indicator, a bin descriptor, and the number
   *  of sums in a bin.
   *
   *  \param varType        variable type indicator
   *  \param theBins        bin descriptor
   *  \param numberOfSums   number of running sums (default 1 if omitted)
   *
   *  The constructor implements default range assignments to each region
   *
   *  <ul>
   *   <li>\p Central \f$ -0.8 < \eta < 0.8 \f$ </li>
   *   <li>\p EndCap \f$ -2.5 < \eta < -0.8 \f$ and \f$ 0.8 < \eta < 2.5 \f$
   *      </li>
   *   <li>\p Forward  \f$ -5.0 < \eta < -2.5 \f$ and \f$ 2.5 < \eta < 5.0 \f$
   *      </li>
   *  </ul>
   *
   *  This constructor implements only the regions defined above.       
   */
  CaloSum(CaloVarType                      varType,
	  const CaloBinDescriptor<double>& theBins,
	  size_t                           numberOfSums = 1);
//  /*! \brief Useful constructor
//   *
//   * \overload
//   *
//   * Constructor allows to give a list of regions.
//   */
  //  CaloSum(CaloVarType                      varType,
  //	  const CaloBinDescriptor<double>& theBins,
  //	  const std::vector<CaloRegion>&   theRegions,
  //	  size_t                           numberOfSums = 1 );
  /*! \brief Descructor*/
  virtual ~CaloSum();

  ///////////////////
  // Configuration //
  ///////////////////

  /*! \brief Configure regions 
   *
   *  Assigns one eta range to a region. This method overwrites the default 
   *  settings. 
   *
   *  \param theRegion    region indicator
   *  \param etaLow       lower eta limit
   *  \param etaHigh      upper eta limit
   */  
  virtual bool setRegion(const CaloSum::CaloRegion& theRegion,
			 double etaLow, double etaHigh);
  /*! \brief Configure regions
   *
   *  \overload
   *
   *  Assigns eta ranges to a region.
   *
   *  \param theRegion    region indicator
   *  \param etaLow       vector of lower eta limits
   *  \param etaHigh      vector of upper eta limits
   *
   *  It is expected that both vectors have the same dimension. Otherwise 
   *  ranges up to the smallest dimension will be configured.    
   */
  virtual bool setRegion(const CaloSum::CaloRegion& theRegion,
			 const std::vector<double>& etaLow,
			 const std::vector<double>& etaHigh);

  /*! \brief Retrieve region for a given eta 
   *
   *  Returns \p UnknownRegion if eta outside of all eta ranges. Returns 
   *  first region in which eta is found.
   *
   *  \param  theEta   eta on input
   *
   */
  virtual CaloRegion getRegion(double theEta) const;

  /*! \brief Returns eta range(s) for a given region.
   *
   *  The ranges are returned in vectors owned by the client. The vectors
   *  are not cleared before filling, i.e. ranges are added if vectors
   *  already contain data.
   *
   *  \param  theRegion   region on input
   *  \param  lowEtas     reference to vector for lower eta limits
   *  \param  highEtas    reference to vector for upper eta limits
   */
  virtual bool       getRange(const CaloRegion&    theRegion,
			      std::vector<double>& lowEtas,
			      std::vector<double>& highEtas) const;
  /*! \brief Returns binned variable type */
  virtual CaloVarType getVarType() const;
  /*! \brief Returns number of bins 
   *
   *  Return value 0 means incomplete configuration. 
   */
  virtual size_t getNumberOfBins() const;
  /*! \brief Returns number of sums in each bin 
   *
   *  Return value 0 means incomplete  configuration.
   */
  virtual size_t getNumberOfSums() const;
  /*! \brief Returs lower and upper limit of binned variable range
   *
   *   Returns \a false if configuration incorrect, especially if 
   *   \a lowLimit < \a highLimit.
   *
   *  \param lowLimit  reference to variable storing lower limit on return
   *  \param highLimit reference to variable storing upper limit on return  
   */ 
  virtual bool getVariableRange(double& lowLimit,double& highLimit) const;
  /*! \brief Returns bin edges for a given bin
   *
   *  Returns \a false if configuration incorrect or index invalid.
   *
   *  \param theIndex    bin index
   *  \param lowEdge     reference to variable containing lower bin limit on 
   *                     return
   *  \param highEdge    reference to variable containing upper bin limit on
   *                     return   
   */
  virtual bool getBinEdges(size_t theIndex,
			   double& lowEdge, double& highEdge) const;
  /*! \brief Returns bin center
   *
   *  Returns \a false if configuration incorrect or index invalid.
   *
   *  \param theIndex    bin index
   *  \param binCenter   center of bin
   */
  virtual bool getBinCenter(size_t theIndex, double& binCenter) const;
  /*! \brief Returns reference to complete bin descriptor */
  virtual const CaloBinDescriptor<double>& getBinDescriptor() const;
  

  ///////////////////////
  // Data Accumulation //
  ///////////////////////

  /*! \brief Add to sums
   *
   *  Returns \a false if problems adding (outside region range, outside binned
   *  variable range).
   *
   *  \param theCell            pointer to a \p CaloCell
   *  \param theBinnedVariable  binned variable
   *  \param theSummedVariables summed variables
   *
   *  In case of dimension mismatch, only the members of \a theSummedVariables
   *  fitting within the configured dimensions will be used.
   *
   *  If the cell is in a new region or sampling, a new key is generated and
   *  added to the internal store. No pre-setting of included (region,sampling)
   *  pairs (the keys) is necessary or possible. Unwanted combinations have to
   *  be avoided by the algorithms. 
   */
  virtual bool add(const CaloCell* theCell,
		   double theBinnedVariable,
		   const std::vector<double>& theSummedVariables);
  /*! \brief Add to sum.
   *
   * \overload
   *
   *  \param theCell            pointer to a \p CaloCell
   *  \param theBinnedVariable  binned variable
   *  \param theSummedVariable  summed variable
   */
  virtual bool add(const CaloCell* theCell,
		   double theBinnedVariable,
		   double theSummedVariable);
  /*! \brief Add to sums.
   * 
   *  \overload
   *
   *  \param  theSampling        sampling indicator
   *  \param  theEta             eta of \p CaloCell
   *  \param  theBinnedVariable  binned variable
   *  \param  theSummedVariables summed variables 
   *
   *   Uses explicit sampling indicator and eta instead of \p CaloCell
   *   reference.
   */
  virtual bool add(const CaloSampling::CaloSample& theSampling,
		   double theEta,
		   double theBinnedVariable,
		   const std::vector<double>& theSummedVariables);
  /*! \brief Add to sums.
   *
   *  \param  theSampling        sampling indicator
   *  \param  theEta             eta of \p CaloCell
   *  \param theBinnedVariable  binned variable
   *  \param theSummedVariable  summed variable
   */
  virtual bool add (const CaloSampling::CaloSample& theSampling,
		    double theEta,
		    double theBinnedVariable,
		    double theSummedVariable);
  /////////////////
  // Data Access //
  /////////////////

  /*! \brief Store begin iterator */
  const_iterator begin() const;
  /*! \brief Store end iterator */
  const_iterator end()   const;
  /*! \brief Returns region for a given iterator
   *
   *  \param theIterator   store iterator
   */
  CaloRegion   getRegion(const_iterator& theIterator)   const;
  /*! \brief Returns sampling for a given iterator 
   *
   *  \param theIterator   store iterator
   */
  CaloSampling::CaloSample getSampling(const_iterator& theIterator) const;

  /*! \brief Returns sums for a given iterator 
   *
   * Returns \a false if invalid iterator or index mismatch. Fills vector
   * on successful return without clearing, i.e. sums are added as new members
   * to the vector.
   *
   * \param  theIterator   store iterator
   * \param  theBinIndex   index of bin
   * \param  theSums       reference to vector with running sums
   */
  bool getSums(const_iterator& theIterator,
	       size_t theBinIndex,
	       std::vector<double>& theSums) const;
  /*! \brief Returns sums for a given binned variable
   *
   * \overload
   *
   * \param  theIterator        store iterator
   * \param  theBinnedVariable  bin variable value
   * \param  theSums            reference to vector with running sums
   */
  bool getSums(const_iterator& theIterator,
	       double theBinnedVariable,
	       std::vector<double>& theSums) const;
  /*! \brief Returns sums for a given key and index 
   *
   *  \overload
   *
   *  \param  theRegion   region indicator
   *  \param  theSampling sampling indicator
   *  \param  theIndex    bin index
   *  \param  theSums     reference to vector with running sums
   * 
   */
  bool getSums(const CaloSum::CaloRegion&      theRegion,
	       const CaloSampling::CaloSample& theSampling,
	       size_t                          theIndex,
	       std::vector<double>&            theSums) const;
  /*! \brief Returns sums for a given key and binned variable 
   *
   *  \overload
   *
   *  \param  theRegion            region indicator
   *  \param  theSampling          sampling indicator
   *  \param  theBinnedVariable    bin index
   *  \param  theSums              reference to vector with running sums
   * 
   */
  bool getSums(const CaloSum::CaloRegion&      theRegion,
	       const CaloSampling::CaloSample& theSampling,
	       double                          theBinnedVariable,
	       std::vector<double>&            theSums) const;
  /*! \brief Returns sums for a given sampling, eta, and index 
   *
   *  \overload
   *
   *  \param  theSampling sampling indicator
   *  \param  theEta      eta value
   *  \param  theIndex    bin index
   *  \param  theSums     reference to vector with running sums
   * 
   */
  //  bool getSums(const CaloSampling::CaloSample& theSampling,
  //	       double                          theEta,
  //	       size_t                          theIndex
  //	       std::vector<double>&            theSums) const;
  /*! \brief Returns sums for a given sampling, eta, and binned variable 
   *
   *  \overload
   *
   *  \param  theSampling        sampling indicator
   *  \param  theEta             eta value
   *  \param  theBinnedVariable  binned variable
   *  \param  theSums            reference to vector with running sums
   * 
   */
  //  bool getSums(const CaloSampling::CaloSample& theSampling,
  //	       double                          theEta,
  //	       double                          theBinnedVariable,
  //	       std::vector<double>&            theSums) const;

  /*! \brief Checks if region in store
   *
   *  Returns \a true if given region is in store.
   *
   *  \param theRegion  region indicator
   */
  virtual bool contains(const CaloSum::CaloRegion&      theRegion) const;
  /*! \brief Checks if sampling in store
   *
   *  \overload
   *
   *  Returns \a true if given sampling is in store.
   *
   *  \param theSampling  sampling indicator
   */ 
  virtual bool contains(const CaloSampling::CaloSample& theSampling) const;
  /*! \brief Checks if region/sampling combination is in store
   *
   *  \overload
   *
   *  Returns \a true if given region/sampling combination is in store.
   *
   *  \param theRegion    region indicator
   *  \param theSampling  sampling indicator
   */ 
  virtual bool contains(const CaloSum::CaloRegion&      theRegion,
			const CaloSampling::CaloSample& theSampling) const;

 protected:

  /*! \brief Translate into key 
   *
   *  \param  theRegion   region indicator
   *  \param  theSample   sampling indicator
   */
  key_t getKey(const CaloRegion&               theRegion,
	       const CaloSampling::CaloSample& theSample) const;
  /*! \brief Translate into key 
   *
   *  \overload
   *
   *  \param theEta       eta value
   *  \param theSample    sampling indicator
   */
  key_t getKey(double theEta, 
	       const CaloSampling::CaloSample& theSample) const;

  /*! \brief Set up defaults */
  bool  setUp(); 

 private:

  /*! \brief Process flow control: indicates default range overwrite */
  bool                      m_defaultRanges;

  /*! \brief Store size: number of sums */
  size_t                    m_numberOfSums;

  /*! \brief Binned variable type */
  CaloVarType               m_varType;

  /*! \brief Bin descriptor */
  CaloBinDescriptor<double> m_binDescriptor;

  /*! \brief Store for region ranges */
  range_map_t               m_regionRanges;

  /*! \brief Store for sums */
  store_t                   m_sumStore;
};

/*! \class CaloSum
 *
 * \brief Container for binned sums of calorimeter variables
 *
 * \author  Peter Loch <loch@physics.arizona.edu>
 * \date    Feb.8, 2005
 * \version 0 
 *
 * Simple data container for indexed running sums. Presently three indices 
 * are supported:
 *
 * <ol>
 *  <li><tt>CaloRegion</tt> indicates a calorimeter region.</li>
 *  <li><tt>CaloSample</tt> indicates a calorimeter sampling.</li>
 *  <li><tt>BinIndex</tt>   indicates a variable bin index</li>
 * </ol>
 *
 * It is possible to run several sums in a given bin. The binned variable can
 * be different from the summed variable(s).
 * 
 * <tt>CaloRegion</tt>s can be configured by the client. 
 * Three named regions are
 * available, indentified by enumerators with default settings:
 *
 * <ol>
 *  <li><tt>Central</tt>  central region \f$ |\eta| < 0.8\f$ ;</li>
 *  <li><tt>EndCap</tt>   endcap region  \f$ 0.8 < |\eta| < 2.5 \f$ ;</li>
 *  <li><tt>Forward</tt>  forward region \f$ |\eta| > 2.5 \f$ ;</li>
 *  <li><tt>EM</tt>       electromagnetic calorimeter only </li>
 *  <li><tt>HAD</tt>      hadronic calorimeter only </li>
 * </ol> 
 *
 * More than one \f$ \eta \f$ range can be assigned for each region, thus
 * allowing categorizing these regions as non-contiguous entities. Also,
 * the regions can overlap. In this case the to-be-summed quantities are
 * added to all sums in overlapping regions.
 *
 * \warning 
 * Overlapping regions not yet correctly implemented!
 *
 * Other configuration parameters are the type of the binned variable and the
 * number of sums in each bin.
 *
 * Code example:
 *
 * \code
 *      // get a bin descriptor with equidistant bins
 *      CaloBinDescriptor<double> theBins(100,0.,100000.);
 *      // instantiate for energy density binning
 *      CaloSum theSums(CaloSums::EDensity,theBins);
 * \endcode
 *
 *
 * \todo
 * Implement an option which allows definition of regions by sampling, i.e. 
 * no \f$ \eta \f$ cuts.
 * 
 */
#endif
