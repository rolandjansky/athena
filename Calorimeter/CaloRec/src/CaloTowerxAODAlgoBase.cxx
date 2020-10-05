/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#include "CaloTowerxAODAlgoBase.h"

#include "xAODCaloEvent/CaloTowerContainer.h"
#include "xAODCaloEvent/CaloTowerAuxContainer.h"

#include "CaloDetDescr/CaloDetDescrManager.h"
#include "CaloDetDescr/CaloDetDescrElement.h"
#include "CaloGeoHelpers/CaloPhiRange.h"
#include "CaloIdentifier/CaloCell_ID.h"

CaloTowerxAODAlgoBase::CaloTowerxAODAlgoBase(const std::string& name,ISvcLocator* pSvcLocator)
    : AthReentrantAlgorithm(name,pSvcLocator),
    m_caloTowerContainerKey("CmbTowers") {
  declareProperty("CaloTowerContainer", m_caloTowerContainerKey,"Output xAOD::CaloTowerContainer");
  declareProperty("minEta",m_minEta=-5,"Tower-grid: lower eta boundary");
  declareProperty("maxEta",m_maxEta=5,"Tower-grid: upper eta boundary");
  declareProperty("nEta",m_nEtaBins=100,"Tower-grid: number of phi bins");
  declareProperty("nPhi",m_nPhiBins=64,"Tower-grid: number of phi bins");
  declareProperty("doCrossChecks",m_doxCheck=false,"Turn on internal checks (debugging)");
}

CaloTowerxAODAlgoBase::~CaloTowerxAODAlgoBase()
{ }


StatusCode CaloTowerxAODAlgoBase::initBase() {
  ATH_CHECK( m_caloTowerContainerKey.initialize() );
  
  return fillIndexCache();
}


SG::WriteHandle<xAOD::CaloTowerContainer>
CaloTowerxAODAlgoBase::makeContainer (const EventContext& ctx) const
{
  SG::WriteHandle<xAOD::CaloTowerContainer> caloTowerContainer
    (m_caloTowerContainerKey, ctx);

  if( caloTowerContainer.record (std::make_unique<xAOD::CaloTowerContainer>(),
                                  std::make_unique<xAOD::CaloTowerAuxContainer>()).isSuccess())
  {
    // configure the container
    caloTowerContainer->configureGrid(m_nEtaBins,m_minEta,m_maxEta,m_nPhiBins);

    //Prefill the container:
    const int nTowers=caloTowerContainer->nTowers();
  
    for (int iTower=0;iTower<nTowers;++iTower) {
      xAOD::CaloTower* tower=new xAOD::CaloTower();
      caloTowerContainer->push_back(tower);
      tower->setEnergy(0.0);
    }
  }
  
  return caloTowerContainer;
}


