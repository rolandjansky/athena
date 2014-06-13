/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// PPGlobalEt.cxx, (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////

#include "HIGlobal/PPGlobalEt.h"
#include "StoreGate/StoreGateSvc.h"

#include "HIGlobal/HIEtData.h"

#include <iostream>
using std::cout;using std::endl;

#include "CaloEvent/CaloCell.h"
#include "CaloEvent/CaloCellContainer.h"

//================ Constructor =================================================

 PPGlobalEt::PPGlobalEt(const std::string& name, ISvcLocator* pSvcLocator)
  :
  Algorithm(name,pSvcLocator),
  m_log(msgSvc(),name)
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
  // Code entered here will be executed once at program start.
  
  m_log.setLevel(outputLevel());
  m_log << MSG::DEBUG << name() << " initialize()" << endreq;

  
  // retrieve the StoreGate Service (delete if not needed)
  StatusCode sc= service("StoreGateSvc",m_sgSvc);
  if (sc.isFailure()) 
    m_log << MSG::ERROR << "Could not retrieve StoreGateSvc!" << endreq;
  else 
    m_log << MSG::DEBUG << "StoreGateSvc retrieved!" << endreq;
  
  m_correction_pp = new HICaloUtil;
  m_log << MSG::DEBUG << "initialize() successful in " << name() << endreq;
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
  // Code entered here will be executed once per event
  m_log << MSG::DEBUG << " PPGlobalEt::execute() " << endreq;

  HIEtData *dobj = new HIEtData;

  const CaloCellContainer  * CellContainer=0;
  StatusCode sc = m_sgSvc->retrieve (CellContainer,"AllCalo");
  if ( sc.isFailure() ) {
      m_log<<MSG::ERROR << "Could not retrieve AllCalo" << endreq;
  }

  dobj->Et( GetEt_pp(CellContainer) );
  dobj->EtvsEta(GetEtvsEta_pp(CellContainer,m_correction_pp)); 
  sc = m_sgSvc->record( dobj,name() );
  if(sc.isFailure())
  	m_log << MSG::ERROR << "Could not record EtData!" << endreq;   
   else
	m_log << MSG::DEBUG << "EtData recorded!" << endreq;
  return StatusCode::SUCCESS;
}

float PPGlobalEt::GetEt_pp(const CaloCellContainer * cCellc)
{

  CaloCellContainer::const_iterator f_cell=cCellc->begin();
  CaloCellContainer::const_iterator l_cell=cCellc->end();

  float et=0;
 
  for ( ; f_cell!=l_cell; ++f_cell)
    {
      const CaloCell* cell = (*f_cell) ;
      et  += cell->et() ;
    }
  return et;
}
//===============================================================================
float* PPGlobalEt::GetEtvsEta_pp(const CaloCellContainer * cccCellc,HICaloUtil *m_correction_pp)
{

    CaloCellContainer::const_iterator f_cell=cccCellc->begin();
    CaloCellContainer::const_iterator l_cell=cccCellc->end();
    float et         =0;
    float eta        =0;
    int   etabin     =0;
    m_et_vs_eta_pp=new float[50];

    memset(m_et_vs_eta_pp,0,50*sizeof(m_et_vs_eta_pp[0]));

    for ( ; f_cell!=l_cell; ++f_cell)
      {
        const CaloCell* cell = (*f_cell)             ;
        eta                  = cell->eta()           ;
        etabin               = (int)((eta+5.001)/0.2);
        et                   = cell->et()            ;
        m_et_vs_eta_pp[etabin] += et;
      }

    for (int i = 0; i<50; i++)
      {
        float m_eta = -4.9+(i*0.2);
        m_et_vs_eta_pp[i] *=m_correction_pp->C_Correction_pp(m_eta);
      }

    return m_et_vs_eta_pp;
}


