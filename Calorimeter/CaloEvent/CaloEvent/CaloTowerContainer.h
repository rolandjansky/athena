///////////////////////// -*- C++ -*- /////////////////////////////

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef CALOEVENT_CALOTOWERCONTAINER_H
#define CALOEVENT_CALOTOWERCONTAINER_H
/********************************************************************

NAME:     CaloTowerContainer.h
PACKAGE:  offline/Calorimeter/CaloRec

AUTHORS:  H. Ma, S. Rajagopalan
CREATED:  Oct, 2000
          Originally it is LArTowerContainer in LArClusterRec 
          Moved to CaloRec package on Feb 5, 2001

PURPOSE:  container class for CaloTower objects.
          Baseclass: ObjectVector<CaloTower>. 
          The tower objects are stored as CaloTower in DataVector
	  In addition to providing  iterators over all towers, 
	  the towers (and their et and energy) can be accessed with 
          index (ieta,iphi), or in real (eta,phi) space. 

          There are two ways of using this container.  In standard usage,
          the towers are in a strict rectangular grid as defined
          by the CaloTowerSeg object passed to the constructor.
          In this case, the container itself creates all the contained
          towers.  Clients should not attempt to modify the container
          itself.  

          For efficiency, all the contained towers are either stored
          in a single vector or in a DataPool (depending on whether
          or not CALOTOWERCONTAINER_USES_DATAPOOL is defined).
          In the former case, the DataVector elements are then
          set to point into that vector.

          The other usage is simply as a list of towers (this is provided
          for the use of trigger software).  In this case, the default
          constructor must be used, and the ownership mode should
          be set to VIEW_ELEMENTS.  The only operations that modify
          the container that are supported are insert(), push_back(),
          and clear().


UPDATE:   MAY-11-01
	  add CLID

********************************************************************/

// If CALOTOWERCONTAINER_USES_DATAPOOL is set, then CaloTowers
//  will be allocated from a DataPool.
// Otherwise, they will be allocated in an internal vector.
//
// This needs to be disabled for Hive

#ifndef ATHENAHIVE
#define CALOTOWERCONTAINER_USES_DATAPOOL
#endif

// INCLUDE HEADER FILES:
#include "DataModel/DataVector.h"
#include "CLIDSvc/CLASS_DEF.h"

#include "CaloIdentifier/CaloCell_ID.h"

#include "CaloEvent/CaloTower.h"
#include "CaloEvent/CaloTowerSeg.h"
#include "NavFourMom/INavigable4MomentumCollection.h"
#include "CaloEvent/CaloTowerDataVector.h"

#include <utility>
#include <vector>


class CaloTowerContainer : public DataVector<CaloTower> 
{
public:
  typedef DataVector<CaloTower> Base;

  /*! \brief Tower map index type */
  typedef size_t index_t;

  /////////////////////////////
  // Constructors/Destructor //
  /////////////////////////////

  /*! \brief Default constructor
   *
   *  Instantiates a \a CaloTowerContainer without a
   *  \f$ \Delta \eta \times \Delta \varphi \f$ grid definition. Limited use
   *  only.
   */ 
  CaloTowerContainer();
  /*! \brief Useable Constructor
   *
   *  \param tseg  input a \a CaloTowerSeg object containing a
   *               \f$ \Delta \eta \times \Delta \varphi \f$ tower grid
   *               description
   *  \param noTowers If true, don't create the contained towers.
   *
   *  Creates an empty \a CaloTower at each \f$ ( \eta, \varphi ) \f$ 
   *  location of the grid.
   */   
  CaloTowerContainer(const CaloTowerSeg& tseg, bool noTowers = false); 

  /*! \brief Copy constructor. */
  CaloTowerContainer (const CaloTowerContainer& other);

  /*! \brief Assignment. */
  CaloTowerContainer& operator= (const CaloTowerContainer& other);

  /*! \brief Swap. */
  void swap (CaloTowerContainer& other);

