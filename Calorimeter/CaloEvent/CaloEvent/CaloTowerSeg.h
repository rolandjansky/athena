// This file's extension implies that it's C, but it's really -*- C++ -*-.

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef CALOEVENT_CALOTOWERSEG_H
#define CALOEVENT_CALOTOWERSEG_H
/********************************************************************

NAME:     CaloTowerSeg
PACKAGE:  offline/Calorimeter/CaloRec

AUTHORS:  S. Rajagopalan 
CREATED:  Oct, 2000
          Originally it is LArTowerSeg in LArClusterRec
	  Moved to CaloRec on Feb 5, 2001  by H.Ma 

PURPOSE:  
	  Store segmentation for the tower configuration.
          Constructor takes segmentation parameters. 
	  Public methods for accessing the limits, and tower sizes,
	  and check bounds. 
	  
MODIFIED: indices now unsigned integers; ranges [1,size]

********************************************************************/

// INCLUDE HEADER FILES:

#include <cmath>
#include <cstdlib>

#include "CaloGeoHelpers/CaloPhiRange.h"

class CaloTowerSeg 
{
  public:

  /// Type for eta, phi indices.
  typedef size_t index_t;

  /// Used to flag out-of-range indices.
  static const index_t outOfRange = static_cast<index_t> (-1);


  /*! \brief Default constructor 
   *
   *  Instantiates useless empty tower grid.
   */
  CaloTowerSeg():
    m_neta(0),m_nphi(0),
    m_etamin(0.),m_etamax(0.),
    m_phimin(0.),m_phimax(0.), 
    m_deta(0),m_dphi(0)
  {} 

  /*! \brief Constructor
   *
   *  \param neta       input number of \f$ \eta \f$ bins
   *  \param nphi       input number of \f$ \varphi \f$ bins
   *  \param etamin     input lower boundary \f$ \eta \f$ value range 
   *  \param etamax     input upper boundary \f$ \eta \f$ value range 
   *  \param phimin     input lower boundary \f$ \varphi \f$ value range
   *  \param phimax     input upper boundary \f$ \varphi \f$ value range
   *
   *  \f$ \varphi \f$ by default covers the full circle. 
   *  \a CaloPhiRange::phi_min() and \a CaloPhiRange::phi_max() are used
   *  to implement the correct \f$ \varphi \f$ range convention.
   *
   *  May have phimin > phimax for the case of wraparound.
   * 
   *  Only regular (equidistant) bins are supported in both directions.
   */
  CaloTowerSeg(index_t neta, index_t nphi, double etamin, double etamax,
	       double phimin=s_range.phi_min(), 
	       double phimax=s_range.phi_max()):
    m_neta(neta),m_nphi(nphi),
    m_etamin(etamin),m_etamax(etamax),
    m_phimin(phimin),m_phimax(phimax) 
  {
    m_deta = (etamax-etamin)/neta;
    m_dphi = (phimax > phimin ? phimax-phimin : s_range.diff (phimax, phimin))/nphi;
  }; 

  /*! \brief Destructor */
  ~CaloTowerSeg ()
    { 
    
    };

  // Use implicit copy ctor and assignment.
	   
 
  /*! \brief Retrieve number of \f$ \eta \f$ bins */
  index_t neta() const;
  /*! \brief Retrieve number of \f$ \varphi \f$ bins */
  index_t nphi() const;
  /*! \brief Retrieve bin size \f$ \Delta \eta \f$ */
  double deta() const;
  /*! \brief Retrieve bin size \f$ \Delta \varphi \f$ */
  double dphi() const;
  /*! \brief Retrieve lower boundary \f$ \eta \f$ value range */
  double etamin() const; 
  /*! \brief Retrieve upper boundary \f$ \eta \f$ value range */
  double etamax() const; 
  /*! \brief Retrieve lower boundary \f$ \varphi \f$ value range */
  double phimin() const; 
  /*! \brief Retrieve upper boundary \f$ \varphi \f$ value range */
  double phimax() const; 

  /*! \brief Check index range in \f$ \eta \f$ and \f$ \varphi \f$
   *
   *  \param etaInd   input \f$ \eta \f$ index
   *  \param phiInd   input \f$ \varphi \f$ index
   *
   *  Returns <tt>true</tt> when both \a eta and \a phi are within their
   *  respective allowed index ranges.
   */
  bool  inbound(index_t etaInd,index_t phiInd) const;
  /*! \brief Check value range in \f$ \eta \f$ and \f$ \varphi \f$ 
   *
   *  \overload
   *   
   *  \param etaVal    input \f$ \eta \f$ coordinate
   *  \param phiVal    input \f$ \varphi \f$ coordinate
   *  
   *  Returns <tt>true</tt> when both \a eta and \a phi are inside their
   *  respective allowed value ranges. 
   */ 
  bool  inbound(double etaVal,double phiVal) const; 

