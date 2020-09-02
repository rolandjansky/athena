/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#include "HIClusterMaker.h"
#include "HIJetRec/HIJetRecDefs.h"
#include "NavFourMom/INavigable4MomentumCollection.h"
#include "CaloEvent/CaloCell.h"
#include "CaloEvent/CaloCellContainer.h"
#include "Navigation/NavigationToken.h"
#include "CaloUtils/CaloClusterStoreHelper.h"
#include "CxxUtils/prefetch.h"
#include <cmath>
#include <TVector2.h>

#include "StoreGate/ReadHandle.h"
#include "StoreGate/WriteHandle.h"

HIClusterMaker::HIClusterMaker(const std::string& name, ISvcLocator* pSvcLocator)
	: AthAlgorithm(name,pSvcLocator)
{
}

StatusCode HIClusterMaker::initialize()
{
	//First we initialize keys - after initialization they are frozen
	ATH_CHECK( m_towerContainerKey.initialize() );
	ATH_CHECK( m_cellContainerKey.initialize() );
	ATH_CHECK( m_outputKey.initialize() );

  return StatusCode::SUCCESS;
}

StatusCode HIClusterMaker::execute()
{

  //retrieve the tower container from store
  const INavigable4MomentumCollection* navInColl = 0;
  SG::ReadHandle<INavigable4MomentumCollection>  readHandleTower ( m_towerContainerKey );
	navInColl = readHandleTower.cptr();

  const CaloCellContainer * cellColl ;
	SG::ReadHandle<CaloCellContainer>  readHandleCell ( m_cellContainerKey );
  cellColl = readHandleCell.cptr();

  //make the container
  //Tricky migration: here we don't have to migrate our methods but what we use from CaloClusterStoreHelper
  SG::WriteHandle<xAOD::CaloClusterContainer> writeHandleContainer ( m_outputKey );
  ATH_CHECK(CaloClusterStoreHelper::AddContainerWriteHandle(&(*evtStore()), writeHandleContainer, msg()));

  //loop on towers
  for(INavigable4MomentumCollection::const_iterator towerItr=navInColl->begin();
      towerItr!=navInColl->end(); towerItr++)
  {

    //initialize variables
    float E_cl=0;
    float eta_cl=0;
    float phi_cl=0;
    float time_cl=0;
    float E2_cl=0;
    uint32_t samplingPattern=0;

    //navigate back to cells
    //Default is to sort the cells by either pointer values leading to irreproducible output
    //CaloCellIDFcn ensures cells are ordered by their IDs
    NavigationToken<CaloCell,double,CaloCellIDFcn> cellToken;
    (*towerItr)->fillToken(cellToken,double(1.));

    // Make the cluster:
    std::unique_ptr<xAOD::CaloCluster> cl(CaloClusterStoreHelper::makeCluster(cellColl));

    if ( cellToken.size() == 0 ) continue;
    for(NavigationToken<CaloCell,double,CaloCellIDFcn>::const_iterator cellItr = cellToken.begin();
	cellItr != cellToken.end(); cellItr++ )
    {
      //Bad cell policy - to be kept
      //if(m_bad_cell_tool->SkipCell(*cellItr))
      //{
      //if(m_skipBadCells && (*cellItr)->badcell()) continue;
      //}

      double geoWeight = cellToken.getParameter(*cellItr);
      double cell_E_w=(*cellItr)->energy()*geoWeight;

      IdentifierHash hashid =(*cellItr)->caloDDE()->calo_hash();
      size_t iCell=cellColl->findIndex(hashid);
      cl->addCell(iCell,geoWeight);

      E_cl+=cell_E_w;
      eta_cl+=cell_E_w*(*cellItr)->eta();
      phi_cl+=cell_E_w*(*cellItr)->phi();
      E2_cl+=cell_E_w*cell_E_w;
      time_cl+=cell_E_w*cell_E_w*(*cellItr)->time();

      unsigned int sample = (CaloSampling::CaloSample) (*cellItr)->caloDDE()->getSampling();
      samplingPattern |= (0x1U<<sample);

    }//end cell loop

    float eta0=(*towerItr)->eta();
    float phi0=(*towerItr)->phi();

    if(E_cl < m_EminMoment)
    {
      eta_cl=eta0;
      phi_cl=phi0;
    }
    else
    {
      eta_cl/=E_cl;
      phi_cl/=E_cl;
    }
    //phi moment does not respect wrap-around
    phi_cl=TVector2::Phi_mpi_pi(phi_cl);
    if(!HIJetRec::inTowerBoundary(eta0,phi0,eta_cl,phi_cl))
    {
      eta_cl=eta0;
      phi_cl=phi0;
    }

    if(E2_cl < 1e-8) time_cl=0.;
    else time_cl/=E2_cl;

    //set initial tower position
    cl->setEta0(eta0);
    cl->setPhi0(phi0);

    //set initial kinematics to be the same for all signal states
    //update upstream
    cl->setRawE(E_cl);
    cl->setRawEta(eta_cl);
    cl->setRawPhi(phi_cl);
    cl->setRawM(0);

    cl->setAltE(E_cl);
    cl->setAltEta(eta_cl);
    cl->setAltPhi(phi_cl);
    cl->setAltM(0);

    cl->setCalE(E_cl);
    cl->setCalEta(eta_cl);
    cl->setCalPhi(phi_cl);
    cl->setCalM(0);

    //extra info
    cl->setTime(time_cl);
    cl->setSamplingPattern(samplingPattern);

    msg(MSG::VERBOSE) << std::setw(20) << "PUSHING CLUSTER"
		      << std::setw(15) << cl->e()
		      << std::setw(15) << cl->eta()
		      << std::setw(15) << cl->phi()
		      << endmsg;

    writeHandleContainer->push_back(std::move(cl));
  }//end tower loop
  return StatusCode::SUCCESS;
}

