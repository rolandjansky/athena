/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/********************************************************************

NAME:     CaloSwDeadOTX_back.cxx
PACKAGE:  offline/Calorimeter/CaloSwCalibHitsCalibration

AUTHORS:  L. Carminati
CREATED:  March 26, 2010

PURPOSE:  Apply specif calibration for the energy in the back 
          compartment and longitudinal leakage when there's a
          dead OTX 
          Calibration constants derived by D. Banfi
          base class: CaloClusterCorrectionCommon
          Uses ToolWithConstants to get correction constants

********************************************************************/

#include "CaloSwDeadOTX_back.h"
#include "CLHEP/Units/SystemOfUnits.h"

using xAOD::CaloCluster;
using CLHEP::TeV;

// -------------------------------------------------------------
// Constructor
// -------------------------------------------------------------

CaloSwDeadOTX_back::CaloSwDeadOTX_back
  (const std::string& type,
   const std::string& name,
   const IInterface* parent)
   : CaloClusterCorrectionCommon(type, name, parent) , 
         m_affectedTool("CaloAffectedTool")
{
   declareConstant("correction"      , m_correction);
   declareConstant("sampling_depth"  , m_sampling_depth);
   declareConstant("eta_start_crack" , m_eta_start_crack);
   declareConstant("eta_end_crack"   , m_eta_end_crack);
   declareConstant("etamax"          , m_etamax);
   declareConstant("use_raw_eta"     , m_use_raw_eta);
   declareProperty("affectedTool"    , m_affectedTool); 
}

StatusCode CaloSwDeadOTX_back::initialize()
{
  CHECK( CaloClusterCorrectionCommon::initialize() );

  // Need the explicit std::string here to avoid matching
  // the template in AlgTool.
  //  CHECK( setProperty ("isDummy", std::string("1")) );

  
  ATH_MSG_DEBUG( " --------------->>>>> CaloSwDeadOTX_back :: retrieving affectedTool" << endreq);

  if (m_affectedTool.retrieve().isFailure()){
      ATH_MSG_DEBUG( " --------------->>>>> CaloSwDeadOTX_back :: failure retrieving affectedTool !! " << endreq);
      return StatusCode::FAILURE;
  } 

  ATH_MSG_DEBUG( " --------------->>>>> CaloSwDeadOTX_back :: affectedTool successfully retrieved" << endreq);

  return StatusCode::SUCCESS;;
}