  /*! \brief Returns combined continous index from \f$ \eta \f$, 
   *          \f$ \varphi \f$ indices
   *
   *  \param etaInd    input \f$ \eta \f$ index
   *  \param phiInd    input \f$ \varphi \f$ index
   *
   *  Calculates a common running index from \a \f$\eta\f$ and \f$\phi\f$ . Returns 
   *  \a size_t(-1) if input outside of allowed index ranges.
   */
  index_t etaphi(index_t etaInd,index_t phiInd) const;  

  /*! \brief Returns \f$ \eta \f$ value for a given \f$ \eta \f$ index
   *
   *  \param etaInd   input \f$ \eta \f$ index
   *
   *  Can return \f$ \eta \f$ value outside of allowed value range if
   *  index out of range (no internal check).
   */
  double eta(index_t etaInd) const;    
  /*! \brief Returns \f$ \eta \f$ index for a given \f$ \eta \f$ value
   *
   *  \param etaVal   input \f$ \eta \f$ value
   *
   *  Returns \a size_t(-1) in case of \f$ \eta \f$ being out of allowed
   *  value range.
   */
  index_t etaIndex(double etaVal) const;
  /*! \brief Returns \f$ \eta \f$ index for a given combined index
   *
   *  \overload
   *
   *  \param comInd    input combined index
   *
   *  Can return an invalid \f$ \eta \f$ index if \a comInd is invalid.
   */
  index_t etaIndex(index_t comInd) const;

  /*! \brief Returns \f$ \varphi \f$ value for a given \f$ \varphi \f$ index
   *
   *  \param phiInd   input \f$ \varphi \f$ index
   *
   *  Can return \f$ \varphi \f$ value outside of allowed value range if
   *  index out of range (no internal check).
   */
  double phi(index_t phiInd) const;    
  /*! \brief Returns \f$ \varphi \f$ index for a given \f$ \varphi \f$ value
   *
   *  \param phiVal   input \f$ \varphi \f$ value
   *
   *  Returns \a size_t(-1) in case of \f$ \varphi \f$ being out of allowed
   *  value range.
   */
  index_t phiIndex(double phiVal) const;
  /*! \brief Returns \f$ \varphi \f$ index for a given combined index
   *
   *  \overload
   *
   *  \param comInd    input combined index
   *
   *  Can return an invalid \f$ \varphi \f$ index if \a comInd 
   *  is invalid.
   */
  index_t phiIndex(index_t comInd) const;


  /**
   * @brief A rectangular window within the segmentation.
   */
  class SubSeg
  {
  public:
    typedef CaloTowerSeg::index_t index_t;

    /**
     * @brief Constructor.
     * @param parent Segmentation of which this is a part.
     * @param etamin Lower eta index.
     * @param etamax Upper eta index (inclusive).
     * @param phimin Lower phi index.
     * @param phimax Upper phi index (inclusive).
     *               If phimax < phimin, then we wrap around in phi.
     */
    SubSeg (const CaloTowerSeg& parent,
            index_t etamin, index_t etamax,
            index_t phimin, index_t phimax);

    /// Lower eta index.
    index_t etamin() const;

    /// Upper eta index (inclusive).
    index_t etamax() const;

    /// Lower phi index.
    index_t phimin() const;

    /// Upper phi index (inclusive).  phimax<phimin indicates phi wraparound.
    index_t phimax() const;

    /// The number of towers in the eta direction in this window.
    size_t neta() const;

    /// The number of towers in the phi direction in this window.
    size_t nphi() const;

    /// The number of towers in this window.
    size_t size() const;

    const CaloTowerSeg& parent() const { return m_parent; }


    /**
     * @brief Return a new segmentation object corresponding to this window.
     */
    CaloTowerSeg segmentation() const;


  private:
    /// Segmentation of which this is a part.
    const CaloTowerSeg& m_parent;

    /// Inclusive indices.  m_phimax < m_phimin indicates phi wraparound.
    index_t m_etamin;
    index_t m_etamax;
    index_t m_phimin;
    index_t m_phimax;
  };


