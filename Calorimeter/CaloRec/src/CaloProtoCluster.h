/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

//Dear emacs, this is -*-c++-*-
#ifndef CALOPROTOCLUSTER
#define CALOPROTOCLUSTER


#include "CaloEvent/CaloClusterCellLink.h"

#include <memory>

class CaloProtoCluster {

 public:
  ///No default constructor, always need a CaloCellContainer to work with
  CaloProtoCluster() = delete;

  ///Constructor with a CaloCellContainer 
  CaloProtoCluster(const CaloCellContainer* cellCont);

  ///Constructor with a CaloCellContainer link
  CaloProtoCluster(const DataLink<CaloCellContainer>& cellCont);

  
 /**@brief Constructor (almost a copy constructor)
  * @param cellLink Ptr to an existng CaloClusterCellLink.
  * Will create a deep copy of the CaloClusterCellLink
  **/
  CaloProtoCluster(const CaloClusterCellLink* cellLink);
  
  ///Return the weighted energy sum of the list of cells
  double e() {
    if (!m_haveKine) getKine();
    return m_e;
  }

  ///Return the weighted Et of the list of cells
  double et() {
    if (!m_haveKine) getKine();
    return m_et;
  }

  ///Add a cell (forward to underlying  CaloClusterCellLink)
  void addCell(const unsigned cellIdx, const CaloClusterCellLink::weight_t weight=1.0) {
    m_cellLinks->addCell(cellIdx,weight);
    m_haveKine=false;
  }


  ///Hand over ownership of CaloClusterCellLink to client
  CaloClusterCellLink* releaseCellLinks() {
    return m_cellLinks.release();
  }

  ///Get a pointer to the underlying CaloClusterCellLink object
  CaloClusterCellLink* getCellLinks() {
    return m_cellLinks.get();
  }

  ///Get the size of the underlying CaloClusterCellLink object
  size_t size() {
    return m_cellLinks->size();
  }

 private:
  ///update m_e and m_et
  void getKine();

  std::unique_ptr<CaloClusterCellLink> m_cellLinks;
  bool m_haveKine;
  double m_e;
  double m_et;

};




#endif
