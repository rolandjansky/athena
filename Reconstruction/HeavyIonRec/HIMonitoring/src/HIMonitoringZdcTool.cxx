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

// #include <xAODTrigEgamma/TrigElectronContainer.h>

#include "LWHists/TH1D_LW.h"
#include "LWHists/TH2D_LW.h"
#include "LWHists/TProfile_LW.h"

HIMonitoringZdcTool::
HIMonitoringZdcTool( const std::string & type, const std::string & name,
const IInterface* parent ): ManagedMonitorToolBase( type, name, parent )   	     	  
{
	m_FCalEt=0;  
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
   
	book_hist(); 

 
	return StatusCode::SUCCESS;
}


StatusCode HIMonitoringZdcTool::fillHistograms()
{
	StatusCode sc; 
	
	/// HI event shape
	const xAOD::HIEventShapeContainer* evtShape;
	sc = evtStore()->retrieve(evtShape, "HIEventShape");
	if (sc.isFailure()) 
	{
		ATH_MSG_ERROR("Could not find HIEventShape");
		return StatusCode::FAILURE;
	} 
	else 
	{
		ATH_MSG_INFO("HIEventShape retrieved from StoreGate");
	}
	
	int size=evtShape->size();
	for(int i=0;i<size;i++)
	{       
		const xAOD::HIEventShape *sh=evtShape->at(i);
		if(sh->layer()==21 || sh->layer()==22 || sh->layer()==23)
		{
			m_FCalEt += sh->et();
		} 
	}
	m_FCalEt = m_FCalEt/1000000.;
	
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


	for(const auto* izdc: *zdcs){
		if(izdc->zdcModule()==0) {
			hamp_module0->Fill(izdc->amplitude());
			henergy_module0->Fill(izdc->energy());
			htime_module0->Fill(izdc->time());
			hqual_module0->Fill(izdc->qual());

			hampG0_module0->Fill(izdc->amplitudeG0());
			henergyG0_module0->Fill(izdc->energyG0());
			htimeG0_module0->Fill(izdc->timeG0());
			hqualG0_module0->Fill(izdc->qualG0());

			hampG1_module0->Fill(izdc->amplitudeG1());
			henergyG1_module0->Fill(izdc->energyG1());
			htimeG1_module0->Fill(izdc->timeG1());
			hqualG1_module0->Fill(izdc->qualG1());
		}
		if(izdc->zdcModule()==1) {
			hamp_module1->Fill(izdc->amplitude());
			henergy_module1->Fill(izdc->energy());
			htime_module1->Fill(izdc->time());
			hqual_module1->Fill(izdc->qual());

			hampG0_module1->Fill(izdc->amplitudeG0());
			henergyG0_module1->Fill(izdc->energyG0());
			htimeG0_module1->Fill(izdc->timeG0());
			hqualG0_module1->Fill(izdc->qualG0());

			hampG1_module1->Fill(izdc->amplitudeG1());
			henergyG1_module1->Fill(izdc->energyG1());
			htimeG1_module1->Fill(izdc->timeG1());
			hqualG1_module1->Fill(izdc->qualG1());
		}
		if(izdc->zdcModule()==2) {
			hamp_module2->Fill(izdc->amplitude());
			henergy_module2->Fill(izdc->energy());
			htime_module2->Fill(izdc->time());
			hqual_module2->Fill(izdc->qual());

			hampG0_module2->Fill(izdc->amplitudeG0());
			henergyG0_module2->Fill(izdc->energyG0());
			htimeG0_module2->Fill(izdc->timeG0());
			hqualG0_module2->Fill(izdc->qualG0());

			hampG1_module2->Fill(izdc->amplitudeG1());
			henergyG1_module2->Fill(izdc->energyG1());
			htimeG1_module2->Fill(izdc->timeG1());
			hqualG1_module2->Fill(izdc->qualG1());
		}
		if(izdc->zdcModule()==3) {
			hamp_module3->Fill(izdc->amplitude());
			henergy_module3->Fill(izdc->energy());
			htime_module3->Fill(izdc->time());
			hqual_module3->Fill(izdc->qual());

			hampG0_module3->Fill(izdc->amplitudeG0());
			henergyG0_module3->Fill(izdc->energyG0());
			htimeG0_module3->Fill(izdc->timeG0());
			hqualG0_module3->Fill(izdc->qualG0());

			hampG1_module3->Fill(izdc->amplitudeG1());
			henergyG1_module3->Fill(izdc->energyG1());
			htimeG1_module3->Fill(izdc->timeG1());
			hqualG1_module3->Fill(izdc->qualG1());
		}
		hamp_sum->Fill(izdc->amplitude());
		hampG0_sum->Fill(izdc->amplitudeG0());
		hampG1_sum->Fill(izdc->amplitudeG1());

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
	
	hamp_module0 = TH1D_LW::create("hamp_module0","hamp_module0",256, 0, 1024);
	regHist(hamp_module0, path, run).ignore();
	
	hamp_module1 = TH1D_LW::create("hamp_module1","hamp_module1",256, 0, 1024);
	regHist(hamp_module1, path, run).ignore();
	
	hamp_module2 = TH1D_LW::create("hamp_module2","hamp_module2",256, 0, 1024);
	regHist(hamp_module2, path, run).ignore();
	
	hamp_module3 = TH1D_LW::create("hamp_module3","hamp_module3",256, 0, 1024);
	regHist(hamp_module3, path, run).ignore();
	
	hamp_sum = TH1D_LW::create("hamp_sum","hamp_sum",256, 0, 4096);
	regHist(hamp_sum, path, run).ignore();

	hampG0_module0 = TH1D_LW::create("hampG0_module0","hampG0_module0",256, 0, 1024);
	regHist(hampG0_module0, path, run).ignore();
	
	hampG0_module1 = TH1D_LW::create("hampG0_module1","hampG0_module1",256, 0, 1024);
	regHist(hampG0_module1, path, run).ignore();
	
	hampG0_module2 = TH1D_LW::create("hampG0_module2","hampG0_module2",256, 0, 1024);
	regHist(hampG0_module2, path, run).ignore();
	
	hampG0_module3 = TH1D_LW::create("hampG0_module3","hampG0_module3",256, 0, 1024);
	regHist(hampG0_module3, path, run).ignore();
	
	hampG0_sum = TH1D_LW::create("hampG0_sum","hampG0_sum",256, 0, 4096);
	regHist(hampG0_sum, path, run).ignore();

	hampG1_module0 = TH1D_LW::create("hampG1_module0","hampG1_module0",256, 0, 1024);
	regHist(hampG1_module0, path, run).ignore();
	
	hampG1_module1 = TH1D_LW::create("hampG1_module1","hampG1_module1",256, 0, 1024);
	regHist(hampG1_module1, path, run).ignore();
	
	hampG1_module2 = TH1D_LW::create("hampG1_module2","hampG1_module2",256, 0, 1024);
	regHist(hampG1_module2, path, run).ignore();
	
	hampG1_module3 = TH1D_LW::create("hampG1_module3","hampG1_module3",256, 0, 1024);
	regHist(hampG1_module3, path, run).ignore();
	
	hampG1_sum = TH1D_LW::create("hampG1_sum","hampG1_sum",256, 0, 4096);
	regHist(hampG1_sum, path, run).ignore();

	henergy_module0 = TH1D_LW::create("henergy_module0","henergy_module0",256, 0, 1024);
	regHist(henergy_module0, path, run).ignore();
	
	henergy_module1 = TH1D_LW::create("henergy_module1","henergy_module1",256, 0, 1024);
	regHist(henergy_module1, path, run).ignore();
	
	henergy_module2 = TH1D_LW::create("henergy_module2","henergy_module2",256, 0, 1024);
	regHist(henergy_module2, path, run).ignore();
	
	henergy_module3 = TH1D_LW::create("henergy_module3","henergy_module3",256, 0, 1024);
	regHist(henergy_module3, path, run).ignore();
	
	henergy_sum = TH1D_LW::create("henergy_sum","henergy_sum",256, 0, 4096);
	regHist(henergy_sum, path, run).ignore();

	henergyG0_module0 = TH1D_LW::create("henergyG0_module0","henergyG0_module0",256, 0, 1024);
	regHist(henergyG0_module0, path, run).ignore();
	
	henergyG0_module1 = TH1D_LW::create("henergyG0_module1","henergyG0_module1",256, 0, 1024);
	regHist(henergyG0_module1, path, run).ignore();
	
	henergyG0_module2 = TH1D_LW::create("henergyG0_module2","henergyG0_module2",256, 0, 1024);
	regHist(henergyG0_module2, path, run).ignore();
	
	henergyG0_module3 = TH1D_LW::create("henergyG0_module3","henergyG0_module3",256, 0, 1024);
	regHist(henergyG0_module3, path, run).ignore();
	
	henergyG0_sum = TH1D_LW::create("henergyG0_sum","henergyG0_sum",256, 0, 4096);
	regHist(henergyG0_sum, path, run).ignore();

	henergyG1_module0 = TH1D_LW::create("henergyG1_module0","henergyG1_module0",256, 0, 1024);
	regHist(henergyG1_module0, path, run).ignore();
	
	henergyG1_module1 = TH1D_LW::create("henergyG1_module1","henergyG1_module1",256, 0, 1024);
	regHist(henergyG1_module1, path, run).ignore();
	
	henergyG1_module2 = TH1D_LW::create("henergyG1_module2","henergyG1_module2",256, 0, 1024);
	regHist(henergyG1_module2, path, run).ignore();
	
	henergyG1_module3 = TH1D_LW::create("henergyG1_module3","henergyG1_module3",256, 0, 1024);
	regHist(henergyG1_module3, path, run).ignore();
	
	henergyG1_sum = TH1D_LW::create("henergyG1_sum","henergyG1_sum",256, 0, 4096);
	regHist(henergyG1_sum, path, run).ignore();

	htime_module0 = TH1D_LW::create("htime_module0","htime_module0",256, 0, 1024);
	regHist(htime_module0, path, run).ignore();
	
	htime_module1 = TH1D_LW::create("htime_module1","htime_module1",256, 0, 1024);
	regHist(htime_module1, path, run).ignore();
	
	htime_module2 = TH1D_LW::create("htime_module2","htime_module2",256, 0, 1024);
	regHist(htime_module2, path, run).ignore();
	
	htime_module3 = TH1D_LW::create("htime_module3","htime_module3",256, 0, 1024);
	regHist(htime_module3, path, run).ignore();

	htimeG0_module0 =TH1D_LW::create ("htimeG0_module0","htimeG0_module0",256, 0, 1024);
	regHist(htimeG0_module0, path, run).ignore();
	htimeG0_module1 = TH1D_LW::create("htimeG0_module1","htimeG0_module1",256, 0, 1024);
	regHist(htimeG0_module1, path, run).ignore();
	htimeG0_module2 = TH1D_LW::create("htimeG0_module2","htimeG0_module2",256, 0, 1024);
	regHist(htimeG0_module2, path, run).ignore();
	htimeG0_module3 = TH1D_LW::create("htimeG0_module3","htimeG0_module3",256, 0, 1024);
	regHist(htimeG0_module3, path, run).ignore();
	
	htimeG1_module0 = TH1D_LW::create("htimeG1_module0","htimeG1_module0",256, 0, 1024);
	regHist(htimeG1_module0, path, run).ignore();
	htimeG1_module1 = TH1D_LW::create("htimeG1_module1","htimeG1_module1",256, 0, 1024);
	regHist(htimeG1_module1, path, run).ignore();
	htimeG1_module2 = TH1D_LW::create("htimeG1_module2","htimeG1_module2",256, 0, 1024);
	regHist(htimeG1_module2, path, run).ignore();
	htimeG1_module3 = TH1D_LW::create("htimeG1_module3","htimeG1_module3",256, 0, 1024);
	regHist(htimeG1_module3, path, run).ignore();
			
	hqual_module0 = TH1D_LW::create("hqual_module0","hqual_module0",256, 0, 1024);
	regHist(hqual_module0, path, run).ignore();
	hqual_module1 = TH1D_LW::create("hqual_module1","hqual_module1",256, 0, 1024);
	regHist(hqual_module1, path, run).ignore();
	hqual_module2 = TH1D_LW::create("hqual_module2","hqual_module2",256, 0, 1024);
	regHist(hqual_module2, path, run).ignore();
	hqual_module3 = TH1D_LW::create("hqual_module3","hqual_module3",256, 0, 1024);
	regHist(hqual_module3, path, run).ignore();

	hqualG0_module0 = TH1D_LW::create("hqualG0_module0","hqualG0_module0",256, 0, 1024);
	regHist(hqualG0_module0, path, run).ignore();
	hqualG0_module1 = TH1D_LW::create("hqualG0_module1","hqualG0_module1",256, 0, 1024);
	regHist(hqualG0_module1, path, run).ignore();
	hqualG0_module2 = TH1D_LW::create("hqualG0_module2","hqualG0_module2",256, 0, 1024);
	regHist(hqualG0_module2, path, run).ignore();
	hqualG0_module3 = TH1D_LW::create("hqualG0_module3","hqualG0_module3",256, 0, 1024);
	regHist(hqualG0_module3, path, run).ignore();

	hqualG1_module0 = TH1D_LW::create("hqualG1_module0","hqualG1_module0",256, 0, 1024);
	regHist(hqualG1_module0, path, run).ignore();
	hqualG1_module1 = TH1D_LW::create("hqualG1_module1","hqualG1_module1",256, 0, 1024);
	regHist(hqualG1_module1, path, run).ignore();
	hqualG1_module2 = TH1D_LW::create("hqualG1_module2","hqualG1_module2",256, 0, 1024);
	regHist(hqualG1_module2, path, run).ignore();
	hqualG1_module3 = TH1D_LW::create("hqualG1_module3","hqualG1_module3",256, 0, 1024);
	regHist(hqualG1_module3, path, run).ignore();

}
