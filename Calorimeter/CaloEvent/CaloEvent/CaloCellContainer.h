///////////////////////// -*- C++ -*- /////////////////////////////

/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#ifndef CALOEVENT_CALOCELLCONTAINER_H
#define CALOEVENT_CALOCELLCONTAINER_H

/** 
   @class CaloCellContainer
   @brief Container class for CaloCell

   CaloCellContainer is a container of all LAr and Tile cells.
It derives from DataVector<CaloCell>. Cells from a given calorimeter 
(LAr e.m., LArHEC, LArFCAl, Tile) can also be accessed individually.
 It has additional methods to allow fast access to any cell, given
its hash identifier.
CaloCellContainer is normally built by CaloRec/CaloCellMaker.cxx, and 
finalized by CaloRec/CaloCellContainerFinalizerTool.cxx. 
CaloCellContainer is NOT meant to be used for private list of cells, a 
simple std::vector<const CaloCell*> is best used.

    
   @author Srini Rajagopalan
   @author David Rousseau

 */

/********************************************************************

NAME:     CaloCellContainer.h
PACKAGE:  offline/Calorimeter/CaloRec

AUTHORS:  S. Rajagopalan
CREATED:  April 2001

PURPOSE:  Container for CaloCell - stored as DataObject in TDS.
MODIFIED: David Rousseau: heavy modification to allow storing all 
          calorimeter cells in one single container
********************************************************************/

#include "AthContainers/DataVector.h"
#include "AthenaKernel/CLASS_DEF.h"
#include "CaloEvent/CaloCell.h"

#include "CaloIdentifier/CaloCell_ID.h"
#include "Identifier/IdentifierHash.h"
#include "CxxUtils/PackedArray.h"
#include "CxxUtils/CachedValue.h"
#include "AthLinks/tools/findInContainer.h"


class CaloCellContainer : public DataVector<CaloCell>
{

// only class allowed to use private functions
// this is because CaloCellContainer need to be told by 
// someone the maximum hash size. Cannot do it by 
// itself to avoid dependencies

 friend class CaloConstCellContainer;
 friend class CaloCellContainerFinalizerTool;
 friend class CaloCompactCellTool; 
 friend class FullCaloCellContMaker;
 friend class EmptyCellBuilderTool;
 public:


 /** @brief type to be used for the internal lookup table, and to return list of cells */
  typedef std::vector<const CaloCell*> CellVector;
  
 /** @brief Return from non-const findCellVector. */
  typedef std::vector<CaloCell*> MutableCellVector;
  
  /** @brief Main constructor */
  CaloCellContainer(SG::OwnershipPolicy ownPolicy=SG::OWN_ELEMENTS ) ;

  
  /** @brief Sized constructor */
  CaloCellContainer(size_t n, SG::OwnershipPolicy ownPolicy=SG::OWN_ELEMENTS );


  /**  @brief destructor */  
  virtual ~CaloCellContainer() { };


  /** @brief reimplementation of const push_back */
  void push_back(CaloCell *);

  /** @brief reimplementation of push_back to gain speed in readin */
  void push_back_fast(CaloCell *);  

  /** @brief dump (obsolete) */
  void print() const;

  /** @brief get const iterators on cell of just one calo */
  CaloCellContainer::const_iterator beginConstCalo(CaloCell_ID::SUBCALO caloNum) const, endConstCalo(CaloCell_ID::SUBCALO caloNum) const;

  /** get non const iterators on cell of just one calo */
  CaloCellContainer::iterator beginCalo(CaloCell_ID::SUBCALO caloNum), endCalo(CaloCell_ID::SUBCALO caloNum);


  /** @brief verify one by one the container is complete (i.e. size is equal to the 
   *  maximum of hash identifier) and in order */
  bool checkOrderedAndComplete() const ;

  /** @brief indicate that the container is complete and in order */
  void setIsOrderedAndComplete(const bool ordered);

  /** @brief tell wether container is complete and in order */
  bool isOrderedAndComplete() const ;

  /** @brief verify one by one the container is ordered */
  bool checkOrdered() const ;

  /** @brief indicates that the container is ordered */
  void setIsOrdered(const bool ordered);

  /** @brief tell wether container is ordered */
  bool isOrdered() const ;

  /** @brief tell wether container has total hash id size */
  bool hasTotalSize() const ;


  /** @brief tell wether it has been filled with cells (maybe none) of a given calo */
  bool hasCalo(const CaloCell_ID::SUBCALO caloNum) const ;


  /** @brief index of first cell of given calorimeter (-1 if none).
      Note that it is normally more efficient to use iterators */
  int indexFirstCellCalo(const CaloCell_ID::SUBCALO caloNum) const ;

  /** @brief index of last cell of given calorimeter (-2 if none)
      Note that it is normally more efficient to use iterators. */
  int indexLastCellCalo(const CaloCell_ID::SUBCALO caloNum) const ;

  /** @brief get number of cels of given calorimeter */
  int nCellsCalo(const CaloCell_ID::SUBCALO caloNum) const ;

