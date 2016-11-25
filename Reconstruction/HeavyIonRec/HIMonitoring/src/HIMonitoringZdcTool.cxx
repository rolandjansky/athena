/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include <sstream>

#include "GaudiKernel/IJobOptionsSvc.h"
#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/StatusCode.h"

#include "AthenaMonitoring/AthenaMonManager.h"
#include "HIMonitoring/HIMonitoringZdcTool.h"
#include <xAODForward/ZdcModule.h>
#include <xAODForward/ZdcModuleAuxContainer.h>
#include <xAODForward/ZdcModuleContainer.h>
#include <xAODForward/xAODForwardDict.h>
#include <ZdcAnalysis/ZdcAnalysisTool.h>
#include "TCanvas.h"
#include "TROOT.h"
#include "TStyle.h"
#include "TLatex.h"
#include "LWHists/TH1D_LW.h"
#include "LWHists/TH2D_LW.h"
#include "LWHists/TProfile_LW.h"

HIMonitoringZdcTool::
HIMonitoringZdcTool( const std::string & type, const std::string & name,
const IInterface* parent ): ManagedMonitorToolBase( type, name, parent )   	     	  
{
}

HIMonitoringZdcTool::~HIMonitoringZdcTool()
{
}

// Description: Used for rebooking unmanaged histograms       
StatusCode HIMonitoringZdcTool::bookHistogramsRecurrent( )
{
	return StatusCode::SUCCESS;
}

// Description: Used for re-booking managed histograms       
StatusCode HIMonitoringZdcTool::bookHistograms( )
{
	if( m_environment == AthenaMonManager::online ) {
		// book histograms that are only made in the online environment...
	}

	if( m_dataType == AthenaMonManager::cosmics ) {
		// book histograms that are only relevant for cosmics data...
	}
   
	gStyle->SetOptStat(0);
	book_hist(); 

	return StatusCode::SUCCESS;
}


