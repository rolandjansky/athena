/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef CALOEVENT_CALOCOMPOSITECELLBASE_H
#define CALOEVENT_CALOCOMPOSITECELLBASE_H

#include "CaloEvent/CaloCell.h"
#include "CaloEvent/CaloCellContainer.h"

template<class NAV>
class CaloCompositeCellBase : public NAV
{
 public:

  /*! \brief Constructor */
  CaloCompositeCellBase();
  /*! \brief Destructor */
  virtual ~CaloCompositeCellBase();

  /*! \brief Iterator type for cell store 
   *
   *  Extracted from the template parameter class prescription.
   *  Is generally equal to the standard \a size_t. 
   */
  typedef typename NAV::object_iter  cell_iterator;

  /*! \brief Add a cell with global kinematic update (slow)
   * 
   *  \param pContainer  input pointer to a \a CaloCellContainer (unchanged)
   *  \param pCell       input pointer to a \a CaloCell (unchanged)
   *  \param weight        signal weight asociated with this cell
   *  \param iCell       input index of the \a CaloCell in the
   *                      \a CaloCellContainer.  This may be omitted,
   *                      but if supplied, a potentially slow search
   *                      of the \a CaloCellContainer for the \a CaloCell
   *                      may be avoided.
   *
   *  Adding a cell usually changes the global kinematics. The concrete 
   *  implementation is left to the \a updateKine method, which needs to be
   *  implemented by derived classes.
   *
   *  Adding cells without updating the global kinematics is also possible
   *  by using the corresponding \a putElement method in the 
   *  \a Navigable<CaloCellContainer,double> base class.
   */
  void addCell(const CaloCellContainer* pContainer,
               const CaloCell*          pCell,
               double                   weight,
               size_t                   iCell=static_cast<size_t>(-1));
  /*! \brief Add a cell with global kinematic update (fast)
   * 
   *  \overload
   * 
   *  \param pContainer  input pointer to a \a CaloCellContainer (unchanged)
   *  \param iCell      input index of \a CaloCell in \a CaloCellConatiner 
   *                       (unchanged)
   *  \param weight        signal weight asociated with this cell
   *
   *  Faster as index is already available, but has to rely on client to 
   *  provide the correct (and valid) index.
   */
  void addCell(const CaloCellContainer* pContainer,
               size_t& iCell,
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
  void addUniqueCell(const CaloCellContainer* theContainer,
                     size_t theIndex,
                     double weight,
                     size_t size_hint = 0);

  /*! \brief Add a cell with no kinematic update (fast)
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
  void addUniqueCellNoKine(const CaloCellContainer* theContainer,
                           size_t theIndex,
                           double weight,
                           size_t size_hint = 0);
  
  /*! \brief Reweight a cell with kinematic update
   *
   *  \param pCell input pointer to a \a CaloCell object (unchanged)
   *  \param weight  input new signal weight associated with this cell
   *
   *  Changes the kinematic weight of a cell in the cluster. No action if
   *  cell not in cluster. In particular, the cell is not added.
   */
  void reweightCell(const CaloCell* pCell,double weight);
  /*! \brief Reweight a cell with kinematic update
   *
   *  \overload
   *
   *  \param pContainer input pointer to a \a CaloCellContainer object 
   *                      (unchanged)
   *  \param iCell     input index of a \a CaloCell in \a CaloCellContainer 
   *                      (unchanged)
   *  \param weight       input new signal weight associated with this cell
   *
   *  Changes the kinematic weight of a cell in the cluster. No action if
   *  not a valid pointer.
   *  
   */
  void reweightCell(const CaloCellContainer* pContainer,
                    size_t& iCell,
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
   *  \param pCell pointer to a \a CaloCell object (unchanged)
   *
   *  Removes the cell pointed to by the input argument. The effect
   *  on the global kinematics is defined by the \a updateKine implementation
   *  in the derived classes.
   *
   *  The \a Navigable<CaloCellContainer,double> base class provides a 
   *  \a remove method which can be used if no kinematic update is required.
   */
  void removeCell(const CaloCell* pCell);
  /*! \brief Remove a cell with kinematic update
   *
   *  \overload 
   *
   *  \param pContainer pointer to cell container (unchanged)
   *  \param iCell     index to cell in container (unchanged)
   */ 
  void removeCell(const CaloCellContainer* pContainer,
                  size_t& iCell);

  /*! \brief Remove all cells without kinematic update */
  void removeCells();

  /*! \brief Retrieve the kinematic weight of a given cell
   *
   *  \param pCell pointer to the cell (unchanged)
   *  
   *  Returns 0. if cell not in store.
   */
  double getCellWeight(const CaloCell* pCell) const;
  /*! \brief Retrieve the kinematic weight of a given cell
   *
   *  \overload
   *
   *  \param pContainer pointer to cell container (unchanged)
   *  \param iCell     index to cell in container (unchanged)
   */ 
  double getCellWeight(const CaloCellContainer* pContainer,
                       size_t& iCell) const;
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
   *  \param pCell pointer to the cell (unchanged)
   *
   *  Returns 0 if cell not in store.
   */ 
  const CaloCellContainer* 
    getCellContainer(const CaloCell* pCell) const;
  /*! \brief Retrieve the pointer to the original cell container for a given 
   *         cell
   *
   *  \overload
   *
   *  \param iterCell iterator to the cell (unchanged)
   *
   *  Returns 0 if cell not in store.
   */ 
  const CaloCellContainer* 
    getCellContainer(cell_iterator& iterCell) const;

  /*! \brief Retrieve the index of a given cell in the cell container
   *
   *  \param pCell  pointer to the cell (unchanged)
   *  \param iCell reference to a modifiable index variable
   *
   *  Returns <tt>true</tt> and a valid index if cell is found in store.
   *  If cell is not found, the value of the index is unchanged (typically
   *  0 if initialized properly by client). 
   *
   *  Note that the index refers to the index of the cell in the cell 
   *  container, not the index of the cell in the local store.
   */
  bool getCellIndex(const CaloCell* pCell, 
                    size_t& iCell) const;

  /*! \brief Retrieve the index of a given cell in the cell container
   *
   *  \overload 
   *
   *  \param iterCell  iterator to the cell (unchanged)
   *  \param iCell reference to a modifiable index variable
   *
   *  Returns <tt>true</tt> and a valid index if cell is found in store.
   *  If cell is not found, the value of the index is unchanged (typically
   *  0 if initialized properly by client). 
   *
   *  Note that the index refers to the index of the cell in the cell 
   *  container, not the index of the cell in the local store.
   */
  bool getCellIndex(cell_iterator& iterCell, 
                    size_t& iCell) const;

  /*! \brief Retrieve a STL-type \a begin() iterator for the cell store */
  cell_iterator cell_begin() const;
  /*! \brief Retrieve a STL-type \a end() iterator for the cell store */ 
  cell_iterator cell_end()   const;

  /*! \brief Return the number of cells in the store */
  unsigned int getNumberOfCells() const;

 protected:

  /*! \brief update kinematics */
  virtual void updateKine(const CaloCell* pCell, double weight) = 0;

/*! \class CaloCompositeCellBase
 *  \brief base class for methods and store of cell links
 * 
 *  This templated abstract base class provides the methods and store for the 
 *  cell links in composite calorimeter reconstruction objects. Its principal
 *  clients are \a CaloCluster and \a CaloTower .
 *
 *  \author  Peter Loch <loch@physics.arizona.edu>
 *  \date    August 25, 2005 (first implementation)
 *  \version 0
 *
 */
};

#include "CaloEvent/CaloCompositeCellBase.icc"
#endif
