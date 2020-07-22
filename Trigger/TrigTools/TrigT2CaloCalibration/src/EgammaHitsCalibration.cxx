/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
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
    m_log = new MsgStream(AthAlgTool::msgSvc(), name() );

    (*m_log) << MSG::DEBUG << "Initialize Tool : " << name() << endmsg;



    return StatusCode::SUCCESS;
}

StatusCode EgammaHitsCalibration::finalize(){
    (*m_log) << MSG::DEBUG << "Finalize Tool : " << name() << endmsg;
    delete m_log;
    return StatusCode::SUCCESS;
}

void EgammaHitsCalibration::makeCorrection(xAOD::TrigEMCluster* clus,
    const void * /*param*/){

    const float etamax = m_etamax();
    const float eta_start_crack = m_eta_start_crack();
    const float eta_end_crack = m_eta_end_crack();
    const CxxUtils::Array<3> correction = m_correction();

    float the_aeta = (clus->eta());

    the_aeta = (the_aeta>0)?(the_aeta):(- the_aeta);

    if (the_aeta >= etamax) return; 

#ifndef NDEBUG
    (*m_log) << MSG::DEBUG <<  "************************************************************************************************" << endmsg;
    (*m_log) << MSG::DEBUG <<  " USING CALIBHITS CALIBRATION " << endmsg;
    (*m_log) << MSG::DEBUG <<  "************************************************************************************************" << endmsg;       
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
        (*m_log) << MSG::DEBUG << "Check etas -------------------------------------------------------------------"<< endmsg;
        (*m_log) << MSG::DEBUG << "Eta --> " << the_aeta << "  Bin --> " << ibin <<" Cluster eta = " << clus->eta() << endmsg;

        (*m_log) << MSG::DEBUG << "Check calibration coefficients -----------------------------------------------"<< endmsg;
        (*m_log) << MSG::DEBUG << "Accordion   :  " << correction[0][ibin][0] <<"  " << correction[0][ibin][1] << " "  << correction[0][ibin][2] << " "  << correction[0][ibin][3] << endmsg;
        (*m_log) << MSG::DEBUG << "OutOfCOne   :  " << correction[1][ibin][0] <<"  " << correction[1][ibin][1] << " "  << correction[1][ibin][2] << " "  << correction[1][ibin][3] << endmsg;
        (*m_log) << MSG::DEBUG << "Leakage     :  " << correction[2][ibin][0] <<"  " << correction[2][ibin][1] << " " << correction[2][ibin][2] << " "  << correction[2][ibin][3] << endmsg;
        (*m_log) << MSG::DEBUG << "Front offset:  " << correction[3][ibin_frontCorr][0] <<"  " <<correction[3][ibin_frontCorr][1]  << " " << correction[3][ibin_frontCorr][2] << " "  << correction[3][ibin_frontCorr][3] <<endmsg;
        (*m_log) << MSG::DEBUG << "Front Slope  :  " << correction[4][ibin_frontCorr][0] <<"  " << correction[4][ibin_frontCorr][1] << " " << correction[4][ibin_frontCorr][2] << " "  << correction[4][ibin_frontCorr][3] <<  endmsg;
        (*m_log) << MSG::DEBUG << "Second order:  " << correction[5][ibin_frontCorr][0] << "  " << correction[5][ibin_frontCorr][1] << "  "  << correction[5][ibin_frontCorr][2] << " " << correction[5][ibin_frontCorr][3] << endmsg;
#endif

    float shower_lbary = m_showerDepth.depth(the_aeta,
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
      (*m_log) << MSG::DEBUG << "replace pathological depth by 15 X0" << endmsg;
#endif
    }

    // Compute total energy in the Accordion (eacc_base) and
    // presampler (eps_base)

    float eacc_base = 0;
    for (int sampling=1; sampling<4; sampling++) {
        eacc_base += clus->energy(EgammaHitsShowerDepth::m_samps[si][sampling]);
#ifndef NDEBUG
        (*m_log) << MSG::DEBUG << "Barrel/endcap = " << si << "  Sampling = " <<   sampling << "   Energy -->> " <<  clus->energy(EgammaHitsShowerDepth::m_samps[si][sampling]) << endmsg;
#endif
    }
    float eps_base = clus->energy (EgammaHitsShowerDepth::m_samps[si][0]);

#ifndef NDEBUG
    (*m_log) << MSG::DEBUG << "E accordion base --->>>> "  << eacc_base <<  "  Eps base " << eps_base << endmsg;
#endif


   // Add a protection against large longitudinal barycenter for clusters from
   // hadrons which may cause over-calibration. A energy dependent upper limit
   // on the long bary is introduced (from 20 at 0 to 23 at 1 TeV)    

   float depth_max = 20. + 3. * (eacc_base+eps_base) * 1e-6; // divided by TeV
#ifndef NDEBUG
   (*m_log) << MSG::DEBUG << "Raw energy ---->> " << (eacc_base+eps_base) << endmsg ;
   (*m_log) << MSG::DEBUG << "Bary max for this event ---->> " << depth_max
       << endmsg ;
#endif

   if ( shower_lbary > depth_max ) {
     shower_lbary = 15.;
     //shower_lbary = depth_max;
#ifndef NDEBUG
     (*m_log) << MSG::DEBUG << " replace pathological depth by 15 X0 " 
         << endmsg; 
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
            (*m_log) << MSG::DEBUG  << " raw event  " << raw_energy << endmsg;
            (*m_log) << MSG::DEBUG  << " froffset coeff " << correction[3][ibin_frontCorr][1] << " " << correction[3][ibin_frontCorr][2] << " " << correction[3][ibin_frontCorr][3]  << endmsg; 
            (*m_log) << MSG::DEBUG  << " frslope coeff  " << correction[4][ibin_frontCorr][1] << " " << correction[4][ibin_frontCorr][2] << " " << correction[4][ibin_frontCorr][3] << endmsg; 
            (*m_log) << MSG::DEBUG  << " WpsOff,WpsSlo " << WpsOff << " " << WpsSlo << endmsg; 
            (*m_log) << MSG::DEBUG  << " eps_base, efront_reco " << eps_base << " " << e_front_reco << endmsg; 
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
            (*m_log) << MSG::DEBUG << " raw energy " << raw_energy << endmsg; 
            (*m_log) << MSG::DEBUG << "p1 " << correction[3][ibin_frontCorr][1] << " " << correction[3][ibin_frontCorr][2] << " " << correction[3][ibin_frontCorr][3] <<  " " << WpsOff << endmsg;
            (*m_log) << MSG::DEBUG << "p2 " << correction[4][ibin_frontCorr][1] << " " << correction[4][ibin_frontCorr][2] << " " << correction[4][ibin_frontCorr][3] << " " << WpsSlo << endmsg;
            (*m_log) << MSG::DEBUG << "p3 " << correction[5][ibin_frontCorr][1] << " " << correction[5][ibin_frontCorr][2] << " " << correction[5][ibin_frontCorr][3] << " " << WpsSlo2 << endmsg;
            (*m_log) << MSG::DEBUG << " WpsOff, WpsSlo, WpsSlo2 " << WpsOff << " " << WpsSlo << " " << WpsSlo2 << endmsg; 
            (*m_log) << MSG::DEBUG << " eps_base, efront_reco " << eps_base << " " <<  e_front_reco << endmsg; 
#endif

        }

    }
    else {
        float p1 =  correction[3][ibin_frontCorr][1] + correction[3][ibin_frontCorr][2] * raw_energy  + correction[3][ibin_frontCorr][3] * raw_energy  * raw_energy ;
        float p2 =  correction[4][ibin_frontCorr][1] + correction[4][ibin_frontCorr][2] * raw_energy  + correction[4][ibin_frontCorr][3] * raw_energy  * raw_energy ;
        float p3 =  correction[5][ibin_frontCorr][1] + correction[5][ibin_frontCorr][2] * raw_energy  + correction[5][ibin_frontCorr][3] * raw_energy  * raw_energy ;

#ifndef NDEBUG
        (*m_log) << MSG::DEBUG << "p1 " << correction[3][ibin_frontCorr][1] << " " << correction[3][ibin_frontCorr][2] << " " << correction[3][ibin_frontCorr][3] << endmsg;
        (*m_log) << MSG::DEBUG << "p2 " << correction[4][ibin_frontCorr][1] << " " << correction[4][ibin_frontCorr][2] << " " << correction[4][ibin_frontCorr][3] << endmsg;
        (*m_log) << MSG::DEBUG << "p3 " << correction[5][ibin_frontCorr][1] << " " << correction[5][ibin_frontCorr][2] << " " << correction[5][ibin_frontCorr][3] << endmsg;
#endif

        e_front_reco= (p1 + p2 * shower_lbary + p3 * shower_lbary * shower_lbary);
        if (e_front_reco<0.) e_front_reco=eps_base;
    }
 
    // -------------------------------------------------------------
    // Now compute the total energy and finally update the cluster energies
    // -------------------------------------------------------------

    float e_calo_reco =e_front_reco + e_leak_reco + e_acc_reco;

#ifndef NDEBUG
    (*m_log) << MSG::DEBUG << "CaloSwCalibrationHits::Final reco energy ---------------------- " << e_calo_reco << endmsg;
    (*m_log) << MSG::DEBUG << "CaloSwCalibrationHits::Front ---------------------- " <<  e_front_reco << endmsg;
    (*m_log) << MSG::DEBUG << "CaloSwCalibrationHits::Accordion ------------------ " <<  e_acc_reco << endmsg;
    (*m_log) << MSG::DEBUG << "CaloSwCalibrationHits::out of cone ---------------- " <<  acc_corr*(eacc_base )*(e_out_perc)/100 << endmsg;     
    (*m_log) << MSG::DEBUG << "CaloSwCalibrationHits::Leakage -------------------- " <<  e_leak_reco << endmsg;
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
    (*m_log) << MSG::DEBUG << "----------  Sum of the sampling energy ---  >> " << e_temp << "  EcaloReco = " << e_calo_reco << endmsg;
    (*m_log) << MSG::DEBUG << "CaloSwCalibHitsCalibration Energy after  correction --> " <<  clus->energy() << endmsg;
#endif

    clus->setEnergy(e_calo_reco);

    clus->setEt(clus->energy()/cosh(the_aeta));

}