  /** @brief set which calo has been filled.  */
  void setHasCalo(const CaloCell_ID::SUBCALO caloNum);

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
  const CaloCell * findCell(const IdentifierHash   theHash) const;

  /** @brief fast find method given identifier hash. If the container is 
not ordered and complete a look up map is used, which is build the first 
time this method of findCellVector is used */
  CaloCell * findCell(const IdentifierHash   theHash);

  /** @brief Return index of the cell with a given hash.
      Returns -1 if the cell isn't found. */
  int findIndex(const IdentifierHash   theHash) const;

  /** @brief fast find method given vector of identifier hash.
  Be careful that the order of cell return may not match the order 
  of the inputs, and that some cells may be missing */
  void findCellVector(const std::vector<IdentifierHash> & theVectorHash,CellVector & theCellVector) const;

  /** @brief fast find method given vector of identifier hash.
  Be careful that the order of cell return may not match the order 
  of the inputs, and that some cells may be missing */
  void findCellVector(const std::vector<IdentifierHash> & theVectorHash,MutableCellVector & theCellVector);

  

  /** @brief initialize look up table.
  Note that the look up table is initialized the first time findcell is called.
  If new cells are added afterwards (should not be the case) the lookuptable 
  need be reinitialized by hand */

  void initializeLookUpTable();

  /** @brief reset look up table */
  void resetLookUpTable();

  /** @brief order container */
  void order();

  /** @brief functor to order cells according to caloHash */
  class orderWithCaloHash
    {
    public:
      bool operator() ( const CaloCell * a ,const CaloCell * b ) const;
      // implement in .cxx so that CaloDetDescrElement.h does not need to be included
      // {return a->caloDDE()->calo_hash() < b->caloDDE()->calo_hash(); }
	bool operator() ( const CaloCell & a ,const CaloCell & b ) const ;
	// { return a.caloDDE()->calo_hash() < b.caloDDE()->calo_hash(); }
    };


 private:
  /** @brief get message service */
  IMessageSvc* msgSvc() const;

  /** @brief If @ flag is true, then the container size equals the maximum hash.
   *         Only CaloCellContainerFinalizer tool is allowed to set this.
   *         Now, set this in EmptyCellBuilderTool as it is always true.
   */
  void setHasTotalSize(const bool);


  /**
   * @brief Recursively find division points between subcalos in the container.
   */
  void updateCaloIteratorsOrdered (CaloCellContainer::iterator beg,
                                   CaloCellContainer::iterator end);
  

  /** @brief order when container is incomplete */
  void orderWhenIncomplete();
  /** @brief order when container is complete */
  void orderWhenComplete();


  /**
   * @brief Look up a group of cells by IdentifierHash.
   *        This is a templated version that can be instantiated for both
   *        const and non-const versions.
   * @param cont The container from which to fetch cells.
   * @param theVectorHash Vector of desired IdentifierHash's.
   * @param theCellVector Found cells will be appended to this vector.
   */
  template <class CONT, class VECT>
  static void findCellVectorT(CONT& cont,
                              const std::vector<IdentifierHash> & theVectorHash,
                              VECT& theCellVector);

  /** @brief Retrieve an initialized lookup table. */
  const CxxUtils::PackedArray& getLookUpTable() const;

  /** @brief look up table of size HashIdentifiermax. */
  CxxUtils::CachedValue<CxxUtils::PackedArray> m_lookUpTable;

  /** @brief true if size correspond to maximum hash. 
      only CaloCellContainerFinalizer tool is allowed to set this */
  bool m_hasTotalSize ;


  /** @brief true if ordered  */
  bool m_isOrdered ;

  /** @brief true if complete and in right order */
  bool m_isOrderedAndComplete ;

  /** @brief true if given cell from given calo has been filled (even if none) */
  std::vector<bool> m_hasCalo;

  /** @brief index of first cell of given calo (-1 if none) */
  std::vector<int> m_indexFirstCellCalo;

  /** @brief index of first cell of given calo (-2 if none) */
  std::vector<int> m_indexLastCellCalo;

  /** @brief const iterators for the different calorimeters */
  std::vector< CaloCellContainer::const_iterator > m_beginConstCalo,m_endConstCalo ;
  /** @brief non const iterators for the different calorimeters */
  std::vector< CaloCellContainer::iterator > m_beginCalo,m_endCalo;
};

inline bool CaloCellContainer::hasTotalSize() const {
  return m_hasTotalSize ;
}

inline bool CaloCellContainer::isOrdered() const {
  return m_isOrdered ;
}


inline bool CaloCellContainer::isOrderedAndComplete() const {
  return m_isOrderedAndComplete ;
}


CLASS_DEF(CaloCellContainer, 2802, 1)
SG_BASE(CaloCellContainer, DataVector<CaloCell> );

namespace SG {


// Specialize this for CaloCellContainer.
// We'll implement it using findIndex.
template <>
bool
findInContainer (const CaloCellContainer& data,
                 CaloCell const * const & element,
                 CaloCellContainer::size_type& index);


} // namespace SG


#endif

