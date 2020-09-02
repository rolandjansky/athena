/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#ifndef CALOCLUSTERCORRECTION_CALOSWCLCON_G3_H
#define CALOCLUSTERCORRECTION_CALOSWCLCON_G3_H
/********************************************************************

NAME:     CaloSwClcon_g3.h
PACKAGE:  offline/Calorimeter/CaloClusterCorrection

AUTHORS:  S. Rajagopalan
CREATED:  Dec. 15, 1999

PURPOSE:  correction for the finite containment for a given cluster size
          (Tuned using 50 GeV Et photons)
	  Base class: CaloClusterCorrection
          Correction tuned on G3 samples.

Atrecon Orig: emrecon/qclcon.F
          
Updated:  May 10, 2000    (SR, HM)
          Migrated to Athena Framework from PASO

Updated:  Jan 5, 2001    (HM)
          QA. 

Updated:  May 5, 2004    (Sven Menke)
	  base class changed from algo to tool

Updated:  June, 2004    (sss)
          Use ToolWithConstants to get correction constants.

********************************************************************/

// INCLUDE HEADER FILES:
#include "CaloClusterCorrection/CaloClusterCorrection.h"
#include "CaloConditions/Array.h"

class CaloSwClcon_g3 : public CaloClusterCorrection
{

 public:
  /// Inherit contructor.
  using CaloClusterCorrection::CaloClusterCorrection;


  // CaloClusterCorrection virtual method
  virtual void makeCorrection (const Context& myctx,
                               xAOD::CaloCluster* cluster) const override;

  /// Currently at version==1.
  virtual int version() const;
  
 private:
  
  CaloSwClcon_g3() = delete;

  // constants : etamin and etamax are the eta range covered
  //             by the fitted corrections => nothing to do with
  //             DetectorDescription.
  Constant<float>               m_etamin     { this, "etamin",     "" };
  Constant<float>               m_etamax     { this, "etamax",     "" };
  Constant<CxxUtils::Array<1> > m_correction { this, "correction", "" };
};


#endif

