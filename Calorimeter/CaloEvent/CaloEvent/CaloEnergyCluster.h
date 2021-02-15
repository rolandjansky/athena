/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#ifndef CALOEVENT_CALOENERGYCLUSTER_H
#define CALOEVENT_CALOENERGYCLUSTER_H


#include "Navigation/Navigable.h"

//#include "RecEvent/EnergyCluster.h"
#include "EventKernel/INavigable4Momentum.h"

#include "FourMom/P4EEtaPhiM.h"

#include "CaloEvent/CaloRecoStatus.h"

#include "CaloEvent/CaloCellContainer.h"

class CaloCell;

class CaloEnergyCluster : public Navigable<CaloCellContainer,double>,
			  public P4EEtaPhiM,
			  virtual public INavigable4Momentum
{
 public:

  /*! \brief Const pointer type for objects in the \a CaloCellContainer
   *
   *  Generally expands to \a const \a CaloCell*.
   */
  typedef CaloCellContainer::const_reference                   const_cell_ptr;
  /*! \brief Index type for \a CaloCellContainer
   *
   *  Extracted from the \a Navigable<CaloCellContainer,double> prescription.
   */
  typedef Navigable<CaloCellContainer,double>::external_index_type index_type;
  /*! \brief Iterator type for cell store 
   *
   *  Extracted from the \a Navigable<CaloCellContainer,double> prescription.
   *  Is generally equal to the standard \a size_t. 
   */
  typedef Navigable<CaloCellContainer,double>::object_iter      cell_iterator;

  /*! \brief Destructor */
  virtual ~CaloEnergyCluster();

  /*! \brief Return energy */
  virtual double energy() const { return P4EEtaPhiM::e(); }

  /*! \brief Return uncalibrated energy */
  virtual double getBasicEnergy() const { return this->energy(); } 

  /*! \brief Set uncalibrated energy
   *
   * \param theEnergy basic energy signal 
   */
  virtual void setBasicEnergy(double theEnergy) { this->setE(theEnergy); }

  /*! \brief Add a cell with global kinematic update (slow)
   * 
   *  \param theContainer  input pointer to a \a CaloCellContainer (unchanged)
   *  \param theCell       input pointer to a \a CaloCell (unchanged)
   *  \param weight        signal weight asociated with this cell
   *
   *  Forces search of \a CaloCellContainer for index of \a CaloCell, i.e.
   *  can be slow.
   *
   *  Adding a cell usually changes the global kinematics. The concrete 
   *  implementation is left to the \a updateKine method, which needs to be
   *  implemented by derived classes.
   *
   *  Adding cells without updating the global kinematics is also possible
   *  by using the corresponding \a putElement method in the 
   *  \a Navigable<CaloCellContainer,double> base class.
   */
  void addCell(const CaloCellContainer* theContainer,
               const CaloCell*          theCell,
               double                   weight);
  /*! \brief Add a cell with global kinematic update (fast)
   * 
   *  \overload
   * 
   *  \param theContainer  input pointer to a \a CaloCellContainer (unchanged)
   *  \param theIndex      input index of \a CaloCell in \a CaloCellConatiner 
   *                       (unchanged)
   *  \param weight        signal weight asociated with this cell
   *
   *  Faster as index is already available, but has to rely on client to 
   *  provide the correct (and valid) index.
   */
  void addCell(const CaloCellContainer* theContainer,
               index_type theIndex,
               double weight);
  
  /*! \brief Add a cell with global kinematic update (fast)
   * 
   *  \overload
   * 
   *  \param theContainer  input pointer to a \a CaloCellContainer (unchanged)
   *  \param theIndex      input index of \a CaloCell in \a CaloCellConatiner 
   *                       (unchanged)
   *  \param weight        signal weight asociated with this cell
   *  \param size_hint     if non-zero, a hint about many cells are likely
   *                       to be in the cluster.
   *
   *  Faster as index is already available, but has to rely on client to 
   *  provide the correct (and valid) index.
   *
   *  The caller also guarantees that the cell is not already
   *  in the cluster.
   */
  virtual void addUniqueCell(const CaloCellContainer* theContainer,
                             index_type theIndex,
                             double weight,
                             size_t size_hint = 0);
							 
  /*! \brief Add a cell (very fast)
   * 
   *  \overload
   * 
   *  \param theContainer  input pointer to a \a CaloCellContainer (unchanged)
   *  \param theIndex      input index of \a CaloCell in \a CaloCellConatiner 
   *                       (unchanged)
   *  \param weight        signal weight asociated with this cell
   *
   *  client has to provide the correct (and valid) index.
   *
   *  The caller also guarantees that the cell is not already
   *  in the cluster.
   * 
   *  The caller MUST update Kinematics by himself
   */
  void addUniqueCellNoKine(const CaloCellContainer* theContainer,
                           index_type theIndex,
                           double weight,
                           size_t size_hint = 0);


  /*! \brief Add a cell (very fast)
   * 
   *  \overload
   * 
   *  \param theContainer  input pointer to a \a CaloCellContainer (unchanged)
   *  \param theIndex      input index of \a CaloCell in \a CaloCellConatiner 
   *                       (unchanged)
   *  \param sg            current event store instance
   *  \param weight        signal weight asociated with this cell
   *
   *  client has to provide the correct (and valid) index.
   *
   *  The caller also guarantees that the cell is not already
   *  in the cluster.
   * 
   *  The caller MUST update Kinematics by himself
   */
  void addUniqueCellNoKine(const CaloCellContainer* theContainer,
                           index_type theIndex,
                           IProxyDict* sg,
                           double weight,
                           size_t size_hint = 0);


  /*! \brief Add a cell (fastest)
   * 
   *  \overload
   * 
   *  \param theContainer  link pointing to some element of a \a CaloCellContainer (unchanged)
   *                       The element index is ignored; this is only to
   *                       identify the container.
   *  \param theIndex      input index of \a CaloCell in \a CaloCellConatiner 
   *                       (unchanged)
   *  \param sg            current event store instance
   *  \param weight        signal weight asociated with this cell
   *
   *  client has to provide the correct (and valid) index.
   *
   *  The caller also guarantees that the cell is not already
   *  in the cluster.
   *
   *  Using this method avoids having to look up the container
   *  in the event store.
   * 
   *  The caller MUST update Kinematics by himself
   */
  void addUniqueCellNoKine(const ElementLink<CaloCellContainer>& theContainer,
                           index_type theIndex,
                           double weight,
                           size_t size_hint = 0);


  /*! \brief Reweight a cell with kinematic update
   *
   *  \param theCell input pointer to a \a CaloCell object (unchanged)
   *  \param weight  input new signal weight associated with this cell
   *
   *  Changes the kinematic weight of a cell in the cluster. No action if
   *  cell not in cluster. In particular, the cell is not added.
   */
  void reweightCell(const CaloCell* theCell,double weight);
  /*! \brief Reweight a cell with kinematic update
   *
   *  \overload
   *
   *  \param theContainer input pointer to a \a CaloCellContainer object 
   *                      (unchanged)
   *  \param theIndex     input index of a \a CaloCell in \a CaloCellContainer 
   *                      (unchanged)
   *  \param weight       input new signal weight associated with this cell
   *
   *  Changes the kinematic weight of a cell in the cluster. No action if
   *  not a valid pointer.
   *  
   */
  void reweightCell(const CaloCellContainer* theContainer,
                    index_type& theIndex,
                    double weight);
  /*! \brief Reweight a cell with kinematic update
   *
   *  \overload
   *
   *  \param iterCell     input iterator referring to a cell
   *                      (unchanged)
   *  \param weight       input new signal weight associated with this cell
   *
   *  Changes the kinematic weight of a cell in the cluster. There is no check 
   *  on the validity of the iterator.
   *  
   */
  void reweightCell(cell_iterator& iterCell,
                    double weight);
  /*! \brief Remove a cell with kinematic update
   *
   *  \param theCell pointer to a \a CaloCell object (unchanged)
   *
   *  Removes the cell pointed to by the input argument. The effect
   *  on the global kinematics is defined by the \a updateKine implementation
   *  in the derived classes.
   *
   *  The \a Navigable<CaloCellContainer,double> base class provides a 
   *  \a remove method which can be used if no kinematic update is required.
   */
  void removeCell(const CaloCell* theCell);
  /*! \brief Remove a cell with kinematic update
   *
   *  \overload 
   *
   *  \param theContainer pointer to cell container (unchanged)
   *  \param theIndex     index to cell in container (unchanged)
   */ 
  void removeCell(const CaloCellContainer* theContainer,
                  index_type& theIndex);

  /*! \brief Remove all cells without kinematic update */
  void removeCells();

  /*! \brief Retrieve the kinematic weight of a given cell
   *
   *  \param theCell pointer to the cell (unchanged)
   *  
   *  Returns 0. if cell not in store.
   */
  double getCellWeight(const CaloCell* theCell) const;
  /*! \brief Retrieve the kinematic weight of a given cell
   *
   *  \overload
   *
   *  \param theContainer pointer to cell container (unchanged)
   *  \param theIndex     index to cell in container (unchanged)
   */ 
  double getCellWeight(const CaloCellContainer* theContainer,
                       index_type& theIndex) const;
  /*! \brief Retrieve the kinematic weight of a given cell
   *
   *  \overload
   *
   *  \param iterCell  iterator pointing to a cell
   */ 
  double getCellWeight(cell_iterator& iterCell) const;
  /*! \brief Retrieve the pointer to the original cell container for a given 
   *         cell
   *
   *  \param theCell pointer to the cell (unchanged)
   *
   *  Returns 0 if cell not in store.
   */ 
  const CaloCellContainer* 
    getCellContainer(const CaloCell* theCell) const;

  /*! \brief Retrieve the index of a given cell in the cell container
   *
   *  \param theCell  pointer to the cell (unchanged)
   *  \param theIndex reference to a modifiable index variable
   *
   *  Returns <tt>true</tt> and a valid index if cell is found in store.
   *  If cell is not found, the value of the index is unchanged (typically
   *  0 if initialized properly by client). 
   *
   *  Note that the index refers to the index of the cell in the cell 
   *  container, not the index of the cell in the local store.
   */
  bool getCellIndex(const CaloCell* theCell, 
                    index_type& theIndex) const;

  /*! \brief Retrieve a STL-type \a begin() iterator for the cell store */
  cell_iterator cell_begin() const;
  /*! \brief Retrieve a STL-type \a end() iterator for the cell store */ 
  cell_iterator cell_end()   const;

  /*! \brief Return the number of cells in the store */
  unsigned int getNumberOfCells() const;

  /*! \brief Set the reconstruction status */
  void setRecoStatus(CaloRecoStatus::StatusIndicator recStatus);

  /*! \brief Retrieve the reconstruction status */
  const CaloRecoStatus& getRecoStatus() const;

 protected:

  /*! \brief Default constructor */
  CaloEnergyCluster();

  /*! \brief Calorimeter reconstruction status */
  CaloRecoStatus m_status;

  /*! \brief Interface to kinematic update implementation */
  virtual void updateKine(const CaloCell* theCell, double weight) = 0;

};

/*! \class CaloEnergyCluster
 *
 *  \brief Implementation of navigation and kinematics for \a CaloCluster and
 *         \a CaloTower
 *
 *  \author P. Loch <loch@physics.arizona.edu>
 *  \date   April 1, 2005
 *  \version 1
 *
 *  The \a CaloEnergyCluster class provides all common implementations to 
 *  manage and navigate a store of pointers to \a CaloCell s and their 
 *  kinematic weight in a \a CaloCluster or \a CaloTower, together with data 
 *  members, set and access methods for the global kinematics of these objects.
 */
#endif
