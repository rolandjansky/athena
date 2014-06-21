/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id$
/**
 * @file TauD3PDMaker/src/TauCalcVarsFillerTool.cxx
 * @author Dugan O'Neil <doneil@sfu.ca>
 * @date December, 2009
 * @brief Block filler tool for calculated variables
 */


#include "TauCalcVarsFillerTool.h"
#include "tauEvent/TauJet.h"
#include "AthenaKernel/errorcheck.h"
#include "TauDiscriminant/TauDetailsManager.h"
#include "TauDiscriminant/FakeTauBits.h"
#include "TauDiscriminant/FakeTauScores.h"
#include "StoreGate/StoreGateSvc.h"
#include "Particle/TrackParticleContainer.h"
#include "Particle/TrackParticle.h"
#include "TrkTrackSummary/TrackSummary.h"
#include <TMath.h>


namespace D3PD {


/**
 * @brief Standard Gaudi tool constructor.
 * @param type The name of the tool type.
 * @param name The tool name.
 * @param parent The tool's Gaudi parent.
 */
    TauCalcVarsFillerTool::TauCalcVarsFillerTool (const std::string& type,
                                                  const std::string& name,
                                                  const IInterface* parent)
       : BlockFillerTool<xAOD::TauJet> (type, name, parent)
       , m_trackToVertexTool("Reco::TrackToVertex")

    {
        book().ignore(); // Avoid coverity warnings.
        declareProperty ("TrackToVertexTool", m_trackToVertexTool);
        declareProperty ("isTrigger", m_isTrigger=false,"isTrigger boolean");

    }

    TauCalcVarsFillerTool::~TauCalcVarsFillerTool()
    {}

