/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/********************************************************************

NAME:     eflowTrackClusterMatcher.cxx
PACKAGE:  offline/Reconstruction/eflowRec

AUTHORS:  M.Hodgkinson, D.R. Tovey
CREATED:  30 June 2005

*******************************************************************/
#include "eflowRec/eflowTrackClusterMatcher.h"
#include "eflowRec/phicorr.h"
#include "eflowRec/cycle.h"

#include "CaloEvent/CaloCell.h"

#include <cmath>

/* Set cone R (90% two-sided) */
double eflowTrackClusterMatcher::m_coneRSq = 1.64*1.64;

eflowTrackClusterMatcher::eflowTrackClusterMatcher() :
    m_etaCluster(0), m_phiCluster(0), m_varEtaCl(0), m_varPhiCl(0) { }
eflowTrackClusterMatcher::eflowTrackClusterMatcher(const xAOD::CaloCluster* cluster) :
    m_etaCluster(0), m_phiCluster(0), m_varEtaCl(0), m_varPhiCl(0) {
  setCluster(cluster);
}

eflowTrackClusterMatcher::~eflowTrackClusterMatcher(){ }

double eflowTrackClusterMatcher::match(const eflowEtaPhiPosition& trackEtaPhi) const {

  double disphi = trackEtaPhi.getPhi().getAbsDifference(m_phiCluster);
  double diseta = trackEtaPhi.getEta() - m_etaCluster;

  /* Use simplified (eta, phi axes) ellipsoidal cone */
  double distSq = ((diseta*diseta)/m_varEtaCl) + ((disphi*disphi)/m_varPhiCl);

  return (distSq < m_coneRSq) ? sqrt(distSq) : 0;
}

void eflowTrackClusterMatcher::setCluster(const xAOD::CaloCluster* cluster) {

  if (!cluster) return;

  m_phiCluster = cluster->phi();
  m_etaCluster = cluster->eta();

  /* TODO: Can we use plain cluster eta and phi instead of recalculated ones? */
  /* TODO: Maybe the cluster itself knows its own width? */

  double sumeta = 0;
  double sumeta2 = 0;
  double sumphi = 0;
  double sumphi2 = 0;
  double cellphi = 0;
  double cellenergy = 0;
  int numcell = 0;

  const CaloClusterCellLink* theCellLink = cluster->getCellLinks();

  if (theCellLink){
  
    CaloClusterCellLink::const_iterator itCell = theCellLink->begin();
    CaloClusterCellLink::const_iterator endCell = theCellLink->end();
    
    for( ; itCell!=endCell; ++itCell) {
      const CaloCell* thisCell = *itCell;
      sumeta += thisCell->eta();
      sumeta2 += thisCell->eta()*thisCell->eta();
      cellphi = eflowAzimuth(thisCell->phi()).cycle(m_phiCluster);
      sumphi += cellphi;
      sumphi2 += cellphi*cellphi;
      cellenergy += thisCell->energy();
      numcell++;
    }

  }//if valid cell link

  const double etaPhiLowerLimit(6.25e-4);
  if (numcell > 1) {
    m_varEtaCl = (sumeta2 - sumeta*sumeta/((double)numcell)) / ((double)(numcell-1));
    m_varPhiCl = (sumphi2 - sumphi*sumphi/((double)numcell)) / ((double)(numcell-1));
  }
  if ((m_varEtaCl < etaPhiLowerLimit) || (numcell <= 1)) m_varEtaCl = etaPhiLowerLimit;
  if ((m_varPhiCl < etaPhiLowerLimit) || (numcell <= 1)) m_varPhiCl = etaPhiLowerLimit;

  if (sumeta && (numcell > 1)) m_etaCluster = sumeta/((double)numcell);
  if (sumphi && (numcell > 1)) m_phiCluster = sumphi/((double)numcell);

  /* Remainder cluster */
  if (cluster->eta() == -10) {
    m_varEtaCl = 0;
    m_varPhiCl = 0;
  }

}