  /**
   * @brief Iterator over a rectangular window of towers.
   *
   * This is based on an existing iterator over all towers, TOWER_ITERATOR.
   * This iterator should supply operator++ and operator+=.
   *
   * There is no end iterator defined; you need to count the iterations
   * up to the size of the window.
   *
   * This is a little complicated; we need to take
   * phi wrapping into account too.
   *
   * Here's some bad ascii art to help illustrate:
   *
   *
   *            *...............*
   *            * ! ! ! ! ! ! ! *
   *            *...*******.....*
   *            * ! *0|1|2* ! ! *        phi ->
   *            *...*-----*.....*
   *            * ! *3|4|5* ! ! *    eta
   *            *...*******.....*     |
   *            * ! ! ! ! ! ! ! *     v
   *            ***.........*****
   *            *2* ! ! ! ! *0|1*
   *            *-*.........*****
   *            *5* ! ! ! ! *3|4*
   *            ***.........*****
   *            *               *
   *
   * Consider the upper window first.
   * We initialize the base TOWER_ITERATOR pointing at the proper
   * tower.  Then m_phipos
   * counts the phi index within the window; here from 0 to 2.
   * When m_phipos reaches the window size, m_nphi, then we need
   * go to the next row.  We add m_nphi_skip to the tower iterator,
   * reset the entries iterator appropriately, and reset m_phipos to 0.
   *
   * For the case of phi-wrapping, this mostly still works: we just
   * need to initialize m_phipos appropriately.  In the example
   * here of the lower window, m_phipos should start at 2.  Then
   * we apply the skip when we go across to
   * the right half of the window, and we just wrap around at the
   * right edge; here, we go from the tower
   * numbered 1 to 5.
   *
   * But this means that in this case, we don't visit the towers in order,
   * so we need some way of getting the tower number within the window
   * from the iterator, so that we can apply the mapping to the
   * correct tower.  That's done by m_itower and m_phipos2.
   * m_phipos2 is similar to m_phipos, except that it always
   * starts at 0.  Thus, then m_phipos reaches m_nphi, we need
   * to skip; when m_phipos2 reaches m_nphi, we're shifting down
   * to the next eta row.
   */
  template <class TOWER_ITERATOR>
  class SubSegIterator
    : public TOWER_ITERATOR
  {
  public:
    /**
     * @brief Construct a new iterator for iterating over a window.
     * @param beg Tower iterator pointing at the start of the segmentation
     *            within which this window is defined.
     * @param subseg The window over which we want to iterate.
     */
    static
    SubSegIterator make (TOWER_ITERATOR beg, const SubSeg& subseg);


    /**
     * @brief Constructor.
     * @param it Iterator of the starting tower.
     * @param entry Iterator of the corresponding starting entry.
     * @param store The CaloTowerStore within which we're iterating.
     * @param nphi The phi size of the window.
     * @param nphi_skip The number of towers to skip to go from
     *                  the right edge of the window to the left.
     *                  Should be
     *                    (phi-size of store) - (phi-size of window) + 1
     * @param phipos The starting phi position for the iteration.
     *               Should be 0 if there's no phi wrapping.
     *               Otherwise, it's the phi index (0-based)
     *               of the window tower at the left side
     *               of the store.
     */
    SubSegIterator (const TOWER_ITERATOR& it,
                    size_t nphi,
                    size_t nphi_skip,
                    size_t phipos);


    /**
     * @brief Advance to the next tower in the window.
     *
     * Note: The iteration may not visit the towers in index order.
     *       Use itower() to find the index of the tower to which
     *       the iterator is currently referring.
     */
    SubSegIterator& operator++ ();


    /**
     * @brief The tower index to which the iterator is referring.
     *
     * This is the index within the window.  0-based.
     */
    size_t itower() const;

                    
  private:
    size_t m_nphi;
    size_t m_nphi_skip;
    size_t m_phipos;
    size_t m_phipos2;
    size_t m_itower;
  };



  /**
   * @brief Return a window within the current segmentation.
   *        The window is centered at eta/phi with half-widths deta/dphi.
   */
  SubSeg subseg (double eta, double deta, double phi, double dphi) const;


 private:
  // --------------------------
  // Segmentation parameters 
  // ------------------------

  /*! \brief Number of \f$ \eta \f$ bins */
  index_t   m_neta;    
  /*! \brief Number of \f$ \varphi \f$ bins */
  index_t   m_nphi; 

  /*! \brief Lower boundary \f$ \eta \f$ value range */
  double m_etamin; 
  /*! \brief Upper boundary \f$ \eta \f$ value range */
  double m_etamax;
  /*! \brief Lower boundary \f$ \varphi \f$ value range */
  double m_phimin;
  /*! \brief Upper boundary \f$ \varphi \f$ value range */
  double m_phimax;

  /*! \brief Bin size \f$ \Delta \eta \f$ */
  double m_deta;  
  /*! \brief Bin size \f$ \Delta \varphi \f$ */
  double m_dphi;

