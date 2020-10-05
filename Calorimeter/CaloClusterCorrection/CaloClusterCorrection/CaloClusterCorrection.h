// This file's extension implies that it's C, but it is really -*- C++ -*-.

/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#ifndef CALOCLUSTERCORRECTION_CALOCLUSTERCORRECTION_H
#define CALOCLUSTERCORRECTION_CALOCLUSTERCORRECTION_H 
/********************************************************************

NAME:     CaloClusterCorrection.h
PACKAGE:  offline/Calorimeter/CaloClusterCorrection

AUTHORS:  H. Ma, S. Rajagopalan
CREATED:  Dec  1999

PURPOSE:  Base class: CaloClusterProcessor
          Individual cluster correction classes inherit from this class 
          and use the "set*" methods to correct the CaloCluster objects

          execute(CaloCluster *) method to be implemented 
          in the derived class
          

Updated:  May 10, 2000    (SR, HM)
          Migrated to Athena Framework from PASO

Updated:  Jan 3, 2001    (HM)
          QA.
Updated:  May 2001, (SR) Move to StoreGate

Updated:  May 5, 2004    (Sven Menke)
	  base class changed from algo to tool

Updated:  January, 2005    (MB)
          Move to Calorimeter/CaloClusterCorrection.

Updated:  February, 2006 (DLelas)
          New method added:
          "makeCorrectionOnClusCollection(CaloClusterContainer *clusColl)"
          Which enables corrections to be applied to a whole
          CaloCluster collection. It is needed by SW cell weights
          calculation.

********************************************************************/

// INCLUDE HEADER FILES:
#include "CaloRec/CaloClusterProcessor.h"
#include "CaloUtils/ToolWithConstants.h"

class CaloClusterCorrection
  : public CaloUtils::ToolWithConstants<CaloClusterProcessor>
{

 public:
  // modifying CaloCluster object 
  virtual void setsample(xAOD::CaloCluster* cluster,
                         CaloSampling::CaloSample sampling,
			 float em, float etam, float phim, 
			 float emax, float etamax, float phimax, 
			 float etas, float phis) const;

  // set energy of cluster.  if samplings=true, rescale the energy
  // in each sampling accordingly.  default is to do the rescaling.
  virtual void setenergy(xAOD::CaloCluster* cluster, float energy) const;

  // derived class implement the real correction.
  virtual void makeCorrection(const Context& myctx,
                              xAOD::CaloCluster*) const = 0;

  using CaloClusterProcessor::execute;
  StatusCode execute(const EventContext& ctx,
                     xAOD::CaloCluster* cluster) const override;

 protected:
  /// Delegate to base class constructor.
  using base_class::base_class;
};

#endif
