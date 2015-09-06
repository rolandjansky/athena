/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/


#include "GaudiKernel/Algorithm.h"
#include "GaudiKernel/MsgStream.h"

#include "StoreGate/StoreGateSvc.h"

#include "CLHEP/Units/SystemOfUnits.h"

#include "AIDA/IHistogram1D.h"
#include "AIDA/IHistogram2D.h"

#include "CaloGeoHelpers/CaloPhiRange.h"

#include "CaloGeoHelpers/CaloSampling.h"
#include "CaloEvent/CaloCell.h"
#include "CaloEvent/CaloTower.h"
#include "CaloEvent/CaloTowerContainer.h"

#include "CaloRec/CaloTowerMonitor.h"

#include <string>
#include <cmath>

using CLHEP::GeV;
using CLHEP::deg;

CaloTowerMonitor::CaloTowerMonitor(const std::string& name,
				   ISvcLocator* pService)
  : AthAlgorithm(name,pService),
    m_nTowers(0),
    m_nTowersVsEta(0),
    m_nTowersVsPhi(0),
    m_cellsInEtaVsPhi(0),
    m_nCellsInTower(0),
    m_nCellsInTowerVsEta(0),
    m_nCellsInTowerVsPhi(0),
    m_eTowers(0),
    m_eTowersVsEta(0),
    m_eTowersVsPhi(0),
    m_eLogTowers(0),
    m_etTowers(0),
    m_etTowersVsEta(0),
    m_etTowersVsPhi(0),
    m_etLogTowers(0),
    m_etaTowerVsCell(0),
    m_phiTowerVsCell(0)
{
  m_collectionNames.resize(0);
  declareProperty("InputTowerCollections",m_collectionNames);
}

CaloTowerMonitor::~CaloTowerMonitor()
{ }