  /*! \brief Destructor */ 
  virtual ~CaloTowerContainer();


  ///////////////////////
  // Initialization //
  ///////////////////////
  /*! \brief Initializes the \a CaloTowerContainer 
   *
   *  A \a CaloTower object with zero energy is put at each 
   * \f$ ( \eta, \varphi ) \f$ position of the tower grid. 
   */
  void init();


  ///////////////////////
  // Tower Information //
  ///////////////////////

  /*! \brief Adds a calorimeter index to the tower
   *
   *  \param nCalo input calorimeter index
   *
   *  Updates the list of calorimeters included in the towers. This
   *  list is needed to restore the \a CaloTowerContainer from 
   *  persistency. 
   */
  void setCalo(const CaloCell_ID::SUBCALO& nCalo);
  /*! \brief Retrieve the list of used calorimeters
   *
   *  \param theCalos reference to a modifiable vector of calorimeter indices
   *
   *  Fills a vector with the list of included calorimeters and returns
   *  the number of included calorimeters.
   */
  size_t getCalos(std::vector<CaloCell_ID::SUBCALO>& theCalos) const;
  /*! \brief Returns the number of included calorimeters */
  size_t getNumberOfCalos() const;

  /*! \brief Return lower \f$ \eta \f$ value range boundary */
  double etamin() const;
  /*! \brief Return lower \f$ \varphi \f$ value range boundary */
  double phimin() const;
  /*! \brief Return bin size \f$ \Delta \eta \f$ */
  double deta()   const;
  /*! \brief Return bin size \f$ \Delta \varphi \f$ */
  double dphi()   const;
  /*! \brief Return number of \f$ \eta \f$ bins */
  size_t neta()  const;
  /*! \brief Return number of \f$ \varphi \f$ bins */
  size_t nphi()  const;

  /*! \brief Return a copy of the attached \a CaloTowerSeg */
  const CaloTowerSeg& towerseg() const; 

  ////////////////
  // Kinematics //
  ////////////////
  
  /*! \brief Return the tower transverse energy at a given grid position
   *
   *  \param theEta  input \f$ \eta \f$ value
   *  \param thePhi  input \f$ \varphi \f$ value
   *
   *   Returns 0 if input out of range.
   */
  double et(double theEta, double thePhi) const;
  /*! \brief Return the tower transverse energy at a given grid position
   *
   *  \overload 
   * 
   *  \param etaIndex  input \f$ \eta \f$ index
   *  \param phiIndex  input \f$ \varphi \f$ index
   */
  double et(index_t etaIndex, index_t phiIndex) const;
  /*! \brief Return the tower transverse energy at a given grid position
   *
   *  \overload 
   */
  double et(int etaIndex, int phiIndex)             const 
    { return et((index_t)etaIndex,(index_t)phiIndex); }  // temp fix!

  /*! \brief Return the tower energy at a given grid position
   *
   *  \param theEta  input \f$ \eta \f$ value
   *  \param thePhi  input \f$ \varphi \f$ value
   *
   *   Returns 0 if input out of range.
   */
  double e(double theEta, double thePhi)            const
    { return this->energy(theEta,thePhi); }
  /*! \brief Return the tower energy at a given grid position
   *
   *  \overload 
   * 
   *  \param etaIndex  input \f$ \eta \f$ index
   *  \param phiIndex  input \f$ \varphi \f$ index
   */
  double e(index_t etaIndex, index_t phiIndex)      const
    { return this->energy(etaIndex,phiIndex); }
  /*! \brief Return the tower energy at a given grid position
   *
   *  \overload 
   */
  double e(int etaIndex, int phiIndex)              const
    { return this->energy(etaIndex,phiIndex); }

