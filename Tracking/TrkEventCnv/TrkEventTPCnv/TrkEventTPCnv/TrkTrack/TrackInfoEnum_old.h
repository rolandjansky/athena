/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRACKINFOENUM_OLD_H
#define TRACKINFOENUM_OLD_H

//-----------------------------------------------------------------------------
//
// file:   TrackInfoCnv_p1.h
//
//-----------------------------------------------------------------------------

 enum TrackAuthor_old
            {
                /**Track author not defined.*/
                unknown_old=0, 
            
                /**tracks produced directly from iPat*/
                iPat_old=1, 
		
		iPatBremFit_old=2, 
            
                /**tracks produced directly from xKalman*/
                xKalman_old=3, 
		xKalmanBremFit_old=4, 
            
                /**tracks converted to Trk::Tracks from iPat tracks*/
                iPatLegacyCnv_old=5,
		iPatLegacyCnvBremFit_old=6,
            
                /**tracks converted to Trk::Tracks from Moore tracks*/
                MooreLegacyCnv_old=7,
		MooreLegacyCnvBremFit_old=8, 
            
                /**tracks produced from MuonBoy*/
                Muonboy_old=9,
            
                /**tracks converted to Trk::Tracks from xkalman tracks*/
                xKalmanLegacyCnv_old=10,
		xKalmanLegacyCnvBremFit_old=11, 
            
                /**tracks produced by the Kalman Fitter*/
                KalmanFitter_old=12,
            
                /**MuID combined track (i.e. track extends from ID to Muon Spectrometer)*/
                MuidComb_old=13,
            
                /**MuID track which is produced using Muon Spectrometer only*/
                MuidStandAlone_old=14,
                
                /**Low \f$ p_T \f$ MuID track*/
                MuidLowPt_old=15,
            
                /**tracks produced by STACO*/
                STACO_old=16,
            
                /**Low \f$ p_T \f$ tracks produced by STACO*/
                StacoLowPt_old=17,
            
                /**TrigInDetTracks produced by TrigIDSCAN, running at trigger LVL2*/
                TrigIDSCAN=18, 
            
                /**TrigInDetTracks produced by TrigSiTrack, running at trigger LVL2*/
                TrigSiTrack_old=19, 
            
                /**TrigInDetTracks produced by TrigTRTxK, running at trigger LVL2*/
                TrigTRTxK_old=20,
            
                /**TrigInDetTracks produced by TrigTRTLUT, running at trigger LVL2*/
                TrigTRTLUT_old=21,
            
                /**Tracks from Gaussian Sum Filter*/
                GaussianSumFilter_old=22,
            
                /**Tracks from the SiSpacePointSeededTrackFinder */
                SiSPSeededTrackFinder_old=23,
            
                /**Track's from Thijs' global chi^2 fitter*/
                GlobalChi2Fitter_old=24,
            
                /**Track's from Thijs' global chi^2 fitter, with Brem fit*/
                GlobalChi2FitterBremFit_old=25, 
            
                /**Fast Kalman filter from HLT with simplified material effects*/
                DistributedKalmanFilter_old=26,

                /**Adpative track fitter with fuzzy hit assignment*/
                DeterministicAnnealingFilter_old=27,
                
                /**Tracks from Kalman DNA Fitter */
                KalmanDNAFitter_old=28,
            
                /**Tracks from Back Tracking*/
                TRT_SeededTrackFinder_old=29,
            
                /**TRT standalone*/
                TRT_StandAlone_old=30,
                
                /**maximum number of enums*/
                NumberOfTrackAuthors_old=31
            };

#endif // TRACKINFOENUM_OLD_H