StatusCode HIClusterMaker::finalize()
{
  return StatusCode::SUCCESS;
}


StatusCode HIClusterMaker::dumpClusters(xAOD::CaloClusterContainer* clusColl)
{
  msg(MSG::INFO) << "Dumping PseudoJets" << endmsg;
  for(xAOD::CaloClusterContainer::iterator clusCollIter= clusColl->begin();
      clusCollIter!= clusColl->end(); clusCollIter++)
  {
    xAOD::CaloCluster* cl = (*clusCollIter);

    float E_cl=0;
    float eta_cl=0;
    float phi_cl=0;

    CaloClusterCellLink* cellLinks=cl->getOwnCellLinks();

    if (!cellLinks)
    {
      msg(MSG::ERROR) << "Can't get valid links to CaloCells (CaloClusterCellLink)!" << endmsg;
      return StatusCode::FAILURE;
    }

    unsigned int ncells=0;
    float sumw=0;
    // CaloClusterCellLink::iterator cellIterEnd=cellLinks->end();
    // for(CaloClusterCellLink::iterator cellIter=cellLinks->end();
    // 	cellIter != cellIterEnd; cellIter++, ncells++ )
    // {
    xAOD::CaloCluster::cell_iterator cellIterEnd = cl->cell_end();
    for(xAOD::CaloCluster::cell_iterator cellIter= cl->cell_begin();
    	cellIter != cellIterEnd; cellIter++ )
    {
      CxxUtils::prefetchNext (cellIter, cellIterEnd);
      const CaloCell* pCell=(*cellIter);
      //double geoWeight =cellIter.weight(); //weird synatx, "." on iterator
      double cell_E_w=pCell->energy();//*geoWeight;

      E_cl+=cell_E_w;
      eta_cl+=cell_E_w*pCell->eta();
      phi_cl+=cell_E_w*pCell->phi();
      //sumw+=geoWeight;
    }
    if(E_cl!=0.)
    {
      eta_cl/=E_cl;
      phi_cl/=E_cl;
    }

    ATH_MSG_INFO( std::setw(10) << "DUMPING CLUSTER"
		   << std::setw(15) << cl->e()
		   << std::setw(15) << cl->eta()
		   << std::setw(15) << cl->phi()
		   << std::setw(15) << E_cl
		   << std::setw(15) << eta_cl
		   << std::setw(15) << phi_cl
		   << std::setw(15) << ncells
		   << std::setw(15) << sumw );

  }
  return StatusCode::SUCCESS;
}