  /*! \brief Return the tower energy at a given grid position
   *
   *  \param theEta  input \f$ \eta \f$ value
   *  \param thePhi  input \f$ \varphi \f$ value
   *
   *   Returns 0 if input out of range.
   */
  double energy(double theEta, double thePhi)       const;
  /*! \brief Return the tower energy at a given grid position
   *
   *  \overload 
   * 
   *  \param etaIndex  input \f$ \eta \f$ index
   *  \param phiIndex  input \f$ \varphi \f$ index
   */
  double energy(index_t etaIndex, index_t phiIndex) const;
  /*! \brief Return the tower energy at a given grid position
   *
   *  \overload 
   */
  double energy(int etaIndex, int phiIndex)         const
    { return energy((index_t)etaIndex,(index_t)phiIndex); }

  //////////////////////////
  // Container Management //
  //////////////////////////

  /*! \brief Returns the combined index of a tower on the grid
   *
   *  \param aTower pointer to a \a CaloTower object
   *
   *  Returns \a index_t(-1) if input invalid.
   */
  index_t getTowerIndex(const CaloTower* aTower)            const;
  /*! \brief Returns the combined index for given \f$ \eta \f$ and 
   *         \f$ \varphi \f$ indices.
   *
   *  \overload
   *
   *  \param etaIndex input \f$ \eta \f$ index
   *  \param phiIndex input \f$ \varphi \f$ index
   */
  index_t getTowerIndex(index_t etaIndex, index_t phiIndex) const;
  /*! \brief Returns the combined index for given \f$ \eta \f$ and 
   *         \f$ \varphi \f$ values.
   *
   *  \overload
   *
   *  \param theEta input \f$ \eta \f$ value
   *  \param thePhi input \f$ \varphi \f$ value
   */
  index_t getTowerIndex(double theEta, double thePhi)       const;
  /*! \brief Returns the \f$ \eta \f$ index for a given tower
   *
   *  \param aTower pointer to a \a CaloTower object
   *
   *  Returns \a index_t(-1) if input invalid.
   */
  index_t getTowerEtaIndex(const CaloTower* aTower)         const;
  /*! \brief Returns the \f$ \eta \f$ index for a given \f$ \eta \f$
   *         value 
   *
   *  \overload
   *
   *  \param theEta input \f$ \eta \f$ value
   */
  index_t getTowerEtaIndex(double theEta)                   const;
  /*! \brief Returns the \f$ \eta \f$ index for a given combined index
   *
   *  \overload
   *
   *  \param towerIndex input combined index
   */
  index_t getTowerEtaIndex(index_t towerIndex)              const;
  /*! \brief Returns the \f$ \varphi \f$ index for a given tower
   *
   *  \param aTower pointer to a \a CaloTower object
   *
   *  Returns \a index_t(-1) if input invalid.
   */
  index_t getTowerPhiIndex(const CaloTower* aTower)         const;
  /*! \brief Returns the \f$ \varphi \f$ index for a given \f$ \varphi \f$
   *         value 
   *
   *  \overload
   *
   *  \param thePhi input \f$ \varphi \f$ value
   */
  index_t getTowerPhiIndex(double thePhi)                   const;
  /*! \brief Returns the \f$ \varphi \f$ index for a given combined index
   *
   *  \overload
   *
   *  \param towerIndex input combined index
   */
  index_t getTowerPhiIndex(index_t towerIndex)              const;

  /*! \brief Returns both \f$ \eta \f$ and \f$ \varphi \f$ indices for
   *         a given tower
   * 
   *  \param  aTower   input pointer to a \a CaloTower
   *  \param  indexEta reference to a modifiable \f$ \eta \f$ index 
   *                   (set on return)
   *  \param  indexPhi reference to a modifiable \f$ \varphi \f$ index 
   *                   (set on return)
   *
   *  Returns <tt>true</tt> if tower allocated with valid indices.
   */
  bool   getTowerIndices(const CaloTower* aTower, 
			 index_t& indexEta,
			 index_t& indexPhi)           const;
  /*! \brief Returns both \f$ \eta \f$ and \f$ \varphi \f$ indices for
   *         given \f$ \eta \f$ and \f$ \varphi \f$ values
   *
   *  \overload 
   * 
   *  \param  theEta   input \f$ \eta \f$ index
   *  \param  thePhi   input \f$ \varphi \f$ index 
   *  \param  indexEta reference to a modifiable \f$ \eta \f$ index 
   *          (set on return)
   *  \param  indexPhi reference to a modifiable \f$ \varphi \f$ index 
   *          (set on return)
   *
   *  Returns <tt>true</tt> if tower allocated with valid indices.
   */
  bool   getTowerIndices(double theEta, double thePhi,
			 index_t& indexEta,
			 index_t& indexPhi)           const;

