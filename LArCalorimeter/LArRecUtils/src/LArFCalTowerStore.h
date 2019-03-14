/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#ifndef LARRECUTILS_LARFCALTOWERSTORE_H
#define LARRECUTILS_LARFCALTOWERSTORE_H
/**

@class      LArFCalTowerStore
@brief      Intermediate store for cell/tower maps. 

          This is NOT to be confused with CaloTowerContainer, even though
          it is a container class for CaloTower objects. In particular, 
          this class is NOT inherited from CaloTowerContainer classes. 
          This class is for exclusive use within LArFCalTowerBuilder.

PACKAGE:  offline/LArCalorimeter/LArClusterRec

@author   P. Loch
@date     May 2001


*/


// include header files
#include "Identifier/IdentifierHash.h"

#include "CaloEvent/CaloTower.h"
#include "CaloEvent/CaloTowerContainer.h"

#include <map>
#include <vector>

class CaloDetDescrManager;

class LArFCalTowerStore
{
 public:
  typedef std::vector<std::pair<unsigned int,double> > tower_data_t;
  typedef tower_data_t::const_iterator                 cell_iterator;
  typedef std::vector<tower_data_t>                    tower_table_t;

  /// \brief constructor
  LArFCalTowerStore();

  /// \brief destructor
  ~LArFCalTowerStore();

  typedef tower_table_t::const_iterator tower_iterator;
  typedef CaloTowerSeg::SubSegIterator<tower_iterator> tower_subseg_iterator;

  tower_iterator towers() const { return m_TTCmatrix.begin(); }

  /**
   * @brief Return an iterator for looping over all towers
   *        defined by the window @c subseg.  xxx
   *
   *        No end iterator is defined; use subseg.size()
   *        to tell when to stop the iteration.
   *
   *        The iteration may not be in tower index order.
   *        Use the @c itower() method of the iterator
   *        to find the current tower index.
   */
  tower_subseg_iterator towers (const CaloTowerSeg::SubSeg& subseg) const;


  /// \brief iterators: cells
  cell_iterator firstCellofTower(tower_iterator t) const { return t->begin(); }
  cell_iterator lastCellofTower (tower_iterator t) const { return t->end();   }
  unsigned int towerSize(tower_iterator t) const  {return t->size();}
  
  /// \brief setup trigger
  bool buildLookUp(const CaloCell_ID& cellIdHelper,
                   const CaloDetDescrManager& theManager,
                   CaloTowerContainer* theTowers);

  /// \brief size of internal data store
  size_t size() const { return m_TTCmatrix.size(); }

 private:

  std::vector<unsigned int> m_ndxFCal;
  std::vector<unsigned int> m_ndyFCal;

  size_t          m_indxOffset;
  size_t          m_indxBound;

  tower_table_t   m_TTCmatrix; // vector of cell indices for each tower.
};
#endif