  // Can get rid of this when we move to the new CaloDetDescr
  // in which all CaloPhiRange methods are static.
  static CaloPhiRange s_range;
};

// ----------------------------------------
// returns parameters
// ----------------------------------------
inline CaloTowerSeg::index_t CaloTowerSeg::neta() const
{
	return m_neta; 
}

inline CaloTowerSeg::index_t CaloTowerSeg::nphi() const
{
	return m_nphi; 
}

inline double CaloTowerSeg::deta() const
{
  return m_deta;
}

inline double CaloTowerSeg::dphi() const
{
  return m_dphi;
}

inline double CaloTowerSeg::etamin() const 
{
  return m_etamin;
}

inline double CaloTowerSeg::etamax() const 
{
  return m_etamax;
}

inline double CaloTowerSeg::phimin() const 
{
  return m_phimin;
}

inline double CaloTowerSeg::phimax() const 
{
  return m_phimax;
}

inline bool CaloTowerSeg::inbound(index_t etaInd, index_t phiInd) const 
{
  return 
    etaInd >= 1 && etaInd <= m_neta && 
    phiInd >= 1 && phiInd <= m_nphi; 
}

inline bool CaloTowerSeg::inbound(double etaVal, double phiVal) const 
{
  if ( ! ( etaVal >= m_etamin && etaVal <= m_etamax ) ) return false;
  if (m_phimax > m_phimin)
    return phiVal >= m_phimin && phiVal <= m_phimax;
  else
    return phiVal <= m_phimax || phiVal >= m_phimin;
}


// combined index 
inline size_t CaloTowerSeg::etaphi(index_t etaInd,index_t phiInd) const
{
  return this->inbound(etaInd,phiInd) 
    ? (etaInd-1) * m_nphi + (phiInd-1) 
    : outOfRange; 
}

// individual indices
inline CaloTowerSeg::index_t CaloTowerSeg::etaIndex(double etaVal) const
{
  return ( etaVal >= m_etamin && etaVal <= m_etamax )
    ? (index_t)floor( ( etaVal - m_etamin ) / m_deta ) + 1
    : outOfRange;
}
inline CaloTowerSeg::index_t CaloTowerSeg::etaIndex(index_t comInd) const
{
  return (index_t)(comInd/m_nphi) + 1;
}
inline CaloTowerSeg::index_t CaloTowerSeg::phiIndex(double phiVal) const
{
  if (m_phimax > m_phimin)
    return ( phiVal >= m_phimin && phiVal <= m_phimax )
      ? (index_t)floor( ( phiVal - m_phimin ) / m_dphi ) + 1
      : outOfRange;
  else
    return ( phiVal >= m_phimin || phiVal <= m_phimax )
      ? (index_t)floor( s_range.diff ( phiVal, m_phimin ) / m_dphi ) + 1
      : outOfRange;
}
inline CaloTowerSeg::index_t CaloTowerSeg::phiIndex(index_t comInd) const
{
  return (index_t)(comInd % m_nphi) + 1;
}

// index to real space conversion.
inline double CaloTowerSeg::eta(index_t etaInd) const 
{
 return  m_etamin + (etaInd-0.5) * m_deta ; 
}

inline double CaloTowerSeg::phi(index_t phiInd) const 
{
  double ret = m_phimin + (phiInd-0.5) * m_dphi;
  if (m_phimin > m_phimax)
    ret = s_range.fix (ret);
  return ret;
}


//************************************************************************


/**
 * @brief Lower eta index.
 */
inline
CaloTowerSeg::SubSeg::index_t
CaloTowerSeg::SubSeg::etamin() const
{
  return m_etamin;
}


/**
 * @brief Upper eta index (inclusive).
 */
inline
CaloTowerSeg::SubSeg::index_t
CaloTowerSeg::SubSeg::etamax() const
{
  return m_etamax;
}


/**
 * @brief Lower phi index.
 */
inline
CaloTowerSeg::SubSeg::index_t
CaloTowerSeg::SubSeg::phimin() const
{
  return m_phimin;
}


/**
 * @brief Upper phi index (inclusive).  phimax<phimin indicates phi wraparound.
 */
inline
CaloTowerSeg::SubSeg::index_t
CaloTowerSeg::SubSeg::phimax() const
{
  return m_phimax;
}


/**
 * @brief The number of towers in the eta direction in this window.
 */
inline
size_t CaloTowerSeg::SubSeg::neta() const
{
  return m_etamax - m_etamin + 1;
}


/**
 * @brief The number of towers in this window.
 */
inline
size_t CaloTowerSeg::SubSeg::size() const
{
  return neta() * nphi();
}


