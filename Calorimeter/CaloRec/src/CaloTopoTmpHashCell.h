/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

//-----------------------------------------------------------------------
// File and Version Information:
// $Id: CaloTopoTmpHashCell.h,v 1.6 2008-08-28 05:15:05 ssnyder Exp $
// 
// Description: HashCell Container for the topological cluster maker
//   
// Environment:
//      Software developed for the ATLAS Detector at the CERN LHC
//
// Author List:
//      Sven Menke
//
//-----------------------------------------------------------------------

#ifndef CALOTOPOTMPHASHCELL_H
#define CALOTOPOTMPHASHCELL_H

template <class T>
class CaloTopoTmpHashCell {

private:

  // Friends
  
  // Data members

  T* m_clusterCell;

public:
  
  // Constructors

  CaloTopoTmpHashCell()
  {
    m_clusterCell = 0;
  }


  CaloTopoTmpHashCell(const CaloTopoTmpHashCell &other) = default;


  CaloTopoTmpHashCell(T* clusterCell) 
  {
    m_clusterCell = clusterCell;
  }

  // Operators
  
  inline bool operator==(const CaloTopoTmpHashCell & other) const
  {
    return (m_clusterCell == other.m_clusterCell);
  }

  CaloTopoTmpHashCell & operator=(const CaloTopoTmpHashCell & other) = default;

  T * getCaloTopoTmpClusterCell() const
  {
    return m_clusterCell;
  }

};

#endif // CALOTOPOTMPHASHCELL_H