  /*! \brief Returns a pointer to a tower with given 
   *         \f$ ( \eta, \varphi ) \f$ indices
   *
   *  \param eta  input \f$ \eta \f$ index
   *  \param phi  input \f$ \varphi \f$ index
   *
   *  Returns 0 if indices out of range.
   */
  CaloTower*       getTower(index_t eta, index_t phi);
  /*! \brief Returns a const pointer to a tower with given 
   *         \f$ ( \eta, \varphi ) \f$ indices
   *
   *  \overload
   */
  const CaloTower* getTower(index_t eta, index_t phi) const;  
  /*! \brief Returns a pointer to a tower with given 
   *         \f$ ( \eta, \varphi ) \f$ indices
   *
   *  \overload
   */
  CaloTower*       getTower(int eta, int phi)
    { return this->getTower(index_t(eta), index_t(phi)); }
  /*! \brief Returns a const pointer to a tower with given 
   *         \f$ ( \eta, \varphi ) \f$ indices
   *
   *  \overload
   */
  const CaloTower* getTower(int eta, int phi) const
    { return this->getTower(index_t(eta), index_t(phi)); }
  /*! \brief Returns a pointer to a tower with given 
   *         \f$ ( \eta, \varphi ) \f$ values
   *
   *  \overload
   *
   *  \param eta input \f$ \eta \f$ value
   *  \param phi input \f$ \varphi \f$ value
   */
  CaloTower*       getTower(double eta, double phi);
  /*! \brief Returns a const pointer to a tower with given 
   *         \f$ ( \eta, \varphi ) \f$ values
   *
   *  \overload
   */
  const CaloTower* getTower(double eta, double phi)   const;

  /*! \brief Returns a pointer to a tower with a given combined index 
   *
   *  \overload
   *
   *  \param  theIndex  input combined index
   */
  CaloTower*       getTower( index_t theIndex )
    { 
      return theIndex < this->size() 
	? (this->operator[])(theIndex) 
	: (CaloTower*)0; 
    }
  /*! \brief Returns a const pointer to a tower with a given combined index 
   *
   *  \overload
   */
  const CaloTower* getTower( index_t theIndex ) const
    { 
      return theIndex < this->size()  
	? (this->operator[])(theIndex) 
	: (const CaloTower*)0; 
    }

  /*! \brief Returns the index out-of-range indicator */ 
  index_t flagOutOfRange()             const;
  /*! \brief Checks if an index is out of range 
   *
   *  \param anIndex input combined tower index
   *
   *  Returns <tt>true</tt> if index is out of range.
   */
  bool   isOutOfRange(index_t anIndex) const;


public:
  //***********************************************************************
  // We don't want to allow clients to change pointers in the container.
  // So change DataVector operations that return lvalues to return
  // instead rvalues.

  using Base::operator[];
  CaloTower* operator[] (size_type n);

  using Base::at;
  CaloTower* at (size_type n);

  using Base::front;
  CaloTower* front ();

  using Base::back;
  CaloTower* back ();


