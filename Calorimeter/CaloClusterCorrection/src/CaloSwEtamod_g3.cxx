/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

/********************************************************************

NAME:     CaloSwEtamod_g3.cxx
PACKAGE:  offline/Calorimeter/CaloClusterCorrection

AUTHORS:  H. Ma, S. Rajagopalan
CREATED:  Dec. 15, 1999

PURPOSE:  correction for the modulations with the position in eta
          (Tuned using 50 GeV Et photons)  
          base class: CaloClusterCorrection
          Correction tuned on G3 samples.

Atrecon Orig: emreco/qetamod.F

Updated:  May 10, 2000    (SR, HM)
          Migrated to Athena Framework from PASO

Updated:  Jan 5, 2001    (HM)
          QA. 

Updated:  May 5, 2004    (Sven Menke)
	  base class changed from algo to tool

Updated:  June, 2004    (sss)
          Use ToolWithConstants to get correction constants.

********************************************************************/
// include header files 
#include "CaloSwEtamod_g3.h"


using xAOD::CaloCluster;


// make correction to one cluster 
void CaloSwEtamod_g3::makeCorrection (const Context& myctx,
                                      CaloCluster* cluster) const
{
  float aeta = fabs (cluster->etaBE(2));
  if (aeta > 2.5) return;

  int ieta = m_neta(myctx)/2;
  if (ieta > 3) return;       // something went wrong

  CaloRec::Array<1> coef = m_correction(myctx)[aeta < 1.5 ? 0 : 1][ieta-1];

  float u = fmod (aeta, 0.025); 
  float qetamod = coef[0] + coef[1]*u + coef[2]*u*u;

  setenergy (cluster, qetamod * cluster->e());
}

