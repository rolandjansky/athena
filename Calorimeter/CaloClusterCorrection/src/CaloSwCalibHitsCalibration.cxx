/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

/********************************************************************

NAME:     CaloSwCalibHitsCalibration.cxx
PACKAGE:  offline/Calorimeter/CaloSwCalibHitsCalibration

AUTHORS:  L. Carminati
CREATED:  January 4, 2007

PURPOSE:  Calibrate SW em clusters with coefficients extracted
          from Calibration Hits.
          Calibration constants derived by D. Banfi, L.C., L.
	  Mandelli and E. Nebot Del Busto
          base class: CaloClusterCorrectionCommon
          Uses ToolWithConstants to get correction constants

Updated:  January 17, 2008 (LC)
          Latest parameterizations from E. Nebot Del Busto for the
          endcap calibration

********************************************************************/

#include "CaloSwCalibHitsCalibration.h"
#include "CLHEP/Units/SystemOfUnits.h"


using xAOD::CaloCluster;
using CLHEP::TeV;


/**
  * @brief Virtual function for the correction-specific code.
  * @param myctx   ToolWithConstants context.
  * @param cluster The cluster to correct.
  *                It is updated in place.
  * @param elt     The detector description element corresponding
  *                to the cluster location.
  * @param eta     The @f$\eta@f$ coordinate of the cluster, in this sampling.
  * @param adj_eta The @f$\eta@f$ adjusted for
  *                any shift between the actual and nominal coordinates.
  *                (This is shifted back to the nominal coordinate system.)
  * @param phi     The @f$\phi@f$ coordinate of the cluster, in this sampling.
  * @param adj_phi The @f$\phi@f$ adjusted for
  *                any shift between the actual and nominal coordinates.
  *                (This is shifted back to the nominal coordinate system.)
  * @param samp    The calorimeter sampling we're examining.
  *                This is a sampling code as defined by
  *                @c CaloSampling::CaloSample; i.e., it has both
  *                the calorimeter region and sampling encoded.
  */


void CaloSwCalibHitsCalibration::makeTheCorrection
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
   float the_aeta;
   if (m_use_raw_eta (myctx))
     the_aeta = std::abs (adj_eta);
   else
     the_aeta = std::abs (eta);

   const float etamax = m_etamax (myctx);
   if (the_aeta >= etamax) return;

   const float eta_start_crack = m_eta_start_crack (myctx);
   const float eta_end_crack = m_eta_end_crack (myctx);
   int si;
   if (the_aeta < eta_start_crack)
     si = 0;
   else if (the_aeta > eta_end_crack)
     si = 1;
   else {
     // No corrections are applied for the crack region.
     return;
   }

   const CxxUtils::Array<3> correction = m_correction (myctx);

//   the_aeta = std::abs (cluster->eta() );


   ATH_MSG_DEBUG(  "************************************************************************************************" << endmsg);
   ATH_MSG_DEBUG(  " USING CALIBHITS CALIBRATION " << endmsg);
   ATH_MSG_DEBUG(  " Tool Name   " << name() << endmsg);
   ATH_MSG_DEBUG(  "************************************************************************************************" << endmsg);  	

   unsigned int shape[] = {2};
   CaloRec::WritableArrayData<1> interp_barriers (shape);
   interp_barriers[0] = eta_start_crack;
   interp_barriers[1] = eta_end_crack;

   int ibin = (static_cast<int> (the_aeta / etamax * 100)) ;

   int ibin_frontCorr = ibin;
   if (m_fix_v6_pathologies (myctx)) {
     if (the_aeta>2.35) ibin_frontCorr = (static_cast<int> (2.35 / etamax * 100)) ;
   }

