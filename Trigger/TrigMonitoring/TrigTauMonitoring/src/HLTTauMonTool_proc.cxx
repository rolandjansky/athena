/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "TProfile.h"
#include "HLTTauMonTool.h"

using namespace std;

///////////////////////////////////////////////////////////////////
//#ifdef ManagedMonitorToolBase_Uses_API_201401
StatusCode HLTTauMonTool::proc()
//#else
//StatusCode  HLTTauMonTool::proc(bool endOfEventsBlock, bool  endOfLumiBlock, bool  endOfRun)
//#endif
{
    
    if(endOfEventsBlockFlag() || endOfLumiBlockFlag()) {}//Get rid of compilation warnings
    if(endOfRunFlag())
    {
/*        if(m_RealZtautauEff)
        {
            setCurrentMonGroup("HLT/TauMon/Expert/RealZtautauEff");
	    for(unsigned int i=0;i<m_trigItemsZtt.size();++i)
	      {
                setCurrentMonGroup("HLT/TauMon/Expert/RealZtautauEff/"+m_trigItems[i]);
                //plotUnderOverFlow(hist("hRealZttPtDenom"));
                //plotUnderOverFlow(hist("hRealZttL1PtNum"));
                //plotUnderOverFlow(hist("hRealZttHLTPtNum"));
	      }
        }
*/
	
        for(unsigned int i=0;i<m_trigItems.size();++i)
	  {
            if(m_truth && m_turnOnCurves)
	      {
		//                setCurrentMonGroup("HLT/TauMon/Expert/"+m_trigItems[i]+"/TurnOnCurves/TruthEfficiency");
		//                plotUnderOverFlow(hist("hTrueTauPtDenom"));
		//                 plotUnderOverFlow(hist("hTrueTauPt1PDenom"));
		//                 plotUnderOverFlow(hist("hTrueTauPt3PDenom"));
		//                 plotUnderOverFlow(hist("hTrueTauEtaDenom"));
		//                 plotUnderOverFlow(hist("hTrueTauPhiDenom"));
		//               plotUnderOverFlow(hist("hTrueTauNTrackDenom"));
                // plotUnderOverFlow(hist("hTrueTauNVtxDenom"));
		//                plotUnderOverFlow(hist("hTrueTauMuDenom"));
		//                plotUnderOverFlow(hist("hTrueL1PtNum"));
		//                plotUnderOverFlow(hist("hTrueL1Pt1PNum"));
		//                plotUnderOverFlow(hist("hTrueL1Pt3PNum"));
		//                plotUnderOverFlow(hist("hTrueL1EtaNum"));
		//                plotUnderOverFlow(hist("hTrueL1PhiNum"));
		//                plotUnderOverFlow(hist("hTrueL1NTrackNum"));
		//                plotUnderOverFlow(hist("hTrueL1NVtxNum"));
		//                plotUnderOverFlow(hist("hTrueL1MuNum"));
		//                plotUnderOverFlow(hist("hTrueHLTPtNum"));
		//                plotUnderOverFlow(hist("hTrueHLTPt1PNum"));
		//                plotUnderOverFlow(hist("hTrueHLTPt3PNum"));
		//                plotUnderOverFlow(hist("hTrueHLTEtaNum"));
		//                plotUnderOverFlow(hist("hTrueHLTPhiNum"));
		//                plotUnderOverFlow(hist("hTrueHLTNTrackNum"));
		//                plotUnderOverFlow(hist("hTrueHLTNVtxNum"));
		//                plotUnderOverFlow(hist("hTrueHLTMuNum"));
		//                setCurrentMonGroup("HLT/TauMon/Expert/"+m_trigItems[i]+"/TurnOnCurves/Truth+RecoEfficiency");
		//                plotUnderOverFlow(hist("hTruthRecoTauPtDenom"));
		//                plotUnderOverFlow(hist("hTruthRecoTauPt1PDenom"));
		//                plotUnderOverFlow(hist("hTruthRecoTauPt3PDenom"));
		//                plotUnderOverFlow(hist("hTruthRecoTauEtaDenom"));
		//                plotUnderOverFlow(hist("hTruthRecoTauPhiDenom"));
		//                plotUnderOverFlow(hist("hTruthRecoTauNTrackDenom"));
		//                plotUnderOverFlow(hist("hTruthRecoTauNVtxDenom"));
		//                plotUnderOverFlow(hist("hTruthRecoTauMuDenom"));
		//                plotUnderOverFlow(hist("hTruthRecoL1PtNum"));
		//                plotUnderOverFlow(hist("hTruthRecoL1Pt1PNum"));
		//                plotUnderOverFlow(hist("hTruthRecoL1Pt3PNum"));
		//                plotUnderOverFlow(hist("hTruthRecoL1EtaNum"));
		//                plotUnderOverFlow(hist("hTruthRecoL1PhiNum"));
		//                plotUnderOverFlow(hist("hTruthRecoL1NTrackNum"));
		//                plotUnderOverFlow(hist("hTruthRecoL1NVtxNum"));
		//                plotUnderOverFlow(hist("hTruthRecoL1MuNum"));
		//                plotUnderOverFlow(hist("hTruthRecoHLTPtNum"));
		//                plotUnderOverFlow(hist("hTruthRecoHLTPt1PNum"));
		//                plotUnderOverFlow(hist("hTruthRecoHLTPt3PNum"));
		//                plotUnderOverFlow(hist("hTruthRecoHLTEtaNum"));
		//                plotUnderOverFlow(hist("hTruthRecoHLTPhiNum"));
		//                plotUnderOverFlow(hist("hTruthRecoHLTNTrackNum"));
		//                plotUnderOverFlow(hist("hTruthRecoHLTNVtxNum"));
		//                plotUnderOverFlow(hist("hTruthRecoHLTMuNum"));
//                setCurrentMonGroup("HLT/TauMon/Expert/"+m_trigItems[i]+"/TurnOnCurves/TauComboEfficiency");
//                plotUnderOverFlow(hist("hCombTauPtDenom"));
//                plotUnderOverFlow(hist("hCombL1TauPtNum"));
//                plotUnderOverFlow(hist("hCombHLTTauPtNum"));
//                plotUnderOverFlow(hist("hCombelPtDenom"));
//                plotUnderOverFlow(hist("hCombL1elPtNum"));
//                plotUnderOverFlow(hist("hCombHLTelPtNum"));
//                plotUnderOverFlow(hist("hCombmuPtDenom"));
//                plotUnderOverFlow(hist("hCombL1muPtNum"));
//                plotUnderOverFlow(hist("hCombHLTmuPtNum"));
//                plotUnderOverFlow(hist("hCombOffjetPtDenom"));
//                plotUnderOverFlow(hist("hCombL1OffjetPtNum"));
//                plotUnderOverFlow(hist("hCombHLTOffjetPtNum"));
//                plotUnderOverFlow(hist("hCombMETDenom"));
//                plotUnderOverFlow(hist("hCombL1METNum"));
//                plotUnderOverFlow(hist("hCombHLTMETNum"));
//                plotUnderOverFlow(hist("hCombdRDenom"));
//                plotUnderOverFlow(hist("hCombL1dRNum"));
//                plotUnderOverFlow(hist("hCombHLTdRNum"));
//                plotUnderOverFlow(hist("hCombdEtaDenom"));
//                plotUnderOverFlow(hist("hCombL1dEtaNum"));
//                plotUnderOverFlow(hist("hCombHLTdEtaNum"));
//                plotUnderOverFlow(hist("hCombdPhiDenom"));
//                plotUnderOverFlow(hist("hCombL1dPhiNum"));
//                plotUnderOverFlow(hist("hCombHLTdPhiNum"));
            }
            if(m_turnOnCurves)
            {
                setCurrentMonGroup("HLT/TauMon/Expert/"+m_trigItems[i]+"/TurnOnCurves/RecoEfficiency");
		//                plotUnderOverFlow(hist("hRecoTauPtDenom"));
		//                plotUnderOverFlow(hist("hRecoTauPt1PDenom"));
		//                plotUnderOverFlow(hist("hRecoTauPt3PDenom"));
		//                plotUnderOverFlow(hist("hRecoTauEtaDenom"));
		//                plotUnderOverFlow(hist("hRecoTauPhiDenom"));
		//                plotUnderOverFlow(hist("hRecoTauNTrackDenom"));
		//                plotUnderOverFlow(hist("hRecoTauNVtxDenom"));
		//                plotUnderOverFlow(hist("hRecoTauMuDenom"));
		//                plotUnderOverFlow(hist("hRecoL1PtNum"));
		//                plotUnderOverFlow(hist("hRecoL1Pt1PNum"));
		//                plotUnderOverFlow(hist("hRecoL1Pt3PNum"));
		//                plotUnderOverFlow(hist("hRecoL1EtaNum"));
		//                plotUnderOverFlow(hist("hRecoL1PhiNum"));
		//                plotUnderOverFlow(hist("hRecoL1NTrackNum"));
		//                plotUnderOverFlow(hist("hRecoL1NVtxNum"));
		//                plotUnderOverFlow(hist("hRecoL1MuNum"));
		//                plotUnderOverFlow(hist("hRecoHLTPtNum"));
		//                plotUnderOverFlow(hist("hRecoHLTPt1PNum"));
		//                plotUnderOverFlow(hist("hRecoHLTPt3PNum"));
		//                plotUnderOverFlow(hist("hRecoHLTEtaNum"));
		//                plotUnderOverFlow(hist("hRecoHLTPhiNum"));
		//                plotUnderOverFlow(hist("hRecoHLTNTrackNum"));
		//                plotUnderOverFlow(hist("hRecoHLTNVtxNum"));
		//                plotUnderOverFlow(hist("hRecoHLTMuNum"));

	    }

	   if (m_doTrackCurves)
	      {
	      setCurrentMonGroup("HLT/TauMon/Expert/"+m_trigItems[i]+"/trackCurves");
		
	      plotUnderOverFlow(hist("hpstau_trk_pt"));
              plotUnderOverFlow(hist("hpstau_trk_eta"));
              plotUnderOverFlow(hist("hpstau_trk_phi"));
              plotUnderOverFlow(hist("hpstau_trk_d0"));
              plotUnderOverFlow(hist("hpstau_trk_z0"));
	      plotUnderOverFlow(hist("hpstau_trkres_pt"));
              plotUnderOverFlow(hist("hpstau_trkres_eta"));
              plotUnderOverFlow(hist("hpstau_trkres_phi"));
              plotUnderOverFlow(hist("hpstau_trkres_d0"));
              plotUnderOverFlow(hist("hpstau_trkres_z0"));
	      }

            
            setCurrentMonGroup("HLT/TauMon/Expert/"+m_trigItems[i]+"/L1RoI");
            plotUnderOverFlow(hist("hL1RoIEta"));
            plotUnderOverFlow(hist("hL1RoIPhi"));
            plotUnderOverFlow(hist("hL1RoIeT"));
            plotUnderOverFlow(hist("hL1RoIisol"));
            plotUnderOverFlow(hist("hL1RoITauClus"));
            //plotUnderOverFlow(hist("hL1RoITauClus2"));
            plotUnderOverFlow(hist("hL1RoIEMIso"));
            plotUnderOverFlow(hist("hL1RoIHadCore"));
            plotUnderOverFlow(hist("hL1RoIHadIsol"));
			if (m_doL1JetPlots)
			{
            	plotUnderOverFlow(hist("hL1JetRoIEta"));            
            	plotUnderOverFlow(hist("hL1JetRoIPhi"));
            	plotUnderOverFlow(hist("hL1JetRoIeT"));
			}

            setCurrentMonGroup("HLT/TauMon/Expert/"+m_trigItems[i]+"/PreselectionTau");
            plotUnderOverFlow(hist("hEFEt"));
            plotUnderOverFlow(hist("hEFEt2"));
            plotUnderOverFlow(hist("hFTFnTrack"));
            plotUnderOverFlow(hist("hEta"));
            plotUnderOverFlow(hist("hPhi"));
            plotUnderOverFlow(hist("hFTFnWideTrack"));
            //plotUnderOverFlow(hist("hdRmax"));
            
            setCurrentMonGroup("HLT/TauMon/Expert/"+m_trigItems[i]+"/EFTau");
            plotUnderOverFlow(hist("hEFEt"));
            plotUnderOverFlow(hist("hEFEta"));
            plotUnderOverFlow(hist("hEFPhi"));
            plotUnderOverFlow(hist("hEFnTrack"));
            plotUnderOverFlow(hist("hEFEtRaw"));
            plotUnderOverFlow(hist("hEFnWideTrack"));
            //plotUnderOverFlow(hist("hEFEMRadius"));
            //plotUnderOverFlow(hist("hEFHADRadius"));
            plotUnderOverFlow(hist("hEFIsoFrac"));
            //plotUnderOverFlow(hist("hEFPSSFraction"));
            plotUnderOverFlow(hist("hEFEMFraction"));
            plotUnderOverFlow(hist("hScore1p"));
            plotUnderOverFlow(hist("hScoremp"));
            
            setCurrentMonGroup("HLT/TauMon/Expert/"+m_trigItems[i]+"/EFTau/BDT/1p_nonCorrected");
            plotUnderOverFlow(hist("hEFinnerTrkAvgDist1PNCorr"));
            plotUnderOverFlow(hist("hEFetOverPtLeadTrk1PNCorr"));
            plotUnderOverFlow(hist("hEFipSigLeadTrk1PNCorr"));
            plotUnderOverFlow(hist("hEFSumPtTrkFrac1PNCorr"));
            plotUnderOverFlow(hist("hEFChPiEMEOverCaloEME1PNCorr"));
            plotUnderOverFlow(hist("hEFEMPOverTrkSysP1PNCorr"));
            plotUnderOverFlow(hist("hEFcentFrac1PNCorr"));
            plotUnderOverFlow(hist("hEFptRatioEflowApprox1PNCorr"));
            
            setCurrentMonGroup("HLT/TauMon/Expert/"+m_trigItems[i]+"/EFTau/BDT/mp_nonCorrected");
            plotUnderOverFlow(hist("hEFinnerTrkAvgDistMPNCorr"));
            plotUnderOverFlow(hist("hEFetOverPtLeadTrkMPNCorr"));
            plotUnderOverFlow(hist("hEFChPiEMEOverCaloEMEMPNCorr"));
            plotUnderOverFlow(hist("hEFEMPOverTrkSysPMPNCorr"));
            plotUnderOverFlow(hist("hEFcentFracMPNCorr"));
            plotUnderOverFlow(hist("hEFptRatioEflowApproxMPNCorr"));
            plotUnderOverFlow(hist("hEFdRmaxMPNCorr"));
            plotUnderOverFlow(hist("hEFtrFlightPathSigMPNCorr"));
            plotUnderOverFlow(hist("hEFmassTrkSysMPNCorr"));
            plotUnderOverFlow(hist("hEFmEflowApproxMPNCorr"));
            
            setCurrentMonGroup("HLT/TauMon/Expert/"+m_trigItems[i]+"/EFTau/BDT/1p_Corrected");
            plotUnderOverFlow(hist("hEFinnerTrkAvgDist1PCorr"));
            plotUnderOverFlow(hist("hEFetOverPtLeadTrk1PCorr"));
            plotUnderOverFlow(hist("hEFipSigLeadTrk1PCorr"));
            plotUnderOverFlow(hist("hEFSumPtTrkFrac1PCorr"));
            plotUnderOverFlow(hist("hEFChPiEMEOverCaloEME1PCorr"));
            plotUnderOverFlow(hist("hEFEMPOverTrkSysP1PCorr"));
            plotUnderOverFlow(hist("hEFcentFrac1PCorr"));
            plotUnderOverFlow(hist("hEFptRatioEflowApprox1PCorr"));
            
            setCurrentMonGroup("HLT/TauMon/Expert/"+m_trigItems[i]+"/EFTau/BDT/mp_Corrected");
            plotUnderOverFlow(hist("hEFinnerTrkAvgDistMPCorr"));
            plotUnderOverFlow(hist("hEFetOverPtLeadTrkMPCorr"));
            plotUnderOverFlow(hist("hEFChPiEMEOverCaloEMEMPCorr"));
            plotUnderOverFlow(hist("hEFEMPOverTrkSysPMPCorr"));
            plotUnderOverFlow(hist("hEFcentFracMPCorr"));
            plotUnderOverFlow(hist("hEFptRatioEflowApproxMPCorr"));
            plotUnderOverFlow(hist("hEFdRmaxMPCorr"));
            plotUnderOverFlow(hist("hEFtrFlightPathSigMPCorr"));
            plotUnderOverFlow(hist("hEFmassTrkSysMPCorr"));
            plotUnderOverFlow(hist("hEFmEflowApproxMPCorr"));
            
            setCurrentMonGroup("HLT/TauMon/Expert/"+m_trigItems[i]+"/L1VsOffline");
            plotUnderOverFlow(hist("hL1EtRatio"));
            
            
            setCurrentMonGroup("HLT/TauMon/Expert/"+m_trigItems[i]+"/PreselectionVsOffline");
            plotUnderOverFlow(hist("hEFEtRatio"));
            plotUnderOverFlow(hist("hEtaRatio"));
            plotUnderOverFlow(hist("hPhiRatio"));
            
            setCurrentMonGroup("HLT/TauMon/Expert/"+m_trigItems[i]+"/EFVsOffline");
            plotUnderOverFlow(hist("hptRatio"));
            plotUnderOverFlow(hist("hetaRatio"));
            plotUnderOverFlow(hist("hphiRatio"));
            //plotUnderOverFlow(hist("hEMRadiusRatio"));
            //plotUnderOverFlow(hist("hHadRadiusRatio"));
            plotUnderOverFlow(hist("hIsoFracRatio"));
            //plotUnderOverFlow(hist("hPSSFracRatio"));
            plotUnderOverFlow(hist("hEMFracRatio"));
            plotUnderOverFlow(hist("hEtRawRatio"));
            plotUnderOverFlow(hist("hOffEFEMDiff"));
            plotUnderOverFlow(hist("hOffEFHADDiff"));
            
            setCurrentMonGroup("HLT/TauMon/Expert/"+m_trigItems[i]+"/EFVsOffline/BDT/1p_nonCorrected");
            plotUnderOverFlow(hist("hInnerTrkAvgDistRatio1P"));
            plotUnderOverFlow(hist("hEtOverPtLeadTrkRatio1P"));
            plotUnderOverFlow(hist("hIpSigLeadTrkRatio1P"));
            plotUnderOverFlow(hist("hSumPtTrkFracRatio1P"));
            plotUnderOverFlow(hist("hChPiEMEOverCaloEMERatio1P"));
            plotUnderOverFlow(hist("hEMPOverTrkSysPRatio1P"));
            plotUnderOverFlow(hist("hCentFracRatio1P"));
            plotUnderOverFlow(hist("hPtRatioEflowApproxRatio1P"));
            plotUnderOverFlow(hist("hDRmaxRatio1P"));
            plotUnderOverFlow(hist("hTopoInvMassRatio1P"));
            
            setCurrentMonGroup("HLT/TauMon/Expert/"+m_trigItems[i]+"/EFVsOffline/BDT/mp_nonCorrected");
            plotUnderOverFlow(hist("hInnerTrkAvgDistRatioMP"));
            plotUnderOverFlow(hist("hEtOverPtLeadTrkRatioMP"));
            plotUnderOverFlow(hist("hChPiEMEOverCaloEMERatioMP"));
            plotUnderOverFlow(hist("hEMPOverTrkSysPRatioMP"));
            plotUnderOverFlow(hist("hCentFracRatioMP"));
            plotUnderOverFlow(hist("hPtRatioEflowApproxRatioMP"));
            plotUnderOverFlow(hist("hDRmaxRatioMP"));
            plotUnderOverFlow(hist("hTrFlightPathSigRatioMP"));
            plotUnderOverFlow(hist("hMassTrkSysRatioMP"));
            plotUnderOverFlow(hist("hMEflowApproxRatioMP"));
            
            if(m_truth && m_turnOnCurves)
            {
                //Truth
	      //divide("hTrueL1PtNum","hTrueTauPtDenom","hTrueL1PtEfficiency","HLT/TauMon/Expert/"+m_trigItems[i]+"/TurnOnCurves/TruthEfficiency");
	      //                divide("hTrueL1Pt1PNum","hTrueTauPt1PDenom","hTrueL1Pt1PEfficiency","HLT/TauMon/Expert/"+m_trigItems[i]+"/TurnOnCurves/TruthEfficiency");
	      //                divide("hTrueL1Pt3PNum","hTrueTauPt3PDenom","hTrueL1Pt3PEfficiency","HLT/TauMon/Expert/"+m_trigItems[i]+"/TurnOnCurves/TruthEfficiency");
	      //                divide("hTrueL1EtaNum","hTrueTauEtaDenom","hTrueL1EtaEfficiency","HLT/TauMon/Expert/"+m_trigItems[i]+"/TurnOnCurves/TruthEfficiency");
	      //                divide("hTrueL1PhiNum","hTrueTauPhiDenom","hTrueL1PhiEfficiency","HLT/TauMon/Expert/"+m_trigItems[i]+"/TurnOnCurves/TruthEfficiency");
	      //                divide("hTrueL1NTrackNum","hTrueTauNTrackDenom","hTrueL1NTrackEfficiency","HLT/TauMon/Expert/"+m_trigItems[i]+"/TurnOnCurves/TruthEfficiency");
	      //                divide("hTrueL1NVtxNum","hTrueTauNVtxDenom","hTrueL1NVtxEfficiency","HLT/TauMon/Expert/"+m_trigItems[i]+"/TurnOnCurves/TruthEfficiency");
	      //                divide("hTrueL1MuNum","hTrueTauMuDenom","hTrueL1MuEfficiency","HLT/TauMon/Expert/"+m_trigItems[i]+"/TurnOnCurves/TruthEfficiency");
	      //divide2("hTrueL1EtaVsPhiNum","hTrueTauEtaVsPhiDenom","hTrueL1EtaVsPhiEfficiency","HLT/TauMon/Expert/"+m_trigItems[i]+"/TurnOnCurves/TruthEfficiency");
	      //                divide("hTrueHLTPtNum","hTrueTauPtDenom","hTrueHLTPtEfficiency","HLT/TauMon/Expert/"+m_trigItems[i]+"/TurnOnCurves/TruthEfficiency");
	      //                divide("hTrueHLTPt1PNum","hTrueTauPt1PDenom","hTrueHLTPt1PEfficiency","HLT/TauMon/Expert/"+m_trigItems[i]+"/TurnOnCurves/TruthEfficiency");
	      //                divide("hTrueHLTPt3PNum","hTrueTauPt3PDenom","hTrueHLTPt3PEfficiency","HLT/TauMon/Expert/"+m_trigItems[i]+"/TurnOnCurves/TruthEfficiency");
	      //                divide("hTrueHLTEtaNum","hTrueTauEtaDenom","hTrueHLTEtaEfficiency","HLT/TauMon/Expert/"+m_trigItems[i]+"/TurnOnCurves/TruthEfficiency");
	      //                divide("hTrueHLTPhiNum","hTrueTauPhiDenom","hTrueHLTPhiEfficiency","HLT/TauMon/Expert/"+m_trigItems[i]+"/TurnOnCurves/TruthEfficiency");
	      //                divide("hTrueHLTNTrackNum","hTrueTauNTrackDenom","hTrueHLTNTrackEfficiency","HLT/TauMon/Expert/"+m_trigItems[i]+"/TurnOnCurves/TruthEfficiency");
	      //                divide("hTrueHLTNVtxNum","hTrueTauNVtxDenom","hTrueHLTNVtxEfficiency","HLT/TauMon/Expert/"+m_trigItems[i]+"/TurnOnCurves/TruthEfficiency");
	      //                divide("hTrueHLTMuNum","hTrueTauMuDenom","hTrueHLTMuEfficiency","HLT/TauMon/Expert/"+m_trigItems[i]+"/TurnOnCurves/TruthEfficiency");
	      //divide2("hTrueHLTEtaVsPhiNum","hTrueTauEtaVsPhiDenom","hTrueHLTEtaVsPhiEfficiency","HLT/TauMon/Expert/"+m_trigItems[i]+"/TurnOnCurves/TruthEfficiency");
                //Truth+Reco
	      //                divide("hTruthRecoL1PtNum","hTruthRecoTauPtDenom","hTruthRecoL1PtEfficiency","HLT/TauMon/Expert/"+m_trigItems[i]+"/TurnOnCurves/Truth+RecoEfficiency");
	      //                divide("hTruthRecoL1Pt1PNum","hTruthRecoTauPt1PDenom","hTruthRecoL1Pt1PEfficiency","HLT/TauMon/Expert/"+m_trigItems[i]+"/TurnOnCurves/Truth+RecoEfficiency");
	      //                divide("hTruthRecoL1Pt3PNum","hTruthRecoTauPt3PDenom","hTruthRecoL1Pt3PEfficiency","HLT/TauMon/Expert/"+m_trigItems[i]+"/TurnOnCurves/Truth+RecoEfficiency");
	      //                divide("hTruthRecoL1EtaNum","hTruthRecoTauEtaDenom","hTruthRecoL1EtaEfficiency","HLT/TauMon/Expert/"+m_trigItems[i]+"/TurnOnCurves/Truth+RecoEfficiency");
	      //                divide("hTruthRecoL1PhiNum","hTruthRecoTauPhiDenom","hTruthRecoL1PhiEfficiency","HLT/TauMon/Expert/"+m_trigItems[i]+"/TurnOnCurves/Truth+RecoEfficiency");
	      //                divide("hTruthRecoL1NTrackNum","hTruthRecoTauNTrackDenom","hTruthRecoL1NTrackEfficiency","HLT/TauMon/Expert/"+m_trigItems[i]+"/TurnOnCurves/Truth+RecoEfficiency");
	      //                divide("hTruthRecoL1NVtxNum","hTruthRecoTauNVtxDenom","hTruthRecoL1NVtxEfficiency","HLT/TauMon/Expert/"+m_trigItems[i]+"/TurnOnCurves/Truth+RecoEfficiency");
	      //                divide("hTruthRecoL1MuNum","hTruthRecoTauMuDenom","hTruthRecoL1MuEfficiency","HLT/TauMon/Expert/"+m_trigItems[i]+"/TurnOnCurves/Truth+RecoEfficiency");
	      //divide2("hTruthRecoL1EtaVsPhiNum","hTruthRecoTauEtaVsPhiDenom","hTruthRecoL1EtaVsPhiEfficiency","HLT/TauMon/Expert/"+m_trigItems[i]+"/TurnOnCurves/Truth+RecoEfficiency");
	      //                divide("hTruthRecoHLTPtNum","hTruthRecoTauPtDenom","hTruthRecoHLTPtEfficiency","HLT/TauMon/Expert/"+m_trigItems[i]+"/TurnOnCurves/Truth+RecoEfficiency");
	      //                divide("hTruthRecoHLTPt1PNum","hTruthRecoTauPt1PDenom","hTruthRecoHLTPt1PEfficiency","HLT/TauMon/Expert/"+m_trigItems[i]+"/TurnOnCurves/Truth+RecoEfficiency");
	      //                divide("hTruthRecoHLTPt3PNum","hTruthRecoTauPt3PDenom","hTruthRecoHLTPt3PEfficiency","HLT/TauMon/Expert/"+m_trigItems[i]+"/TurnOnCurves/Truth+RecoEfficiency");
	      //                divide("hTruthRecoHLTEtaNum","hTruthRecoTauEtaDenom","hTruthRecoHLTEtaEfficiency","HLT/TauMon/Expert/"+m_trigItems[i]+"/TurnOnCurves/Truth+RecoEfficiency");
	      //                divide("hTruthRecoHLTPhiNum","hTruthRecoTauPhiDenom","hTruthRecoHLTPhiEfficiency","HLT/TauMon/Expert/"+m_trigItems[i]+"/TurnOnCurves/Truth+RecoEfficiency");
	      //                divide("hTruthRecoHLTNTrackNum","hTruthRecoTauNTrackDenom","hTruthRecoHLTNTrackEfficiency","HLT/TauMon/Expert/"+m_trigItems[i]+"/TurnOnCurves/Truth+RecoEfficiency");
	      //                divide("hTruthRecoHLTNVtxNum","hTruthRecoTauNVtxDenom","hTruthRecoHLTNVtxEfficiency","HLT/TauMon/Expert/"+m_trigItems[i]+"/TurnOnCurves/Truth+RecoEfficiency");
	      //                divide("hTruthRecoHLTMuNum","hTruthRecoTauMuDenom","hTruthRecoHLTMuEfficiency","HLT/TauMon/Expert/"+m_trigItems[i]+"/TurnOnCurves/Truth+RecoEfficiency");
	      //divide2("hTruthRecoHLTEtaVsPhiNum","hTruthRecoTauEtaVsPhiDenom","hTruthRecoHLTEtaVsPhiEfficiency","HLT/TauMon/Expert/"+m_trigItems[i]+"/TurnOnCurves/Truth+RecoEfficiency");
                
                //Combined trig eff
                /*divide("hCombL1TauPtNum","hCombTauPtDenom","hCombL1TauPtEfficiency","HLT/TauMon/Expert/"+m_trigItems[i]+"/TurnOnCurves/TauComboEfficiency");
                divide("hCombHLTTauPtNum","hCombTauPtDenom","hCombHLTTauPtEfficiency","HLT/TauMon/Expert/"+m_trigItems[i]+"/TurnOnCurves/TauComboEfficiency");
                divide("hCombL1elPtNum","hCombelPtDenom","hCombL1elPtEfficiency","HLT/TauMon/Expert/"+m_trigItems[i]+"/TurnOnCurves/TauComboEfficiency");
                divide("hCombHLTelPtNum","hCombelPtDenom","hCombHLTelPtEfficiency","HLT/TauMon/Expert/"+m_trigItems[i]+"/TurnOnCurves/TauComboEfficiency");
                divide("hCombL1muPtNum","hCombmuPtDenom","hCombL1muPtEfficiency","HLT/TauMon/Expert/"+m_trigItems[i]+"/TurnOnCurves/TauComboEfficiency");
                divide("hCombHLTmuPtNum","hCombmuPtDenom","hCombHLTmuPtEfficiency","HLT/TauMon/Expert/"+m_trigItems[i]+"/TurnOnCurves/TauComboEfficiency");
                divide("hCombL1OffjetPtNum","hCombOffjetPtDenom","hCombL1OffjetPtEfficiency","HLT/TauMon/Expert/"+m_trigItems[i]+"/TurnOnCurves/TauComboEfficiency");
                divide("hCombHLTOffjetPtNum","hCombOffjetPtDenom","hCombHLTOffjetPtEfficiency","HLT/TauMon/Expert/"+m_trigItems[i]+"/TurnOnCurves/TauComboEfficiency");
                divide("hCombL1METNum","hCombMETDenom","hCombL1METEfficiency","HLT/TauMon/Expert/"+m_trigItems[i]+"/TurnOnCurves/TauComboEfficiency");
                divide("hCombHLTMETNum","hCombMETDenom","hCombHLTMETEfficiency","HLT/TauMon/Expert/"+m_trigItems[i]+"/TurnOnCurves/TauComboEfficiency");
                divide("hCombL1dRNum","hCombdRDenom","hCombL1dREfficiency","HLT/TauMon/Expert/"+m_trigItems[i]+"/TurnOnCurves/TauComboEfficiency");
                divide("hCombHLTdRNum","hCombdRDenom","hCombHLTdREfficiency","HLT/TauMon/Expert/"+m_trigItems[i]+"/TurnOnCurves/TauComboEfficiency");
                divide("hCombL1dEtaNum","hCombdEtaDenom","hCombL1dEtaEfficiency","HLT/TauMon/Expert/"+m_trigItems[i]+"/TurnOnCurves/TauComboEfficiency");
                divide("hCombHLTdEtaNum","hCombdEtaDenom","hCombHLTdEtaEfficiency","HLT/TauMon/Expert/"+m_trigItems[i]+"/TurnOnCurves/TauComboEfficiency");
                divide("hCombL1dPhiNum","hCombdPhiDenom","hCombL1dPhiEfficiency","HLT/TauMon/Expert/"+m_trigItems[i]+"/TurnOnCurves/TauComboEfficiency");
                divide("hCombHLTdPhiNum","hCombdPhiDenom","hCombHLTdPhiEfficiency","HLT/TauMon/Expert/"+m_trigItems[i]+"/TurnOnCurves/TauComboEfficiency");*/
            }
            if(m_turnOnCurves)
            {
                //Reco Only
	      //                divide("hRecoL1PtNum","hRecoTauPtDenom","hRecoL1PtEfficiency","HLT/TauMon/Expert/"+m_trigItems[i]+"/TurnOnCurves/RecoEfficiency");
	      //                divide("hRecoL1Pt1PNum","hRecoTauPt1PDenom","hRecoL1Pt1PEfficiency","HLT/TauMon/Expert/"+m_trigItems[i]+"/TurnOnCurves/RecoEfficiency");
	      //                divide("hRecoL1Pt3PNum","hRecoTauPt3PDenom","hRecoL1Pt3PEfficiency","HLT/TauMon/Expert/"+m_trigItems[i]+"/TurnOnCurves/RecoEfficiency");
	      //                divide("hRecoL1EtaNum","hRecoTauEtaDenom","hRecoL1EtaEfficiency","HLT/TauMon/Expert/"+m_trigItems[i]+"/TurnOnCurves/RecoEfficiency");
	      //                divide("hRecoL1PhiNum","hRecoTauPhiDenom","hRecoL1PhiEfficiency","HLT/TauMon/Expert/"+m_trigItems[i]+"/TurnOnCurves/RecoEfficiency");
	      //                divide("hRecoL1NTrackNum","hRecoTauNTrackDenom","hRecoL1NTrackEfficiency","HLT/TauMon/Expert/"+m_trigItems[i]+"/TurnOnCurves/RecoEfficiency");
	      //                divide("hRecoL1NVtxNum","hRecoTauNVtxDenom","hRecoL1NVtxEfficiency","HLT/TauMon/Expert/"+m_trigItems[i]+"/TurnOnCurves/RecoEfficiency");
	      //                divide("hRecoL1MuNum","hRecoTauMuDenom","hRecoL1MuEfficiency","HLT/TauMon/Expert/"+m_trigItems[i]+"/TurnOnCurves/RecoEfficiency");
	      //                divide2("hRecoL1EtaVsPhiNum","hRecoTauEtaVsPhiDenom","hRecoL1EtaVsPhiEfficiency","HLT/TauMon/Expert/"+m_trigItems[i]+"/TurnOnCurves/RecoEfficiency");
	      //                divide("hRecoHLTPtNum","hRecoTauPtDenom","hRecoHLTPtEfficiency","HLT/TauMon/Expert/"+m_trigItems[i]+"/TurnOnCurves/RecoEfficiency");
	      //                divide("hRecoHLTPt1PNum","hRecoTauPt1PDenom","hRecoHLTPt1PEfficiency","HLT/TauMon/Expert/"+m_trigItems[i]+"/TurnOnCurves/RecoEfficiency");
	      //                divide("hRecoHLTPt3PNum","hRecoTauPt3PDenom","hRecoHLTPt3PEfficiency","HLT/TauMon/Expert/"+m_trigItems[i]+"/TurnOnCurves/RecoEfficiency");
	      //                divide("hRecoHLTEtaNum","hRecoTauEtaDenom","hRecoHLTEtaEfficiency","HLT/TauMon/Expert/"+m_trigItems[i]+"/TurnOnCurves/RecoEfficiency");
	      //                divide("hRecoHLTPhiNum","hRecoTauPhiDenom","hRecoHLTPhiEfficiency","HLT/TauMon/Expert/"+m_trigItems[i]+"/TurnOnCurves/RecoEfficiency");
	      //                divide("hRecoHLTNTrackNum","hRecoTauNTrackDenom","hRecoHLTNTrackEfficiency","HLT/TauMon/Expert/"+m_trigItems[i]+"/TurnOnCurves/RecoEfficiency");
	      //                divide("hRecoHLTNVtxNum","hRecoTauNVtxDenom","hRecoHLTNVtxEfficiency","HLT/TauMon/Expert/"+m_trigItems[i]+"/TurnOnCurves/RecoEfficiency");
	      //                divide("hRecoHLTMuNum","hRecoTauMuDenom","hRecoHLTMuEfficiency","HLT/TauMon/Expert/"+m_trigItems[i]+"/TurnOnCurves/RecoEfficiency");
	      //                divide2("hRecoHLTEtaVsPhiNum","hRecoTauEtaVsPhiDenom","hRecoHLTEtaVsPhiEfficiency","HLT/TauMon/Expert/"+m_trigItems[i]+"/TurnOnCurves/RecoEfficiency");
                
            }
 
	   
        }//End of trigItem loop


      
	if(m_turnOnCurves){
	  /*
		setCurrentMonGroup("HLT/TauMon/Expert/HLTefficiency"); 

		plotUnderOverFlow(hist("hRecoHLT25PtNum"));
		plotUnderOverFlow(hist("hRecoHLT25Pt1PNum"));
		plotUnderOverFlow(hist("hRecoHLT25Pt3PNum"));
		plotUnderOverFlow(hist("hRecoHLT25EtaNum"));
		plotUnderOverFlow(hist("hRecoHLT25PhiNum"));	
		plotUnderOverFlow(hist("hRecoHLT25NTrackNum"));
		plotUnderOverFlow(hist("hRecoHLT25NVtxNum"));
		plotUnderOverFlow(hist("hRecoHLT25MuNum"));		

		plotUnderOverFlow(hist("hRecoTau25PtDenom"));
                plotUnderOverFlow(hist("hRecoTau25Pt1PDenom"));
                plotUnderOverFlow(hist("hRecoTau25Pt3PDenom"));
                plotUnderOverFlow(hist("hRecoTau25EtaDenom"));
                plotUnderOverFlow(hist("hRecoTau25PhiDenom"));
                plotUnderOverFlow(hist("hRecoTau25NTrackDenom"));    
                plotUnderOverFlow(hist("hRecoTau25NVtxDenom"));
                plotUnderOverFlow(hist("hRecoTau25MuDenom"));
   
                plotUnderOverFlow(hist("hRecoHLT25PtEfficiency"));
                plotUnderOverFlow(hist("hRecoHLT25Pt1PEfficiency"));
                plotUnderOverFlow(hist("hRecoHLT25Pt3PEfficiency"));
                plotUnderOverFlow(hist("hRecoHLT25EtaEfficiency"));
                plotUnderOverFlow(hist("hRecoHLT25PhiEfficiency"));
                plotUnderOverFlow(hist("hRecoHLT25NTrackEfficiency"));
                plotUnderOverFlow(hist("hRecoHLT25NVtxEfficiency"));
                plotUnderOverFlow(hist("hRecoHLT25MuEfficiency"));
    

                plotUnderOverFlow(hist("hRecoHLT25PtNum_2"));
                plotUnderOverFlow(hist("hRecoHLT25Pt1PNum_2"));
                plotUnderOverFlow(hist("hRecoHLT25Pt3PNum_2"));
                plotUnderOverFlow(hist("hRecoHLT25EtaNum_2"));
                plotUnderOverFlow(hist("hRecoHLT25PhiNum_2"));    
                plotUnderOverFlow(hist("hRecoHLT25NTrackNum_2"));
                plotUnderOverFlow(hist("hRecoHLT25NVtxNum_2"));
                plotUnderOverFlow(hist("hRecoHLT25MuNum_2"));             
     
                plotUnderOverFlow(hist("hRecoTau25PtDenom_2"));
                plotUnderOverFlow(hist("hRecoTau25Pt1PDenom_2"));
                plotUnderOverFlow(hist("hRecoTau25Pt3PDenom_2"));
                plotUnderOverFlow(hist("hRecoTau25EtaDenom_2"));
                plotUnderOverFlow(hist("hRecoTau25PhiDenom_2"));
                plotUnderOverFlow(hist("hRecoTau25NTrackDenom_2"));
                plotUnderOverFlow(hist("hRecoTau25NVtxDenom_2"));
                plotUnderOverFlow(hist("hRecoTau25MuDenom_2"));
      
                plotUnderOverFlow(hist("hRecoHLT25PtEfficiency_2"));
                plotUnderOverFlow(hist("hRecoHLT25Pt1PEfficiency_2"));
                plotUnderOverFlow(hist("hRecoHLT25Pt3PEfficiency_2"));
                plotUnderOverFlow(hist("hRecoHLT25EtaEfficiency_2"));
                plotUnderOverFlow(hist("hRecoHLT25PhiEfficiency_2"));
                plotUnderOverFlow(hist("hRecoHLT25NTrackEfficiency_2"));
                plotUnderOverFlow(hist("hRecoHLT25NVtxEfficiency_2"));
                plotUnderOverFlow(hist("hRecoHLT25MuEfficiency_2"));
       

		divide("hRecoHLT25PtNum","hRecoTau25PtDenom","hRecoHLT25PtEfficiency","HLT/TauMon/Expert/HLTefficiency");
		divide("hRecoHLT25Pt1PNum","hRecoTau25Pt1PDenom","hRecoHLT25Pt1PEfficiency","HLT/TauMon/Expert/HLTefficiency");
		divide("hRecoHLT25Pt3PNum","hRecoTau25Pt3PDenom","hRecoHLT25Pt3PEfficiency","HLT/TauMon/Expert/HLTefficiency");
		divide("hRecoHLT25EtaNum","hRecoTau25EtaDenom","hRecoHLT25EtaEfficiency","HLT/TauMon/Expert/HLTefficiency");
		divide("hRecoHLT25PhiNum","hRecoTau25PhiDenom","hRecoHLT25PhiEfficiency","HLT/TauMon/Expert/HLTefficiency");
		divide("hRecoHLT25NTrackNum","hRecoTau25NTrackDenom","hRecoHLT25NTrackEfficiency","HLT/TauMon/Expert/HLTefficiency");
		divide("hRecoHLT25NVtxNum","hRecoTau25NVtxDenom","hRecoHLT25NVtxEfficiency","HLT/TauMon/Expert/HLTefficiency");
		divide("hRecoHLT25MuNum","hRecoTau25MuDenom","hRecoHLT25MuEfficiency","HLT/TauMon/Expert/HLTefficiency");
*/	
		divide2("hRecoHLT25EtaVsPhiNum","hRecoTau25EtaVsPhiDenom","hRecoHLT25EtaVsPhiEfficiency","HLT/TauMon/Expert/HLTefficiency");
		divide2("hRecoHLT25EtaVsPhiNum_2","hRecoTau25EtaVsPhiDenom_2","hRecoHLT25EtaVsPhiEfficiency_2","HLT/TauMon/Expert/HLTefficiency");
/*
		divide("hRecoHLT25PtNum_2","hRecoTau25PtDenom_2","hRecoHLT25PtEfficiency_2","HLT/TauMon/Expert/HLTefficiency");
                divide("hRecoHLT25Pt1PNum_2","hRecoTau25Pt1PDenom_2","hRecoHLT25Pt1PEfficiency_2","HLT/TauMon/Expert/HLTefficiency");
                divide("hRecoHLT25Pt3PNum_2","hRecoTau25Pt3PDenom_2","hRecoHLT25Pt3PEfficiency_2","HLT/TauMon/Expert/HLTefficiency");
                divide("hRecoHLT25EtaNum_2","hRecoTau25EtaDenom_2","hRecoHLT25EtaEfficiency_2","HLT/TauMon/Expert/HLTefficiency");
                divide("hRecoHLT25PhiNum_2","hRecoTau25PhiDenom_2","hRecoHLT25PhiEfficiency_2","HLT/TauMon/Expert/HLTefficiency");
                divide("hRecoHLT25NTrackNum_2","hRecoTau25NTrackDenom_2","hRecoHLT25NTrackEfficiency_2","HLT/TauMon/Expert/HLTefficiency");
                divide("hRecoHLT25NVtxNum_2","hRecoTau25NVtxDenom_2","hRecoHLT25NVtxEfficiency_2","HLT/TauMon/Expert/HLTefficiency");
                divide("hRecoHLT25MuNum_2","hRecoTau25MuDenom_2","hRecoHLT25MuEfficiency_2","HLT/TauMon/Expert/HLTefficiency");
                
*/
		setCurrentMonGroup("HLT/TauMon/Expert/HLTefficiency");

                
		//plotUnderOverFlow(hist("hHLTdRDenom"));
    		//plotUnderOverFlow(hist("hHLTdRNum_tsf"));
    		//plotUnderOverFlow(hist("hHLTdRNum_notsf"));
    		//plotUnderOverFlow(hist("hHLTdRNum_jet"));

	}

    	for(unsigned int i=0;i<m_topo_chains_ditau.size(); ++i){ 
        	setCurrentMonGroup("HLT/TauMon/Expert/TopoDiTau/"+m_topo_chains_ditau.at(i));  
		plotUnderOverFlow(hist("hHLTdR"));  
    	}

	if(m_doTopoValidation){
		for(unsigned int topo=0;topo<m_topo_chains_ditau.size();topo++){
			setCurrentMonGroup("HLT/TauMon/Expert/TopoValidation/"+m_topo_chains_ditau.at(topo));
			plotUnderOverFlow(hist("hDR"));	
			plotUnderOverFlow(hist("hDRjets"));
			plotUnderOverFlow(hist("hDRnoJets"));
		        plotUnderOverFlow(hist("hDR_noTopo"));
                        plotUnderOverFlow(hist("hDRjets_noTopo"));
                        plotUnderOverFlow(hist("hDRnoJets_noTopo"));
		}
	}

	/*if(m_doTestTracking){
		setCurrentMonGroup("HLT/TauMon/Expert/FTF_track_comparison");
	}*/


        // clone shifter histograms
        std::vector<string> lowest_names, lowest_trigger_names;
        lowest_names.push_back("lowest_singletau"); 	lowest_trigger_names.push_back(m_lowest_singletau);
        //lowest_names.push_back("lowest_ditau"); 	lowest_trigger_names.push_back(m_lowest_ditau);
        //lowest_names.push_back("lowest_etau"); 	lowest_trigger_names.push_back(m_lowest_etau);
        //lowest_names.push_back("lowest_mutau");	lowest_trigger_names.push_back(m_lowest_mutau);
        //lowest_names.push_back("lowest_mettau");	lowest_trigger_names.push_back(m_lowest_mettau);
        //lowest_names.push_back("cosmic_chain");       lowest_trigger_names.push_back(m_cosmic_chain);
        
        for(unsigned int i=0;i<lowest_names.size();++i)
        {
            bool good_item(false);
            for(unsigned int j=0;j<m_trigItems.size();++j)
            {
                if(m_trigItems.at(j)==lowest_trigger_names.at(i)) good_item = true;
            }
            if(!good_item) continue;
            
            setCurrentMonGroup("HLT/TauMon/Shifter/"+lowest_names.at(i)+"/L1RoI");
            cloneHistogram("hL1RoIEta","HLT/TauMon/Expert/"+lowest_trigger_names.at(i)+"/L1RoI");
            cloneHistogram("hL1RoIPhi","HLT/TauMon/Expert/"+lowest_trigger_names.at(i)+"/L1RoI");
            cloneHistogram("hL1RoITauClus","HLT/TauMon/Expert/"+lowest_trigger_names.at(i)+"/L1RoI");
            cloneHistogram("hL1RoIeT","HLT/TauMon/Expert/"+lowest_trigger_names.at(i)+"/L1RoI");
            cloneHistogram("hL1RoIisol","HLT/TauMon/Expert/"+lowest_trigger_names.at(i)+"/L1RoI");
            cloneHistogram("hL1RoIEMIso","HLT/TauMon/Expert/"+lowest_trigger_names.at(i)+"/L1RoI");
            cloneHistogram("hL1RoIHadCore","HLT/TauMon/Expert/"+lowest_trigger_names.at(i)+"/L1RoI");
            cloneHistogram("hL1RoIHadIsol","HLT/TauMon/Expert/"+lowest_trigger_names.at(i)+"/L1RoI");
            cloneHistogram2("hL1EtaVsPhi","HLT/TauMon/Expert/"+lowest_trigger_names.at(i)+"/L1RoI");
            cloneHistogram2("hL1RoITauClusEMIso","HLT/TauMon/Expert/"+lowest_trigger_names.at(i)+"/L1RoI");

	   // if(m_emulation){
		//divide("hL1Emulation","hL1EmulationPassTDT","hL1Emulation","HLT/TauMon/Expert/Emulation");
		setCurrentMonGroup("HLT/TauMon/Shifter/"+lowest_names.at(i)+"/Emulation");
	   // 	cloneProfile("hL1Emulation","HLT/TauMon/Expert/Emulation");
	   // }

            setCurrentMonGroup("HLT/TauMon/Shifter/"+lowest_names.at(i)+"/PreselectionTau");
            cloneHistogram("hEFEt","HLT/TauMon/Expert/"+lowest_trigger_names.at(i)+"/PreselectionTau");
            cloneHistogram("hFTFnTrack","HLT/TauMon/Expert/"+lowest_trigger_names.at(i)+"/PreselectionTau");
            cloneHistogram("hFTFnWideTrack","HLT/TauMon/Expert/"+lowest_trigger_names.at(i)+"/PreselectionTau");
            cloneHistogram("hEta","HLT/TauMon/Expert/"+lowest_trigger_names.at(i)+"/PreselectionTau");
            cloneHistogram("hPhi","HLT/TauMon/Expert/"+lowest_trigger_names.at(i)+"/PreselectionTau");
            //cloneHistogram("hdRmax","HLT/TauMon/Expert/"+lowest_trigger_names.at(i)+"/PreselectionTau");
            cloneHistogram2("hEFEtaVsPhi","HLT/TauMon/Expert/"+lowest_trigger_names.at(i)+"/PreselectionTau");
            
            setCurrentMonGroup("HLT/TauMon/Shifter/"+lowest_names.at(i)+"/PreselectionVsOffline");
            cloneHistogram2("hPreselvsOffnTrks","HLT/TauMon/Expert/"+lowest_trigger_names.at(i)+"/PreselectionVsOffline");
            cloneHistogram2("hPreselvsOffnWideTrks","HLT/TauMon/Expert/"+lowest_trigger_names.at(i)+"/PreselectionVsOffline");
            
            setCurrentMonGroup("HLT/TauMon/Shifter/"+lowest_names.at(i)+"/EFTau");
            cloneHistogram("hEFEt","HLT/TauMon/Expert/"+lowest_trigger_names.at(i)+"/EFTau");
            cloneHistogram("hEFEta","HLT/TauMon/Expert/"+lowest_trigger_names.at(i)+"/EFTau");
            cloneHistogram("hEFPhi","HLT/TauMon/Expert/"+lowest_trigger_names.at(i)+"/EFTau");
            cloneHistogram("hEFnTrack","HLT/TauMon/Expert/"+lowest_trigger_names.at(i)+"/EFTau");
            cloneHistogram("hEFnWideTrack","HLT/TauMon/Expert/"+lowest_trigger_names.at(i)+"/EFTau");
            cloneHistogram("hEFEtRaw","HLT/TauMon/Expert/"+lowest_trigger_names.at(i)+"/EFTau");
            //cloneHistogram("hEFEMRadius","HLT/TauMon/Expert/"+lowest_trigger_names.at(i)+"/EFTau");
            //cloneHistogram("hEFHADRadius","HLT/TauMon/Expert/"+lowest_trigger_names.at(i)+"/EFTau");
            cloneHistogram("hEFIsoFrac","HLT/TauMon/Expert/"+lowest_trigger_names.at(i)+"/EFTau");
            //cloneHistogram("hEFPSSFraction","HLT/TauMon/Expert/"+lowest_trigger_names.at(i)+"/EFTau");
            cloneHistogram("hEFEMFraction","HLT/TauMon/Expert/"+lowest_trigger_names.at(i)+"/EFTau");
            cloneHistogram("hScore1p","HLT/TauMon/Expert/"+lowest_trigger_names.at(i)+"/EFTau");
            cloneHistogram("hScoremp","HLT/TauMon/Expert/"+lowest_trigger_names.at(i)+"/EFTau");
            cloneHistogram2("hEFEtaVsPhi","HLT/TauMon/Expert/"+lowest_trigger_names.at(i)+"/EFTau");
            cloneHistogram2("hEFEtVsPhi","HLT/TauMon/Expert/"+lowest_trigger_names.at(i)+"/EFTau");
            cloneHistogram2("hEFEtVsEta","HLT/TauMon/Expert/"+lowest_trigger_names.at(i)+"/EFTau");
            //      setCurrentMonGroup("HLT/TauMon/Shifter/"+lowest_names.at(i)+"/EFTau/BDT/1p_nonCorrected");
            cloneHistogram("hEFinnerTrkAvgDist1PNCorr","HLT/TauMon/Expert/"+lowest_trigger_names.at(i)+"/EFTau/BDT/1p_nonCorrected");
            cloneHistogram("hEFetOverPtLeadTrk1PNCorr","HLT/TauMon/Expert/"+lowest_trigger_names.at(i)+"/EFTau/BDT/1p_nonCorrected");
            cloneHistogram("hEFipSigLeadTrk1PNCorr","HLT/TauMon/Expert/"+lowest_trigger_names.at(i)+"/EFTau/BDT/1p_nonCorrected");
            cloneHistogram("hEFSumPtTrkFrac1PNCorr","HLT/TauMon/Expert/"+lowest_trigger_names.at(i)+"/EFTau/BDT/1p_nonCorrected");
            //      cloneHistogram("hEFChPiEMEOverCaloEME1PNCorr","HLT/TauMon/Expert/"+lowest_trigger_names.at(i)+"/EFTau/BDT/1p_nonCorrected");
            cloneHistogram("hEFEMPOverTrkSysP1PNCorr","HLT/TauMon/Expert/"+lowest_trigger_names.at(i)+"/EFTau/BDT/1p_nonCorrected");
            cloneHistogram("hEFcentFrac1PNCorr","HLT/TauMon/Expert/"+lowest_trigger_names.at(i)+"/EFTau/BDT/1p_nonCorrected");
            //      cloneHistogram("hEFptRatioEflowApprox1PNCorr","HLT/TauMon/Expert/"+lowest_trigger_names.at(i)+"/EFTau/BDT/1p_nonCorrected");
            //      setCurrentMonGroup("HLT/TauMon/Shifter/"+lowest_names.at(i)+"/EFTau/BDT/mp_nonCorrected");
            cloneHistogram("hEFinnerTrkAvgDistMPNCorr","HLT/TauMon/Expert/"+lowest_trigger_names.at(i)+"/EFTau/BDT/mp_nonCorrected");
            cloneHistogram("hEFetOverPtLeadTrkMPNCorr","HLT/TauMon/Expert/"+lowest_trigger_names.at(i)+"/EFTau/BDT/mp_nonCorrected");
            //      cloneHistogram("hEFChPiEMEOverCaloEMEMPNCorr","HLT/TauMon/Expert/"+lowest_trigger_names.at(i)+"/EFTau/BDT/mp_nonCorrected");
            cloneHistogram("hEFEMPOverTrkSysPMPNCorr","HLT/TauMon/Expert/"+lowest_trigger_names.at(i)+"/EFTau/BDT/mp_nonCorrected");
            cloneHistogram("hEFcentFracMPNCorr","HLT/TauMon/Expert/"+lowest_trigger_names.at(i)+"/EFTau/BDT/mp_nonCorrected");
            //      cloneHistogram("hEFptRatioEflowApproxMPNCorr","HLT/TauMon/Expert/"+lowest_trigger_names.at(i)+"/EFTau/BDT/mp_nonCorrected");
            cloneHistogram("hEFdRmaxMPNCorr","HLT/TauMon/Expert/"+lowest_trigger_names.at(i)+"/EFTau/BDT/mp_nonCorrected");
            cloneHistogram("hEFtrFlightPathSigMPNCorr","HLT/TauMon/Expert/"+lowest_trigger_names.at(i)+"/EFTau/BDT/mp_nonCorrected");
            //      cloneHistogram("hEFmassTrkSysMPNCorr","HLT/TauMon/Expert/"+lowest_trigger_names.at(i)+"/EFTau/BDT/mp_nonCorrected");
            //      cloneHistogram("hEFmEflowApproxMPNCorr","HLT/TauMon/Expert/"+lowest_trigger_names.at(i)+"/EFTau/BDT/mp_nonCorrected");
            //      setCurrentMonGroup("HLT/TauMon/Shifter/"+lowest_names.at(i)+"/EFTau/BDT/1p_Corrected");
            cloneHistogram("hEFinnerTrkAvgDist1PCorr","HLT/TauMon/Expert/"+lowest_trigger_names.at(i)+"/EFTau/BDT/1p_Corrected");
            cloneHistogram("hEFetOverPtLeadTrk1PCorr","HLT/TauMon/Expert/"+lowest_trigger_names.at(i)+"/EFTau/BDT/1p_Corrected");
            cloneHistogram("hEFipSigLeadTrk1PCorr","HLT/TauMon/Expert/"+lowest_trigger_names.at(i)+"/EFTau/BDT/1p_Corrected");
            cloneHistogram("hEFSumPtTrkFrac1PCorr","HLT/TauMon/Expert/"+lowest_trigger_names.at(i)+"/EFTau/BDT/1p_Corrected");
            //      cloneHistogram("hEFChPiEMEOverCaloEME1PCorr","HLT/TauMon/Expert/"+lowest_trigger_names.at(i)+"/EFTau/BDT/1p_Corrected");
            cloneHistogram("hEFEMPOverTrkSysP1PCorr","HLT/TauMon/Expert/"+lowest_trigger_names.at(i)+"/EFTau/BDT/1p_Corrected");
            cloneHistogram("hEFcentFrac1PCorr","HLT/TauMon/Expert/"+lowest_trigger_names.at(i)+"/EFTau/BDT/1p_Corrected");
            //      cloneHistogram("hEFptRatioEflowApprox1PCorr","HLT/TauMon/Expert/"+lowest_trigger_names.at(i)+"/EFTau/BDT/1p_Corrected");
            //      setCurrentMonGroup("HLT/TauMon/Shifter/"+lowest_names.at(i)+"/EFTau/BDT/mp_Corrected");
            cloneHistogram("hEFinnerTrkAvgDistMPCorr","HLT/TauMon/Expert/"+lowest_trigger_names.at(i)+"/EFTau/BDT/mp_Corrected");
            cloneHistogram("hEFetOverPtLeadTrkMPCorr","HLT/TauMon/Expert/"+lowest_trigger_names.at(i)+"/EFTau/BDT/mp_Corrected");
            //      cloneHistogram("hEFChPiEMEOverCaloEMEMPCorr","HLT/TauMon/Expert/"+lowest_trigger_names.at(i)+"/EFTau/BDT/mp_Corrected");
            cloneHistogram("hEFEMPOverTrkSysPMPCorr","HLT/TauMon/Expert/"+lowest_trigger_names.at(i)+"/EFTau/BDT/mp_Corrected");
            cloneHistogram("hEFcentFracMPCorr","HLT/TauMon/Expert/"+lowest_trigger_names.at(i)+"/EFTau/BDT/mp_Corrected");
            //      cloneHistogram("hEFptRatioEflowApproxMPCorr","HLT/TauMon/Expert/"+lowest_trigger_names.at(i)+"/EFTau/BDT/mp_Corrected");
            cloneHistogram("hEFdRmaxMPCorr","HLT/TauMon/Expert/"+lowest_trigger_names.at(i)+"/EFTau/BDT/mp_Corrected");
            cloneHistogram("hEFtrFlightPathSigMPCorr","HLT/TauMon/Expert/"+lowest_trigger_names.at(i)+"/EFTau/BDT/mp_Corrected");
            //      cloneHistogram("hEFmassTrkSysMPCorr","HLT/TauMon/Expert/"+lowest_trigger_names.at(i)+"/EFTau/BDT/mp_Corrected");
            //      cloneHistogram("hEFmEflowApproxMPCorr","HLT/TauMon/Expert/"+lowest_trigger_names.at(i)+"/EFTau/BDT/mp_Corrected");
            
            setCurrentMonGroup("HLT/TauMon/Shifter/"+lowest_names.at(i)+"/L1VsOffline");
            cloneHistogram("hL1EtRatio","HLT/TauMon/Expert/"+lowest_trigger_names.at(i)+"/L1VsOffline");
            
            setCurrentMonGroup("HLT/TauMon/Shifter/"+lowest_names.at(i)+"/EFVsOffline");
            cloneHistogram2("hEFvsOffnTrks","HLT/TauMon/Expert/"+lowest_trigger_names.at(i)+"/EFVsOffline");
            cloneHistogram2("hEFvsOffnWideTrks","HLT/TauMon/Expert/"+lowest_trigger_names.at(i)+"/EFVsOffline");
            cloneHistogram("hptRatio","HLT/TauMon/Expert/"+lowest_trigger_names.at(i)+"/EFVsOffline");
            cloneHistogram("hetaRatio","HLT/TauMon/Expert/"+lowest_trigger_names.at(i)+"/EFVsOffline");
            cloneHistogram("hphiRatio","HLT/TauMon/Expert/"+lowest_trigger_names.at(i)+"/EFVsOffline");
            //cloneHistogram("hEMRadiusRatio","HLT/TauMon/Expert/"+lowest_trigger_names.at(i)+"/EFVsOffline");
            //cloneHistogram("hHadRadiusRatio","HLT/TauMon/Expert/"+lowest_trigger_names.at(i)+"/EFVsOffline");
            //      cloneHistogram("hIsoFracRatio","HLT/TauMon/Expert/"+lowest_trigger_names.at(i)+"/EFVsOffline");
            //      cloneHistogram("hPSSFracRatio","HLT/TauMon/Expert/"+lowest_trigger_names.at(i)+"/EFVsOffline");
            cloneHistogram("hEMFracRatio","HLT/TauMon/Expert/"+lowest_trigger_names.at(i)+"/EFVsOffline");
            cloneHistogram("hEtRawRatio","HLT/TauMon/Expert/"+lowest_trigger_names.at(i)+"/EFVsOffline");
            //      cloneHistogram("hOffEFEMDiff","HLT/TauMon/Expert/"+lowest_trigger_names.at(i)+"/EFVsOffline");
            //      cloneHistogram("hOffEFHADDiff","HLT/TauMon/Expert/"+lowest_trigger_names.at(i)+"/EFVsOffline");
            //      setCurrentMonGroup("HLT/TauMon/Shifter/"+lowest_names.at(i)+"/EFVsOffline/BDT/1p_nonCorrected");
            //      cloneHistogram("hInnerTrkAvgDistRatio1P","HLT/TauMon/Expert/"+lowest_trigger_names.at(i)+"/EFVsOffline/BDT/1p_nonCorrected");
            //      cloneHistogram("hEtOverPtLeadTrkRatio1P","HLT/TauMon/Expert/"+lowest_trigger_names.at(i)+"/EFVsOffline/BDT/1p_nonCorrected");
            //      cloneHistogram("hIpSigLeadTrkRatio1P","HLT/TauMon/Expert/"+lowest_trigger_names.at(i)+"/EFVsOffline/BDT/1p_nonCorrected");
            //      cloneHistogram("hSumPtTrkFracRatio1P","HLT/TauMon/Expert/"+lowest_trigger_names.at(i)+"/EFVsOffline/BDT/1p_nonCorrected");
            //      cloneHistogram("hChPiEMEOverCaloEMERatio1P","HLT/TauMon/Expert/"+lowest_trigger_names.at(i)+"/EFVsOffline/BDT/1p_nonCorrected");
            //      cloneHistogram("hEMPOverTrkSysPRatio1P","HLT/TauMon/Expert/"+lowest_trigger_names.at(i)+"/EFVsOffline/BDT/1p_nonCorrected");
            //      cloneHistogram("hCentFracRatio1P","HLT/TauMon/Expert/"+lowest_trigger_names.at(i)+"/EFVsOffline/BDT/1p_nonCorrected");
            //      cloneHistogram("hPtRatioEflowApproxRatio1P","HLT/TauMon/Expert/"+lowest_trigger_names.at(i)+"/EFVsOffline/BDT/1p_nonCorrected");
            //      cloneHistogram("hDRmaxRatio1P","HLT/TauMon/Expert/"+lowest_trigger_names.at(i)+"/EFVsOffline/BDT/1p_nonCorrected");
            //      cloneHistogram("hTopoInvMassRatio1P","HLT/TauMon/Expert/"+lowest_trigger_names.at(i)+"/EFVsOffline/BDT/1p_nonCorrected");
            //      setCurrentMonGroup("HLT/TauMon/Shifter/"+lowest_names.at(i)+"/EFVsOffline/BDT/mp_nonCorrected");
            //      cloneHistogram("hInnerTrkAvgDistRatioMP","HLT/TauMon/Expert/"+lowest_trigger_names.at(i)+"/EFVsOffline/BDT/mp_nonCorrected");
            //      cloneHistogram("hEtOverPtLeadTrkRatioMP","HLT/TauMon/Expert/"+lowest_trigger_names.at(i)+"/EFVsOffline/BDT/mp_nonCorrected");
            //      cloneHistogram("hChPiEMEOverCaloEMERatioMP","HLT/TauMon/Expert/"+lowest_trigger_names.at(i)+"/EFVsOffline/BDT/mp_nonCorrected");
            //      cloneHistogram("hEMPOverTrkSysPRatioMP","HLT/TauMon/Expert/"+lowest_trigger_names.at(i)+"/EFVsOffline/BDT/mp_nonCorrected");
            //      cloneHistogram("hCentFracRatioMP","HLT/TauMon/Expert/"+lowest_trigger_names.at(i)+"/EFVsOffline/BDT/mp_nonCorrected");
            //      cloneHistogram("hPtRatioEflowApproxRatioMP","HLT/TauMon/Expert/"+lowest_trigger_names.at(i)+"/EFVsOffline/BDT/mp_nonCorrected");
            //      cloneHistogram("hDRmaxRatioMP","HLT/TauMon/Expert/"+lowest_trigger_names.at(i)+"/EFVsOffline/BDT/mp_nonCorrected");
            //      cloneHistogram("hTrFlightPathSigRatioMP","HLT/TauMon/Expert/"+lowest_trigger_names.at(i)+"/EFVsOffline/BDT/mp_nonCorrected");
            //      cloneHistogram("hMassTrkSysRatioMP","HLT/TauMon/Expert/"+lowest_trigger_names.at(i)+"/EFVsOffline/BDT/mp_nonCorrected");
            //      cloneHistogram("hMEflowApproxRatioMP","HLT/TauMon/Expert/"+lowest_trigger_names.at(i)+"/EFVsOffline/BDT/mp_nonCorrected");
            
            if(m_turnOnCurves){
                setCurrentMonGroup("HLT/TauMon/Shifter/"+lowest_names.at(i)+"/TurnOnCurves");
                //         cloneHistogram("hRecoL1PtEfficiency","HLT/TauMon/Expert/"+lowest_trigger_names.at(i)+"/TurnOnCurves/RecoEfficiency");
                //         cloneHistogram("hRecoHLTPtEfficiency","HLT/TauMon/Expert/"+lowest_trigger_names.at(i)+"/TurnOnCurves/RecoEfficiency");
                //         cloneHistogram("hRecoL1PhiEfficiency","HLT/TauMon/Expert/"+lowest_trigger_names.at(i)+"/TurnOnCurves/RecoEfficiency");
                //         cloneHistogram("hRecoHLTPhiEfficiency","HLT/TauMon/Expert/"+lowest_trigger_names.at(i)+"/TurnOnCurves/RecoEfficiency");
                //         cloneHistogram("hRecoL1EtaEfficiency","HLT/TauMon/Expert/"+lowest_trigger_names.at(i)+"/TurnOnCurves/RecoEfficiency");
                //         cloneHistogram("hRecoHLTEtaEfficiency","HLT/TauMon/Expert/"+lowest_trigger_names.at(i)+"/TurnOnCurves/RecoEfficiency");
                //         cloneHistogram("hRecoL1NVtxEfficiency","HLT/TauMon/Expert/"+lowest_trigger_names.at(i)+"/TurnOnCurves/RecoEfficiency");
                //         cloneHistogram("hRecoHLTNVtxEfficiency","HLT/TauMon/Expert/"+lowest_trigger_names.at(i)+"/TurnOnCurves/RecoEfficiency");
                
                cloneProfile("TProfRecoL1PtEfficiency","HLT/TauMon/Expert/"+lowest_trigger_names.at(i)+"/TurnOnCurves/RecoEfficiency");
                cloneProfile("TProfRecoL1HighPtEfficiency","HLT/TauMon/Expert/"+lowest_trigger_names.at(i)+"/TurnOnCurves/RecoEfficiency");
                cloneProfile("TProfRecoHLTHighPtEfficiency","HLT/TauMon/Expert/"+lowest_trigger_names.at(i)+"/TurnOnCurves/RecoEfficiency");
                cloneProfile("TProfRecoHLTPtEfficiency","HLT/TauMon/Expert/"+lowest_trigger_names.at(i)+"/TurnOnCurves/RecoEfficiency");
                cloneProfile("TProfRecoL1PhiEfficiency","HLT/TauMon/Expert/"+lowest_trigger_names.at(i)+"/TurnOnCurves/RecoEfficiency");
                cloneProfile("TProfRecoHLTPhiEfficiency","HLT/TauMon/Expert/"+lowest_trigger_names.at(i)+"/TurnOnCurves/RecoEfficiency");
                cloneProfile("TProfRecoL1EtaEfficiency","HLT/TauMon/Expert/"+lowest_trigger_names.at(i)+"/TurnOnCurves/RecoEfficiency");
                cloneProfile("TProfRecoHLTEtaEfficiency","HLT/TauMon/Expert/"+lowest_trigger_names.at(i)+"/TurnOnCurves/RecoEfficiency");
                cloneProfile("TProfRecoL1NVtxEfficiency","HLT/TauMon/Expert/"+lowest_trigger_names.at(i)+"/TurnOnCurves/RecoEfficiency");
                cloneProfile("TProfRecoHLTNVtxEfficiency","HLT/TauMon/Expert/"+lowest_trigger_names.at(i)+"/TurnOnCurves/RecoEfficiency");
                
            }

	    setCurrentMonGroup("HLT/TauMon/Shifter/"+lowest_names.at(i)+"/OtherPlots");
	    cloneHistogram("hL1Counts","HLT/TauMon/Expert");
	    cloneHistogram("hHLTCounts","HLT/TauMon/Expert");
	    if(m_emulation) cloneProfile("hL1Emulation","HLT/TauMon/Expert/Emulation");
            for(unsigned int j=0;j<m_topo_chains_ditau.size(); ++j){
		setCurrentMonGroup("HLT/TauMon/Shifter/"+lowest_names.at(i)+"/OtherPlots/"+m_topo_chains_ditau.at(j));
		cloneProfile("TProfRecoL1_dREfficiency","HLT/TauMon/Expert/TopoDiTau/"+m_topo_chains_ditau.at(j));	

            }
	    
            
        }//End of lowest name loop
       
	

 
    }//End of Run  
    return StatusCode::SUCCESS;
}

///////////////////////////////////////////////////////////////////