StatusCode HIMonitoringZdcTool::fillHistograms()
{
	StatusCode sc; 
	
	const xAOD::ZdcModuleContainer* zdcs = 0;
	sc = evtStore()->retrieve(zdcs,"ZdcModules");
	if (sc.isFailure()) {
		ATH_MSG_ERROR("Could not find Zdc");
		return StatusCode::FAILURE;
	}
	else {
		ATH_MSG_INFO("Zdcs retrieved from StoreGate");
//		std::cout << "Number of zdcs " << zdcs->size() << std::endl;
	}

	xAOD::ZdcModuleContainer::const_iterator zdc_itr = zdcs->begin();
	xAOD::ZdcModuleContainer::const_iterator zdc_end = zdcs->end();

	for( ; zdc_itr != zdc_end; ++zdc_itr){

		if((*zdc_itr)->type()!=0) continue;
		int thisSide = ((*zdc_itr)->side() == -1 ? 0 : 1);
		int imod = (*zdc_itr)->zdcModule();

		if(thisSide==1&&imod==0)NEW_AEM =   ((*zdc_itr)->auxdataConst<float>("Amplitude"));
		if(thisSide==1&&imod==1)NEW_AHAD1 = ((*zdc_itr)->auxdataConst<float>("Amplitude"));
		if(thisSide==1&&imod==2)NEW_AHAD2 = ((*zdc_itr)->auxdataConst<float>("Amplitude"));
		if(thisSide==1&&imod==3)NEW_AHAD3 = ((*zdc_itr)->auxdataConst<float>("Amplitude"));
		if(thisSide==0&&imod==0)NEW_CEM =   ((*zdc_itr)->auxdataConst<float>("Amplitude"));
		if(thisSide==0&&imod==1)NEW_CHAD1 = ((*zdc_itr)->auxdataConst<float>("Amplitude"));
		if(thisSide==0&&imod==2)NEW_CHAD2 = ((*zdc_itr)->auxdataConst<float>("Amplitude"));
		if(thisSide==0&&imod==3)NEW_CHAD3 = ((*zdc_itr)->auxdataConst<float>("Amplitude"));
/*
		if(thisSide==1&&imod==0)AEM =   ((*zdc_itr)->amplitude());
		if(thisSide==1&&imod==1)AHAD1 = ((*zdc_itr)->amplitude());
		if(thisSide==1&&imod==2)AHAD2 = ((*zdc_itr)->amplitude());
		if(thisSide==1&&imod==3)AHAD3 = ((*zdc_itr)->amplitude());
		if(thisSide==0&&imod==0)CEM =   ((*zdc_itr)->amplitude());
		if(thisSide==0&&imod==1)CHAD1 = ((*zdc_itr)->amplitude());
		if(thisSide==0&&imod==2)CHAD2 = ((*zdc_itr)->amplitude());
		if(thisSide==0&&imod==3)CHAD3 = ((*zdc_itr)->amplitude());
*/
		if(thisSide==1&&imod==0)G0AEM =   ((*zdc_itr)->amplitudeG0());
		if(thisSide==1&&imod==1)G0AHAD1 = ((*zdc_itr)->amplitudeG0());
		if(thisSide==1&&imod==2)G0AHAD2 = ((*zdc_itr)->amplitudeG0());
		if(thisSide==1&&imod==3)G0AHAD3 = ((*zdc_itr)->amplitudeG0());
		if(thisSide==0&&imod==0)G0CEM =   ((*zdc_itr)->amplitudeG0());
		if(thisSide==0&&imod==1)G0CHAD1 = ((*zdc_itr)->amplitudeG0());
		if(thisSide==0&&imod==2)G0CHAD2 = ((*zdc_itr)->amplitudeG0());
		if(thisSide==0&&imod==3)G0CHAD3 = ((*zdc_itr)->amplitudeG0());

		if(thisSide==1&&imod==0)G1AEM =   ((*zdc_itr)->amplitudeG1());
		if(thisSide==1&&imod==1)G1AHAD1 = ((*zdc_itr)->amplitudeG1());
		if(thisSide==1&&imod==2)G1AHAD2 = ((*zdc_itr)->amplitudeG1());
		if(thisSide==1&&imod==3)G1AHAD3 = ((*zdc_itr)->amplitudeG1());
		if(thisSide==0&&imod==0)G1CEM =   ((*zdc_itr)->amplitudeG1());
		if(thisSide==0&&imod==1)G1CHAD1 = ((*zdc_itr)->amplitudeG1());
		if(thisSide==0&&imod==2)G1CHAD2 = ((*zdc_itr)->amplitudeG1());
		if(thisSide==0&&imod==3)G1CHAD3 = ((*zdc_itr)->amplitudeG1());

	//	hamp_NEW[imod][thisSide]->Fill((*zdc_itr)->auxdataConst<float>("Amplitude"));
	//	hamp[imod][thisSide]->Fill((*zdc_itr)->amplitude());
		hamp[imod][thisSide]->Fill((*zdc_itr)->auxdataConst<float>("Amplitude"));
		hampG0[imod][thisSide]->Fill((*zdc_itr)->amplitudeG0());
		hampG1[imod][thisSide]->Fill((*zdc_itr)->amplitudeG1());

		//	hSumSideAmp_NEW[1]->Fill(NEW_AEM+NEW_AHAD1+NEW_AHAD2+NEW_AHAD3);
		//	hSumSideAmp_NEW[0]->Fill(NEW_CEM+NEW_CHAD1+NEW_CHAD2+NEW_CHAD3);
		//	hSumSideAmp[1]->Fill(AEM+AHAD1+AHAD2+AHAD3);
		//	hSumSideAmp[0]->Fill(CEM+CHAD1+CHAD2+CHAD3);

		
		//	if(thisSide==1) NEW_SumSideA = NEW_AEM+NEW_AHAD1+NEW_AHAD2+NEW_AHAD3;
		//	if(thisSide==0) NEW_SumSideC = NEW_CEM+NEW_CHAD1+NEW_CHAD2+NEW_CHAD3;
		//if(thisSide==1) SumSideA = AEM+AHAD1+AHAD2+AHAD3;
		//if(thisSide==0) SumSideC = CEM+CHAD1+CHAD2+CHAD3;
	}

	//if(thisSide==1) SumSideA = NEW_AEM+NEW_AHAD1+NEW_AHAD2+NEW_AHAD3;
	//if(thisSide==0) SumSideC = NEW_CEM+NEW_CHAD1+NEW_CHAD2+NEW_CHAD3;
	NEW_SumSideA = NEW_AEM+NEW_AHAD1+NEW_AHAD2+NEW_AHAD3;
	NEW_SumSideC = NEW_CEM+NEW_CHAD1+NEW_CHAD2+NEW_CHAD3;

	//hSumSideAmp[1]->Fill(NEW_AEM+NEW_AHAD1+NEW_AHAD2+NEW_AHAD3);
	//hSumSideAmp[0]->Fill(NEW_CEM+NEW_CHAD1+NEW_CHAD2+NEW_CHAD3);
	hSumSideAmp[1]->Fill(NEW_SumSideA);
	hSumSideAmp[0]->Fill(NEW_SumSideC);
	hSideAC->Fill(NEW_SumSideA, NEW_SumSideC);

	hEM_HAD1[0]->Fill(NEW_CEM, NEW_CHAD1);
	hHAD1_HAD2[0]->Fill(NEW_CHAD1, NEW_CHAD2);
	hHAD2_HAD3[0]->Fill(NEW_CHAD2, NEW_CHAD3);
	hEM_HAD1[1]->Fill(NEW_AEM, NEW_AHAD1);
	hHAD1_HAD2[1]->Fill(NEW_AHAD1, NEW_AHAD2);
	hHAD2_HAD3[1]->Fill(NEW_AHAD2, NEW_AHAD3);

	hSumSideAmpG0[1]->Fill(G0AEM+G0AHAD1+G0AHAD2+G0AHAD3);
	hSumSideAmpG0[0]->Fill(G0CEM+G0CHAD1+G0CHAD2+G0CHAD3);
	hSumSideAmpG1[1]->Fill(G1AEM+G1AHAD1+G1AHAD2+G1AHAD3);
	hSumSideAmpG1[0]->Fill(G1CEM+G1CHAD1+G1CHAD2+G1CHAD3);
		
	/*
	hEM_HAD1[0]->Fill(CEM, CHAD1);
	hHAD1_HAD2[0]->Fill(CHAD1, CHAD2);
	hHAD2_HAD3[0]->Fill(CHAD2, CHAD3);
	hEM_HAD1[1]->Fill(AEM, AHAD1);
	hHAD1_HAD2[1]->Fill(AHAD1, AHAD2);
	hHAD2_HAD3[1]->Fill(AHAD2, AHAD3);
	hSideAC->Fill(SumSideA, SumSideC);

	hEM_HAD1_NEW[0]->Fill(NEW_CEM, NEW_CHAD1);
	hHAD1_HAD2_NEW[0]->Fill(NEW_CHAD1, NEW_CHAD2);
	hHAD2_HAD3_NEW[0]->Fill(NEW_CHAD2, NEW_CHAD3);
	hEM_HAD1_NEW[1]->Fill(NEW_AEM, NEW_AHAD1);
	hHAD1_HAD2_NEW[1]->Fill(NEW_AHAD1, NEW_AHAD2);
	hHAD2_HAD3_NEW[1]->Fill(NEW_AHAD2, NEW_AHAD3);
	hSideAC_NEW->Fill(NEW_SumSideA, NEW_SumSideC);
	*/
	return StatusCode::SUCCESS;
}


