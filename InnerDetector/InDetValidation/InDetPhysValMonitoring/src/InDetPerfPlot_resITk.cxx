/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/**
 * @file InDetPerfPlot_resITk.cxx
 * @author me!
**/

#include "InDetPerfPlot_resITk.h"
#include <iostream>
#include <map>

InDetPerfPlot_resITk::InDetPerfPlot_resITk(PlotBase* pParent, const std::string & sDir)  :PlotBase(pParent, sDir)
{
	m_primTrk = false;
	m_secdTrk = false;
	m_allTrk = false;
  TString tsDir = (TString) sDir;
	if(tsDir.Contains("Primary")) m_primTrk=true; //control if sec/prim from init
	else if(tsDir.Contains("Secondary")) m_secdTrk=true;
	else { m_allTrk=true; }

	//Temporary until method/bins/Etc is established
	for(int ieta = 0; ieta <= m_nEtaBins; ieta++){
		m_EtaBins[ieta] = -4.0+(8.0/m_nEtaBins)*ieta;
	}
	//Parameter definitions 
	paramProp[D0].paraName = std::string("d0"); paramProp[D0].paraLabel = std::string("d_{0}"); paramProp[D0].paraUnit = std::string("[mm]"); 
	paramProp[D0].nBinsRes = 200; paramProp[D0].limRes = { -0.1, 0.1 };
	paramProp[D0].nBinsPrp = 200; paramProp[D0].limPrp = { -0.2, 0.2 };
	paramProp[D0].nBinsSig = 200; paramProp[D0].limSig = { 0.005, 0.04 };
	paramProp[D0].resUnit = std::string("[#mum]");

	paramProp[Z0].paraName = std::string("z0"); paramProp[Z0].paraLabel = std::string("z_{0}"); paramProp[Z0].paraUnit = std::string("[mm]"); 
	paramProp[Z0].nBinsRes = 200; paramProp[Z0].limRes = { -0.2, 0.2 };
	paramProp[Z0].nBinsPrp = 200; paramProp[Z0].limPrp = { -150.0, 150.0 };
	paramProp[Z0].nBinsSig = 200; paramProp[Z0].limSig = { 0.005, 0.04 };
	paramProp[Z0].resUnit = std::string("[#mum]");

	paramProp[Z0SIN].paraName = std::string("z0sin"); paramProp[Z0SIN].paraLabel = std::string("z_{0}#times sin(#theta)"); paramProp[Z0SIN].paraUnit = std::string("[mm]"); 
	paramProp[Z0SIN].nBinsRes = 200; paramProp[Z0SIN].limRes = { -0.2, 0.2 };
	paramProp[Z0SIN].nBinsPrp = 200; paramProp[Z0SIN].limPrp = { -150.0, 150.0 };
	paramProp[Z0SIN].nBinsSig = 200; paramProp[Z0SIN].limSig = { 0.005, 0.04 };
	paramProp[Z0SIN].resUnit = std::string("[#mum]");


	paramProp[QOVERP].paraName = std::string("qoverp"); paramProp[QOVERP].paraLabel = std::string("(q/p)"); paramProp[QOVERP].paraUnit = std::string("[MeV^{-1}]"); 
	paramProp[QOVERP].nBinsRes = 200; paramProp[QOVERP].limRes = { -0.2e-5, 0.2e-5 };
	paramProp[QOVERP].nBinsPrp = 200; paramProp[QOVERP].limPrp = { -0.2e-5, 0.2e-5 };
	paramProp[QOVERP].nBinsSig = 100; paramProp[QOVERP].limSig = { 0.0, 1.0e-6 };
	paramProp[QOVERP].resUnit = std::string("[MeV^{-1}]");

	paramProp[QOVERPT].paraName = std::string("qoverpt"); paramProp[QOVERPT].paraLabel = std::string("(q/p_{T})"); paramProp[QOVERPT].paraUnit = std::string("[MeV^{-1}]"); 
	paramProp[QOVERPT].nBinsRes = 200; paramProp[QOVERPT].limRes = { -0.2e-5, 0.2e-5 };
	paramProp[QOVERPT].nBinsPrp = 200; paramProp[QOVERPT].limPrp = { -0.2e-5, 0.2e-5 };
	paramProp[QOVERPT].nBinsSig = 100; paramProp[QOVERPT].limSig = { 0.0, 1.0e-6 };
	paramProp[QOVERPT].resUnit = std::string("[MeV^{-1}]");

	paramProp[THETA].paraName = std::string("theta"); paramProp[THETA].paraLabel = std::string("#theta"); paramProp[THETA].paraUnit = std::string("[rad]"); 
	paramProp[THETA].nBinsRes = 200; paramProp[THETA].limRes = { -0.0005, 0.0005 };
	paramProp[THETA].nBinsPrp = 200; paramProp[THETA].limPrp = { 0.0, 3.14 };
	paramProp[THETA].nBinsSig = 150; paramProp[THETA].limSig = { 0.0, 0.0005 };
	paramProp[THETA].resUnit = std::string("[mrad]");

	paramProp[PHI].paraName = std::string("phi"); paramProp[PHI].paraLabel = std::string("#phi"); paramProp[PHI].paraUnit = std::string("[rad]"); 
	paramProp[PHI].nBinsRes = 200; paramProp[PHI].limRes = { -0.0005, 0.0005 };
	paramProp[PHI].nBinsPrp = 100; paramProp[PHI].limPrp = { -3.14, 3.14 };
	paramProp[PHI].nBinsSig = 200; paramProp[PHI].limSig = { 0.0, 0.0005 };
	paramProp[PHI].resUnit = std::string("[mrad]");

	paramProp[PT].paraName = std::string("pt"); paramProp[PT].paraLabel = std::string("p_{T}"); paramProp[PT].paraUnit = std::string("[GeV]"); 
	paramProp[PT].nBinsRes = 200; paramProp[PT].limRes = { -2.0, 2.0 };
	paramProp[PT].nBinsPrp = 200; paramProp[PT].limPrp = { 0.0, 50.0 };
	paramProp[PT].nBinsSig = 200; paramProp[PT].limSig = { 0.0, 0.1 };
	paramProp[PT].resUnit = std::string("[GeV]");
}


