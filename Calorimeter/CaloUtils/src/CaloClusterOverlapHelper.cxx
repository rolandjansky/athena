/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include <iostream>

//#include "FourMomUtils/P4Helpers.h"

#include "CaloUtils/CaloClusterOverlapHelper.h"
using xAOD::CaloCluster;

bool CaloClusterOverlapHelper::getOverlap(const CaloCluster* pClus1,
					  const CaloCluster* pClus2,
					  double             pl,
					  double             pr,
					  cell_list_t&       cellList)
{
  // convention: pClus1 must have cells, pClus2 may not have cells but needs
  // to have moments (x,y,z).
  
  // loop cells of cluster 1 and return shared cell list
  CaloCluster::const_cell_iterator fCell(pClus1->cell_begin());
  CaloCluster::const_cell_iterator lCell(pClus1->cell_end());
  cellList = getOverlapCells(fCell,lCell,pClus2,pl,pr);
 
  return !cellList.empty();
}

bool CaloClusterOverlapHelper::getOverlap(const cell_list_t& cells,
					  const CaloCluster* pClus,
					  double             pl,
					  double             pr,
					  cell_list_t&       cellList)
{
  if ( pClus->size() == 0 || cells.empty() ) return false;

  // loop cells of cluster 1
  cell_list_t::const_iterator fCell(cells.begin());
  cell_list_t::const_iterator lCell(cells.end());
  cellList = getOverlapCells(fCell,lCell,pClus,pl,pr);

  return !cellList.empty();
}

bool CaloClusterOverlapHelper::getOverlap(const CaloCluster* pClus1,
					  const CaloCluster* pClus2,
					  double pl,
					  double pr,
					  double& ef1,
					  double& ef2)
{
  // get shared energy
  double eShared(0.);
  if ( ! getOverlap(pClus1,pClus2,pl,pr,eShared) )
    {
      ef1 = 0.;
      ef2 = 0.;
      return false;
    }
  ef1 = eShared / pClus1->e();
  ef2 = eShared / pClus2->e();
  
  return (ef1 != 0. && ef2 != 0.);
}

bool CaloClusterOverlapHelper::getOverlap(const cell_list_t& cells,
					  const CaloCluster* pClus,
					  double pl,
					  double pr,
					  double& ef1,
					  double& ef2)
{
  ef1 = 0.;
  ef2 = 0.;
  double eShared(0.);
  if ( getOverlap(cells,pClus,pl,pr,eShared) )
    {
      // loop cells
      cell_list_t::const_iterator fCell(cells.begin());
      cell_list_t::const_iterator lCell(cells.end());
      for ( ; fCell != lCell; fCell++ )
	{
	  ef1 += (*fCell)->e();
	}
      ef1 = eShared / ef1;
      // cluster 
      ef2 = eShared / pClus->e();
    }
  return (ef1 != 0 && ef2 != 0.);
}

bool CaloClusterOverlapHelper::getOverlap(const CaloCluster* pClus1,
					  const CaloCluster* pClus2,
					  double pl, double pr,
					  double& eShared)
{
  eShared = 0.;
  // fill cell list
  cell_list_t cList;
  if ( getOverlap(pClus1,pClus2,pl,pr,cList) )
    {
      // calculate shared energy
      cell_list_t::const_iterator fCell(cList.begin());
      cell_list_t::const_iterator lCell(cList.end());
      for ( ; fCell != lCell; fCell++ )
	{
	  eShared += (*fCell)->e();
	}
    }

  return (eShared != 0.);
}

bool CaloClusterOverlapHelper::getOverlap(const cell_list_t& cells,
					  const CaloCluster* pClus,
					  double pl, double pr,
					  double& eShared)
{
  eShared = 0.;
  // fill cell list
  cell_list_t cList;
  if ( getOverlap(cells,pClus,pl,pr,cList) )
    {
      // calculate shared energy
      cell_list_t::const_iterator fCell(cList.begin());
      cell_list_t::const_iterator lCell(cList.end());
      for ( ; fCell != lCell; fCell++ )
	{
	  eShared += (*fCell)->e();
	}
    }

  return (eShared != 0.);
}

bool CaloClusterOverlapHelper::getOverlap(const CaloCluster* pClus1,
					  const CaloCluster* pClus2,
					  double pl,
					  double pr,
					  unsigned int& nCells)
{
  // fill cell list
  cell_list_t cList;
  getOverlap(pClus1,pClus2,pl,pr,cList);
  nCells = cList.size();
  return (nCells > 0);
}

bool CaloClusterOverlapHelper::getOverlap(const cell_list_t& cells,
					  const CaloCluster* pClus,
					  double pl,
					  double pr,
					  unsigned int& nCells)
{
  // fill cell list
  cell_list_t cList;
  getOverlap(cells,pClus,pl,pr,cList); 
  nCells = cList.size();
  return (nCells > 0);
}
 
bool CaloClusterOverlapHelper::isNear(const CaloCluster* pClus1,
				      const CaloCluster* pClus2,
				      double deltaR)
{
  //return P4Helpers::deltaR(pClus1,pClus2) < deltaR;
  return pClus1->p4().DeltaR(pClus2->p4()) < deltaR;
}
