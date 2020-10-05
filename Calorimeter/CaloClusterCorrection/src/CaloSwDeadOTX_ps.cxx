/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

/********************************************************************

NAME:     CaloSwDeadOTX_ps.cxx
PACKAGE:  offline/Calorimeter/CaloSwCalibHitsCalibration

AUTHORS:  L. Carminati
CREATED:  March 26, 2010

PURPOSE:  Apply specif calibration for the energy lost in front
          when the PS is missing 
          Calibration constants derived by D. Banfi
          base class: CaloClusterCorrectionCommon
          Uses ToolWithConstants to get correction constants

********************************************************************/

#include "CaloSwDeadOTX_ps.h"
#include "CLHEP/Units/SystemOfUnits.h"

using xAOD::CaloCluster;
using CLHEP::TeV;


StatusCode CaloSwDeadOTX_ps::initialize()
{
  CHECK( CaloClusterCorrectionCommon::initialize() );

  // Need the explicit std::string here to avoid matching
  // the template in AlgTool.
  //  CHECK( setProperty ("isDummy", std::string("1")) );

  ATH_CHECK(m_affKey.initialize());

  ATH_MSG_DEBUG( " --------------->>>>> CaloSwDeadOTX_ps :: retrieving affectedTool" << endmsg);

  if (m_affectedTool.retrieve().isFailure()){
      ATH_MSG_DEBUG( " --------------->>>>> CaloSwDeadOTX_ps :: failure retrieving affectedTool !! " << endmsg);
      return StatusCode::FAILURE;
  } 

  ATH_MSG_DEBUG( " --------------->>>>> CaloSwDeadOTX_ps :: affectedTool successfully retrieved" << endmsg);

  return StatusCode::SUCCESS;
}