void InDetPerfPlot_resITk::initializePlots() {
	
	//Bins for resolutions
	int nBinsEta = 50.0; float nMinEta = -5.0; float nMaxEta = fabs(nMinEta);
	int nBinsPt = 500.0; float nMinPt = 0.0; float nMaxPt = 500.0;


	for(unsigned int iparam = 0; iparam < NPARAMS; iparam++){
		///pull
		std::string tmpName = "pull_" + paramProp[iparam].paraName;
		std::string tmpTitle = tmpName + "; (" + paramProp[iparam].paraLabel + "^{reco}-" + paramProp[iparam].paraLabel + "^{true})/#sigma_{" + paramProp[iparam].paraLabel + "}";
		m_resITk_pull[iparam] = Book1D(tmpName,tmpTitle,200,-5.0,5.0,false);
		//res
		tmpName = "res_" + paramProp[iparam].paraName;
		tmpTitle = tmpName + "; " + paramProp[iparam].paraLabel + "^{reco}-" + paramProp[iparam].paraLabel + "^{true} " + paramProp[iparam].paraUnit;
		m_resITk_res[iparam] = Book1D(tmpName,tmpTitle,paramProp[iparam].nBinsRes,paramProp[iparam].limRes.at(0),paramProp[iparam].limRes.at(1),false);
		//reco
		tmpName = "reco_" + paramProp[iparam].paraName;
		tmpTitle = tmpName + "; " + paramProp[iparam].paraLabel + "^{reco} " + paramProp[iparam].paraUnit;
		m_resITk_reco[iparam] = Book1D(tmpName,tmpTitle,paramProp[iparam].nBinsPrp,paramProp[iparam].limPrp.at(0),paramProp[iparam].limPrp.at(1),false);
		//true
		tmpName = "true_" + paramProp[iparam].paraName;
		tmpTitle = tmpName + "; " + paramProp[iparam].paraLabel + "^{true} " + paramProp[iparam].paraUnit;
		m_resITk_true[iparam] = Book1D(tmpName,tmpTitle,paramProp[iparam].nBinsPrp,paramProp[iparam].limPrp.at(0),paramProp[iparam].limPrp.at(1),false);
		//sigma
		tmpName = "sigma_" + paramProp[iparam].paraName;
		tmpTitle = tmpName + "; #sigma_{" + paramProp[iparam].paraLabel + "} " + paramProp[iparam].paraUnit;
		m_resITk_sigma[iparam] = Book1D(tmpName,tmpTitle,paramProp[iparam].nBinsSig,paramProp[iparam].limSig.at(0),paramProp[iparam].limSig.at(1),false);
		//res versus eta
		tmpName = "resHelpereta_" + paramProp[iparam].paraName;
		tmpTitle = tmpName + "; true track #eta; " + paramProp[iparam].paraLabel + "^{reco}-" + paramProp[iparam].paraLabel + "^{true} " + paramProp[iparam].paraUnit;
		m_resITk_resHelpereta[iparam] = Book2D(tmpName,tmpTitle,nBinsEta,nMinEta,nMaxEta,paramProp[iparam].nBinsRes,paramProp[iparam].limRes.at(0),paramProp[iparam].limRes.at(1),false);
		for(unsigned int ibins = 0; ibins < m_nEtaBins; ibins++){
			tmpName = "EtaProjections/resProjection_" + paramProp[iparam].paraName + std::to_string(ibins+1);
			tmpTitle = tmpName + "; " + paramProp[iparam].paraLabel + "^{reco}-" + paramProp[iparam].paraLabel + "^{true} " + paramProp[iparam].paraUnit;
			m_resITk_ResProjections_vs_eta[iparam][ibins] = Book1D(tmpName,tmpTitle,paramProp[iparam].nBinsRes,paramProp[iparam].limRes.at(0),paramProp[iparam].limRes.at(1),false);
		}
		std::string ytitle[4] = { " resolution ", " mean ", " resolution ", " mean " };
		std::string ytitlePull[4] = { " pull width ", " pull mean ", " pull width ", " pull mean " };

		for(unsigned int ires = 0; ires < m_nResHist; ires++){
			tmpName = paramProp[iparam].paraName + m_resHisto[ires] + "_vs_eta";
			tmpTitle = tmpName + "; true track #eta [rad]; " + paramProp[iparam].paraLabel + ytitle[ires] + paramProp[iparam].resUnit;
			m_resITk_Resolution_vs_eta[iparam][ires] = Book1D(tmpName,tmpTitle,m_nEtaBins,-4.0,4.0,false);
		}
		tmpName = "resHelpereta_pos" + paramProp[iparam].paraName;
		tmpTitle = tmpName + "; true track #eta; " + paramProp[iparam].paraLabel + "^{reco}-" + paramProp[iparam].paraLabel + "^{true} " + paramProp[iparam].paraUnit;
		m_resITk_resHelpereta_pos[iparam] = Book2D(tmpName,tmpTitle,nBinsEta,nMinEta,nMaxEta,paramProp[iparam].nBinsRes,paramProp[iparam].limRes.at(0),paramProp[iparam].limRes.at(1),false);
		for(unsigned int ires = 0; ires < m_nResHist; ires++){
			tmpName = paramProp[iparam].paraName + m_resHisto[ires] + "_vs_eta_pos";
			tmpTitle = tmpName + "; true track #eta [rad]; " + paramProp[iparam].paraLabel + ytitle[ires] + paramProp[iparam].resUnit;
			m_resITk_Resolution_vs_eta_pos[iparam][ires] = Book1D(tmpName,tmpTitle,m_nEtaBins,-4.0,4.0,false);
		}
		tmpName = "resHelpereta_neg" + paramProp[iparam].paraName;
		tmpTitle = tmpName + "; true track #eta; " + paramProp[iparam].paraLabel + "^{reco}-" + paramProp[iparam].paraLabel + "^{true} " + paramProp[iparam].paraUnit;
		m_resITk_resHelpereta_neg[iparam] = Book2D(tmpName,tmpTitle,nBinsEta,nMinEta,nMaxEta,paramProp[iparam].nBinsRes,paramProp[iparam].limRes.at(0),paramProp[iparam].limRes.at(1),false);
		for(unsigned int ires = 0; ires < m_nResHist; ires++){
			tmpName = paramProp[iparam].paraName + m_resHisto[ires] + "_vs_eta_neg";
			tmpTitle = tmpName + "; true track #eta [rad]; " + paramProp[iparam].paraLabel + ytitle[ires] + paramProp[iparam].resUnit;
			m_resITk_Resolution_vs_eta_neg[iparam][ires] = Book1D(tmpName,tmpTitle,m_nEtaBins,-4.0,4.0,false);
		}

		//res versus pt
		tmpName = "resHelperpt_" + paramProp[iparam].paraName;
		tmpTitle = tmpName + "; true track p_{T} [GeV]; " + paramProp[iparam].paraLabel + "^{reco}-" + paramProp[iparam].paraLabel + "^{true} " + paramProp[iparam].paraUnit;
		m_resITk_resHelperpt[iparam] = Book2D(tmpName,tmpTitle,nBinsPt,nMinPt,nMaxPt,paramProp[iparam].nBinsRes,paramProp[iparam].limRes.at(0),paramProp[iparam].limRes.at(1),false);
		tmpName = "pullHelperpt_" + paramProp[iparam].paraName;
		tmpTitle = tmpName + "; true track p_{T} [GeV]; (" + paramProp[iparam].paraLabel + "^{reco}-" + paramProp[iparam].paraLabel + "^{true})/#sigma_{" + paramProp[iparam].paraLabel + "}";
		m_resITk_pullHelperpt[iparam] = Book2D(tmpName,tmpTitle,nBinsPt,nMinPt,nMaxPt,200,-10.0,10.0,false);
		
		for(unsigned int ires = 0; ires < m_nResHist; ires++){
			tmpName = paramProp[iparam].paraName + "pull" + m_resHisto[ires] + "_vs_pt";
			tmpTitle = tmpName + "; true track p_{T} [GeV]; " + paramProp[iparam].paraLabel + ytitlePull[ires];
			m_resITk_pullResolution_vs_pt[iparam][ires] = Book1D(tmpName,tmpTitle,m_nPtBins,0.,50.0,false);
			m_resITk_pullResolution_vs_pt[iparam][ires]->GetXaxis()->Set(m_nPtBins,m_PtBins);
		}

		for(unsigned int ibins = 0; ibins < m_nPtBins; ibins++){
			tmpName = "PtPullProjections/pullProjection_" + paramProp[iparam].paraName + std::to_string(ibins+1);
			tmpTitle = tmpName + "; (" + paramProp[iparam].paraLabel + "^{reco}-" + paramProp[iparam].paraLabel + "^{true})/#sigma_{" + paramProp[iparam].paraLabel + "}";
			m_resITk_pullProjections_vs_pt[iparam][ibins] = Book1D(tmpName,tmpTitle,200,-10.0,10.0,false);
		}

		for(unsigned int ibins = 0; ibins < m_nPtBins; ibins++){
			tmpName = "PtProjections/resProjection_" + paramProp[iparam].paraName + std::to_string(ibins+1);
			tmpTitle = tmpName + "; " + paramProp[iparam].paraLabel + "^{reco}-" + paramProp[iparam].paraLabel + "^{true} " + paramProp[iparam].paraUnit;
			m_resITk_ResProjections_vs_pt[iparam][ibins] = Book1D(tmpName,tmpTitle,paramProp[iparam].nBinsRes,paramProp[iparam].limRes.at(0),paramProp[iparam].limRes.at(1),false);
		}
		for(unsigned int ires = 0; ires < m_nResHist; ires++){
			tmpName = paramProp[iparam].paraName + m_resHisto[ires] + "_vs_pt";
			tmpTitle = tmpName + "; true track p_{T} [GeV]; " + paramProp[iparam].paraLabel + ytitle[ires] + paramProp[iparam].resUnit;
			m_resITk_Resolution_vs_pt[iparam][ires] = Book1D(tmpName,tmpTitle,m_nPtBins,0.,50.0,false);
			m_resITk_Resolution_vs_pt[iparam][ires]->GetXaxis()->Set(m_nPtBins,m_PtBins);
		}
		tmpName = "resHelperpt_pos" + paramProp[iparam].paraName;
		tmpTitle = tmpName + "; true track p_{T} [GeV]; " + paramProp[iparam].paraLabel + "^{reco}-" + paramProp[iparam].paraLabel + "^{true} " + paramProp[iparam].paraUnit;
		m_resITk_resHelperpt_pos[iparam] = Book2D(tmpName,tmpTitle,nBinsPt,nMinPt,nMaxPt,paramProp[iparam].nBinsRes,paramProp[iparam].limRes.at(0),paramProp[iparam].limRes.at(1),false);
		for(unsigned int ires = 0; ires < m_nResHist; ires++){
			tmpName = paramProp[iparam].paraName + m_resHisto[ires] + "_vs_pt_pos";
			tmpTitle = tmpName + "; true track p_{T} [GeV]; " + paramProp[iparam].paraLabel + ytitle[ires] + paramProp[iparam].resUnit;
			m_resITk_Resolution_vs_pt_pos[iparam][ires] = Book1D(tmpName,tmpTitle,m_nPtBins,0.,50.0,false);
			m_resITk_Resolution_vs_pt_pos[iparam][ires]->GetXaxis()->Set(m_nPtBins,m_PtBins);
		}
		tmpName = "resHelperpt_neg" + paramProp[iparam].paraName;
		tmpTitle = tmpName + "; true track p_{T} [GeV]; " + paramProp[iparam].paraLabel + "^{reco}-" + paramProp[iparam].paraLabel + "^{true} " + paramProp[iparam].paraUnit;
		m_resITk_resHelperpt_neg[iparam] = Book2D(tmpName,tmpTitle,nBinsPt,nMinPt,nMaxPt,paramProp[iparam].nBinsRes,paramProp[iparam].limRes.at(0),paramProp[iparam].limRes.at(1),false);
		for(unsigned int ires = 0; ires < m_nResHist; ires++){
			tmpName = paramProp[iparam].paraName + m_resHisto[ires] + "_vs_pt_neg";
			tmpTitle = tmpName + "; true track p_{T} [GeV]; " + paramProp[iparam].paraLabel + ytitle[ires] + paramProp[iparam].resUnit;
			m_resITk_Resolution_vs_pt_neg[iparam][ires] = Book1D(tmpName,tmpTitle,m_nPtBins,0.,50.0,false);
			m_resITk_Resolution_vs_pt_neg[iparam][ires]->GetXaxis()->Set(m_nPtBins,m_PtBins);
		}
		//res versus eta pt
		tmpName = "resHelperetapt_" + paramProp[iparam].paraName;
		tmpTitle = tmpName + "; true track p_{T}; true track #eta; " + paramProp[iparam].paraLabel + "^{reco}-" + paramProp[iparam].paraLabel + "^{true} " + paramProp[iparam].paraUnit;
		m_resITk_resHelperetapt[iparam] = Book3D(tmpName,tmpTitle,nBinsPt,nMinPt,nMaxPt,nBinsEta,nMinEta,nMaxEta,paramProp[iparam].nBinsRes,paramProp[iparam].limRes.at(0),paramProp[iparam].limRes.at(1),false);	
	
		for(unsigned int ires = 0; ires < 2; ires++){	
			for(int ibin = 0; ibin < 4; ibin++){
				int tmpInt = ibin + 1;
				tmpName = paramProp[iparam].paraName + m_resHisto[ires] + "_vs_pt_EtaBin" + std::to_string(tmpInt);
				tmpTitle = tmpName + "; true track p_{T} [GeV]; " + paramProp[iparam].paraLabel + ytitle[ires] + paramProp[iparam].resUnit;
				m_resITk_Resolution_vs_pt_EtaBin[iparam][ibin][ires] = Book1D(tmpName,tmpTitle,m_nPtBins,0.,50.0,false);
				m_resITk_Resolution_vs_pt_EtaBin[iparam][ibin][ires]->GetXaxis()->Set(m_nPtBins,m_PtBins);
			}
			for(int iPtBin = 0; iPtBin < 4; iPtBin++){
				tmpName = paramProp[iparam].paraName + m_resHisto[ires] + "_vs_eta_PtBin" + std::to_string(iPtBin+1);
				tmpTitle = tmpName + "; true track #eta; " + paramProp[iparam].paraLabel + ytitle[ires] + paramProp[iparam].resUnit;
				m_resITk_Resolution_vs_eta_PtBin[iparam][iPtBin][ires] = Book1D(tmpName,tmpTitle,m_nEtaBins,-4.0,4.0,false);
			}
		}

		tmpName = "meanProfeta_" + paramProp[iparam].paraName;
		tmpTitle = tmpName + "; true track #eta; #sigma_{" + paramProp[iparam].paraLabel + "} " + paramProp[iparam].paraUnit;
		m_resITk_meanProfeta[iparam] = BookTProfile(tmpName,tmpTitle,nBinsEta,nMinEta,nMaxEta,-1.0,1.0,false);
	
		tmpName = "SigmaVsEta_" + paramProp[iparam].paraName;
		tmpTitle = tmpName + "; true track #eta; #sigma_{" + paramProp[iparam].paraLabel + "} " + paramProp[iparam].paraUnit;
		m_resITk_sigmaVsEta[iparam] = Book2D(tmpName,tmpTitle,nBinsEta,nMinEta,nMaxEta,paramProp[iparam].nBinsSig,-1.0,1.0,false);
	
		tmpName = "meanProfpt_" + paramProp[iparam].paraName;
		tmpTitle = tmpName + "; true track p_{T}; #sigma_{" + paramProp[iparam].paraLabel + "} " + paramProp[iparam].paraUnit;
		m_resITk_meanProfpt[iparam] = BookTProfile(tmpName,tmpTitle,nBinsPt,nMinPt,nMaxPt,-1.0,1.0,false);
		m_resITk_meanProfpt[iparam]->GetXaxis()->Set(m_nPtBins,m_PtBins);

		tmpName = "DEBUG_D0dep_" + paramProp[iparam].paraName;
		tmpTitle = tmpName + "; " + paramProp[iparam].paraLabel + "^{reco} " + paramProp[iparam].paraUnit + "; d_{0} [mm]"; 
		m_DEBUG_D0dep[iparam] = BookTProfile(tmpName,tmpTitle,paramProp[iparam].nBinsPrp,paramProp[iparam].limPrp.at(0),paramProp[iparam].limPrp.at(1),-0.1,0.1,false);

		

	}

	m_resITk_chargeID = Book1D("chargeID","chargeID",2,0.,2.,false);
	m_resITk_chargeID->GetXaxis()->SetBinLabel(1,"ID");
	m_resITk_chargeID->GetXaxis()->SetBinLabel(2,"misID");
	m_resITk_chargeID_chgvschg = Book2D("chargeID_chgvschg","chargeID_chgvschg; charge_{reco}; charge_{true}",5,-2.0,2.0,5,-2.0,2.0,false);

	m_resITk_chargeID_vs_eta = Book1D("chargeID_vs_eta","chargeID_vs_eta; truth track #eta; Fraction of misID [%]",nBinsEta,nMinEta,nMaxEta,false);
	m_resITk_chargeID_vs_pt = Book1D("chargeID_vs_pt","chargeID_vs_pt; truth track p_{T} [GeV]; Fraction of misID [%]",nBinsPt,nMaxPt,nMaxPt,false);
	m_resITk_chargeID_vs_pt->GetXaxis()->Set(m_nPtBins,m_PtBins);

	m_resITk_momTail = Book1D("momTail","momTail",2,0.,2.,false);
	m_resITk_momTail->GetXaxis()->SetBinLabel(1,"Core");
	m_resITk_momTail->GetXaxis()->SetBinLabel(2,"Tail");

	m_resITk_momTail_vs_eta = Book1D("momTail_vs_eta","momTail_vs_eta; truth track #eta; Fraction of p_{T}-tail [%]",nBinsEta,nMinEta,nMaxEta,false);
	m_resITk_momTail_vs_pt = Book1D("momTail_vs_pt","momTail_vs_pt; truth track p_{T} [GeV]; Fraction of p_{T}-tail [%]",nBinsPt,nMinPt,nMaxPt,false);
	m_resITk_momTail_vs_pt->GetXaxis()->Set(m_nPtBins,m_PtBins);
	m_resITk_momTail_vs_phi = Book1D("momTail_vs_phi","momTail_vs_phi; truth track #phi; Fraction of p_{T}-tail [%]",nBinsEta,nMinEta,nMaxEta,false);

	m_resITk_momTail_Frac = Book1D("momTail_Frac","momTail_Frac; (p^{reco}_{T}-p^{true}_{T})/p^{true}_{T}",100,0.,100.,false);

	m_DEBUG_BREM_d0 = Book1D("DEBUG_BREM_d0","DEBUG_BREM_d0; d_{0} [mm]",paramProp[D0].nBinsPrp,paramProp[D0].limPrp.at(0),paramProp[D0].limPrp.at(1),false);
	m_DEBUG_NOBREM_d0 = Book1D("DEBUG_NOBREM_d0","DEBUG_NOBREM_d0; d_{0} [mm]",paramProp[D0].nBinsPrp,paramProp[D0].limPrp.at(0),paramProp[D0].limPrp.at(1),false);

	m_DEBUG_FirstHitR_d0 = BookTProfile("DEBUG_FirstHitR_d0","DEBUG_FirstHitR_d0; R [mm]",150,0.,10.,-1.0,1.0,false);
}