  /**
   * @brief Iterator class for which the reference type is not an lvalue.
   *
   *        Normally, the iterator for a DataVector uses ElementProxy
   *        as the reference type, so a dereferenced iterator can act
   *        as an lvalue.  Here, we change it to an rvalue (just a
   *        CaloTower*) so that the pointers can't be changed
   *        through the iterator.
   */
  class iterator
    : public boost::iterator_adaptor<iterator,
                                     Base::iterator, 
                                     Base::value_type,
                                     Base::iterator::iterator_category,
                                     // Reference
                                     Base::value_type>
  {
  private:
    // Shorthand for the base class type.
    typedef boost::iterator_adaptor<iterator,
                                    Base::iterator,
                                    Base::value_type,
                                    Base::iterator::iterator_category,
                                    Base::value_type>
    iterator_adaptor_;
    
  public:
    iterator() {}
    iterator (Base::iterator it)  : iterator_adaptor_ (it) {}

    operator const_iterator() const
    { return const_iterator (this->base()); }
  };


  typedef std::reverse_iterator<iterator> reverse_iterator;


  using Base::begin;
  using Base::end;
  iterator begin();
  iterator end();

  using Base::rbegin;
  using Base::rend;
  reverse_iterator rbegin();
  reverse_iterator rend();


  //***********************************************************************
  // We'll be disallowing most operations that modify the container.
  // However, these operations are needed by trigger code.
  // They are only allowed when the segmentation is null and
  // the ownership policy is set to @c VIEW_ELEMENTS.

  void insert(iterator position, iterator first, iterator last);
  void clear(SG::OwnershipPolicy p);
  void push_back(value_type pElem);


private:
  /*! \brief Tower grid \f$ ( \Delta \eta \times \Delta \varphi ) \f$ 
   *         descriptor 
   */
  CaloTowerSeg   m_towerSeg;

  /*! \brief Vector of calorimeter regions contributing to the tower signals */
  std::vector<unsigned int> m_caloRegions;

#ifndef CALOTOWERCONTAINER_USES_DATAPOOL
  /*! \brief Vector of contained towers. */
  std::vector<CaloTower> m_towers;

  void setTowers();
#endif

  /*! \brief Index overflow indicator */
  static const index_t m_outOfRange = static_cast<index_t>(-1);


private:
  // Hide methods from DataVector that modify the container.
  template <class InputIterator>
  void assign(InputIterator first, InputIterator last);
  iterator insert(iterator position, value_type pElem);
  template <class InputIterator>
  void insert(iterator position, InputIterator first, InputIterator last);
  iterator erase(iterator position);
  iterator erase(iterator first, iterator last);
  void sort();
  template <class COMPARE>
  void sort(COMPARE comp);
  void swapElement(size_type index, value_type newElem, reference oldElem);
  void resize(size_type sz);
  void reserve (size_type n);
  void pop_back();
  void clear();
};
 
CLASS_DEF(CaloTowerContainer, 2804, 1)
SG_BASE(CaloTowerContainer, DataVector<CaloTower> );

//////////////////////
// Inline Functions //
//////////////////////

// included calorimeter
inline size_t CaloTowerContainer::getNumberOfCalos() const
{
  return m_caloRegions.size();
}

// tower binning
inline double CaloTowerContainer::etamin() const
{
  return m_towerSeg.etamin();
}
inline double CaloTowerContainer::deta() const
{
  return m_towerSeg.deta();
}
inline size_t CaloTowerContainer::neta() const
{
  return m_towerSeg.neta();
}
inline double CaloTowerContainer::phimin() const
{
  return m_towerSeg.phimin();
}

inline double CaloTowerContainer::dphi() const
{
  return m_towerSeg.dphi();
}
inline size_t CaloTowerContainer::nphi() const
{
  return m_towerSeg.nphi();
}
inline const CaloTowerSeg& CaloTowerContainer::towerseg() const
{
  return m_towerSeg;
}