StatusCode CaloTowerMonitor::initialize()
{
  // messaging
  MsgStream log(messageService(),name());

  ////////////////////////
  // Book Distributions //
  ////////////////////////

  // number of towers
  m_nTowers = histoSvc()->book("/stat/towermonitor", 100, 
			      "Number of CaloTowers",
			      500, 0., 10000.);
  m_nTowersVsEta = histoSvc()->book("/stat/towermonitor", 101, 
				   "Number of CaloTowers vs Eta",
				   100, -5., 5.); 
  m_nTowersVsPhi = histoSvc()->book("/stat/towermonitor", 102,
				   "Number of CaloTowers vs Phi [deg]",
				   100, -200., 200. );
  // tower energies
  m_eTowers = histoSvc()->book("/stat/towermonitor", 200,
			      "CaloTower E [GeV]",
			       100, 0., 100.);
  m_eTowersVsEta = histoSvc()->book("/stat/towermonitor", 201,
				   "CaloTower E [GeV] vs Eta",
				   100, -5., 5., 100, 0., 100. );
  m_eTowersVsPhi = histoSvc()->book("/stat/towermonitor", 202,
				   "CaloTower E [Gev] vs Phi [deg]",
				   100, -200., 200., 100, 0., 100. );
  m_eLogTowers = histoSvc()->book("/stat/towermonitor", 1200,
				  "CaloTower log(E/GeV)",
				  100, -7., 3. );
  // tower et's
  m_etTowers = histoSvc()->book("/stat/towermonitor", 300,
				"CaloTower Et [GeV]",
				100, 0., 100.);
  m_etTowersVsEta = histoSvc()->book("/stat/towermonitor", 301,
				    "CaloTower Et [GeV] vs Eta",
				    100, -5., 5., 100, 0., 100. );
  m_etTowersVsPhi = histoSvc()->book("/stat/towermonitor", 302,
				    "CaloTower Et [Gev] vs Phi [deg]",
				    100, -200., 200., 100, 0., 100. );
  m_etLogTowers = histoSvc()->book("/stat/towermonitor", 1300,
				   "CaloTower log(Et/GeV)",
				   100, -7., 3. );
  // tower shapes
  m_cellsInEtaVsPhi = histoSvc()->book("/stat/towermonitor", 410,
				      "CaloTower Shape",
				      100, -0.5, 0.5,
				      100, -0.5, 0.5 );
  m_nCellsInTower = histoSvc()->book("/stat/towermonitor", 400,
				    "Number of Cells in CaloTower",
				    100, 0., 100.);
  m_nCellsInTowerVsEta = histoSvc()->book("/stat/towermonitor", 401,
					 "Number of Cells vs Eta",
					 100, -5., 5., 100, 0., 100. );
  m_nCellsInTowerVsPhi = histoSvc()->book("/stat/towermonitor", 402,
					 "Number of Cells vs Phi [deg]",
					 100, -200., 200., 100, 0., 100. );
  // eta direction matches
  m_etaTowerVsCell = histoSvc()->book("/stat/towermonitor", 600,
				      "Cell Eta vs TowerEta",
				      100, -5., 5., 100, -5., 5.);
  m_etaTowerVsCellCalos[CaloSampling::PreSamplerB]
    = histoSvc()->book("/stat/towermonitor", 500,
		       "Cell Eta vs Tower Eta PreSampB",
		       100, -5., 5., 100, -5., 5.);
  m_etaTowerVsCellCalos[CaloSampling::EMB1]
    = histoSvc()->book("/stat/towermonitor", 501,
		       "Cell Eta vs Tower Eta EMB",
		       100, -5., 5., 100, -5., 5.);
  m_etaTowerVsCellCalos[CaloSampling::PreSamplerE]
    = histoSvc()->book("/stat/towermonitor", 502,
		       "Cell Eta vs Tower Eta PreSampE",
		       100, -5., 5., 100, -5., 5.);
  m_etaTowerVsCellCalos[CaloSampling::EME1] 
    = histoSvc()->book("/stat/towermonitor", 503,
		       "Cell Eta vs Tower Eta EMEC",
		       100, -5., 5., 100, -5., 5.);
  m_etaTowerVsCellCalos[CaloSampling::HEC0]
    = histoSvc()->book("/stat/towermonitor", 504,
		       "Cell Eta vs Tower Eta HEC",
		       100, -5., 5., 100, -5., 5.);
  m_etaTowerVsCellCalos[CaloSampling::TileBar0]
    = histoSvc()->book("/stat/towermonitor", 505,
		       "Cell Eta vs Tower Eta TileBarrel",
		       100, -5., 5., 100, -5., 5.);
  m_etaTowerVsCellCalos[CaloSampling::TileExt0]
    = histoSvc()->book("/stat/towermonitor", 506,
		       "Cell Eta vs Tower Eta TileExtended",
		       100, -5., 5., 100, -5., 5.);
  m_etaTowerVsCellCalos[CaloSampling::FCAL0]
    = histoSvc()->book("/stat/towermonitor", 507,
		       "Cell Eta vs Tower Eta FCal1",
		       100, -5., 5., 100, -5., 5.);
  m_etaTowerVsCellCalos[CaloSampling::FCAL1]
    = histoSvc()->book("/stat/towermonitor", 508,
		       "Cell Eta vs Tower Eta FCal2",
		       100, -5., 5., 100, -5., 5.);
  m_etaTowerVsCellCalos[CaloSampling::FCAL2]
    = histoSvc()->book("/stat/towermonitor", 509,
		       "Cell Eta vs Tower Eta FCal3",
		       100, -5., 5., 100, -5., 5.);

  // phi direction matches
  m_phiTowerVsCell = histoSvc()->book("/stat/towermonitor", 800,
				      "Cell Phi vs TowerPhi",
				      100, -200., 200., 100, -200., 200.);
  m_phiTowerVsCellCalos[CaloSampling::PreSamplerB]
    = histoSvc()->book("/stat/towermonitor", 700,
		       "Cell Phi vs Tower Phi PreSampB",
		       100, -200., 200., 100, -200., 200.);
  m_phiTowerVsCellCalos[CaloSampling::EMB1]
    = histoSvc()->book("/stat/towermonitor", 701,
		       "Cell Phi vs Tower Phi EMB",
		       100, -200., 200., 100, -200., 200.);
  m_phiTowerVsCellCalos[CaloSampling::PreSamplerE]
    = histoSvc()->book("/stat/towermonitor", 702,
		       "Cell Phi vs Tower Phi PreSampE",
		       100, -200., 200., 100, -200., 200.);
  m_phiTowerVsCellCalos[CaloSampling::EME1] 
    = histoSvc()->book("/stat/towermonitor", 703,
		       "Cell Phi vs Tower Phi EMEC",
		       100, -200., 200., 100, -200., 200.);
  m_phiTowerVsCellCalos[CaloSampling::HEC0]
    = histoSvc()->book("/stat/towermonitor", 704,
		       "Cell Phi vs Tower Phi HEC",
		       100, -200., 200., 100, -200., 200.);
  m_phiTowerVsCellCalos[CaloSampling::TileBar0]
    = histoSvc()->book("/stat/towermonitor", 705,
		       "Cell Phi vs Tower Phi TileBarrel",
		       100, -200., 200., 100, -200., 200.);
  m_phiTowerVsCellCalos[CaloSampling::TileExt0]
    = histoSvc()->book("/stat/towermonitor", 706,
		       "Cell Phi vs Tower Phi TileExtended",
		       100, -200., 200., 100, -200., 200.);
  m_phiTowerVsCellCalos[CaloSampling::FCAL0]
    = histoSvc()->book("/stat/towermonitor", 707,
		       "Cell Phi vs Tower Phi FCal1",
		       100, -200., 200., 100, -200., 200.);
  m_phiTowerVsCellCalos[CaloSampling::FCAL1]
    = histoSvc()->book("/stat/towermonitor", 708,
		       "Cell Phi vs Tower Phi FCal2",
		       100, -200., 200., 100, -200., 200.);
  m_phiTowerVsCellCalos[CaloSampling::FCAL2]
    = histoSvc()->book("/stat/towermonitor", 709,
		       "Cell Phi vs Tower Phi FCal3",
		       100, -200., 200., 100, -200., 200.);

  return StatusCode::SUCCESS;
}


