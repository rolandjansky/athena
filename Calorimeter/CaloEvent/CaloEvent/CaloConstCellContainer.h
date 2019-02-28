// This file's extension implies that it's C, but it's really -*- C++ -*-.

/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

// $Id$
/**
 * @file CaloEvent/CaloConstCellContainer.h
 * @author scott snyder <snyder@bnl.gov>
 * @date Aug, 2014
 * @brief @c CaloCellContainer that can accept const cell pointers.
 */


#ifndef CALOEVENT_CALOCONSTCELLCONTAINER_H
#define CALOEVENT_CALOCONSTCELLCONTAINER_H


#include "CaloEvent/CaloCellContainer.h"
#include "AthContainers/ConstDataVector.h"


/**
 * @brief @c CaloCellContainer that can accept const cell pointers.
 *
 * For more details about what's going on here,
 * see AthContainers/ConstDataVector.h.
 *
 * In brief, @c CaloCellContainer acts as a container of (non-const)
 * `CaloCell*`, and thus you can't put a `const CaloCell*` into it.
 * @c CaloConstCellContainer derives privately from @c CaloCellContainer.
 * It overrides all accessor methods so that they only return `const CaloCell*`,
 * and overrides the insertion methods so that they can take const pointers.
 * There are two ways to convert this to a `const CaloCellContainer`.
 * The first is by calling @c asDataVector.  The second is by recording
 * it in StoreGate and then retrieving it as a @c CaloCellContainer.
 * When the object is recorded, it will automatically be locked.
 *
 * Here, we declare only the methods from @c CaloCellContainer that we
 * want to make visible.  The @c DataVector methods are overridden
 * in @c ConstDataVector.
 */
class CaloConstCellContainer
  : public ConstDataVector<CaloCellContainer>
{
public:
  /** @brief Constructor */
  CaloConstCellContainer (SG::OwnershipPolicy ownPolicy=SG::OWN_ELEMENTS );


  /** @brief get const begin iterator on cell of just one calo */
  ::CaloCellContainer::const_iterator
  beginConstCalo (CaloCell_ID::SUBCALO caloNum) const;


  /** @brief get const begin iterator on cell of just one calo */
  ::CaloCellContainer::const_iterator
  endConstCalo(CaloCell_ID::SUBCALO caloNum) const;


  /** @brief verify one by one the container is complete (i.e. size is equal to the 
   *  maximum of hash identifier) and in order */
  bool checkOrderedAndComplete() const ;


  /** @brief indicate that the container is complete and in order */
  void setIsOrderedAndComplete(bool ordered);


  /** @brief tell wether container is complete and in order */
  bool isOrderedAndComplete() const ;


  /** @brief verify one by one the container is ordered */
  bool checkOrdered() const ;


  /** @brief indicates that the container is ordered */
  void setIsOrdered(bool ordered);


  /** @brief tell wether container is ordered */
  bool isOrdered() const;


  /** @brief tell wether container has total hash id size */
  bool hasTotalSize() const;


  /** @brief tell whether it has been filled with cells (maybe none) of a given calo */
  bool hasCalo(const CaloCell_ID::SUBCALO caloNum) const;


  /** @brief index of first cell of given calorimeter (-1 if none).
      Note that it is normally more efficient to use iterators */
  int indexFirstCellCalo(const CaloCell_ID::SUBCALO caloNum) const ;


  /** @brief index of last cell of given calorimeter (-2 if none)
      Note that it is normally more efficient to use iterators. */
  int indexLastCellCalo(const CaloCell_ID::SUBCALO caloNum) const ;


  /** @brief get number of cels of given calorimeter */
  int nCellsCalo(const CaloCell_ID::SUBCALO caloNum) const ;


  /** @brief set which calo has been filled.  */
  void setHasCalo(CaloCell_ID::SUBCALO caloNum);


  /** @brief fill calo iterators and the index of first and last cell 
      IT IS THE RESPONSABILITY OF THE PRODUCER TO CALL THIS
      METHOD ONCE THE FILLING IS FINISHED
      Please use CaloCellContainerFinalizerTool to make sure the
      container is properly finalised.*/
  void updateCaloIterators();


  /** @brief fills calo iterators and the index of first cell
     for a given subcalo */
  void updateCaloBeginIterators(int ic, int ind);

  
  /** @brief fills calo iterators and the index of last cell
     for a given subcalo */
  void updateCaloEndIterators(int ic,int ind);


  /** @brief fast find method given identifier hash. If the container is 
      not ordered and complete a look up map is used, which is build the first 
      time this method of findCellVector is used */
  const CaloCell * findCell (IdentifierHash theHash) const;


  /** @brief Return index of the cell with a given hash.
      Returns -1 if the cell isn't found. */
  int findIndex (IdentifierHash theHash) const;


  /** @brief fast find method given vector of identifier hash.
      Be careful that the order of cell return may not match the order 
      of the inputs, and that some cells may be missing */
  void findCellVector(const std::vector<IdentifierHash> & theVectorHash,
                      ::CaloCellContainer::CellVector & theCellVector) const;


  /** @brief order container */
  void order();


  /** @brief reimplementation of push_back to gain speed in readin */
  void push_back_fast (const CaloCell* cell);


  /** @brief reset look up table */
  void resetLookUpTable();


private:
 friend class CaloCellContainerFinalizerTool ;

  /** @brief If @ flag is true, then the container size equals the maximum hash.
   *         Only CaloCellContainerFinalizer tool is allowed to set this.
   */
  void setHasTotalSize(bool flag);

  /// Return a non-const pointer to the base container.
  ::CaloCellContainer* baseContainer();
};


/**
 * @brief Specialize @c ClassID_traits for @c ConstDataVector so that
 *        they will be automatically made const when recorded in StoreGate.
 */
template <>
struct ClassID_traits<CaloConstCellContainer>
  : public ClassID_traits<ConstDataVector<CaloCellContainer> >
{
};


namespace SG {


// Also need to specialize these for CaloConstCellContainer.


template <class U>
struct DataBucketTrait<CaloConstCellContainer, U>
  : public DataBucketTrait<ConstDataVector<CaloCellContainer> >
{
};


template <>
class BaseInfo<CaloConstCellContainer>
  : public BaseInfo<ConstDataVector<CaloCellContainer> >
{
};


// Specialize this for CaloConstCellContainer.
template <>
inline
bool
findInContainer (const CaloConstCellContainer& data,
                 CaloCell const * const & element,
                 CaloConstCellContainer::size_type& index)
{
  return SG::findInContainer (*data.asDataVector(), element, index);
}


} // namespace SG


#include "CaloEvent/CaloConstCellContainer.icc"


#endif // not CALOEVENT_CALOCONSTCELLCONTAINER_H