void InDetPerfPlot_resITk::fill(const xAOD::TrackParticle& trkprt, const xAOD::TruthParticle& truthprt) {
  
	//Check whether the track is primary or secondary
	int trueBC=-9999;
  trueBC = truthprt.barcode();
	int isPrimTrk = 0; int isSecdTrk = 0;
	if((trueBC > 0) && (trueBC < 200000)) isPrimTrk = 1;
	if((trueBC > 200000)) isSecdTrk = 1;
  
	//Move on to the next track incase the wrong track category
	if(!isPrimTrk && !isSecdTrk) return;
	if(!isPrimTrk && m_primTrk) return;
	if(!isSecdTrk && m_secdTrk) return;

	//Get track paramters for truth and reco tracks for easy access
	//store all in track parameter maps trkP/truetrkP
 	getTrackParameters(trkprt);
	getTrackParameters(truthprt);
	getPlotParameters();
	getPlots();
	
}

void InDetPerfPlot_resITk::getPlots(){

	float eta = -TMath::Log(TMath::Tan(truetrkP[THETA]/2));

	for(unsigned int iparam = 0; iparam < NPARAMS; iparam++){
	
		m_resITk_pull[iparam]->Fill(pullP[iparam]);
		m_resITk_res[iparam]->Fill(resP[iparam]);
		m_resITk_reco[iparam]->Fill(trkP[iparam]);
		m_resITk_true[iparam]->Fill(truetrkP[iparam]);
		m_resITk_sigma[iparam]->Fill(sigP[iparam]);

		m_resITk_meanProfeta[iparam]->Fill(eta,sigP[iparam]);
		m_resITk_sigmaVsEta[iparam]->Fill(eta,sigP[iparam]);
		
		m_resITk_meanProfpt[iparam]->Fill(truetrkP[PT],sigP[iparam]);
		m_resITk_resHelpereta[iparam]->Fill(eta,resP[iparam]);
		m_resITk_resHelperpt[iparam]->Fill(truetrkP[PT],resP[iparam]);
		m_resITk_resHelperetapt[iparam]->Fill(truetrkP[PT],eta,resP[iparam]);
	
		m_resITk_pullHelperpt[iparam]->Fill(truetrkP[PT],pullP[iparam]);
		
		if(trkP[QOVERP] >= 0.){
			m_resITk_resHelpereta_pos[iparam]->Fill(eta,resP[iparam]);
			m_resITk_resHelperpt_pos[iparam]->Fill(truetrkP[PT],resP[iparam]);
		}	
		if(trkP[QOVERP] < 0.){
			m_resITk_resHelpereta_neg[iparam]->Fill(eta,resP[iparam]);
			m_resITk_resHelperpt_neg[iparam]->Fill(truetrkP[PT],resP[iparam]);
		}	

		if(iparam == QOVERP){
			if(trkP[QOVERP]/truetrkP[QOVERP] > 0.) m_resITk_chargeID->Fill(0.);
			if(trkP[QOVERP]/truetrkP[QOVERP] < 0.) {
				m_resITk_chargeID->Fill(1.);
				m_resITk_chargeID_vs_eta->Fill(eta);
				m_resITk_chargeID_vs_pt->Fill(truetrkP[PT]);

				m_resITk_chargeID_chgvschg->Fill(trkP[QOVERP]/fabs(trkP[QOVERP]),truetrkP[QOVERP]/fabs(truetrkP[QOVERP]));
			}
	 	}
		//Look at PT tails
		if(iparam == PT){
			m_resITk_momTail_Frac->Fill((trkP[PT]-truetrkP[PT])/truetrkP[PT]);
			if((trkP[PT]-truetrkP[PT])/truetrkP[PT] > 0.5 && truetrkP[PT] > 0.0){
				m_resITk_momTail->Fill(1.);
				m_resITk_momTail_vs_phi->Fill(truetrkP[PHI]);
				m_resITk_momTail_vs_eta->Fill(eta);
				m_resITk_momTail_vs_pt->Fill(truetrkP[PT]);
			}
			else {	m_resITk_momTail->Fill(0.); }

		}

		m_DEBUG_D0dep[iparam]->Fill(trkP[iparam],trkP[D0]);
	}

}