void CaloSwDeadOTX_ps::makeTheCorrection
   (const Context& myctx,
    CaloCluster* cluster,
    const CaloDetDescrElement* /*elt*/,
    float eta,
    float adj_eta,
    float /*phi*/,
    float /*adj_phi*/,
    CaloSampling::CaloSample /*samp*/) const
{
// ??? In principle, we should use adj_eta for the interpolation
//     and range checks.  However, the v2 corrections were derived
//     using regular eta instead.

   //Get affected info for this event
   SG::ReadCondHandle<CaloAffectedRegionInfoVec> affHdl{m_affKey, myctx.ctx()};
   const CaloAffectedRegionInfoVec* affCont=*affHdl;
   if(!affCont) {
     ATH_MSG_WARNING("Do not have affected regions info, is this expected ?");
   }

   float the_aeta;
   if (m_use_raw_eta(myctx))
      the_aeta = std::abs (adj_eta);
   else
      the_aeta = std::abs (eta);
	
  const float etamax = m_etamax (myctx);
   if (the_aeta >= etamax) return;
	
   int si;
   if (the_aeta < m_eta_start_crack(myctx))
      si = 0;
   else if (the_aeta > m_eta_end_crack(myctx))
      si = 1;
   else {
   // No corrections are applied for the crack region.
      return;
   } 

   const CxxUtils::Array<3> correction = m_correction (myctx);

   ATH_MSG_DEBUG(  "************************************************************************************************" << endmsg);
   ATH_MSG_DEBUG(  " USING CALIBHITS CALIBRATION : apply correction for dead OTX in the presampler" << endmsg);
   ATH_MSG_DEBUG(  " Tool Name   " << name() << endmsg);
   ATH_MSG_DEBUG(  "************************************************************************************************" << endmsg);  	

// -------------------------------------------------------------
// check if the cluster is in an affected region
// -------------------------------------------------------------

   static const CaloSampling::CaloSample samps[2][4] = {
      { CaloSampling::PreSamplerB,
        CaloSampling::EMB1,
        CaloSampling::EMB2,
        CaloSampling::EMB3 },
      { CaloSampling::PreSamplerE,
        CaloSampling::EME1,
        CaloSampling::EME2,
        CaloSampling::EME3 }
   };

// Keep memory of the energy in front from the standard correction

   float enePSold =  cluster->eSample (samps[si][0]) ;
  

// Set the safety margin around the dead region     
   float deta = 0.025;
   float dphi = 0.025;     

   int layer = si * 4 ;
   bool check = m_affectedTool->isAffected(cluster, affCont, deta , dphi ,layer,layer,0) ; //  check dead OTX in barrel PS 

 
// if a cluster is in an affected region set the PS energy to zero else return

   if (check) { 
       ATH_MSG_DEBUG(  "The cluster is in an affected region, apply corrections" << endmsg);
       cluster->setEnergy (samps[si][0], 0 );
   } else { 
       ATH_MSG_DEBUG(  "The cluster is NOT in an affected region, skip correction" << endmsg);
       return; 
   }

// -------------------------------------------------------------
// If the cluster is in an affected region load the corrections
// -------------------------------------------------------------

// Determine the eta bin for the correction
   int ibin = (static_cast<int> (the_aeta / etamax * 100)) ;  

// Load the correction coefficients 
   CaloRec::Array<1> froffset = correction[0][ibin];       
   CaloRec::Array<1> frslope  = correction[1][ibin];
   CaloRec::Array<1> sec      = correction[2][ibin];   

// -------------------------------------------------------------
// Compute longitudinal barycenter: this is a very old and approximate
// parametrization that needs to be updated.
// -------------------------------------------------------------
	
   double raw_energy =0;
   double shower_lbary_raw =0;
   for (int nl = 0 ; nl< 4 ; nl++){
      raw_energy +=  cluster->eSample (samps[si][nl]);
      shower_lbary_raw += (m_sampling_depth(myctx)[ibin][nl+1] * cluster->eSample (samps[si][nl])) ;
   }
	       
   if (raw_energy == 0) return;
   double shower_lbary = shower_lbary_raw / raw_energy;
	
   ATH_MSG_DEBUG( "Shower longitudinal barycenter ---->> " << shower_lbary << endmsg);

   double depth_max = 20. + raw_energy*(3./TeV);

   if ( shower_lbary > depth_max || shower_lbary<0.) {
     shower_lbary = 15.;
     ATH_MSG_DEBUG( " replace pathological depth by 15 X0 "
         << endmsg);
   }

   raw_energy = raw_energy * 1e-3;

// -------------------------------------------------------------
// Estimate the total energy lost in front of the accordion using the 
// longitudinal barycenter   
// -------------------------------------------------------------

   double p1 = froffset[1] + froffset[2] * raw_energy + froffset[3] * raw_energy * raw_energy ; 
   double p2 = frslope[1]  + frslope[2]  * raw_energy + frslope[3]  * raw_energy * raw_energy ; 
   double p3 = sec[1]      + sec[2]      * raw_energy + sec[3]      * raw_energy * raw_energy ; 

   ATH_MSG_DEBUG( "p1 " << froffset[1] << " " << froffset[2] << " " << froffset[3] << endmsg); 
   ATH_MSG_DEBUG( "p2 " << frslope[1] << " " << frslope[2] << " " << frslope[3] << endmsg); 
   ATH_MSG_DEBUG( "p3 " << sec[1] << " " << sec[2] << " " << sec[3] << endmsg); 
  
   double e_front_reco= (p1 + p2 * shower_lbary + p3 * shower_lbary * shower_lbary); 

// If something goes wrong keep the old energy
   if (e_front_reco<0.) {
       e_front_reco= enePSold; 
       ATH_MSG_DEBUG( "CaloSwDeadOTX_ps:: new E front is negative, no correction!" << endmsg); 
   }
// -------------------------------------------------------------
// Now set the energies of the cluster  
// -------------------------------------------------------------

// set the energy of the presampler
   cluster->setEnergy (samps[si][0], e_front_reco );
   
// take the cluster energy, subtract the old PS energy and add the new one
 
   ATH_MSG_DEBUG( "CaloSwDeadOTX_ps:: cluster energy before correction --> " <<  cluster->e() << endmsg);
   ATH_MSG_DEBUG( "CaloSwDeadOTX_ps:: subtracting PS energy --> " << enePSold << endmsg);
   ATH_MSG_DEBUG( "CaloSwDeadOTX_ps:: adding new PS energy -->> " <<  e_front_reco << endmsg);
                      
   float e_temp = cluster->e() - enePSold + e_front_reco ; 

// set the energy of the cluster
   cluster->setE (e_temp); 
 
   ATH_MSG_DEBUG( "CaloSwDeadOTX_ps::Energy after  correction --> " <<  cluster->e() << endmsg); 

}