StatusCode CaloTowerMonitor::execute()
{
  // messaging
  MsgStream log(messageService(),name());

  // constant
  //  double mathPi = 2. * asin(1.);
  // retrieve data container
  std::vector<std::string>::const_iterator fColl = m_collectionNames.begin();
  std::vector<std::string>::const_iterator lColl = m_collectionNames.end();
  for ( ; fColl != lColl; fColl++ )
    {
      // pointer to tower container
      const CaloTowerContainer* theTowers;
      StatusCode checkOut = evtStore()->retrieve(theTowers,*fColl);
      if ( ! checkOut.isFailure() )
	{
	  CaloTowerContainer::const_iterator fTower = theTowers->begin();
	  CaloTowerContainer::const_iterator lTower = theTowers->end();
	  m_nTowers->fill((double)theTowers->size(),1.);
	  for ( ; fTower != lTower; fTower++ )
	    {
	      const CaloTower* aTower = *fTower;
	      // check tower properties
	      //	      if ( aTower->eta() == 0 || aTower->getNumberOfCells() == 0 )
		//		{
		  //		  log << MSG::WARNING
		  //		      << "CaloTower @ "
		  //		      << aTower
		  //		      << " with (eta/phi) = ("
		  //		      << aTower->eta()
		  //		      << "/"
		  //		      << aTower->phi()
		  //		      << ") has e = "
		  //		      << aTower->e() / GeV
		  //		      << " GeV and #cells = "
		  //		      << aTower->getNumberOfCells()
		  //		      << endreq;
		  //		}
	      //	      else
	      if ( aTower->getNumberOfCells() != 0 )
		{
		  // variables
		  double e     = aTower->e()  * (1./GeV);
		  double et    = aTower->et() * (1./GeV);
		  double eta   = aTower->eta();
		  double phi   = aTower->phi();
		  //		  if ( phi > mathPi ) phi -= 2. * mathPi;
		  double cells = (double) aTower->getNumberOfCells();
		  log << MSG::DEBUG
		      << "Tower @"
		      << aTower
		      << " E = "
		      << e << " GeV (eta,phi) = ("
		      << eta << "," << phi << ")" << endreq;
		  // fill distributions
		  m_nTowersVsEta->fill(eta,1.);
		  m_nTowersVsPhi->fill(phi*(1/deg),1.);
		  // log << MSG::INFO << "fill tower e   " << e << endreq;
		  m_eTowers->fill(e,1.);
		  m_eTowersVsEta->fill(eta,e,1.);
		  m_eTowersVsPhi->fill(phi*(1./deg),e,1.);
		  if ( e > 0. )
		    {
		      m_eLogTowers->fill(log10(e),1.);
		    }
		  // log << MSG::INFO << "fill tower et  " << et << endreq;
		  m_etTowers->fill(et,1.);
		  m_etTowersVsEta->fill(eta,et,1.);
		  m_etTowersVsPhi->fill(phi*(1./deg),et,1.);
		  if ( et > 0. )
		    {
		      m_etLogTowers->fill(log10(et),1.);
		    }
		  // tower shape
		  // log << MSG::INFO << "fill tower cls " << cells << endreq;
		  m_nCellsInTower->fill(cells,1.);
		  m_nCellsInTowerVsEta->fill(eta,cells,1.);
		  m_nCellsInTowerVsPhi->fill(phi*(1./deg),cells,1.);
		  CaloTower::cell_iterator fCell = aTower->begin();
		  CaloTower::cell_iterator lCell = aTower->end();
		  CaloPhiRange correctPhi;
		  for ( ; fCell != lCell; fCell++ )
		    {
		      const CaloCell* aCell = *fCell;
		      // calculate distance
		      double cellEta  = aCell->eta();
		      double cellPhi  = aCell->phi();
		      //  if ( cellPhi > mathPi ) cellPhi -= 2. * mathPi;
		      double deltaEta = eta - aCell->eta();
		      double deltaPhi = correctPhi.diff(phi, aCell->phi());
		      // log << MSG::INFO << "fill cell deta,dphi " 
		      //  << deltaEta << "," << deltaPhi << endreq;
		      m_cellsInEtaVsPhi->fill(deltaEta,deltaPhi,1.);
		      // direction matches
		      //log << MSG::INFO << "fill cell eta " 
		      //	  << cellEta << endreq;
		      m_etaTowerVsCell->fill(cellEta,eta,1.);
		      //log << MSG::INFO << "fill tower phi " 
		      //		  << cellPhi/deg << endreq;
		      m_phiTowerVsCell->fill(cellPhi*(1./deg),phi*(1./deg),1.);
		      CaloSampling::CaloSample theSample = aCell->caloDDE()->getSampling();
		      CaloSampling::CaloSample takeSample = theSample;
		      switch ( theSample )
			{
			case CaloSampling::EMB2:
			case CaloSampling::EMB3:
			  takeSample = CaloSampling::EMB1;
			  break;
			case CaloSampling::EME2:
			case CaloSampling::EME3:
			  takeSample = CaloSampling::EME1;
			  break;
			case CaloSampling::HEC1:
			case CaloSampling::HEC2:
			case CaloSampling::HEC3:
			  takeSample = CaloSampling::HEC0;
			  break;
			case CaloSampling::TileBar1:
			case CaloSampling::TileBar2:
			case CaloSampling::TileGap1:
			case CaloSampling::TileGap2:
			case CaloSampling::TileGap3:
			  takeSample = CaloSampling::TileBar0;
			  break;
			case CaloSampling::TileExt1:
			case CaloSampling::TileExt2:
			  takeSample = CaloSampling::TileExt0;
			  break;
			default:
			  break;
			}
		      m_etaTowerVsCellCalos[takeSample]->fill(cellEta,eta,1.);
		      m_phiTowerVsCellCalos[takeSample]->fill(cellPhi*(1./deg),
							      phi*(1./deg),1.);
		    } // cell loop
		} // tower kinematics ok
	    } // tower loop
	} // check out test
    } // collection loop
  return StatusCode::SUCCESS;
}		    

StatusCode CaloTowerMonitor::finalize()
{
  return StatusCode::SUCCESS;
}
