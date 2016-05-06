/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef EFLOWREC_EFLOWTRACKCLUSTERMATCHER_H
#define EFLOWREC_EFLOWTRACKCLUSTERMATCHER_H
/********************************************************************

NAME:     eflowClus.h
PACKAGE:  offline/Reconstruction/eflowRec

AUTHORS:  M.Hodgkinson, D.Tovey
CREATED:  30 June 2005

********************************************************************/

#include "eflowRec/eflowUtil.h"
#include "xAODCaloEvent/CaloCluster.h"

class eflowTrackClusterMatcher  {

 public:

  eflowTrackClusterMatcher();
  eflowTrackClusterMatcher(const xAOD::CaloCluster* cluster);
  ~eflowTrackClusterMatcher();

  void setCluster(const xAOD::CaloCluster* clus);

  /** Matches point on track to calo cluster */
  double match(const eflowEtaPhiPosition& trackEtaPhi) const;

 private:

  static double m_coneRSq;

  double m_etaCluster;
  eflowAzimuth m_phiCluster;
  double m_varEtaCl;
  double m_varPhiCl;

};
#endif
