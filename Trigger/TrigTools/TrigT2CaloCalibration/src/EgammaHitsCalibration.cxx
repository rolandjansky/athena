/*
  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
*/

// ********************************************************************
//
// NAME:     EgammaHitsCalibration.cxx
// PACKAGE:  Trigger/TrigTools/EgammaHitsCalibration.cxx
//
// AUTHOR:  Werner Spolidoro Freund (wsfreund@cern.ch)
//
// REFERENCES: Tool to provide Hits Calibration to HLT (BASED ON OFFLINE)
//
// Updated: January 14, 2011
//  Modifications on EgammaHitsCalibration updated for offline rev 30069
//
// ********************************************************************

#include "EgammaHitsCalibration.h"
//#include "TrigCaloEvent/TrigEMCluster.h"
#include "xAODTrigCalo/TrigEMCluster.h"
#include "GaudiKernel/MsgStream.h"
// Need Cosh
#include <math.h>


StatusCode EgammaHitsCalibration::initialize(){

    CHECK (base_class::initialize());

    ATH_MSG_DEBUG( "Initialize Tool : " << name()  );



    return StatusCode::SUCCESS;
}

StatusCode EgammaHitsCalibration::finalize(){
    ATH_MSG_DEBUG( "Finalize Tool : " << name()  );
    return StatusCode::SUCCESS;
}