StatusCode HIMonitoringZdcTool::procHistograms( )
{
   
	if( endOfLowStat || endOfLumiBlock ) 
	{
	   
	}


	if( endOfRun ) 
	{
        for(int k = 0; k < Nside; k++)
        {
            if(hSumSideAmp[k]->GetEntries() > 0) hSumSideAmp[k]->Scale(1./hSumSideAmp[k]->GetEntries());
     //       if(hSumSideAmp_NEW[k]->GetEntries() > 0) hSumSideAmp_NEW[k]->Scale(1./hSumSideAmp_NEW[k]->GetEntries());
            if(hSumSideAmpG0[k]->GetEntries() > 0) hSumSideAmpG0[k]->Scale(1./hSumSideAmpG0[k]->GetEntries());
            if(hSumSideAmpG1[k]->GetEntries() > 0) hSumSideAmpG1[k]->Scale(1./hSumSideAmpG1[k]->GetEntries());
            
            for(int i = 0; i < Nmod; i++)
            {
                if(hamp[i][k]->GetEntries() > 0) hamp[i][k]->Scale(1./hamp[i][k]->GetEntries());
   //             if(hamp_NEW[i][k]->GetEntries() > 0) hamp_NEW[i][k]->Scale(1./hamp_NEW[i][k]->GetEntries());
                if(hampG0[i][k]->GetEntries() > 0) hampG0[i][k]->Scale(1./hampG0[i][k]->GetEntries());
                if(hampG1[i][k]->GetEntries() > 0) hampG1[i][k]->Scale(1./hampG1[i][k]->GetEntries());
            }
        }
	}

	return StatusCode::SUCCESS;
}