//************************************************************************


/**
 * @brief Construct a new iterator for iterating over a window.
 * @param beg Tower iterator pointing at the start of the segmentation
 *            within which this window is defined.
 * @param subseg The window over which we want to iterate.
 */
template <class TOWER_ITERATOR>
CaloTowerSeg::SubSegIterator<TOWER_ITERATOR>
CaloTowerSeg::SubSegIterator<TOWER_ITERATOR>::make
   (TOWER_ITERATOR beg, const SubSeg& subseg)
{
  const CaloTowerSeg& parent = subseg.parent();
  size_t itower;
  size_t phipos;

  if (subseg.phimax() < subseg.phimin()) {
    // phi wraparound case
    itower = parent.etaphi (subseg.etamin(), 1);
    phipos = subseg.nphi() - subseg.phimax();
  }
  else {
    // no phi wrapping
    itower = parent.etaphi (subseg.etamin(), subseg.phimin());
    phipos = 0;
  }

  beg += itower;
  return SubSegIterator (beg,
                         subseg.nphi(),
                         parent.nphi() - subseg.nphi() + 1,
                         phipos);
}


/**
 * @brief Constructor.
 * @param it Iterator of the starting tower.
 * @param entry Iterator of the corresponding starting entry.
 * @param store The CaloTowerStore within which we're iterating.
 * @param nphi The phi size of the window.
 * @param nphi_skip The number of towers to skip to go from
 *                  the right edge of the window to the left.
 *                  Should be
 *                    (phi-size of store) - (phi-size of window) + 1
 * @param phipos The starting phi position for the iteration.
 *               Should be 0 if there's no phi wrapping.
 *               Otherwise, it's the phi index (0-based)
 *               of the window tower at the left side
 *               of the store.
 */
template <class TOWER_ITERATOR>
CaloTowerSeg::SubSegIterator<TOWER_ITERATOR>::SubSegIterator
  (const TOWER_ITERATOR& it,
   size_t nphi,
   size_t nphi_skip,
   size_t phipos)
  : TOWER_ITERATOR (it),
    m_nphi (nphi),
    m_nphi_skip (nphi_skip),
    m_phipos (phipos),
    m_phipos2 (0),
    m_itower (0)
{
}


/**
 * @brief Advance to the next tower in the window.
 *
 * Note: The iteration may not visit the towers in index order.
 *       Use itower() to find the index of the tower to which
 *       the iterator is currently referring.
 */
template <class TOWER_ITERATOR>
inline
CaloTowerSeg::SubSegIterator<TOWER_ITERATOR>&
CaloTowerSeg::SubSegIterator<TOWER_ITERATOR>::operator++ ()
{
  // Advance phi position.
  ++m_phipos;
  if (m_phipos < m_nphi) {
    // Not to the gap yet; just advance underlying iterator.
    TOWER_ITERATOR::operator++();
  }
  else {
    // At the gap.  Skip ahead by the size of the gap,
    // and reset the phi position.
    m_phipos = 0;
    TOWER_ITERATOR::operator+= (m_nphi_skip);
  }
  
  // Bump the tower index counter when we wrap around in phi.
  if (++m_phipos2 >= m_nphi) {
    m_phipos2 = 0;
    m_itower += m_nphi;
  }

  return *this;
}


/**
 * @brief The tower index to which the iterator is referring.
 *
 * This is the index within the window.  0-based.
 */
template <class TOWER_ITERATOR>
inline
size_t
CaloTowerSeg::SubSegIterator<TOWER_ITERATOR>::itower() const
{
  return m_itower + m_phipos;
}
                    

//************************************************************************


/*! \class CaloTowerSeg
 *
 *  \brief Data object stores \a CaloTower segmentation
 *
 *  \author  S. Rajagopalan <srinir@bnl.gov>
 *  \author  P. Loch <loch@physics.arizona.edu>
 *  \date    April 1, 2005
 *  \version 2 
 *
 *  The \a CaloTowerSeg class generates an object which stores the tower
 *  segmentation in reguar \f$ \Delta \eta \times \Delta \varphi \f$ bins.
 *  The  bin index convention is \f$ i \in [1,n] \f$, where \f$ n \f$ is
 *  the number of bins for a given variable. In addition a 
 *  continously running index \f$ k \f$ 
 *  is providing by combining the \f$ \eta \f$ 
 *  and \f$ \varphi \f$ indices, with \f$ k \in [0,N-1] \f$. \f$ N \f$ is
 *  total number of bins (sum of the numbers of \f$ \eta \f$ and 
 *  \f$ \varphi \f$ bins).
 *  
 */
#endif 
