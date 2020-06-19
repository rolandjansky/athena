/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#ifndef CALOCLUSTERCORRECTION_CALOSWECCRACK_G3_H
#define CALOCLUSTERCORRECTION_CALOSWECCRACK_G3_H

/********************************************************************

NAME:     CaloSwECcrack_g3.h
PACKAGE:  offline/Calorimeter/CaloClusterCorrection

AUTHORS:  H. Ma, S. Rajagopalan
CREATED:  Dec. 15, 1999

PURPOSE:  correction for the cracks (for the time being only barrel - endcap)
          Tuned using 50 GeV Et photons
          Correction tuned on G3 samples.

atrecon Orig: emreco/qecrack.F           

Updated:  May 10, 2000    (SR, HM)
          Migrated to Athena Framework from PASO

Updated:  Jan 5, 2001    (HM)
          QA. 

Updated:  May 5, 2004    (Sven Menke)
	  base class changed from algo to tool


********************************************************************/

// INCLUDE HEADER FILES:


#include "CaloClusterCorrection/CaloClusterCorrection.h"

class CaloSwECcrack_g3 : public CaloClusterCorrection
{

 public:
  // Inherit constructor.
  using CaloClusterCorrection::CaloClusterCorrection;
  
  virtual void makeCorrection (const Context& myctx,
                               xAOD::CaloCluster* cluster) const override;

 private:

  CaloSwECcrack_g3() = delete;

  // constants 
  static const float P0[3];
  static const float P1[1];
  static const float P2[3];

};
#endif
