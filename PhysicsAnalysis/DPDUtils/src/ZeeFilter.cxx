/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// STL includes
#include <algorithm>
#include <iostream>

// FrameWork includes
#include "GaudiKernel/Property.h"
#include "AthenaKernel/IThinningSvc.h"
// StoreGate
#include "StoreGate/StoreGateSvc.h"
#include "StoreGate/DataHandle.h"
//Collections 
#include "egammaEvent/ElectronContainer.h"
#include "egammaEvent/Electron.h"
#include "egammaEvent/egamma.h"
#include "egammaEvent/egammaContainer.h"
#include "egammaEvent/egDetail.h"

//Actual Class 
#include "DPDUtils/ZeeFilter.h"

/////////////////////////////////////////////////////////////////// 
/// Public methods: 
/////////////////// //////////////////////////////////////////////// 

/// Constructors
////////////////

ZeeFilter::ZeeFilter( const std::string& name,ISvcLocator* pSvcLocator ) : 
  AthAlgorithm (name, pSvcLocator),
  m_All(0),
  m_pass(0),
  m_EventCounter(0),
  m_ThinningSvc(nullptr)
{
  //
  // Property declaration
  // 
  declareProperty("ElectronCollectionName",m_ElectronCollectionName="ElectronAODCollection");
  declareProperty("ElecEtCut",m_ElectronEtCut=10000);
  declareProperty("ElecEtaCut", m_ElectronEtaCut=2.5);
}

/// Destructor
///////////////
ZeeFilter::~ZeeFilter()
{

}

/// Athena Algorithm's Hooks
////////////////////////////
StatusCode ZeeFilter::initialize()
{

  StatusCode  sc;

  ATH_MSG_INFO( "Initializing ZeeFilter" );
  
  if ( !evtStore().retrieve().isSuccess() ) 
    {
      ATH_MSG_ERROR ("Unable to retrieve evtStore");
      return StatusCode::FAILURE;
    }
 
  sc = service("ThinningSvc", m_ThinningSvc);
  if (!sc.isSuccess()) {
    ATH_MSG_ERROR("ThinningSvc not found");
    return sc;
  }

  return StatusCode::SUCCESS;
}


StatusCode ZeeFilter::finalize()
{
  ATH_MSG_INFO("Zee Filter PASSED "<<m_pass<<" FROM "<< m_All);
  return StatusCode::SUCCESS;
}

StatusCode ZeeFilter::execute()
{  
  m_All++;
  StatusCode sc = StatusCode::SUCCESS;
  
  /// read the AOD electron container from persistecy storage
  const ElectronContainer* elecTES(0);
  sc = evtStore()->retrieve( elecTES, m_ElectronCollectionName);
  if( sc.isFailure()  ||  !elecTES ) {
    ATH_MSG_ERROR("No AOD electron container found in TDS");
    return sc;
  }
  ATH_MSG_DEBUG("ElectronContainer successfully retrieved");

  //////////////////////////////////////////////////////////////////////

  ATH_MSG_DEBUG("Zee Filter");
  //Collection
  unsigned int electronCounter=0;
  std::vector<bool> vbool(elecTES->size(),false);

  for(unsigned int i=0;i<elecTES->size();++i)
    {
      if (elecTES->at(i)->author(egammaParameters::AuthorElectron)
	  &&elecTES->at(i)->et()>m_ElectronEtCut && fabs(elecTES->at(i)->eta() ) < m_ElectronEtaCut
	  &&elecTES->at(i)->isem(egammaPID::ElectronLoose)==0)
	{
	  electronCounter++;
	  vbool.at(i)=true;
	}
    }

  if(electronCounter>1)
    {
      ATH_MSG_DEBUG("Filter Passed");
      setFilterPassed(true);
      m_pass++;
      if (m_ThinningSvc->filter(*elecTES, vbool) != StatusCode::SUCCESS) {
	ATH_MSG_ERROR("Error in Thinning");
      }
    }
  else
    {
      ATH_MSG_DEBUG("Filter Failed");
      setFilterPassed(false);
    }

  return StatusCode::SUCCESS;
}