    StatusCode TauCalcVarsFillerTool::initialize()
    {
    MsgStream log(msgSvc(), name());

    // get StoreGate 

    StatusCode sc = service("StoreGateSvc", m_storeGate);
  
    if ( sc == StatusCode::FAILURE )
    {
        REPORT_MESSAGE(MSG::ERROR) << "   could not access StoreGate ";
        return StatusCode::FAILURE;
    }

    this->m_detailsManager = new TauDetailsManager(m_storeGate,m_isTrigger);
    
    sc = m_trackToVertexTool.retrieve();

    if(sc.isFailure())
    {
        REPORT_MESSAGE (MSG::ERROR) << "Could not retrieve TrackToVertexTool";
        return StatusCode::FAILURE;
    }
    
    return StatusCode::SUCCESS;
    }

/**
 * @brief Book variables for this block.
 */
    StatusCode TauCalcVarsFillerTool::book()
    {
        // these variables are part of the future EDM. I have
        // given them their EDM names (after the calcVars_). They will
        // be turned off as soon as we are producing tau candidates
        // with them as part of the EDM.
        // variables from old safe variable list
        CHECK( addVariable ("calcVars_etHad_EMScale_Pt3Trks",   m_etHadEMSumPt3Tracks)  );
        CHECK( addVariable ("calcVars_etEM_EMScale_Pt3Trks",   m_etEMEMSumPt3Tracks)  );
        CHECK( addVariable ("calcVars_ipSigLeadLooseTrk",   m_ipSigLeadLooseTrk)  );
        CHECK( addVariable ("calcVars_drMin",   m_drMin)  );
        CHECK( addVariable ("calcVars_calRadius", m_calRadius)  );
        CHECK( addVariable ("calcVars_EMFractionAtEMScale", m_EMFractionAtEMScale)  );
        CHECK( addVariable ("calcVars_trackIso", m_trackIso) );
        CHECK( addVariable ("calcVars_ChPiEMEOverCaloEME", m_ChPiEMEOverCaloEME) );
        CHECK( addVariable ("calcVars_PSSFraction", m_PSSFraction) );
        CHECK( addVariable ("calcVars_EMPOverTrkSysP", m_EMPOverTrkSysP) );
        CHECK( addVariable ("calcVars_pi0BDTPrimaryScore", m_pi0BDTPrimaryScore) );
        CHECK( addVariable ("calcVars_pi0BDTSecondaryScore", m_pi0BDTSecondaryScore) );
        CHECK( addVariable ("calcVars_corrCentFrac", m_corrCentFrac) );
        CHECK( addVariable ("calcVars_corrFTrk", m_corrFTrk) );
        CHECK( addVariable ("calcVars_interAxis_eta", m_interAxisEta) );
        CHECK( addVariable ("calcVars_interAxis_phi", m_interAxisPhi) );
        CHECK( addVariable ("calcVars_absDeltaEta", m_absDeltaEta) );
        CHECK( addVariable ("calcVars_absDeltaPhi", m_absDeltaPhi) );
        CHECK( addVariable ("calcVars_EMFractionAtEMScale_MoveE3", m_EMFractionAtEMScale_MoveE3) );
        CHECK( addVariable ("calcVars_SecMaxStripEtOverPt", m_seedTrk_SecMaxStripEtOverPt) );

        CHECK( addVariable ( "pi0_cl1_pt",  m_cl1_Pt) );
        CHECK( addVariable ( "pi0_cl1_eta",  m_cl1_Eta) );
        CHECK( addVariable ( "pi0_cl1_phi",  m_cl1_Phi) );
        CHECK( addVariable ( "pi0_cl2_pt",  m_cl2_Pt) );
        CHECK( addVariable ( "pi0_cl2_eta",  m_cl2_Eta) );
        CHECK( addVariable ( "pi0_cl2_phi",  m_cl2_Phi) );
        CHECK( addVariable ( "pi0_vistau_pt",  m_tau_vis_Pt) );
        CHECK( addVariable ( "pi0_vistau_eta",  m_tau_vis_Eta) );
        CHECK( addVariable ( "pi0_vistau_phi",  m_tau_vis_Phi) );
        CHECK( addVariable ( "pi0_vistau_m",  m_tau_vis_M) );
        CHECK( addVariable ( "pi0_n", m_nPi0s_cl) );

        CHECK( addVariable ( "out_track_n", m_out_track_n) );
        CHECK( addVariable ( "out_track_n_extended", m_out_track_n_extended) );
        


        return StatusCode::SUCCESS;
    }


/**
 * @brief Fill one block --- type-safe version.
 * @param p The input object.
 *
 * This is called once per object.  The caller
 * is responsible for arranging that all the pointers for booked variables
 * are set appropriately upon entry.
 */
    StatusCode TauCalcVarsFillerTool::fill (const xAOD::TauJet& p)
    {
        // update details calculation map to correspond to this tau
        m_detailsManager->updateEvent();
        m_detailsManager->update(p);

        *m_etHadEMSumPt3Tracks  = m_detailsManager->getFloatDetailValue(Details::ETHAD_EM_OVER_SUMPT3TRK);
        *m_etEMEMSumPt3Tracks   = m_detailsManager->getFloatDetailValue(Details::ETEM_EM_OVER_SUMPT3TRK);
        *m_ipSigLeadLooseTrk    = m_detailsManager->getFloatDetailValue(Details::IPSIGLEADLOOSETRK);
        *m_drMin                = m_detailsManager->getFloatDetailValue(Details::DRMIN);
        *m_EMFractionAtEMScale  = m_detailsManager->getFloatDetailValue(Details::EMFRACTIONATEMSCALE);
        *m_calRadius            = m_detailsManager->getFloatDetailValue(Details::CALRADIUS);
        *m_trackIso             = m_detailsManager->getFloatDetailValue(Details::TRACK_ISO);
        *m_ChPiEMEOverCaloEME   = m_detailsManager->getFloatDetailValue(Details::CHPIEMEOVERCALOEME);
        *m_PSSFraction          = m_detailsManager->getFloatDetailValue(Details::PSSFRACTION);
        *m_EMPOverTrkSysP       = m_detailsManager->getFloatDetailValue(Details::EMPOVERTRKSYSP);
        *m_corrCentFrac         = m_detailsManager->getFloatDetailValue(Details::CORRCENTFRAC);
        *m_corrFTrk             = m_detailsManager->getFloatDetailValue(Details::CORRFTRK);
        *m_interAxisEta         = m_detailsManager->getFloatDetailValue(Details::INTERAXIS_ETA);
        *m_interAxisPhi         = m_detailsManager->getFloatDetailValue(Details::INTERAXIS_PHI);
        *m_cl1_Pt               = m_detailsManager->getFloatDetailValue(Details::PI0CL1_PT);
        *m_cl1_Eta              = m_detailsManager->getFloatDetailValue(Details::PI0CL1_ETA);
        *m_cl1_Phi              = m_detailsManager->getFloatDetailValue(Details::PI0CL1_PHI);
        *m_cl2_Pt               = m_detailsManager->getFloatDetailValue(Details::PI0CL2_PT);
        *m_cl2_Eta              = m_detailsManager->getFloatDetailValue(Details::PI0CL2_ETA);
        *m_cl2_Phi              = m_detailsManager->getFloatDetailValue(Details::PI0CL2_PHI);
        *m_tau_vis_Pt           = m_detailsManager->getFloatDetailValue(Details::VISTAU_PI0CL_PT);
        *m_tau_vis_Eta          = m_detailsManager->getFloatDetailValue(Details::VISTAU_PI0CL_ETA);
        *m_tau_vis_Phi          = m_detailsManager->getFloatDetailValue(Details::VISTAU_PI0CL_PHI);
        *m_tau_vis_M            = m_detailsManager->getFloatDetailValue(Details::VISTAU_PI0CL_M);
        *m_absDeltaEta                 = m_detailsManager->getFloatDetailValue(Details::TAU_ABSDELTAETA);
        *m_absDeltaPhi                 = m_detailsManager->getFloatDetailValue(Details::TAU_ABSDELTAPHI);
        *m_EMFractionAtEMScale_MoveE3  = m_detailsManager->getFloatDetailValue(Details::EMFRACTIONATEMSCALE_MOVEE3);
        *m_seedTrk_SecMaxStripEtOverPt = m_detailsManager->getFloatDetailValue(Details::TAU_SEEDTRK_SECMAXSTRIPETOVERPT);

        // get pi0 BDT scores (temporary, remove in release 18) 
        const FakeTauScoresContainer* fakeScoresContainer(0);
        StatusCode sc = m_storeGate->retrieve(fakeScoresContainer, "FakeTauScoresContainer");
            
        const FakeTauScores* scores(0);
            
        if(StatusCode::SUCCESS != sc ) 
        {
            REPORT_MESSAGE(MSG::DEBUG) << " unable to retrieve fakeTauScores container, fake tau scores filled with dummy values (-1111) ";
        
            //Fill in dummy values
            *m_pi0BDTPrimaryScore  = -1111;
            *m_pi0BDTSecondaryScore  = -1111;
            return StatusCode::SUCCESS;
        }
        else
            scores = fakeScoresContainer->getScoresAssocTo(p);
        
        
        if(!scores)
        {
            REPORT_MESSAGE(MSG::DEBUG) << " Cannot get fake scores associated to Tau (null pointer), fake tau scores filled with dummy values (-1111) ";
        
            //Fill in dummy values
            *m_pi0BDTPrimaryScore  = -1111;
            *m_pi0BDTSecondaryScore  = -1111;
        }
        else
        {
            float Primary   = scores->getScore(TauScore::BDT_PI0_PRIMARY);
            float Secondary = scores->getScore(TauScore::BDT_PI0_SECONDARY);

            int nPi0s = 0;

            if (Primary < 0.465)
            nPi0s += 1;

            if (Secondary < 0.565)
            nPi0s += 1;

            *m_nPi0s_cl = nPi0s;
            

            *m_pi0BDTPrimaryScore   = Primary;
            *m_pi0BDTSecondaryScore = Secondary;
        }

    /// TRACK COUNTING
        const Rec::TrackParticleContainer *tpc(0);
        sc = m_storeGate->retrieve(tpc, "TrackParticleCandidate");
        if(sc!=StatusCode::SUCCESS) {
            REPORT_MESSAGE(MSG::ERROR) << " Cannot retrieve TrackParticleContainer ";
            return StatusCode::SUCCESS;
        }

        float trk_atTJVA_phi;
        float trk_atTJVA_d0;
        float trk_atTJVA_z0;
        float trk_atTJVA_theta;
        float trk_atTJVA_qoverp;
        //float trk_atTJVA_phi_err;
        //float trk_atTJVA_d0_err;
        //float trk_atTJVA_z0_err;
        //float trk_atTJVA_theta_err;
        //float trk_atTJVA_qoverp_err;
        //float trk_atTJVA_cov_phi_d0;
        //float trk_atTJVA_cov_phi_z0;
        //float trk_atTJVA_cov_phi_theta;
        //float trk_atTJVA_cov_phi_qoverp;
        //float trk_atTJVA_cov_d0_z0;
        //float trk_atTJVA_cov_d0_theta;
        //float trk_atTJVA_cov_d0_qoverp;
        //float trk_atTJVA_cov_z0_theta;
        //float trk_atTJVA_cov_z0_qoverp;
        //float trk_atTJVA_cov_theta_qoverp;

        int nb;         
        int np;
        int npd;
        int ns;
        int nsd;

        Rec::TrackParticleContainer::const_iterator trkItr  = tpc->begin();
        Rec::TrackParticleContainer::const_iterator trkItrE = tpc->end();

        int nOuterKtTrack1 = 0;
        int nOuterKtTrack2 = 0;
        int nOuterKtTrack_tmp = 0;

        if (p.vertexLink().isValid() /*&& p.origin()->position()*/){
            for(; trkItr != trkItrE; ++trkItr)
            {
                const Rec::TrackParticle &trk = *(*trkItr);
                const Trk::Perigee* perigee = m_trackToVertexTool->perigeeAtVertex(trk, (*p.vertexLink())->position());
                
                trk_atTJVA_phi = (perigee->parameters()[Trk::phi0]); 
                trk_atTJVA_d0 = (perigee->parameters()[Trk::d0]);
                trk_atTJVA_z0 = (perigee->parameters()[Trk::z0]);
                trk_atTJVA_theta = (perigee->parameters()[Trk::theta]);
                trk_atTJVA_qoverp = (perigee->parameters()[Trk::qOverP]);
                
                //const Trk::MeasuredPerigee* mperigee = m_trackToVertexTool->perigeeAtVertex(trk, p.origin()->position());
                
                //trk_atTJVA_phi_err = (mperigee->localErrorMatrix().error(Trk::phi0));
                //trk_atTJVA_d0_err = (mperigee->localErrorMatrix().error(Trk::d0));
                //trk_atTJVA_z0_err = (mperigee->localErrorMatrix().error(Trk::z0));
                //trk_atTJVA_theta_err = (mperigee->localErrorMatrix().error(Trk::theta));
                //trk_atTJVA_qoverp_err = (mperigee->localErrorMatrix().error(Trk::qOverP));
                
                //trk_atTJVA_cov_phi_d0 = (mperigee->localErrorMatrix().covariance()[Trk::phi0][Trk::d0]);
                //trk_atTJVA_cov_phi_z0 = (mperigee->localErrorMatrix().covariance()[Trk::phi0][Trk::z0]);
                //trk_atTJVA_cov_phi_theta = (mperigee->localErrorMatrix().covariance()[Trk::phi0][Trk::theta]);
                //trk_atTJVA_cov_phi_qoverp = (mperigee->localErrorMatrix().covariance()[Trk::phi0][Trk::qOverP]);
                
                //trk_atTJVA_cov_d0_z0 = (mperigee->localErrorMatrix().covariance()[Trk::d0][Trk::z0]);
                //trk_atTJVA_cov_d0_theta = (mperigee->localErrorMatrix().covariance()[Trk::d0][Trk::theta]);
                //trk_atTJVA_cov_d0_qoverp = (mperigee->localErrorMatrix().covariance()[Trk::d0][Trk::qOverP]);
                
                //trk_atTJVA_cov_z0_theta = (mperigee->localErrorMatrix().covariance()[Trk::z0][Trk::theta]);
                //trk_atTJVA_cov_z0_qoverp = (mperigee->localErrorMatrix().covariance()[Trk::z0][Trk::qOverP]);
                
                //trk_atTJVA_cov_theta_qoverp = (mperigee->localErrorMatrix().covariance()[Trk::theta][Trk::qOverP]);
               
                nb = trk.trackSummary()->get(Trk::numberOfBLayerHits);   
                np = trk.trackSummary()->get(Trk::numberOfPixelHits);
                npd = trk.trackSummary()->get(Trk::numberOfPixelDeadSensors);
                ns = trk.trackSummary()->get(Trk::numberOfSCTHits);
                nsd = trk.trackSummary()->get(Trk::numberOfSCTDeadSensors);
		if(nb<0) nb=0;
		if(np<0) np=0;
		if(npd<0) npd=0;
		if(ns<0) ns=0;
		if(nsd<0) nsd=0;


                delete perigee;
                //delete mperigee;

                /// P-O/Daniele's code
                double sinth  = TMath::Sin((trk_atTJVA_theta));
                double trkpt  = sinth/TMath::Abs((trk_atTJVA_qoverp));
                double trketa = -TMath::Log(TMath::Tan(0.5*(trk_atTJVA_theta)));
                double trkphi = (trk_atTJVA_phi);
                double dR = deltaR(p.eta(),trketa,p.phi(),trkphi);

                // track selection
                if(dR>0.6) continue;
                if(trkpt/1000.0<0.5) continue;
                if(fabs(trk_atTJVA_d0)>1.0) continue;
                if(fabs(trk_atTJVA_z0*sinth)>1.5) continue;
                if(nb<1) continue; // should probably check if expect BLayerHit
                if(np+npd<2) continue;
                if(np+npd+ns+nsd<7) continue;

		if(dR > 0.2) nOuterKtTrack_tmp++;

                int iCheckKtTrack = 0;
                int iCheckKtTrack2 = 0;
                for(unsigned int j = 0; j < p.nTracks(); ++j)
                {
                  const Trk::Perigee* perigee = m_trackToVertexTool->perigeeAtVertex(*(p.track(j)), (*p.vertexLink())->position());

                    double dR1 = deltaR(-TMath::Log(TMath::Tan(0.5*(perigee->parameters()[Trk::theta]))),trketa,perigee->parameters()[Trk::phi0],trkphi);
                    double ptdR1 = (TMath::Sin((perigee->parameters()[Trk::theta]))/TMath::Abs((perigee->parameters()[Trk::qOverP])))*dR1/trkpt;
                    if (ptdR1<4.0 && dR>0.2)                     { iCheckKtTrack++; }
                    if (ptdR1<4.0 && dR<0.2 && trkpt/1000.0<1.0) { iCheckKtTrack2++; }
                    delete perigee;
                }

                if (iCheckKtTrack>0 || iCheckKtTrack2>0) { nOuterKtTrack1++; }
                if (iCheckKtTrack>0) { nOuterKtTrack2++; }
            }

            *m_out_track_n_extended = nOuterKtTrack1 + p.nTracks();
            *m_out_track_n = nOuterKtTrack2 + p.nTracks();
        }
    
        return StatusCode::SUCCESS;
    }
} // namespace D3PD