// -------------------------------------------------------------
// Load calibration coefficients
// -------------------------------------------------------------

   CaloRec::Array<1> acc      = correction[0][ibin];
   CaloRec::Array<1> ooc      = correction[1][ibin];
   CaloRec::Array<1> lleak    = correction[2][ibin];
   CaloRec::Array<1> froffset = correction[3][ibin_frontCorr];	
   CaloRec::Array<1> frslope  = correction[4][ibin_frontCorr];
   CaloRec::Array<1> sec      = correction[5][ibin_frontCorr];

   ATH_MSG_DEBUG( "Check etas -------------------------------------------------------------------"<< endmsg);
   ATH_MSG_DEBUG( "Eta --> " << the_aeta << "  Bin --> " << ibin <<" Cluster eta = " << cluster->eta() << endmsg);
   ATH_MSG_DEBUG( "ETA = " << std::abs (adj_eta) << "    ADJ_ETA = " << std::abs (eta)<<  endmsg);

   ATH_MSG_DEBUG( "Check calibration coefficients -----------------------------------------------"<< endmsg);
   ATH_MSG_DEBUG( "Accordion   :  " << acc[0] <<"  " << acc[1]
          << " "  << acc[2] << " "  << acc[3] << endmsg);
   ATH_MSG_DEBUG( "OutOfCOne   :  " << ooc[0] <<"  " << ooc[1]
          << " "  << ooc[2] << " "  << ooc[3] << endmsg);
   ATH_MSG_DEBUG( "Leakage     :  " << lleak[0] <<"  " << lleak[1]
          << " " << lleak[2] << " "  << lleak[3] << endmsg);
   ATH_MSG_DEBUG( "Front offset:  " << froffset[0] <<"  " 
          <<froffset[1]  << " " << froffset[2] << " "  << froffset[3] <<endmsg);
   ATH_MSG_DEBUG( "Front Slope  :  " << frslope[0] <<"  " << frslope[1]
          << " " << frslope[2] << " "  << frslope[3] <<  endmsg);
   ATH_MSG_DEBUG( "Second order:  " << sec[0] << "  " << sec[1]
          << "  "  << sec[2] << " " << sec[3] << endmsg);

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

   // Compute longitudinal barycenter: this is a very old and approximate
   // parametrization that needs to be updated.
   double shower_lbary = m_showerDepth.depth (the_aeta,
                                              eta_start_crack,
                                              eta_end_crack,
                                              m_sampling_depth(myctx),
                                              etamax,
                                              cluster, msg() );
   if (shower_lbary == 0) return;

   if (shower_lbary < 5. || shower_lbary > 25.) {
    shower_lbary =15.;
    ATH_MSG_DEBUG( " replace pathological depth by 15 X0" << endmsg); 
   }

   // Compute total energy in the accordion (eacc_base) and
   // presampler (eps_base)

   double eacc_base = 0;
   for (int sampling=1; sampling<4; sampling++) {
       eacc_base += cluster->eSample(samps[si][sampling]);
       ATH_MSG_DEBUG( "Barrel/endcap = " << si << "  Sampling = " <<   sampling << "   Energy -->> " <<  cluster->eSample(samps[si][sampling]) << endmsg);
   }
   double eps_base = cluster->eSample (samps[si][0]);

   ATH_MSG_DEBUG( "E accordion base --->>>> "  << eacc_base <<  "  Eps base " << eps_base << endmsg);

   // Add a protection against large longitudinal barycenter for clusters from
   // hadrons which may cause over-calibration. A energy dependent upper limit
   // on the long bary is introduced (from 20 at 0 to 23 at 1 TeV)    

   double depth_max = 20. + (eacc_base+eps_base)*(3./TeV) ;
   ATH_MSG_DEBUG( "Raw energy ---->> " << (eacc_base+eps_base) << endmsg) ;
   ATH_MSG_DEBUG( "Bary max for this event ---->> " << depth_max
       << endmsg) ;

   if ( shower_lbary > depth_max ) {
     shower_lbary = 15.;
     //shower_lbary = depth_max;
     ATH_MSG_DEBUG( " replace pathological depth by 15 X0 " 
         << endmsg); 
   } 