// tower indices
inline CaloTowerContainer::index_t 
CaloTowerContainer::getTowerIndex(const CaloTower* aTower) const
{
  return aTower != 0
    ? this->getTowerIndex(aTower->eta(),aTower->phi())
    : m_outOfRange;
}
inline CaloTowerContainer::index_t 
CaloTowerContainer::getTowerIndex(index_t etaIndex,index_t phiIndex) const
{
  return m_towerSeg.etaphi(etaIndex,phiIndex);
}
inline CaloTowerContainer::index_t 
CaloTowerContainer::getTowerIndex(double theEta, double thePhi) const
{
  return this->getTowerIndex(this->getTowerEtaIndex(theEta),
			     this->getTowerPhiIndex(thePhi));
}
inline CaloTowerContainer::index_t 
CaloTowerContainer::getTowerEtaIndex(const CaloTower* aTower) const
{
  return aTower != 0 
    ? this->getTowerEtaIndex(aTower->eta())
    : m_outOfRange;
}
inline CaloTowerContainer::index_t 
CaloTowerContainer::getTowerEtaIndex(double theEta) const
{
  return m_towerSeg.etaIndex(theEta);
}
inline CaloTowerContainer::index_t
CaloTowerContainer::getTowerEtaIndex(index_t combinedIndex) const
{
  return m_towerSeg.etaIndex(combinedIndex);
}
inline CaloTowerContainer::index_t 
CaloTowerContainer::getTowerPhiIndex(const CaloTower* aTower) const
{
  return aTower != 0 
    ? this->getTowerPhiIndex(aTower->phi())
    : m_outOfRange;
}
inline CaloTowerContainer::index_t 
CaloTowerContainer::getTowerPhiIndex(double thePhi) const
{
  return m_towerSeg.phiIndex(thePhi);
}
inline CaloTowerContainer::index_t
CaloTowerContainer::getTowerPhiIndex(index_t combinedIndex) const
{
  return m_towerSeg.phiIndex(combinedIndex);
} 

// useful help
inline CaloTowerContainer::index_t CaloTowerContainer::flagOutOfRange() const 
{
  return m_outOfRange;
}
inline bool CaloTowerContainer::isOutOfRange(index_t anIndex) const
{
  return anIndex >= this->size() || anIndex == m_outOfRange;
}

inline CaloTower* CaloTowerContainer::operator[] (size_type n)
{
  return Base::operator[](n);
}

inline CaloTower* CaloTowerContainer::at (size_type n)
{
  return Base::at(n);
}

inline CaloTower* CaloTowerContainer::front ()
{
  return Base::front();
}

inline CaloTower* CaloTowerContainer::back ()
{
  return Base::back();
}

inline CaloTowerContainer::iterator CaloTowerContainer::begin()
{
  return iterator (Base::begin());
}

inline CaloTowerContainer::iterator CaloTowerContainer::end()
{
  return iterator (Base::end());
}

inline CaloTowerContainer::reverse_iterator CaloTowerContainer::rbegin()
{
  return reverse_iterator (end());
}

inline CaloTowerContainer::reverse_iterator CaloTowerContainer::rend()
{
  return reverse_iterator (begin());
}


/*! \class CaloTowerContainer
 *
 *  \brief Storable container class for \a CaloTower
 *
 *  \author H. Ma <hma@bnl.gov>
 *  \author S. Rajagopalan <srinir@bnl.gov>
 *  \author P. Loch <loch@physics.arizona.edu>
 *  \date April 1, 2005
 *  \version 2
 *
 *  The \a CaloTowerContainer holds an ordered list of pointers to
 *  \a CaloTower s, where the index of a given tower in the list is
 *  determined by its \f$ ( \eta, \varphi ) \f$ location on the tower
 *  grid. When persistified, only the tower grid as stored in a
 *  \a CaloTowerSeg object held by the \a CaloTowerContainer object,
 *  and the list of contributing calorimeters are saved to disk. The
 *  \a CaloTowerContainer is refilled on the fly using this information when
 *  retrieved from the persistent store into the transient store 
 *  (requires original \a CaloCellContainer to be available also).
 *  
 */
#endif 