void InDetPerfPlot_resITk::getPlotParameters(){
	
	for(unsigned int iparam = 0; iparam < NPARAMS; iparam++){
		resP[iparam] = trkP[iparam] - truetrkP[iparam]; 
		sigP[iparam] = trkErrP[iparam];
		(sigP[iparam] != 0) ? pullP[iparam] = resP[iparam]/sigP[iparam] : pullP[iparam] = -9999.;
	}
		
}

void InDetPerfPlot_resITk::getTrackParameters(const xAOD::TrackParticle& trkprt){
	//std::cout << "---------------------------" << std::endl;
	//std::cout << trkprt.vx() << "	" << trkprt.vy() << "	" << trkprt.vz() << std::endl;
	//std::cout << trkprt.beamlineTiltX() << "	" << trkprt.beamlineTiltY() << "	" << trkprt.beamlineTiltX()<< std::endl;
	//std::cout << "---------------------------" << std::endl;

	//std::cout << trkprt.patternRecoInfo() << std::endl;
	//std::cout << trkprt.trackFitter() << std::endl;
	//std::cout << trkprt.track()->info().dumpInfo() << std::endl;
 	//Track perigee, BS?
 	//std::cout << trkprt.track()->info().trackProperties(Trk::TrackInfo::BremFit) << std::endl; 	
	//std::cout << trkprt.trackProperties(xAOD::BremFit) << std::endl;

	trkP[D0] 				= trkprt.d0();
  trkP[Z0] 				= trkprt.z0();
  trkP[QOVERP] 		= trkprt.qOverP();
	trkP[QOVERPT]		= trkprt.qOverP()*(1/TMath::Sin(trkprt.theta()));
	trkP[THETA] 		= trkprt.theta();
  trkP[PHI] 			= trkprt.phi0();
	trkP[PT]				= trkprt.pt()/1000.;
	trkP[Z0SIN]			= trkprt.z0()*TMath::Sin(trkprt.theta());


	m_DEBUG_FirstHitR_d0->Fill(trkprt.radiusOfFirstHit(),trkP[D0]);
	if(trkprt.track()){
		if(trkprt.track()->info().trackProperties(Trk::TrackInfo::BremFit) && trkprt.track()->info().trackProperties(Trk::TrackInfo::BremFitSuccessful)){
			m_DEBUG_BREM_d0->Fill(trkP[D0]);
		}
		else{ m_DEBUG_NOBREM_d0->Fill(trkP[D0]); }
	}
  //Track fit errors 	
	trkErrP[D0] 		= TMath::Sqrt(trkprt.definingParametersCovMatrix()(0,0));
  trkErrP[Z0] 		= TMath::Sqrt(trkprt.definingParametersCovMatrix()(1,1));
	trkErrP[PHI] 		= TMath::Sqrt(trkprt.definingParametersCovMatrix()(2,2));
	trkErrP[THETA] 	= TMath::Sqrt(trkprt.definingParametersCovMatrix()(3,3));
	trkErrP[QOVERP] = TMath::Sqrt(trkprt.definingParametersCovMatrix()(4,4));
	trkErrP[QOVERPT] = trkErrP[QOVERP]*(1/TMath::Sin(trkprt.theta()));	
	trkErrP[Z0SIN] 	=	TMath::Sqrt(pow(trkErrP[THETA]*TMath::Sin(trkP[THETA]),2)+pow(trkP[Z0]*trkErrP[THETA]*TMath::Cos(trkP[THETA]),2));
	
	//Get error on pT, taken from xAOD::TrackingHelpers.pTErr() but this function only works on a pointer input...
  if ( trkprt.definingParametersCovMatrixVec().size()<15 ) {
		throw std::runtime_error("TrackParticle without covariance matrix for defining parameters or the covariance matrix is wrong dimensionality.");
	}
	if (std::abs(trkprt.qOverP())<0) {
		trkErrP[PT] = 0.0;
		throw std::runtime_error("q/p is zero");
	}
	else{
		double pt = trkprt.pt();
		double diff_qp = - pt / std::abs(trkprt.qOverP());
		double diff_theta = pt / tan(trkprt.theta());
		const std::vector<float>&cov = trkprt.definingParametersCovMatrixVec();
		double pt_err2 = diff_qp * (diff_qp * cov[14] + diff_theta * cov[13] ) + diff_theta*diff_theta * cov[9];
		trkErrP[PT] = sqrt(pt_err2)/1000.;
	}	

}

