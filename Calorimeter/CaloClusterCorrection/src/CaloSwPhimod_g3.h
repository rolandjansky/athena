/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#ifndef CALOCLUSTERCORRECTION_CALOSWPHIMOD_G3_H
#define CALOCLUSTERCORRECTION_CALOSWPHIMOD_G3_H
/********************************************************************

NAME:     CaloSwPhimod_g3.h
PACKAGE:  offline/Calorimeter/CaloClusterCorrection

AUTHORS:  H. Ma, S. Rajagopalan
CREATED:  Dec. 15, 1999

PURPOSE:  correction for the modulations with the position in phi
          (Tuned using 50 GeV Et photons)  
          base class: CaloClusterCorrection
          Correction tuned on G3 samples.

Atrecon Orig: emreco/qphimod.F

Updated:  May 10, 2000    (SR, HM)
          Migrated to Athena Framework from PASO

Updated:  Jan 5, 2001    (HM)
          QA. 

Updated:  May 5, 2004    (Sven Menke)
	  base class changed from algo to tool

Updated:  June, 2004    (sss)
          Use ToolWithConstants to get correction constants.

********************************************************************/


#include "CaloClusterCorrection/CaloClusterCorrection.h"
#include "CaloConditions/Array.h"
#include <vector>

class CaloSwPhimod_g3 : public CaloClusterCorrection
{
public:
  /// Inherit constructor.
  using CaloClusterCorrection::CaloClusterCorrection;

  // virtual method in CaloClusterCorrection
  virtual void makeCorrection (const Context& myctx,
                               xAOD::CaloCluster* cluster) const override;

 private:
  CaloSwPhimod_g3() = delete;
  std::vector<float> qphmop (const Context& myctx,
                             float eta) const;

  Constant<CxxUtils::Array<2> > m_correction { this, "correction", "" };
};

#endif
