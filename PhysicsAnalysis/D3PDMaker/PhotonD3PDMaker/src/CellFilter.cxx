/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include <algorithm>

#include "CellFilter.h"
#include "egammaEvent/PhotonContainer.h"
#include "egammaEvent/ElectronContainer.h"
#include "egammaEvent/egammaPIDdefs.h"
#include "DataModel/ConstDataVector.h"

#include "D3PDMakerInterfaces/IObjGetterTool.h"

#include "GaudiKernel/ISvcLocator.h"
#include "GaudiKernel/StatusCode.h"
 
#include "GaudiKernel/MsgStream.h"

#include "AthenaKernel/errorcheck.h"

#include "StoreGate/StoreGateSvc.h" 

#define PI 2.*asin(1.)

namespace D3PD{
CellFilter::CellFilter(const std::string& name, 
			 ISvcLocator* pSvcLocator) : 
  Algorithm(name, pSvcLocator), 
  m_inputCellsName("CellParticleCandidate"),
  m_outputCellsName("SelectedCells"),
  m_ptCut(10000),
  m_etaCut(2.5),
  m_deltaRCut(.6),
  m_photonID(egammaPID::PhotonIDLoose),
  m_electronID(egammaPID::ElectronIDMediumPP),
  m_storeGate(0)
{ 
  declareProperty("InputCellCollectionName" ,m_inputCellsName);
  declareProperty("OutputCellCollectionName",m_outputCellsName);
  declareProperty("photonGetter"             ,m_ph_getter);
  declareProperty("electronGetter"           ,m_el_getter);
  declareProperty("ptCut"                    ,m_ptCut);
  declareProperty("etaCut"                   ,m_etaCut);
  declareProperty("deltaRCut"                ,m_deltaRCut);
  declareProperty("photonID"                 ,m_photonID);
  declareProperty("electronID"               ,m_electronID);
  declareProperty("AcceptAll"                ,m_AcceptAll);
}

CellFilter::~CellFilter()
{ }

StatusCode CellFilter::initialize()
{

  MsgStream log(msgSvc(), name());


  StatusCode sc;
   
   // get StoreGate 
  sc = service("StoreGateSvc", m_storeGate);
  if ( sc == StatusCode::FAILURE ) {
    log<<MSG::ERROR << "   could not access StoreGate " << endreq;
    return StatusCode::FAILURE;
  }
 
  // Retrieve the getter tool and configure it, saying that we want
  // to get @c egamma objects from it.
  CHECK( m_ph_getter.retrieve() );
  CHECK( m_ph_getter->configureD3PD<Analysis::Photon>() );

  // Retrieve the getter tool and configure it, saying that we want
  // to get @c egamma objects from it.
  CHECK( m_el_getter.retrieve() );
  CHECK( m_el_getter->configureD3PD<Analysis::Electron>() );

  return StatusCode::SUCCESS;
    
}


StatusCode CellFilter::finalize()
{

 return StatusCode::SUCCESS;
}

///////////////
StatusCode CellFilter::execute()
{
  MsgStream log( messageService(), name() );
  log << MSG::DEBUG << " in execute" << endreq;

  // Get the input cells container
  const CaloCellContainer* cellcoll ;
  StatusCode sc = m_storeGate->retrieve(cellcoll, m_inputCellsName) ; 
  if(StatusCode::SUCCESS != sc ){
    log<<MSG::ERROR<<" unable to retrieve CaloCell container: "<< m_inputCellsName <<endreq; 
    return sc;
  }

  // Record the output cells container
  ConstDataVector<CaloCellContainer>* outputCont =
    new ConstDataVector<CaloCellContainer>(SG::VIEW_ELEMENTS) ;
  sc = m_storeGate->record(outputCont, m_outputCellsName) ; 
  if(StatusCode::SUCCESS != sc ) 
    {
      log<<MSG::ERROR<<" unable to record Cells container: "<< m_outputCellsName <<endreq; 
      return sc;
    }

  // clean vectors
  etas.clear();
  phis.clear();

  // a vector to keep cell of the cells from electrons and photons that pass our basic selection
  std::vector<const CaloCell*> electron_muon_photon_cells;

  // Get the photons
  if (m_ph_getter->reset().isFailure()) {
    REPORT_MESSAGE (MSG::ERROR) << "Can't reset input getter.";
    return 0;
  }

  // now, loop over all the photons, store the eta and phi positions of qualifying photons
  //const Analysis::Photon* eout = 0;
  while ( const Analysis::Photon* obj = m_ph_getter->next<Analysis::Photon>()) {

    if(fabs(obj->cluster()->etaBE(2))>m_etaCut) continue;
    if(obj->cluster()->pt() < m_ptCut) continue;
    if(!obj->passID((egammaPID::egammaIDQuality)m_photonID)) continue;

    fillDirection((egamma*)obj);
  }

  // Get the electrons
  if (m_el_getter->reset().isFailure()) {
    REPORT_MESSAGE (MSG::ERROR) << "Can't reset input getter.";
    return 0;
  }

  // now, loop over all the electrons, store the eta and phi positions of qualifying electrons
  //const Analysis::Electron* eout = 0;
  while ( const Analysis::Electron* obj = m_el_getter->next<Analysis::Electron>()) {

    // only keep the rest of the cells if they're nearby an interesting electron
    if(fabs(obj->cluster()->etaBE(2))>m_etaCut) continue;
    if(obj->cluster()->pt() < m_ptCut) continue;
    if(!obj->passID((egammaPID::egammaIDQuality)m_electronID)) continue;

    fillDirection((egamma*)obj);
  }


  // if there are no qualifying photons/electrons/muons, no need to look any further
  if(etas.size() == 0 && electron_muon_photon_cells.size() == 0) return sc;

  // now loop over all the cells, and keep the ones that are within m_deltaRCut of at least one of the qualifying photons
  CaloCellContainer::const_iterator f_cell = cellcoll->begin();
  CaloCellContainer::const_iterator l_cell = cellcoll->end();
  
  for ( ; f_cell!=l_cell; ++f_cell) {
    const CaloCell* cell = (*f_cell);

    if(m_AcceptAll){
      outputCont->push_back(const_cast<CaloCell*>(cell));
      continue;
    }

    for(unsigned int i=0; i<etas.size(); i++){
      float deltaR = sqrt( pow(etas[i]-cell->eta()            ,2.) +
			   pow(acos(cos(phis[i]-cell->phi() )),2.) );
      if( deltaR < m_deltaRCut ){
	outputCont->push_back(const_cast<CaloCell*> (cell));
	break;
      }
    }     
  }

  return sc;
}

void CellFilter::fillDirection(egamma* obj)
{  
  // consider both eta and phi directions, the egamma's object and the cluster ones

  float eta = obj->eta();
  float phi = obj->phi();
  
  etas.push_back(eta);
  phis.push_back(phi);
  
  float eta_clus = obj->cluster()->eta();
  float phi_clus = obj->cluster()->phi();
  
  etas.push_back(eta_clus);
  phis.push_back(phi_clus);
  
}

float CellFilter::correct_phi(float phi)
{
  if (phi>    PI) return phi-2.*PI;
  if (phi<-1.*PI) return phi+2.*PI;
  return phi;
}

}
