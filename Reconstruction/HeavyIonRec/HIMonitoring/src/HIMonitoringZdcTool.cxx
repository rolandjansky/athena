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
		std::cout << "Number of zdcs " << zdcs->size() << std::endl;
	}

	xAOD::ZdcModuleContainer::const_iterator zdc_itr = zdcs->begin();
	xAOD::ZdcModuleContainer::const_iterator zdc_end = zdcs->end();
	for( ; zdc_itr != zdc_end; ++zdc_itr){
	  hside->Fill((*zdc_itr)->side());
		if((*zdc_itr)->side()==1){
			if((*zdc_itr)->zdcModule()==0) {
				A_hamp_module0->Fill((*zdc_itr)->amplitude());
				A_htime_module0->Fill((*zdc_itr)->time());
				A_hampG0_module0->Fill((*zdc_itr)->amplitudeG0());
				A_htimeG0_module0->Fill((*zdc_itr)->timeG0());
				A_hampG1_module0->Fill((*zdc_itr)->amplitudeG1());
				A_htimeG1_module0->Fill((*zdc_itr)->timeG1());
			}
			if((*zdc_itr)->zdcModule()==1) {
				A_hamp_module1->Fill((*zdc_itr)->amplitude());
				A_htime_module1->Fill((*zdc_itr)->time());
				A_hampG0_module1->Fill((*zdc_itr)->amplitudeG0());
				A_htimeG0_module1->Fill((*zdc_itr)->timeG0());
				A_hampG1_module1->Fill((*zdc_itr)->amplitudeG1());
				A_htimeG1_module1->Fill((*zdc_itr)->timeG1());
			}
			if((*zdc_itr)->zdcModule()==2) {
				A_hamp_module2->Fill((*zdc_itr)->amplitude());
				A_htime_module2->Fill((*zdc_itr)->time());
				A_hampG0_module2->Fill((*zdc_itr)->amplitudeG0());
				A_htimeG0_module2->Fill((*zdc_itr)->timeG0());
				A_hampG1_module2->Fill((*zdc_itr)->amplitudeG1());
				A_htimeG1_module2->Fill((*zdc_itr)->timeG1());
			}
			if((*zdc_itr)->zdcModule()==3) {
				A_hamp_module3->Fill((*zdc_itr)->amplitude());
				A_htime_module3->Fill((*zdc_itr)->time());
				A_hampG0_module3->Fill((*zdc_itr)->amplitudeG0());
				A_htimeG0_module3->Fill((*zdc_itr)->timeG0());
				A_hampG1_module3->Fill((*zdc_itr)->amplitudeG1());
				A_htimeG1_module3->Fill((*zdc_itr)->timeG1());
			}
			A_hamp_sum->Fill((*zdc_itr)->amplitude());
			A_hampG0_sum->Fill((*zdc_itr)->amplitudeG0());
			A_hampG1_sum->Fill((*zdc_itr)->amplitudeG1());
		}
		if((*zdc_itr)->side()==-1){
			if((*zdc_itr)->zdcModule()==0) {
				C_hamp_module0->Fill((*zdc_itr)->amplitude());
				C_htime_module0->Fill((*zdc_itr)->time());
				C_hampG0_module0->Fill((*zdc_itr)->amplitudeG0());
				C_htimeG0_module0->Fill((*zdc_itr)->timeG0());
				C_hampG1_module0->Fill((*zdc_itr)->amplitudeG1());
				C_htimeG1_module0->Fill((*zdc_itr)->timeG1());
			}
			if((*zdc_itr)->zdcModule()==1) {
				C_hamp_module1->Fill((*zdc_itr)->amplitude());
				C_htime_module1->Fill((*zdc_itr)->time());
				C_hampG0_module1->Fill((*zdc_itr)->amplitudeG0());
				C_htimeG0_module1->Fill((*zdc_itr)->timeG0());
				C_hampG1_module1->Fill((*zdc_itr)->amplitudeG1());
				C_htimeG1_module1->Fill((*zdc_itr)->timeG1());
			}
			if((*zdc_itr)->zdcModule()==2) {
				C_hamp_module2->Fill((*zdc_itr)->amplitude());
				C_htime_module2->Fill((*zdc_itr)->time());
				C_hampG0_module2->Fill((*zdc_itr)->amplitudeG0());
				C_htimeG0_module2->Fill((*zdc_itr)->timeG0());
				C_hampG1_module2->Fill((*zdc_itr)->amplitudeG1());
				C_htimeG1_module2->Fill((*zdc_itr)->timeG1());
			}
			if((*zdc_itr)->zdcModule()==3) {
				C_hamp_module3->Fill((*zdc_itr)->amplitude());
				C_htime_module3->Fill((*zdc_itr)->time());
				C_hampG0_module3->Fill((*zdc_itr)->amplitudeG0());
				C_htimeG0_module3->Fill((*zdc_itr)->timeG0());
				C_hampG1_module3->Fill((*zdc_itr)->amplitudeG1());
				C_htimeG1_module3->Fill((*zdc_itr)->timeG1());
			}
			C_hamp_sum->Fill((*zdc_itr)->amplitude());
			C_hampG0_sum->Fill((*zdc_itr)->amplitudeG0());
			C_hampG1_sum->Fill((*zdc_itr)->amplitudeG1());
		}
	}


	return StatusCode::SUCCESS;
}


