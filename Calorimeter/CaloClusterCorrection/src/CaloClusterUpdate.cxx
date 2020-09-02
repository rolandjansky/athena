/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

/********************************************************************

NAME:     CaloClusterUpdate.cxx
PACKAGE:  offline/LArCalorimeter/LArClusterRec

AUTHORS:  H. Ma, S. Rajagopalan
CREATED:  Nov, 2000

PURPOSE:  Recalculate the total energy, eta,phi of a cluster.
	  This should be called after all corrections to individual 
	  samplings are done. 

	  energy = Sum of energy in all sampling
	  eta = average of eta1 and eta2, weighted by energy and
                relative resolution.  
		This needs to be tuned. 
	  phi    = phi of second sampling. 

Base class: LArClusterCorrection (Algorithm)

Atrecon Orig: emreco/qetamod.F

Updated:  May 10, 2000    (SR, HM)
          Migrated to Athena Framework from PASO

Updated:  Jan 5, 2001    (HM)
          QA. 

Updated:  Feb 28, 2003, Mar 4, 2003    (MW)
          protect against unphysical clusters

Updated:  May 5, 2004    (Sven Menke)
	  base class changed from algo to tool

********************************************************************/
// include header files
#include "CaloClusterUpdate.h"

#include "GaudiKernel/MsgStream.h"
#include "CaloGeoHelpers/proxim.h" 
#include "CaloGeoHelpers/CaloPhiRange.h"

using xAOD::CaloCluster;
void CaloClusterUpdate::makeCorrection (const Context& myctx,
                                        CaloCluster* cluster) const
{
  float energy=0; 
  float eta=0; 
  float phi2=0; 
  float weta=0; 
  float wphi=0; 
  // set eta to be weighted average of eta1 and eta2 
 
  ATH_MSG_DEBUG(" inBarrel "<<cluster->inBarrel() << " inEndcap "<<cluster->inEndcap() << endmsg) ;

  for(int i=0; i<5; i=i+4 )
  { 
  
   if(i==0 && ! cluster->inBarrel() ) continue; 
   if(i==4 && ! cluster->inEndcap() ) continue; 

   CaloSampling::CaloSample sam0 = (CaloSampling::CaloSample)(CaloSampling::PreSamplerB+i);   
   CaloSampling::CaloSample sam1 = (CaloSampling::CaloSample)(CaloSampling::PreSamplerB+i+1);   
   CaloSampling::CaloSample sam2 = (CaloSampling::CaloSample)(CaloSampling::PreSamplerB+i+2);   
   CaloSampling::CaloSample sam3 = (CaloSampling::CaloSample)(CaloSampling::PreSamplerB+i+3);

   float eta1 = cluster->etaSample(sam1);
   float eta2 = cluster->etaSample(sam2);
   
   float e0 = cluster->eSample(sam0);
   float e1 = cluster->eSample(sam1);
   float e2 = cluster->eSample(sam2);
   float e3 = cluster->eSample(sam3);

   // total energy is the sum of each sampling, which had all corrections
   energy += (e0 + e1 + e2 + e3) ; 

   if (eta2 == -999.) continue;

   // eta1 has better resolution.  so weight it differently. 
   float w1 = e1*3. ; 
   float w2 = e2    ; 

   if (e1 <= 0 && e2 <= 0) {
     eta1 = cluster->eta0();
     w1 = std::abs(energy);
     w2 = 0;
   }
   else if (e1 <= 0) {
     w1 = 0;
   }
   else if (e2 <= 0) {
     w2 = 0;
   }

   eta   += (eta1*w1+eta2*w2) ;
   weta  += w1+w2; 

   // phi2 for phi. 
   if(wphi!=0) { 
    float phi_old = phi2/wphi; 
    float phi_new = cluster->phiSample(sam2); 
    phi_new = proxim ( phi_new,phi_old ); 
    phi2 += e2*phi_new; 
    wphi += e2; 
   } 
   else {
    phi2 += e2*cluster->phiSample(sam2); 
    wphi += e2;     
   }
  // do some checks for unphysical cluster
   if (cluster->phiSample(sam2) == -999.) phi2 = -999.;
   if (cluster->etaSample(sam2) == -999. ||
       cluster->etaSample(sam1) == -999.) eta  = -999.;
  }

  //
  // set them in the cluster. 
  //

  if (eta  != -999. && weta != 0)
    eta  = eta/weta; 
  else {
      if (cluster->inBarrel() && ! cluster->inEndcap()) eta = cluster->etaSample(CaloSampling::EMB2);      
      else if (cluster->inEndcap() && !cluster->inBarrel()) eta = cluster->etaSample(CaloSampling::EME2);      
      else {
        if (cluster->eSample(CaloSampling::EMB2) > cluster->eSample(CaloSampling::EME2))  eta = cluster->etaSample(CaloSampling::EMB2);
        else eta = cluster->etaSample(CaloSampling::EME2);
      }
  }

  if (phi2 != -999. && wphi != 0) {
    phi2 = phi2/wphi ; 
    phi2 = CaloPhiRange::fix (phi2);
  }
  else {
      if (cluster->inBarrel() && ! cluster->inEndcap()) phi2 = cluster->phiSample(CaloSampling::EMB2);      
      else if (cluster->inEndcap() && !cluster->inBarrel()) phi2 = cluster->phiSample(CaloSampling::EME2);      
      else {  
        if (cluster->eSample(CaloSampling::EMB2) > cluster->eSample(CaloSampling::EME2))  phi2 = cluster->phiSample(CaloSampling::EMB2);
        else phi2 = cluster->phiSample(CaloSampling::EME2);
      }

  }

  cluster->setEta(eta); 
  cluster->setPhi(phi2); 

  if (m_update_energy (myctx))
    cluster->setE(energy); 
}