void EgammaHitsCalibration::makeCorrection(xAOD::TrigEMCluster* clus,
    const void * /*param*/) const{

    const float etamax = m_etamax();
    const float eta_start_crack = m_eta_start_crack();
    const float eta_end_crack = m_eta_end_crack();
    const CxxUtils::Array<3> correction = m_correction();

    float the_aeta = (clus->eta());

    the_aeta = (the_aeta>0)?(the_aeta):(- the_aeta);

    if (the_aeta >= etamax) return; 

#ifndef NDEBUG
    ATH_MSG_DEBUG(  "************************************************************************************************"  );
    ATH_MSG_DEBUG(  " USING CALIBHITS CALIBRATION "  );
    ATH_MSG_DEBUG(  "************************************************************************************************"  );
#endif
    int si;
    if (the_aeta < eta_start_crack)
        si = 0; //barrel
    else if (the_aeta > eta_end_crack)
        si = 1; //endcap
    else {
        // No corrections are applied for the crack region.
        return;
    }

     
    int ibin = (static_cast<int> (the_aeta / etamax * 100));
    int ibin_frontCorr = ibin; 
    if (m_fix_v6_pathologies()) {
      if ( the_aeta>2.35) 
        ibin_frontCorr = (static_cast<int> (235. / etamax )); 
   }

#ifndef NDEBUG
        ATH_MSG_DEBUG( "Check etas -------------------------------------------------------------------" );
        ATH_MSG_DEBUG( "Eta --> " << the_aeta << "  Bin --> " << ibin <<" Cluster eta = " << clus->eta()  );
        
        ATH_MSG_DEBUG( "Check calibration coefficients -----------------------------------------------" );
        ATH_MSG_DEBUG( "Accordion   :  " << correction[0][ibin][0] <<"  " << correction[0][ibin][1] << " "  << correction[0][ibin][2] << " "  << correction[0][ibin][3]  );
        ATH_MSG_DEBUG( "OutOfCOne   :  " << correction[1][ibin][0] <<"  " << correction[1][ibin][1] << " "  << correction[1][ibin][2] << " "  << correction[1][ibin][3]  );
        ATH_MSG_DEBUG( "Leakage     :  " << correction[2][ibin][0] <<"  " << correction[2][ibin][1] << " " << correction[2][ibin][2] << " "  << correction[2][ibin][3]  );
        ATH_MSG_DEBUG( "Front offset:  " << correction[3][ibin_frontCorr][0] <<"  " <<correction[3][ibin_frontCorr][1]  << " " << correction[3][ibin_frontCorr][2] << " "  << correction[3][ibin_frontCorr][3]  );
        ATH_MSG_DEBUG( "Front Slope  :  " << correction[4][ibin_frontCorr][0] <<"  " << correction[4][ibin_frontCorr][1] << " " << correction[4][ibin_frontCorr][2] << " "  << correction[4][ibin_frontCorr][3]  );
        ATH_MSG_DEBUG( "Second order:  " << correction[5][ibin_frontCorr][0] << "  " << correction[5][ibin_frontCorr][1] << "  "  << correction[5][ibin_frontCorr][2] << " " << correction[5][ibin_frontCorr][3]  );
#endif

    EgammaHitsShowerDepth showerDepth;
    float shower_lbary = showerDepth.depth(the_aeta,
                                             eta_start_crack,
                                             eta_end_crack,
                                             m_sampling_depth(),
                                             etamax,
                                             clus);
    if (!shower_lbary)
      return;

    if (shower_lbary < 5. || shower_lbary > 25.){
      shower_lbary = 15.;
#ifndef NDEBUG
      ATH_MSG_DEBUG( "replace pathological depth by 15 X0"  );
#endif
    }

    // Compute total energy in the Accordion (eacc_base) and
    // presampler (eps_base)

    float eacc_base = 0;
    for (int sampling=1; sampling<4; sampling++) {
        eacc_base += clus->energy(EgammaHitsShowerDepth::m_samps[si][sampling]);
#ifndef NDEBUG
        ATH_MSG_DEBUG( "Barrel/endcap = " << si << "  Sampling = " <<   sampling << "   Energy -->> " <<  clus->energy(EgammaHitsShowerDepth::m_samps[si][sampling])  );
#endif
    }
    float eps_base = clus->energy (EgammaHitsShowerDepth::m_samps[si][0]);

#ifndef NDEBUG
    ATH_MSG_DEBUG( "E accordion base --->>>> "  << eacc_base <<  "  Eps base " << eps_base  );
#endif


   // Add a protection against large longitudinal barycenter for clusters from
   // hadrons which may cause over-calibration. A energy dependent upper limit
   // on the long bary is introduced (from 20 at 0 to 23 at 1 TeV)    

   float depth_max = 20. + 3. * (eacc_base+eps_base) * 1e-6; // divided by TeV
#ifndef NDEBUG
   ATH_MSG_DEBUG( "Raw energy ---->> " << (eacc_base+eps_base)  );
   ATH_MSG_DEBUG( "Bary max for this event ---->> " << depth_max  );
#endif

   if ( shower_lbary > depth_max ) {
     shower_lbary = 15.;
     //shower_lbary = depth_max;
#ifndef NDEBUG
     ATH_MSG_DEBUG( " replace pathological depth by 15 X0 " );
#endif
   } 
    // -------------------------------------------------------------
    // Now calibrate the Accordion
    // -------------------------------------------------------------

    float acc_corr = correction[0][ibin][1] + correction[0][ibin][2] * shower_lbary + correction[0][ibin][3] * shower_lbary * shower_lbary ;

    float e_out_perc = 0;
    if (the_aeta < eta_start_crack) {
        e_out_perc = correction[1][ibin][1] + correction[1][ibin][2] * shower_lbary + correction[1][ibin][3] * shower_lbary * shower_lbary ;
    }
    else {
        e_out_perc = correction[1][ibin][1] + correction[1][ibin][2] * shower_lbary + correction[1][ibin][3] / shower_lbary ;
    }   

    float e_acc_reco=acc_corr*(eacc_base )*(1+(e_out_perc)/100);

    // -------------------------------------------------------------
    // Now estimate the longitudinal leakage
    // -------------------------------------------------------------

    float e_leak_perc = 0;
    e_leak_perc = correction[2][ibin][3] + correction[2][ibin][1] * shower_lbary + correction[2][ibin][2] * exp(shower_lbary);

    if (e_leak_perc < 0 ) e_leak_perc = 0.;
    if (e_leak_perc > 100.) e_leak_perc = 100.;
    float e_leak_reco = e_leak_perc * (e_acc_reco)/100;

    // -------------------------------------------------------------
    // Now estimate the energy lost in front of the calorimeter
    // -------------------------------------------------------------

    float raw_energy=e_acc_reco*.001;
    float e_front_reco = eps_base;

    if (raw_energy <= 1) {
        // Protect against log() going negative in pow();
        // also protect against sqrt() of a negative number and div-by-zero.
    }
    else if (the_aeta < 1.8) {

        if (the_aeta < eta_start_crack) {
            float WpsOff      = correction[3][ibin_frontCorr][1] + correction[3][ibin_frontCorr][2] * raw_energy + correction[3][ibin_frontCorr][3] * raw_energy * raw_energy;
            float WpsSlo      = correction[4][ibin_frontCorr][1]  * pow(log(raw_energy),correction[4][ibin_frontCorr][2]) + correction[4][ibin_frontCorr][3] *sqrt( raw_energy );
            e_front_reco=WpsOff + WpsSlo*(eps_base);

#ifndef NDEBUG
            ATH_MSG_DEBUG( " raw event  " << raw_energy  );
            ATH_MSG_DEBUG( " froffset coeff " << correction[3][ibin_frontCorr][1] << " " << correction[3][ibin_frontCorr][2] << " " << correction[3][ibin_frontCorr][3]   );
            ATH_MSG_DEBUG( " frslope coeff  " << correction[4][ibin_frontCorr][1] << " " << correction[4][ibin_frontCorr][2] << " " << correction[4][ibin_frontCorr][3]  );
            ATH_MSG_DEBUG( " WpsOff,WpsSlo " << WpsOff << " " << WpsSlo  );
            ATH_MSG_DEBUG( " eps_base, efront_reco " << eps_base << " " << e_front_reco  );
#endif
        }
        else{


            if (raw_energy<20.) raw_energy=20.;

            float WpsOff      = correction[3][ibin_frontCorr][1] + correction[3][ibin_frontCorr][2] * raw_energy + correction[3][ibin_frontCorr][3] * sqrt(raw_energy);
            float WpsSlo      = correction[4][ibin_frontCorr][1]  + correction[4][ibin_frontCorr][2] * log(raw_energy) + correction[4][ibin_frontCorr][3] * sqrt(raw_energy);
            float WpsSlo2     = correction[5][ibin_frontCorr][1]  + correction[5][ibin_frontCorr][2] * raw_energy - correction[5][ibin_frontCorr][3] / (raw_energy * raw_energy) ;
            e_front_reco=WpsOff + WpsSlo*(eps_base) + WpsSlo2*(eps_base)*(eps_base);
            if (e_front_reco<0.) e_front_reco= eps_base;
#ifndef NDEBUG
            ATH_MSG_DEBUG( " raw energy " << raw_energy  );
            ATH_MSG_DEBUG( "p1 " << correction[3][ibin_frontCorr][1] << " " << correction[3][ibin_frontCorr][2] << " " << correction[3][ibin_frontCorr][3] <<  " " << WpsOff  );
            ATH_MSG_DEBUG( "p2 " << correction[4][ibin_frontCorr][1] << " " << correction[4][ibin_frontCorr][2] << " " << correction[4][ibin_frontCorr][3] << " " << WpsSlo  );
            ATH_MSG_DEBUG( "p3 " << correction[5][ibin_frontCorr][1] << " " << correction[5][ibin_frontCorr][2] << " " << correction[5][ibin_frontCorr][3] << " " << WpsSlo2  );
            ATH_MSG_DEBUG( " WpsOff, WpsSlo, WpsSlo2 " << WpsOff << " " << WpsSlo << " " << WpsSlo2  );
            ATH_MSG_DEBUG( " eps_base, efront_reco " << eps_base << " " <<  e_front_reco  );
#endif

        }

    }
    else {
        float p1 =  correction[3][ibin_frontCorr][1] + correction[3][ibin_frontCorr][2] * raw_energy  + correction[3][ibin_frontCorr][3] * raw_energy  * raw_energy ;
        float p2 =  correction[4][ibin_frontCorr][1] + correction[4][ibin_frontCorr][2] * raw_energy  + correction[4][ibin_frontCorr][3] * raw_energy  * raw_energy ;
        float p3 =  correction[5][ibin_frontCorr][1] + correction[5][ibin_frontCorr][2] * raw_energy  + correction[5][ibin_frontCorr][3] * raw_energy  * raw_energy ;

#ifndef NDEBUG
        ATH_MSG_DEBUG( "p1 " << correction[3][ibin_frontCorr][1] << " " << correction[3][ibin_frontCorr][2] << " " << correction[3][ibin_frontCorr][3]  );
        ATH_MSG_DEBUG( "p2 " << correction[4][ibin_frontCorr][1] << " " << correction[4][ibin_frontCorr][2] << " " << correction[4][ibin_frontCorr][3]  );
        ATH_MSG_DEBUG( "p3 " << correction[5][ibin_frontCorr][1] << " " << correction[5][ibin_frontCorr][2] << " " << correction[5][ibin_frontCorr][3]  );
#endif

        e_front_reco= (p1 + p2 * shower_lbary + p3 * shower_lbary * shower_lbary);
        if (e_front_reco<0.) e_front_reco=eps_base;
    }
 
    // -------------------------------------------------------------
    // Now compute the total energy and finally update the cluster energies
    // -------------------------------------------------------------

    float e_calo_reco =e_front_reco + e_leak_reco + e_acc_reco;

#ifndef NDEBUG
    ATH_MSG_DEBUG( "CaloSwCalibrationHits::Final reco energy ---------------------- " << e_calo_reco  );
    ATH_MSG_DEBUG( "CaloSwCalibrationHits::Front ---------------------- " <<  e_front_reco  );
    ATH_MSG_DEBUG( "CaloSwCalibrationHits::Accordion ------------------ " <<  e_acc_reco  );
    ATH_MSG_DEBUG( "CaloSwCalibrationHits::out of cone ---------------- " <<  acc_corr*(eacc_base )*(e_out_perc)/100  );
    ATH_MSG_DEBUG( "CaloSwCalibrationHits::Leakage -------------------- " <<  e_leak_reco  );
#endif



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

    // presampler
    clus->setEnergy (EgammaHitsShowerDepth::m_samps[si][0], e_front_reco );

    // strips and middle
    for (int sampling=1; sampling<=2; sampling++){
        clus->setEnergy (EgammaHitsShowerDepth::m_samps[si][sampling], clus->energy(EgammaHitsShowerDepth::m_samps[si][sampling]) * acc_corr );
    }

    // back
    clus->setEnergy (EgammaHitsShowerDepth::m_samps[si][3], clus->energy (EgammaHitsShowerDepth::m_samps[si][3]) * acc_corr + e_leak_reco);

#ifndef NDEBUG
    // total energy
    float e_temp = 0;
    for (int nl = 0 ; nl< 4 ; nl++) e_temp +=  clus->energy (EgammaHitsShowerDepth::m_samps[si][nl]);
    ATH_MSG_DEBUG( "----------  Sum of the sampling energy ---  >> " << e_temp << "  EcaloReco = " << e_calo_reco  );
    ATH_MSG_DEBUG( "CaloSwCalibHitsCalibration Energy after  correction --> " <<  clus->energy()  );
#endif

    clus->setEnergy(e_calo_reco);

    clus->setEt(clus->energy()/cosh(the_aeta));

}