StatusCode HIMonitoringZdcTool::procHistograms( )
{
   
	if( endOfLowStat || endOfLumiBlock ) 
	{
	   
	}


	if( endOfRun ) 
	{
   
	}

	return StatusCode::SUCCESS;
}


/// *** FCal sum Et *** /// 
void HIMonitoringZdcTool::book_hist()
{
	std::string path = "HeavyIon/ZDC"; 
  A_hamp_module0 = TH1D_LW::create("A_amp_module0","A_amp_module0",1024, 0, 1024);
	regHist(A_hamp_module0, path, run).ignore();
	
	A_hamp_module1 = TH1D_LW::create("A_amp_module1","A_amp_module1",1024, 0, 1024);
	regHist(A_hamp_module1, path, run).ignore();
	
	A_hamp_module2 = TH1D_LW::create("A_amp_module2","A_amp_module2",1024, 0, 1024);
	regHist(A_hamp_module2, path, run).ignore();
	
	A_hamp_module3 = TH1D_LW::create("A_amp_module3","A_amp_module3",1024, 0, 1024);
	regHist(A_hamp_module3, path, run).ignore();
	
	A_hamp_sum = TH1D_LW::create("A_amp_sum","A_amp_sum",1024, 0, 1024);
	regHist(A_hamp_sum, path, run).ignore();
	
	A_hampG0_module0 = TH1D_LW::create("A_ampG0_module0","A_ampG0_module0",1024, 0, 1024);
	regHist(A_hampG0_module0, path, run).ignore();
	
	A_hampG0_module1 = TH1D_LW::create("A_ampG0_module1","A_ampG0_module1",1024, 0, 1024);
	regHist(A_hampG0_module1, path, run).ignore();
	
	A_hampG0_module2 = TH1D_LW::create("A_ampG0_module2","A_ampG0_module2",1024, 0, 1024);
	regHist(A_hampG0_module2, path, run).ignore();
	
	A_hampG0_module3 = TH1D_LW::create("A_ampG0_module3","A_ampG0_module3",1024, 0, 1024);
	regHist(A_hampG0_module3, path, run).ignore();
	
	A_hampG0_sum = TH1D_LW::create("A_ampG0_sum","A_ampG0_sum",1024, 0, 1024);
	regHist(A_hampG0_sum, path, run).ignore();
	
	A_hampG1_module0 = TH1D_LW::create("A_ampG1_module0","A_ampG1_module0",1024, 0, 1024);
	regHist(A_hampG1_module0, path, run).ignore();
	
	A_hampG1_module1 = TH1D_LW::create("A_ampG1_module1","A_ampG1_module1",1024, 0, 1024);
	regHist(A_hampG1_module1, path, run).ignore();
	
	A_hampG1_module2 = TH1D_LW::create("A_ampG1_module2","A_ampG1_module2",1024, 0, 1024);
	regHist(A_hampG1_module2, path, run).ignore();
	
	A_hampG1_module3 = TH1D_LW::create("A_ampG1_module3","A_ampG1_module3",1024, 0, 1024);
	regHist(A_hampG1_module3, path, run).ignore();
	
	A_hampG1_sum = TH1D_LW::create("A_ampG1_sum","A_ampG1_sum",1024, 0, 1024);
	regHist(A_hampG1_sum, path, run).ignore();
	
	C_hamp_module0 = TH1D_LW::create("C_amp_module0","C_amp_module0",1024, 0, 1024);
	regHist(C_hamp_module0, path, run).ignore();
	
	C_hamp_module1 = TH1D_LW::create("C_amp_module1","C_amp_module1",1024, 0, 1024);
	regHist(C_hamp_module1, path, run).ignore();
	
	C_hamp_module2 = TH1D_LW::create("C_amp_module2","C_amp_module2",1024, 0, 1024);
	regHist(C_hamp_module2, path, run).ignore();
	
	C_hamp_module3 = TH1D_LW::create("C_amp_module3","C_amp_module3",1024, 0, 1024);
	regHist(C_hamp_module3, path, run).ignore();
	
	C_hamp_sum = TH1D_LW::create("C_amp_sum","C_amp_sum",1024, 0, 1024);
	regHist(C_hamp_sum, path, run).ignore();
	
	C_hampG0_module0 = TH1D_LW::create("C_ampG0_module0","C_ampG0_module0",1024, 0, 1024);
	regHist(C_hampG0_module0, path, run).ignore();
	
	C_hampG0_module1 = TH1D_LW::create("C_ampG0_module1","C_ampG0_module1",1024, 0, 1024);
	regHist(C_hampG0_module1, path, run).ignore();
	
	C_hampG0_module2 = TH1D_LW::create("C_ampG0_module2","C_ampG0_module2",1024, 0, 1024);
	regHist(C_hampG0_module2, path, run).ignore();
	
	C_hampG0_module3 = TH1D_LW::create("C_ampG0_module3","C_ampG0_module3",1024, 0, 1024);
	regHist(C_hampG0_module3, path, run).ignore();
	
	C_hampG0_sum = TH1D_LW::create("C_ampG0_sum","C_ampG0_sum",1024, 0, 1024);
	regHist(C_hampG0_sum, path, run).ignore();
	
	C_hampG1_module0 = TH1D_LW::create("C_ampG1_module0","C_ampG1_module0",1024, 0, 1024);
	regHist(C_hampG1_module0, path, run).ignore();
	
	C_hampG1_module1 = TH1D_LW::create("C_ampG1_module1","C_ampG1_module1",1024, 0, 1024);
	regHist(C_hampG1_module1, path, run).ignore();
	
	C_hampG1_module2 = TH1D_LW::create("C_ampG1_module2","C_ampG1_module2",1024, 0, 1024);
	regHist(C_hampG1_module2, path, run).ignore();
	
	C_hampG1_module3 = TH1D_LW::create("C_ampG1_module3","C_ampG1_module3",1024, 0, 1024);
	regHist(C_hampG1_module3, path, run).ignore();
	
	C_hampG1_sum = TH1D_LW::create("C_ampG1_sum","C_ampG1_sum",1024, 0, 1024);
	regHist(C_hampG1_sum, path, run).ignore();
	
	A_htime_module0 = TH1D_LW::create("A_time_module0","A_time_module0",128, 0, 128);
	regHist(A_htime_module0, path, run).ignore();
	
	A_htime_module1 = TH1D_LW::create("A_time_module1","A_time_module1",128, 0, 128);
	regHist(A_htime_module1, path, run).ignore();
	
	A_htime_module2 = TH1D_LW::create("A_time_module2","A_time_module2",128, 0, 128);
	regHist(A_htime_module2, path, run).ignore();
	
	A_htime_module3 = TH1D_LW::create("A_time_module3","A_time_module3",128, 0, 128);
	regHist(A_htime_module3, path, run).ignore();
	
	A_htimeG0_module0 = TH1D_LW::create("A_timeG0_module0","A_timeG0_module0", 128, 0,128 );
	regHist(A_htimeG0_module0, path, run).ignore();
	
	A_htimeG0_module1 = TH1D_LW::create("A_timeG0_module1","A_timeG0_module1",128, 0,128 );
	regHist(A_htimeG0_module1, path, run).ignore();
	
	A_htimeG0_module2 = TH1D_LW::create("A_timeG0_module2","A_timeG0_module2",128, 0,128 );
	regHist(A_htimeG0_module2, path, run).ignore();
	
	A_htimeG0_module3 = TH1D_LW::create("A_timeG0_module3","A_timeG0_module3",128, 0,128 );
	regHist(A_htimeG0_module3, path, run).ignore();
	
	A_htimeG1_module0 = TH1D_LW::create("A_timeG1_module0","A_timeG1_module0",128, 0,128 );
	regHist(A_htimeG1_module0, path, run).ignore();
	
	A_htimeG1_module1 = TH1D_LW::create("A_timeG1_module1","A_timeG1_module1",128, 0,128 );
	regHist(A_htimeG1_module1, path, run).ignore();
	
	A_htimeG1_module2 = TH1D_LW::create("A_timeG1_module2","A_timeG1_module2",128, 0,128 );
	regHist(A_htimeG1_module2, path, run).ignore();
	
	A_htimeG1_module3 = TH1D_LW::create("A_timeG1_module3","A_timeG1_module3",128, 0,128 );
	regHist(A_htimeG1_module3, path, run).ignore();
	
	C_htime_module0 = TH1D_LW::create("C_time_module0","C_time_module0",128, 0, 128);
	regHist(C_htime_module0, path, run).ignore();
	
	C_htime_module1 = TH1D_LW::create("C_time_module1","C_time_module1",128, 0, 128);
	regHist(C_htime_module1, path, run).ignore();
	
	C_htime_module2 = TH1D_LW::create("C_time_module2","C_time_module2",128, 0, 128);
	regHist(C_htime_module2, path, run).ignore();
	
	C_htime_module3 = TH1D_LW::create("C_time_module3","C_time_module3",128, 0, 128);
	regHist(C_htime_module3, path, run).ignore();
	
	C_htimeG0_module0 = TH1D_LW::create("C_timeG0_module0","C_timeG0_module0", 128, 0,128 );
	regHist(C_htimeG0_module0, path, run).ignore();
	
	C_htimeG0_module1 = TH1D_LW::create("C_timeG0_module1","C_timeG0_module1",128, 0,128 );
	regHist(C_htimeG0_module1, path, run).ignore();
	
	C_htimeG0_module2 = TH1D_LW::create("C_timeG0_module2","C_timeG0_module2",128, 0,128 );
	regHist(C_htimeG0_module2, path, run).ignore();
	
	C_htimeG0_module3 = TH1D_LW::create("C_timeG0_module3","C_timeG0_module3",128, 0,128 );
	regHist(C_htimeG0_module3, path, run).ignore();
	
	C_htimeG1_module0 = TH1D_LW::create("C_timeG1_module0","C_timeG1_module0",128, 0,128 );
	regHist(C_htimeG1_module0, path, run).ignore();
	
	C_htimeG1_module1 = TH1D_LW::create("C_timeG1_module1","C_timeG1_module1",128, 0,128 );
	regHist(C_htimeG1_module1, path, run).ignore();
	
	C_htimeG1_module2 = TH1D_LW::create("C_timeG1_module2","C_timeG1_module2",128, 0,128 );
	regHist(C_htimeG1_module2, path, run).ignore();
	
	C_htimeG1_module3 = TH1D_LW::create("C_timeG1_module3","C_timeG1_module3",128, 0,128 );
	regHist(C_htimeG1_module3, path, run).ignore();
	
	hside= TH1D_LW::create("hside","hside",10, -2,8);
	regHist(hside, path, run).ignore();

}