/// *** FCal sum Et *** /// 
void HIMonitoringZdcTool::book_hist()
{
	std::string path = "HeavyIon/ZDC"; 

	std::stringstream nameEM_HAD1;
	std::stringstream nameHAD1_HAD2;
	std::stringstream nameHAD2_HAD3;
	std::stringstream histnameamp;
	std::stringstream histnameampG0;
	std::stringstream histnameampG1;
	std::stringstream histnameg0d0;
	std::stringstream histnameg1d0;
	std::stringstream histnameg0d1;
	std::stringstream histnameg1d1;
	std::stringstream nameSumSideAmp;
	std::stringstream nameSumSideAmpG1;
	std::stringstream nameSumSideAmpG0;
	std::stringstream nameSideAC;

	nameSideAC.str("");
	nameSideAC<<"hSideA_sideC";
	hSideAC = TH2D_LW::create(nameSideAC.str().c_str(), nameSideAC.str().c_str(),4097, -5, 40965, 4097, -5, 40965);
	regHist(hSideAC, path, run).ignore();
/*	
	nameSideAC.str("");
	nameSideAC<<"h_NEW_SideA_sideC";
	hSideAC_NEW = TH2D_LW::create(nameSideAC.str().c_str(), nameSideAC.str().c_str(),10241,-0.5,10240.5,10241,-0.5,10240.5);
	regHist(hSideAC_NEW, path, run).ignore();
	*/
	for(int k = 0; k<Nside; k++){
		
		nameEM_HAD1.str("");
		nameEM_HAD1<<"hEM_HAD1_side"<<k;
		hEM_HAD1[k] = TH2D_LW::create(nameEM_HAD1.str().c_str(),nameEM_HAD1.str().c_str(), 1025, -5, 10245, 1025, -5, 10245);
		regHist(hEM_HAD1[k], path, run).ignore();
/*
		nameEM_HAD1.str("");
		nameEM_HAD1<<"h_NEW_EM_HAD1_side"<<k;
		hEM_HAD1_NEW[k] = TH2D_LW::create(nameEM_HAD1.str().c_str(),nameEM_HAD1.str().c_str(),10241,-0.5,10240.5,10241,-0.5,10240.5);
		regHist(hEM_HAD1_NEW[k], path, run).ignore();
*/
		nameHAD1_HAD2.str("");
		nameHAD1_HAD2<<"hHAD1_HAD2_side"<<k;
		hHAD1_HAD2[k] = TH2D_LW::create(nameHAD1_HAD2.str().c_str(),nameHAD1_HAD2.str().c_str(), 1025, -5, 10245, 1025, -5, 10245);
		regHist(hHAD1_HAD2[k], path, run).ignore();
/*
		nameHAD1_HAD2.str("");
		nameHAD1_HAD2<<"h_NEW_HAD1_HAD2_side"<<k;
		hHAD1_HAD2_NEW[k] = TH2D_LW::create(nameHAD1_HAD2.str().c_str(),nameHAD1_HAD2.str().c_str(),10241,-0.5,10240.5,10241,-0.5,10240.5);
		regHist(hHAD1_HAD2_NEW[k], path, run).ignore();
	*/		
		nameHAD2_HAD3.str("");
		nameHAD2_HAD3<<"hHAD2_HAD3_side"<<k;
		hHAD2_HAD3[k] = TH2D_LW::create(nameHAD2_HAD3.str().c_str(),nameHAD2_HAD3.str().c_str(), 1025, -5, 10245, 1025, -5, 10245);
		regHist(hHAD2_HAD3[k], path, run).ignore();
/*
		nameHAD2_HAD3.str("");
		nameHAD2_HAD3<<"h_NEW_HAD2_HAD3_side"<<k;
		hHAD2_HAD3_NEW[k] = TH2D_LW::create(nameHAD2_HAD3.str().c_str(),nameHAD2_HAD3.str().c_str(),10241,-0.5,10240.5,10241,-0.5,10240.5);
		regHist(hHAD2_HAD3_NEW[k], path, run).ignore();
	*/		
		nameSumSideAmp.str("");
		nameSumSideAmp<<"hSumSideAmp_side"<<k;
		hSumSideAmp[k] = new TH1D(nameSumSideAmp.str().c_str(), nameSumSideAmp.str().c_str(), 4097,-5,40965);
		regHist(hSumSideAmp[k], path, run).ignore();
/*
		nameSumSideAmp.str("");
		nameSumSideAmp<<"h_NEW_SumSideAmp_side"<<k;
		hSumSideAmp_NEW[k] = new TH1D(nameSumSideAmp.str().c_str(), nameSumSideAmp.str().c_str(),40961,-0.5,40960.5);
		regHist(hSumSideAmp_NEW[k], path, run).ignore();
	*/	
		nameSumSideAmpG0.str("");
		nameSumSideAmpG0<<"hSumSideAmpG0_side"<<k;
		hSumSideAmpG0[k] = new TH1D(nameSumSideAmpG0.str().c_str(), nameSumSideAmpG0.str().c_str(),4097,-0.5,4096.5);
		regHist(hSumSideAmpG0[k], path, run).ignore();
		
		nameSumSideAmpG1.str("");
		nameSumSideAmpG1<<"hSumSideAmpG1_side"<<k;
		hSumSideAmpG1[k] = new TH1D(nameSumSideAmpG1.str().c_str(), nameSumSideAmpG1.str().c_str(),4097,-0.5,4096.5);
		regHist(hSumSideAmpG1[k], path, run).ignore();
		
		for(int i = 0; i<Nmod; i++){
			histnameamp.str("");
			histnameamp<<"h_amplitude_mod"<<i<<"_side"<<k;
			hamp[i][k] = new TH1D(histnameamp.str().c_str(), histnameamp.str().c_str(), 1025, -5, 10245);
			regHist(hamp[i][k], path, run).ignore();
	/*		
			histnameamp.str("");
			histnameamp<<"h_NEW_amplitude_mod"<<i<<"_side"<<k;
			hamp_NEW[i][k] = new TH1D(histnameamp.str().c_str(), histnameamp.str().c_str(), 10241,-0.5,10240.5);
			regHist(hamp_NEW[i][k], path, run).ignore();
		*/	
			histnameampG0.str("");
			histnameampG0<<"h_amplitudeG0_mod"<<i<<"_side"<<k;
			hampG0[i][k] = new TH1D(histnameampG0.str().c_str(), histnameampG0.str().c_str(), 1025, -0.5, 1024.5);
			regHist(hampG0[i][k], path, run).ignore();
			
			histnameampG1.str("");
			histnameampG1<<"h_amplitudeG1_mod"<<i<<"_side"<<k;
			hampG1[i][k] = new TH1D(histnameampG1.str().c_str(), histnameampG1.str().c_str(), 1025, -0.5, 1024.5);
			regHist(hampG1[i][k], path, run).ignore();
		}
	}

}
