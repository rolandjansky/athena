/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// HIGlobalEt.cxx, (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////

#include "HIGlobal/HIGlobalEt.h"
#include "StoreGate/StoreGateSvc.h"

#include "HIGlobal/HIEtData.h"

#include "CaloEvent/CaloCell.h"
#include "CaloEvent/CaloCellContainer.h"

//================ Constructor =================================================

 HIGlobalEt::HIGlobalEt(const std::string& name, ISvcLocator* pSvcLocator)
  :
   AthAlgorithm(name,pSvcLocator)
{
  //  template for property decalration
  //declareProperty("PropertyName", m_propertyName);
}

//================ Destructor =================================================

 HIGlobalEt::~HIGlobalEt()
{}


//================ Initialisation =================================================

StatusCode  HIGlobalEt::initialize()
{
  ATH_MSG_DEBUG( name() << " initialize()" );
  return StatusCode::SUCCESS;
}

//================ Finalisation =================================================

StatusCode  HIGlobalEt::finalize()
{
  return StatusCode::SUCCESS;
}

//================ Execution ====================================================

StatusCode  HIGlobalEt::execute()
{
  // Code entered here will be executed once per event
  ATH_MSG_DEBUG( " HIGlobalEt::execute() " );

  const CaloCellContainer  * CellContainer = nullptr;
  ATH_CHECK( evtStore()->retrieve (CellContainer,"AllCalo") );

  HIEtData *dobj = new HIEtData;
  dobj->Et( GetEt(CellContainer) );
  dobj->EtvsEta(GetEtvsEta(CellContainer));
  ATH_CHECK( evtStore()->record( dobj,name()) );
  return StatusCode::SUCCESS;
}

float HIGlobalEt::GetEt(const CaloCellContainer * cCellc)
{
  float et = 0;
  for (const CaloCell* cell : *cCellc)
    et  += cell->et();
  return et;
}

 
float* HIGlobalEt::GetEtvsEta(const CaloCellContainer * cccCellc)
  {
    float* et_vs_eta=new float[50]; 

    std::fill (et_vs_eta, et_vs_eta+50, 0);

    for (const CaloCell* cell : *cccCellc)
      {
        float eta                  = cell->eta()           ;
        int etabin                 = (int)((eta+5.001)/0.2); 
        float et                   = cell->et()            ;
        et_vs_eta[etabin] += et;
      }

    for (int i = 0; i<50; i++)
      {   
        float eta = -4.9+(i*0.2);
	et_vs_eta[i] *=m_correction.C_Correction(eta);
      }
      
    return et_vs_eta;
  }
//===============================================================================