void InDetPerfPlot_resITk::getTrackParameters(const xAOD::TruthParticle& truthprt){
		
	//"d0", "z0", "phi", "theta" , "qOverP"
	//Perigee for truth particles are in aux container
  int nParams = 6;
	for(int iParams = 0; iParams < nParams; iParams++) {
	    truetrkP[iParams] = -9999.;
			if(truthprt.isAvailable<float>(paramProp[iParams].paraName)) truetrkP[iParams] = (truthprt.auxdata<float>(paramProp[iParams].paraName));
	}
	
	(truthprt.isAvailable<float>("d0")) ? truetrkP[D0] = truthprt.auxdata<float>("d0") : truetrkP[D0] = -9999.;
	(truthprt.isAvailable<float>("z0")) ? truetrkP[Z0] = truthprt.auxdata<float>("z0") : truetrkP[Z0] = -9999.;
	(truthprt.isAvailable<float>("qOverP")) ? truetrkP[QOVERP] = truthprt.auxdata<float>("qOverP") : truetrkP[QOVERP] = -9999.;
	(truthprt.isAvailable<float>("theta")) ? truetrkP[THETA] = truthprt.auxdata<float>("theta") : truetrkP[THETA] = -9999.;
	(truthprt.isAvailable<float>("phi")) ? truetrkP[PHI] = truthprt.auxdata<float>("phi") : truetrkP[PHI] = -9999.;
	(truthprt.isAvailable<float>("theta") && truthprt.isAvailable<float>("qOverP")) ? truetrkP[QOVERPT] = truthprt.auxdata<float>("qOverP")*(1/TMath::Sin(truthprt.auxdata<float>("theta"))) : truetrkP[QOVERPT] = -9999.;

	
	truetrkP[PT] 		= truthprt.pt()/1000.;
	(truthprt.isAvailable<float>("z0") && truthprt.isAvailable<float>("theta")) ? truetrkP[Z0SIN] = truetrkP[Z0]*TMath::Sin(truetrkP[THETA]) : truetrkP[Z0SIN] = -9999.;
}