// -------------------------------------------------------------
// Now calibrate the accordion
// -------------------------------------------------------------

   double acc_corr    =
     acc[1] + acc[2] * shower_lbary + acc[3] * shower_lbary * shower_lbary ;
 
   double e_out_perc = 0;
   if (the_aeta < eta_start_crack) {
       e_out_perc = ooc[1] + ooc[2] * shower_lbary + ooc[3] * shower_lbary * shower_lbary ;
   }
   else if (the_aeta > eta_end_crack) {
       e_out_perc = ooc[1] + ooc[2] * shower_lbary + ooc[3] / shower_lbary ;
   }   

   double e_acc_reco=acc_corr*(eacc_base )*(1+(e_out_perc)*0.01);

// -------------------------------------------------------------
// Now estimate the longitudinal leakage
// -------------------------------------------------------------

   double e_leak_perc = 0;
/*
   if (the_aeta < eta_start_crack) { 
       e_leak_perc = lleak[1] + lleak[2] * shower_lbary + lleak[3] * exp(shower_lbary);
   } else if (the_aeta > eta_end_crack) {
       e_leak_perc = lleak[1] * shower_lbary + lleak[2] * exp(shower_lbary);
   }
*/
   e_leak_perc = lleak[3] + lleak[1] * shower_lbary + lleak[2] * exp(shower_lbary);

   if (e_leak_perc < 0 ) e_leak_perc = 0.;
   if (e_leak_perc > 100.) e_leak_perc = 100.;
   double e_leak_reco = e_leak_perc * (e_acc_reco)*0.01;

// -------------------------------------------------------------
// Now estimate the energy lost in front of the calorimeter
// -------------------------------------------------------------

 //  raw_energy = e_acc_reco ;

   double raw_energy=e_acc_reco*1e-3;
   double e_front_reco = eps_base;

   if (raw_energy <= 1) {
     // Protect against log() going negative in pow();
     // also protect against sqrt() of a negative number and div-by-zero.
   }
   else if (the_aeta < 1.8) {

     if (the_aeta < eta_start_crack) {
       double WpsOff      = froffset[1] + froffset[2]     * raw_energy + 
                            froffset[3] * raw_energy * raw_energy;
       double WpsSlo      = frslope[1]  * pow(log(raw_energy),
                                              static_cast<double>(frslope[2])) +
                            frslope[3] *sqrt( raw_energy );
       e_front_reco=WpsOff + WpsSlo*(eps_base );

       ATH_MSG_DEBUG( " raw event  " << raw_energy << endmsg);
       ATH_MSG_DEBUG( " froffset coeff " << froffset[1] << " " << froffset[2] << " " << froffset[3]  << endmsg);
       ATH_MSG_DEBUG( " frslope coeff  " << frslope[1] << " " << frslope[2] << " " << frslope[3] << endmsg);
       ATH_MSG_DEBUG( " WpsOff,WpsSlo " << WpsOff << " " << WpsSlo << endmsg);
        ATH_MSG_DEBUG( " eps_base, efront_reco " << eps_base << " " << e_front_reco << endmsg);
     }
     else{

       if (raw_energy<20.) raw_energy=20.;

       double WpsOff      = froffset[1] + froffset[2]     * raw_energy + 
                            froffset[3] * sqrt(raw_energy);
       double WpsSlo      = frslope[1]  + frslope[2] * log(raw_energy) +
                            frslope[3] * sqrt(raw_energy);
       double WpsSlo2     = sec[1]  + sec[2] * raw_energy -
                            sec[3] / (raw_energy * raw_energy) ;
       e_front_reco=WpsOff + WpsSlo*(eps_base) + WpsSlo2*(eps_base)*(eps_base);
       if (e_front_reco<0.) e_front_reco= eps_base;
          
       ATH_MSG_DEBUG( " raw energy " << raw_energy << endmsg);
       ATH_MSG_DEBUG( "p1 " << froffset[1] << " "
              << froffset[2] << " " << froffset[3] <<  " " << WpsOff << endmsg);
       ATH_MSG_DEBUG( "p2 " << frslope[1] << " "
              << frslope[2] << " " << frslope[3] << " " << WpsSlo << endmsg);
       ATH_MSG_DEBUG( "p3 " << sec[1] << " "
              << sec[2] << " " << sec[3] << " " << WpsSlo2 << endmsg);
       ATH_MSG_DEBUG( " WpsOff, WpsSlo, WpsSlo2 " << WpsOff << " " << WpsSlo << " " << WpsSlo2 << endmsg);
       ATH_MSG_DEBUG( " eps_base, efront_reco " << eps_base << " " <<  e_front_reco << endmsg);

     }

   }
   else {

     double p1 =  froffset[1] + froffset[2]     * raw_energy  +
                  froffset[3]* raw_energy  * raw_energy ;
     double p2 =  frslope[1]  + frslope[2]      * raw_energy  +
                  frslope[3] * raw_energy  * raw_energy ;
     double p3 =  sec[1]      + sec[2]          * raw_energy  +
                  sec[3] * raw_energy  * raw_energy ;

     ATH_MSG_DEBUG( "p1 " << froffset[1] << " "
            << froffset[2] << " " << froffset[3] << endmsg);
     ATH_MSG_DEBUG( "p2 " << frslope[1] << " "
            << frslope[2] << " " << frslope[3] << endmsg);
     ATH_MSG_DEBUG( "p3 " << sec[1] << " "
            << sec[2] << " " << sec[3] << endmsg);

     e_front_reco= (p1 + p2 * shower_lbary + p3 * shower_lbary * shower_lbary);
     if (e_front_reco<0.) e_front_reco=eps_base;
   }