void CaloSwDeadOTX_back::makeTheCorrection
   (const EventContext& /*ctx*/,
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

   float the_aeta;
   if (m_use_raw_eta)
      the_aeta = std::abs (adj_eta);
   else
      the_aeta = std::abs (eta);
	
   if (the_aeta >= m_etamax) return;
	
   int si;
   if (the_aeta < m_eta_start_crack)
      si = 0;
   else if (the_aeta > m_eta_end_crack)
      si = 1;
   else {
   // No corrections are applied for the crack region.
      return;
   } 


   ATH_MSG_DEBUG(  "************************************************************************************************" << endreq);
   ATH_MSG_DEBUG(  " USING CALIBHITS CALIBRATION : apply correction for dead OTX in the back" << endreq);
   ATH_MSG_DEBUG(  " Tool Name   " << name() << endreq);
   ATH_MSG_DEBUG(  "************************************************************************************************" << endreq);  	

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

// Keep memory of the energy stored in the back

   float eneBackold =  cluster->eSample (samps[si][3]) ;

// Set the safety margin around the dead region     
   float deta = 0.025;
   float dphi = 0.025;     

   int layer = si * 4 + 3 ;
   bool check = m_affectedTool->isAffected(cluster ,deta , dphi ,layer,layer,0) ; //  check dead OTX in barrel PS 

// bool checke = m_affectedTool->isAffected(cluster ,deta , dphi ,4,4,0) ; //  check dead OTX in endcap PS 
// bool checkb = m_affectedTool->isAffected(cluster ,deta , dphi ,0,0,0) ; //  check dead OTX in barrel PS 
 
// if a cluster is in an affected region set the back energy to zero else return

   if (check) { 
       ATH_MSG_DEBUG(  "The cluster is in an affected region, apply corrections" << endreq);
       cluster->setEnergy (samps[si][3], 0 );
   } else { 
       ATH_MSG_DEBUG(  "The cluster is NOT in an affected region, skip correction" << endreq);
       return; 
   }

// -------------------------------------------------------------
// If the cluster is in an affected region load the corrections
// -------------------------------------------------------------

// Determine the eta bin for the correction
   int ibin = (static_cast<int> (the_aeta / m_etamax * 100)) ;  

// Load the correction coefficients 
   CaloRec::Array<1> lleak    = m_correction[0][ibin];

// -------------------------------------------------------------
// Compute longitudinal barycenter: this is a very old and approximate
// parametrization that needs to be updated.
// -------------------------------------------------------------

   double raw_energy =0;
   double shower_lbary_raw =0;
   for (int nl = 0 ; nl< 4 ; nl++){
      raw_energy +=  cluster->eSample (samps[si][nl]);
      shower_lbary_raw += (m_sampling_depth[ibin][nl+1] * cluster->eSample (samps[si][nl])) ;
   }
	       
   if (raw_energy == 0) return;
   double shower_lbary = shower_lbary_raw / raw_energy;
	
   ATH_MSG_DEBUG( "Shower longitudinal barycenter ---->> " << shower_lbary << endreq);
   
   double depth_max = 20. + raw_energy*(3./TeV) ;

   if ( shower_lbary > depth_max   || shower_lbary < 0.) {
     shower_lbary = 15.;
     ATH_MSG_DEBUG( " replace pathological depth by 15 X0 "
         << endreq);
   }


   raw_energy = raw_energy * 1e-3;

// -------------------------------------------------------------
// Estimate the energy deposited in the back sampling and the 
// longitudinal leakage based on the longitudinal barycenter   
// -------------------------------------------------------------
   
   double e_acc_reco = 0;
   for (int nl = 1 ; nl< 4 ; nl++){
      e_acc_reco +=  cluster->eSample (samps[si][nl]);
   }

   ATH_MSG_DEBUG( "Leakage     :  " << lleak[0] <<"  " << lleak[1]
          << " " << lleak[2] << " "  << lleak[3] << endreq);

   double e_leak_perc = lleak[3] + lleak[1] * shower_lbary + lleak[2] * exp(shower_lbary);

   ATH_MSG_DEBUG( "LongLeakage correction --> " << e_leak_perc << endreq);
    
   if (e_leak_perc < 0 ) e_leak_perc = 0.;
   if (e_leak_perc > 100.) e_leak_perc = 100.;

   double e_leak_reco = e_leak_perc * (e_acc_reco)*0.01; 
 
// If something goes wrong keep the old energy
   if (e_leak_reco<0.) e_leak_reco= eneBackold; 

// -------------------------------------------------------------
// Now set the energies of the cluster  
// -------------------------------------------------------------

// set the energy of the presampler
   cluster->setEnergy (samps[si][3], e_leak_reco );
   
// take the cluster energy, subtract the old PS energy and add the new one
 
   ATH_MSG_DEBUG( "CaloSwDeadOTX_back:: cluster energy before correction --> " <<  cluster->e() << endreq);
   ATH_MSG_DEBUG( "subtracting back energy -->> " << eneBackold << endreq);
   ATH_MSG_DEBUG( "adding new back energy  -->> " << e_leak_reco << endreq);

   float e_temp = cluster->e() - eneBackold + e_leak_reco ; 

// set the energy of the cluster
   cluster->setE (e_temp); 
 
   ATH_MSG_DEBUG( "CaloSwDeadOTX_back::Energy after  correction --> " <<  cluster->e() << endreq); 

}