void InDetPerfPlot_resITk::finalizePlots(){

	for(unsigned int iparam = 0; iparam < NPARAMS; iparam++){
		makeResolutions(m_resITk_resHelpereta[iparam],m_resITk_Resolution_vs_eta[iparam],m_resITk_ResProjections_vs_eta[iparam],true);
		makeResolutions(m_resITk_resHelperpt[iparam],m_resITk_Resolution_vs_pt[iparam],m_resITk_ResProjections_vs_pt[iparam],true);	
		makeResolutions(m_resITk_resHelperpt_pos[iparam],m_resITk_Resolution_vs_pt_pos[iparam]);
		makeResolutions(m_resITk_resHelperpt_neg[iparam],m_resITk_Resolution_vs_pt_neg[iparam]);
		makeResolutions(m_resITk_resHelpereta_pos[iparam],m_resITk_Resolution_vs_eta_pos[iparam]);
		makeResolutions(m_resITk_resHelpereta_neg[iparam],m_resITk_Resolution_vs_eta_neg[iparam]);
		makeResolutions(m_resITk_resHelperetapt[iparam],m_resITk_Resolution_vs_pt_EtaBin[iparam],m_resITk_Resolution_vs_eta_PtBin[iparam]);

		//add for the pull vs pT
		makeResolutions(m_resITk_pullHelperpt[iparam],m_resITk_pullResolution_vs_pt[iparam],m_resITk_pullProjections_vs_pt[iparam],true);	

	}

	//Get total fraction of miss ID and Momentum tails
	m_resITk_chargeID_vs_pt->Scale(1/m_resITk_chargeID->GetEntries());
	m_resITk_chargeID_vs_eta->Scale(1/m_resITk_chargeID->GetEntries());

	m_resITk_momTail_vs_pt->Scale(1/m_resITk_momTail->GetEntries());
	m_resITk_momTail_vs_phi->Scale(1/m_resITk_momTail->GetEntries());
	m_resITk_momTail_vs_eta->Scale(1/m_resITk_momTail->GetEntries());


}

