/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include <sstream>

#include "GaudiKernel/IJobOptionsSvc.h"
#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/StatusCode.h"

#include "AthenaMonitoring/AthenaMonManager.h"
#include "HIMonitoring/HIMonitoringElectronsTool.h"

#include "LWHists/TH1D_LW.h"
#include "LWHists/TH2D_LW.h"
#include "LWHists/TProfile_LW.h"

HIMonitoringElectronsTool::
HIMonitoringElectronsTool( const std::string & type, const std::string & name,
const IInterface* parent ): ManagedMonitorToolBase( type, name, parent )   	     	  
{
	m_FCalEt=0;  
	m_FCalEt_nbins=85; 
	m_low_FCalEt=-0.5; 
	m_high_FCalEt=8.0;   
}


HIMonitoringElectronsTool::~HIMonitoringElectronsTool()
{
}

// Description: Used for rebooking unmanaged histograms       
StatusCode HIMonitoringElectronsTool::bookHistogramsRecurrent( )
{
	return StatusCode::SUCCESS;
}


// Description: Used for re-booking managed histograms       
StatusCode HIMonitoringElectronsTool::bookHistograms( )
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


StatusCode HIMonitoringElectronsTool::fillHistograms()
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
	h_FCalEt->Fill(m_FCalEt);
	
	
	return StatusCode::SUCCESS;
}


StatusCode HIMonitoringElectronsTool::procHistograms( )
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
void HIMonitoringElectronsTool::book_hist()
{
	std::string path = "HeavyIon/Electrons"; 
	
	h_FCalEt = TH1D_LW::create( "h_FCalEt", "; FCal #Sigma E_{T} [TeV]; entries", m_FCalEt_nbins, m_low_FCalEt, m_high_FCalEt); 
	regHist(h_FCalEt, path, run).ignore();
	
}