StatusCode CaloTowerxAODAlgoBase::fillIndexCache() {

  ATH_MSG_INFO("Filling cell -> tower index map");

  //Some basic sanity checks:
  if (m_minEta>=m_maxEta) {
    ATH_MSG_ERROR( "Configuration problem: minEta>=maxEta " << m_minEta << "," << m_maxEta  );
    return StatusCode::FAILURE;
  }
  
  const CaloDetDescrManager* theManager;
 
  ATH_CHECK(detStore()->retrieve(theManager));

  if ( ! theManager->isInitialized() ){
    ATH_MSG_ERROR( "CaloDetDescrManager is not yet initialized!"  );
    return StatusCode::FAILURE;
  }

  //Build a dummy CaloTowerContainer
  std::unique_ptr<xAOD::CaloTowerContainer> dummy(new xAOD::CaloTowerContainer());

  dummy->configureGrid(m_nEtaBins,m_minEta,m_maxEta,m_nPhiBins);
  
  const int nTowers=dummy->nTowers();
  
  if (nTowers<1) {
    ATH_MSG_ERROR( "Something went wrong with tower grid config: Got container with " << nTowers << " Towers."  );
    return StatusCode::FAILURE;
  }

  ATH_MSG_INFO("Working on tower container with dEta=" << dummy->deltaEta() << ", dPhi=" 
	       << dummy->deltaPhi() << ", nTowers=" << nTowers);

  m_cellToTower.clear();
  m_cellToTower.resize(theManager->element_size()); //fixme, get calo-hash-max

  //FCAL cells are not pointing. We slice them in smaller chunks to see in which tower they go 
  const std::array<double,3> ndxFCal={{4.,4.,6.}};
  const std::array<double,3> ndyFCal={{4.,6.,6.}};

  for (CaloDetDescrManager::calo_element_const_iterator ddeIt=theManager->element_begin();
       ddeIt!=theManager->element_end();++ddeIt) {

    const CaloDetDescrElement* dde=*ddeIt;

    if (dde->is_lar_fcal()) {
      //special handling of FCAL cells. Inspired by LArRecUtils/src/LArFCalTowerStore.cxx 
      double xCell  = dde->x();
      double yCell  = dde->y();
      double zCell  = dde->z();
      double dxCell = dde->dx();
      double dyCell = dde->dy();
 
      // get cell logical location
      int thisModule = dde->getLayer()-1; //modules are counted 1..n, convert to 0..n-1
      double theXBin   = dxCell / ndxFCal[thisModule];
      double theYBin   = dyCell / ndyFCal[thisModule];
      double theWeight = 1. / (ndxFCal[thisModule] * ndyFCal[thisModule]);

      // loop the cell fragments
      for (int iX=0;iX<ndxFCal[thisModule];++iX) {
	double x=xCell-dxCell/2.0 + iX*theXBin;
	for (int iY=0;iY<ndyFCal[thisModule];++iY) {
	  double y=yCell-dyCell/2.0 + iY*theYBin;

	  // eta,phi of fragment
	  double r   = sqrt( x * x + y * y + zCell * zCell );
	  double eta = -0.5 * log((r-zCell)/(r+zCell));
	  double phi =  CaloPhiRange::fix(atan2(y,x));
	  //int towerIdx=xAOD::CaloTowerGrid::index(dummy.get(),eta,phi);
	  int towerIdx=dummy->index(eta,phi);
	  if (towerIdx>=nTowers || towerIdx<0) {
	    ATH_MSG_ERROR("Found invalid tower index for FCAL cell eta/phi " << eta << "/" << phi << ", x/y=" << x << "/" << y);
	    return StatusCode::FAILURE;
	  }
	  //m_cellToTower[dde->calo_hash()].push_back(cellToTower_t(towerIdx,theWeight));
	  m_cellToTower[dde->calo_hash()].emplace_back(towerIdx,theWeight);
	  ATH_MSG_VERBOSE("cell hash " << dde->calo_hash() << ", goes into tower " << towerIdx << " with weight " << theWeight);
	} //end y loop
      } //end x loop
    }//end FCAL

    else { //Non-FCAL subcalos:

      const double cellDeta = dde->deta();
      const double cellDphi = dde->dphi();
      const double etaRaw   = dde->eta_raw();
      const double phiRaw   = CaloPhiRange::fix(dde->phi_raw());
      //The following lines ar copied from the old CaloTowerStore.cxx
      // calculate cell/tower size ratio (integer,[1..n])
      size_t ke = (size_t) (cellDeta/dummy->deltaEta()+0.5);
      ke = (ke==0) ? 1 : ke;
      size_t kp = (size_t) (cellDphi/dummy->deltaPhi()+0.5);
      kp = (kp==0) ? 1 : kp;
      // signal weight
      if ( ke>1 || kp>1 ) {
	ATH_MSG_VERBOSE( "Found cell [0x" << std::hex << dde->identify().get_compact() << std::dec 
                         << "] spanning several towers. nEta=" << ke << "nPhi="<<kp );
      }


      double theWeight = 1. / ( (double) ke * kp );
      // fractional cell sizes
      double cellDdeta = cellDeta / (double) ke;
      double cellDdphi = cellDphi / (double) kp;
      double etaMin    = etaRaw - cellDeta / 2.;
      double phiMin    = phiRaw - cellDphi / 2.;
    
      //Loop over cell fragments:

      for ( size_t ie=1; ie<=ke; ie++ ){                            // outer (eta) fragment loop
	double cellEta  = etaMin + ((double)ie - 0.5) * cellDdeta;        // eta of fragment
	for ( size_t ip=1; ip<=kp; ip++ ){                          // inner (phi) fragement loop
	  double cellPhi = phiMin + ((double)ip - 0.5) * cellDdphi;       // phi of fragment
	  int towerIdx=dummy->index(cellEta,cellPhi);
	  if (towerIdx>=nTowers || towerIdx<0) {
	    ATH_MSG_ERROR("Found invalid tower index " << towerIdx << " for cell eta/phi " << cellEta << "/" << cellPhi << " coming from " <<  dde->calo_hash() << "/" << ie << "/" << ip);
	    return StatusCode::FAILURE;
	  }
	  m_cellToTower[dde->calo_hash()].push_back(cellToTower_t(towerIdx,theWeight));
	  ATH_MSG_VERBOSE("cell hash " << dde->calo_hash() << ", goes into tower " << towerIdx << "with weight" << theWeight);
	}//end loop over fragments (phi)
      }// end loop over fragments (eta)
    }//end else FCAL
  }//end loop over detDescrElements
  
  
  if (m_doxCheck) {
    const CaloCell_ID* caloCellId;
    ATH_CHECK(detStore()->retrieve(caloCellId));
    for (size_t i=0;i<m_cellToTower.size(); ++i) {
      const auto& towerinfo=m_cellToTower[i];
      if (!towerinfo.empty()) 
	ATH_MSG_DEBUG("Cell with index " << i << " contributes to " << towerinfo.size() << " Towers.");
      else {
	const Identifier id=caloCellId->cell_id(i);
	ATH_MSG_ERROR("Cell with index " << i << ", id=0x" 
		      << std::hex << id.get_identifier32().get_compact() << std::dec 
		      << "does not contribute to any tower!");

}
      double sumWeight=0;
      for (const  cellToTower_t& ct : towerinfo) 
	sumWeight+=ct.m_weight;
      if (fabs(sumWeight-1)>0.001) {
	const Identifier id=caloCellId->cell_id(i);
	ATH_MSG_ERROR( "Cell with index " << i << ", id=0x" 
                       << std::hex << id.get_identifier32().get_compact() << std::dec 
                       << ": Weights don't add up to 1.0, got " << sumWeight  );
      }
    }//end loop over cells
  }//end if doxCheck

  ATH_MSG_DEBUG("Built CelltoTower index table. nCells=" << m_cellToTower.size() << ", nTowers=" << nTowers);

  return StatusCode::SUCCESS;
}