void InDetPerfPlot_resITk::makeResolutions(TH2* h, TH1* hres[4]){
	//Should fix this in a better way
	TString hname = h->GetName();
	
	if(hname.Contains("Helpereta")){
		for(unsigned int ieta = 0; ieta < m_nEtaBins; ieta++){
			std::string tmpName = h->GetName() + std::string("py_bin") + std::to_string(ieta+1);
			TH1D *tmp = (TH1D*)h->ProjectionY(tmpName.c_str(),h->GetXaxis()->FindBin(m_EtaBins[ieta]),h->GetXaxis()->FindBin(m_EtaBins[ieta+1]));
			if(tmp->Integral() < 1) continue;
			std::vector<float> result = getResolution(tmp,std::string("RMS"));	
			hres[0]->SetBinContent(ieta+1,result.at(0));
			hres[0]->SetBinError(ieta+1,result.at(1));
			hres[1]->SetBinContent(ieta+1,result.at(2));
			hres[1]->SetBinError(ieta+1,result.at(3));
			result.clear();
	/*	  result = getResolution(tmp,std::string("GAUS"));	
			hres[2]->SetBinContent(ieta+1,result.at(0));
			hres[2]->SetBinError(ieta+1,result.at(1));
			hres[3]->SetBinContent(ieta+1,result.at(2));
			hres[3]->SetBinError(ieta+1,result.at(3));*/
			delete tmp;
		}
	}
	else if(hname.Contains("Helperpt")){
		for(unsigned int ipt = 0; ipt < m_nPtBins; ipt++){
			std::string tmpName = h->GetName() + std::string("py_bin") + std::to_string(ipt+1);
			TH1D *tmp = (TH1D*)h->ProjectionY(tmpName.c_str(),h->GetXaxis()->FindBin(m_PtBins[ipt]),h->GetXaxis()->FindBin(m_PtBins[ipt+1]));
		  if(tmp->Integral() < 1) continue;
			std::vector<float> result = getResolution(tmp,std::string("RMS"));	
			hres[0]->SetBinContent(ipt+1,result.at(0));
			hres[0]->SetBinError(ipt+1,result.at(1));
			hres[1]->SetBinContent(ipt+1,result.at(2));
			hres[1]->SetBinError(ipt+1,result.at(3));
			result.clear();
/*		  result = getResolution(tmp,std::string("GAUS"));	
			hres[2]->SetBinContent(ipt+1,result.at(0));
			hres[2]->SetBinError(ipt+1,result.at(1));
			hres[3]->SetBinContent(ipt+1,result.at(2));
			hres[3]->SetBinError(ipt+1,result.at(3));*/
			delete tmp;
		}
	}
}


void InDetPerfPlot_resITk::makeResolutions(TH2* h, TH1* hres[4], TH1* hproj[m_nEtaBins],bool save){

	TString hname = h->GetName();
	
	if(hname.Contains("Helpereta")){
		for(unsigned int ieta = 0; ieta < m_nEtaBins; ieta++){
			std::string tmpName = h->GetName() + std::string("py_bin") + std::to_string(ieta+1);
			TH1D *tmp = (TH1D*)h->ProjectionY(tmpName.c_str(),h->GetXaxis()->FindBin(m_EtaBins[ieta]),h->GetXaxis()->FindBin(m_EtaBins[ieta+1]));
			if(tmp->Integral() < 1) continue;
			if(save) cloneHistogram(tmp, hproj[ieta]);
			std::vector<float> result = getResolution(tmp,std::string("RMS"));	
			hres[0]->SetBinContent(ieta+1,result.at(0));
			hres[0]->SetBinError(ieta+1,result.at(1));
			hres[1]->SetBinContent(ieta+1,result.at(2));
			hres[1]->SetBinError(ieta+1,result.at(3));
			result.clear();
	/*	  result = getResolution(hproj[ieta],std::string("GAUS"));	
			hres[2]->SetBinContent(ieta+1,result.at(0));
			hres[2]->SetBinError(ieta+1,result.at(1));
			hres[3]->SetBinContent(ieta+1,result.at(2));
			hres[3]->SetBinError(ieta+1,result.at(3));*/
			delete tmp;
		}
	}
	else if(hname.Contains("Helperpt")){
		for(unsigned int ipt = 0; ipt < m_nPtBins; ipt++){
			std::string tmpName = h->GetName() + std::string("py_bin") + std::to_string(ipt+1);
			TH1D *tmp = (TH1D*)h->ProjectionY(tmpName.c_str(),h->GetXaxis()->FindBin(m_PtBins[ipt]),h->GetXaxis()->FindBin(m_PtBins[ipt+1]));
		  if(tmp->Integral() < 1) continue;
			if(save) cloneHistogram(tmp, hproj[ipt]);
			std::vector<float> result = getResolution(tmp,std::string("RMS"));	
			hres[0]->SetBinContent(ipt+1,result.at(0));
			hres[0]->SetBinError(ipt+1,result.at(1));
			hres[1]->SetBinContent(ipt+1,result.at(2));
			hres[1]->SetBinError(ipt+1,result.at(3));
			result.clear();
		/*  result = getResolution(hproj[ipt],std::string("GAUS"));	
			hres[2]->SetBinContent(ipt+1,result.at(0));
			hres[2]->SetBinError(ipt+1,result.at(1));
			hres[3]->SetBinContent(ipt+1,result.at(2));
			hres[3]->SetBinError(ipt+1,result.at(3));*/
			delete tmp;
		}
	}
}

void InDetPerfPlot_resITk::cloneHistogram(TH1D* h, TH1* hcopy){

	unsigned int nbin = h->GetNbinsX();
	for(unsigned int ibin = 0; ibin < nbin; ibin++){
		float binContent = h->GetBinContent(ibin+1);
		float binError = h->GetBinError(ibin+1);
		hcopy->SetBinContent(ibin+1,binContent);
		hcopy->SetBinError(ibin+1,binError);
	}

}

