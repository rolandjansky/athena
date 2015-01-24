/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// PPGlobalEt.cxx, (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////

#include "HIGlobal/PPGlobalEt.h"
#include "StoreGate/StoreGateSvc.h"

#include "HIGlobal/HIEtData.h"

#include "CaloEvent/CaloCell.h"
#include "CaloEvent/CaloCellContainer.h"

//================ Constructor =================================================

 PPGlobalEt::PPGlobalEt(const std::string& name, ISvcLocator* pSvcLocator)
  :
   AthAlgorithm(name,pSvcLocator)
{
  //  template for property decalration
  //declareProperty("PropertyName", m_propertyName);
}

//================ Destructor =================================================

 PPGlobalEt::~PPGlobalEt()
{}

//================ Initialisation =================================================

StatusCode  PPGlobalEt::initialize()
{
  ATH_MSG_DEBUG( name() << " initialize()" );
  return StatusCode::SUCCESS;
}

//================ Finalisation =================================================

StatusCode  PPGlobalEt::finalize()
{
  // Code entered here will be executed once at the end of the program run.
  return StatusCode::SUCCESS;
}

//================ Execution ====================================================

StatusCode  PPGlobalEt::execute()
{
  ATH_MSG_DEBUG( " PPGlobalEt::execute() " );

  const CaloCellContainer* cellContainer = nullptr;
  ATH_CHECK( evtStore()->retrieve (cellContainer,"AllCalo") );

  HIEtData *dobj = new HIEtData;
  dobj->Et( GetEt_pp(cellContainer) );
  dobj->EtvsEta(GetEtvsEta_pp(cellContainer));
  ATH_CHECK( evtStore()->record( dobj,name()) );
  return StatusCode::SUCCESS;
}

float PPGlobalEt::GetEt_pp(const CaloCellContainer * cCellc)
{
  float et=0;
  for (const CaloCell* cell : *cCellc)
    et  += cell->et() ;
  return et;
}
//===============================================================================
float* PPGlobalEt::GetEtvsEta_pp(const CaloCellContainer * cccCellc)
{
    float *et_vs_eta_pp=new float[50];

    std::fill (et_vs_eta_pp, et_vs_eta_pp + 50, 0);

    for (const CaloCell* cell : *cccCellc) {
      float eta                  = cell->eta()           ;
      int   etabin               = (int)((eta+5.001)/0.2);
      float et                   = cell->et()            ;
      et_vs_eta_pp[etabin] += et;
    }

    for (int i = 0; i<50; i++)
      {
        float eta = -4.9+(i*0.2);
        et_vs_eta_pp[i] *=m_correction_pp.C_Correction_pp(eta);
      }

    return et_vs_eta_pp;
}


