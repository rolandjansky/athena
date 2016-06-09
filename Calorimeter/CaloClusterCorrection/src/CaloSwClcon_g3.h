/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
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

  // constructor 
  CaloSwClcon_g3(const std::string& type,
                 const std::string& name,
                 const IInterface* parent);
  // Destructor 
  virtual ~CaloSwClcon_g3() override;
  
  // CaloClusterCorrection virtual method
  virtual void makeCorrection(const EventContext& ctx,
                              xAOD::CaloCluster* cluster) const override;

  /// Currently at version==1.
  virtual int version() const;
  
 private:
  
  CaloSwClcon_g3() = delete;

  // constants : etamin and etamax are the eta range covered
  //             by the fitted corrections => nothing to do with
  //             DetectorDescription.
  float             m_etamin;
  float             m_etamax;
  CaloRec::Array<1> m_correction;
};


#endif