void InDetPerfPlot_resITk::makeResolutions(TH3* h, TH1* hres_eta[4][4], TH1* hres_pt[4][4]){

	float BinEta[5] = { 0.0, 1.0, 1.5, 2.7, 5.0 };
	float BinPt[5] = { 0.0, 1.0, 2.0, 5.0, 500.0 };

	for(unsigned int ieta = 0; ieta < 4; ieta++){
		for(unsigned int ipt = 0; ipt < m_nPtBins; ipt++){
			std::string tmpName = h->GetName() + std::string("pz_binPt") + std::to_string(ipt+1) + std::string("pz_binEta") + std::to_string(ieta+1);
			std::string tmpName1 = tmpName + std::string("_neg");
			std::string tmpName2 = tmpName + std::string("_pos");
			TH1D *tmp1 = (TH1D*)h->ProjectionZ(tmpName1.c_str(),h->GetXaxis()->FindBin(m_PtBins[ipt]),h->GetXaxis()->FindBin(m_PtBins[ipt+1]),h->GetYaxis()->FindBin(-BinEta[ieta+1]),h->GetYaxis()->FindBin(-BinEta[ieta]));
			TH1D *tmp2 = (TH1D*)h->ProjectionZ(tmpName2.c_str(),h->GetXaxis()->FindBin(m_PtBins[ipt]),h->GetXaxis()->FindBin(m_PtBins[ipt+1]),h->GetYaxis()->FindBin(BinEta[ieta]),h->GetYaxis()->FindBin(BinEta[ieta+1]));
			tmp1->Add(tmp2);
			if(tmp1->Integral() < 1) continue;
			std::vector<float> result = getResolution(tmp1,std::string("RMS"));	
			hres_eta[ieta][0]->SetBinContent(ipt+1,result.at(0));
			hres_eta[ieta][0]->SetBinError(ipt+1,result.at(1));
			hres_eta[ieta][1]->SetBinContent(ipt+1,result.at(2));
			hres_eta[ieta][1]->SetBinError(ipt+1,result.at(3));
			delete tmp1; delete tmp2;
		}
	}

	for(unsigned int ipt = 0; ipt < 5; ipt++){
		for(unsigned int ieta = 0; ieta < m_nEtaBins; ieta++){
			std::string tmpName = h->GetName() + std::string("pz_binPt") + std::to_string(ipt+1) + std::string("pz_binEta") + std::to_string(ieta+1);
			TH1D *tmp = (TH1D*)h->ProjectionZ(tmpName.c_str(),h->GetXaxis()->FindBin(BinPt[ipt]),h->GetXaxis()->FindBin(BinPt[ipt+1]),h->GetYaxis()->FindBin(m_EtaBins[ieta]),h->GetYaxis()->FindBin(m_EtaBins[ieta+1]));
		  if(tmp->Integral() < 1) continue;
			std::vector<float> result = getResolution(tmp,std::string("RMS"));	
			hres_pt[ipt][0]->SetBinContent(ieta+1,result.at(0));
			hres_pt[ipt][0]->SetBinError(ieta+1,result.at(1));
			hres_pt[ipt][1]->SetBinContent(ieta+1,result.at(2));
			hres_pt[ipt][1]->SetBinError(ieta+1,result.at(3));
			delete tmp;
		}
	}
}

std::vector<float> InDetPerfPlot_resITk::getResolution(TH1* h, std::string s){

	std::vector<float> result;	
	if(h->GetEntries() == 0.0 || h->Integral() < 100.0){
		result.push_back(0.0);	
		result.push_back(0.0);
		result.push_back(0.0);	
		result.push_back(0.0);
		result.push_back(0.0);
		result.push_back(0.0);
		return result;
	}
	float mean = h->GetMean(); float meanErr = h->GetMeanError();	
	float rms = h->GetRMS(); float rmsErr = h->GetRMSError();
	
//	std::cout << "RMSRMS" << std::endl;
//	std::cout << mean << " " << rms << std::endl;
	float nTot = h->Integral(0.,h->GetNbinsX()+1);
	float nSig = 0.0;
 	float itr_rms = 0.0;
	//Stolen from Max
  itr_rms = rms + 1.0;
  if( s == "RMS"){	
		while(fabs(itr_rms - rms) > 0.001){
			rms = h->GetRMS();
			double min = -3.0*rms;
			double max = 3.0*rms;
			if(min < h->GetBinLowEdge(1)) min = h->GetBinLowEdge(1);
			if(max > h->GetBinCenter(h->GetNbinsX())) max = h->GetBinCenter(h->GetNbinsX());
			h->SetAxisRange(min, max);
			mean = h->GetMean();
			itr_rms = h->GetRMS();
			nSig = h->Integral(h->GetXaxis()->FindBin(min), h->GetXaxis()->FindBin(max));
	}
	rms = itr_rms;
	meanErr = h->GetMeanError();
  rmsErr = h->GetRMSError();
  }	else if(s == "GAUS"){
		int fitStatus = h->Fit("gaus","QS0");
		TFitResultPtr r = h->Fit("gaus", "QS0");
		if(fitStatus){
				mean = r->Parameter(1);
	      meanErr = r->ParError(1);
  	    rms = r->Parameter(2);
    	  rmsErr = r->ParError(2);
				nSig = h->Integral(h->GetXaxis()->FindBin(-3.0*rms),h->GetXaxis()->FindBin(3.0*rms));
		}
 	}

	TString vari = h->GetName();
	if(!vari.Contains("pull") && (vari.Contains("d0") || vari.Contains("z0"))) {
		rms = rms*1000.0;
		rmsErr = rmsErr*1000.0;
		mean = mean*1000.0;
		meanErr = meanErr*1000.0;
	}	
	float nOut = nTot-nSig;
	
	result.push_back(rms);
	result.push_back(rmsErr);
	result.push_back(mean);
	result.push_back(meanErr);

	if(nTot > 0.0){
		if(nOut/nTot > 0.9) nOut = 0.;
		result.push_back(nOut/nTot);
		if(nOut > 0.0)result.push_back((nOut/nTot)*TMath::Sqrt(1/nOut+1/nTot));
		else { result.push_back(0.0); }
	}
	else { result.push_back(0.); result.push_back(0.); }


	return result;

}