// -------------------------------------------------------------
// Now compute the total energy and finally update the cluster energies
// -------------------------------------------------------------

   double e_calo_reco =e_front_reco + e_leak_reco + e_acc_reco;

   ATH_MSG_DEBUG( "CaloSwCalibrationHits::Final reco energy ---------------------- " << e_calo_reco << endmsg);
   ATH_MSG_DEBUG( "CaloSwCalibrationHits::Front ---------------------- " <<  e_front_reco << endmsg);
   ATH_MSG_DEBUG( "CaloSwCalibrationHits::Accordion ------------------ " <<  e_acc_reco << endmsg);
   ATH_MSG_DEBUG( "CaloSwCalibrationHits::out of cone ---------------- " <<  acc_corr*(eacc_base )*(e_out_perc)*0.01 << endmsg);      
   ATH_MSG_DEBUG( "CaloSwCalibrationHits::Leakage -------------------- " <<  e_leak_reco << endmsg);
   
   

/*
 -------------------------------------------------------------
 Conventions discussed in Dec 2007 larg week to fill energies in the samplings:
 E = total energy including all corrections
 E0 = total energy in front of the accordion :
        presampler energy + energy lost in front + energy lost between PS and strips
 E1 = energy in the strips with no out of cone corrections
 E2 = energy in the middle with no out of cone corrections
 E3 = energy in the back with no out of cone correction + longitudinal leakage 

 Please note that E != E0+E1+E2+E3 
  -------------------------------------------------------------
*/
   if (m_updateSamplingEnergies (myctx))
   {
       // presampler

       cluster->setEnergy (samps[si][0], e_front_reco );

       // strips and middle

       for (int sampling=1; sampling<=2; sampling++){
	   cluster->setEnergy (samps[si][sampling], cluster->eSample(samps[si][sampling]) * acc_corr );
       }
       
       // back
       
       cluster->setEnergy (samps[si][3], cluster->eSample (samps[si][3]) * acc_corr + e_leak_reco);
   }
// total energy
   
   double e_temp = 0;
   for (int nl = 0 ; nl< 4 ; nl++) e_temp +=  cluster->eSample (samps [si][nl]);
   
   ATH_MSG_DEBUG( "----------  Sum of the sampling energy ---  >> " << e_temp << "  EcaloReco = " << e_calo_reco << endmsg);

   cluster->setE (e_calo_reco);
   
   ATH_MSG_DEBUG( "CaloSwCalibHitsCalibration::Energy after  correction --> " <<  cluster->e() << endmsg);

}

