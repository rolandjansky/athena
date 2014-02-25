/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include <vector>
#include <list>
#include <algorithm>
#include <math.h>
// 
#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/IToolSvc.h"
#include "GaudiKernel/StatusCode.h"
#include "StoreGate/StoreGateSvc.h"

#include "LArMuId/LArMuIdAlgorithm.h"
#include "TrackToCalo/ImpactInCaloCollection.h"
#include "CaloUtils/CaloCellList.h"
#include "CaloEvent/CaloCellContainer.h"
//#include "CaloUtils/CaloClusterList.h"
//#include "CaloEvent/CaloClusterContainer.h"
//#include "CaloEvent/CaloCluster.h"
#include "xAODCaloEvent/CaloClusterContainer.h"
#include "xAODCaloEvent/CaloClusterKineHelper.h"
#include "CaloUtils/CaloClusterStoreHelper.h"

LArMuIdAlgorithm::LArMuIdAlgorithm(const std::string& name,ISvcLocator* pSvcLocator):Algorithm(name, pSvcLocator)
{
  declareProperty("CellsNames", m_cellContainer);
  declareProperty("ImpactNames", m_ImpactInCaloCollectionName);
  declareProperty("ClustersOutputName",m_clustersOutputName = "LArMuClusterCandidates");
  declareProperty("Window", m_dR);
  declareProperty("Threshold_High", m_thr1 = 100.);
  declareProperty("Threshold_Low", m_thr2 = 50.);
  declareProperty("Threshold_Strips", m_thr3 = 40.);  // setting this to 1TeV (1.e6) disables strips. 
  declareProperty("DeltaEta_Strips", m_Deta = 0.03 );
  declareProperty("DeltaPhi_Strips", m_Dphi = 0.06 );
  declareProperty("StartFromTracks", m_tracks = false);
  declareProperty("SelectDetector", m_detSel= 1); // bit 0 == Barrel, bit 1 == Endcap 

  return;
}

LArMuIdAlgorithm::~LArMuIdAlgorithm()
{}

StatusCode LArMuIdAlgorithm::initialize()
{
  StatusCode sc;

  MsgStream log(messageService(), name());
  
  sc = service("StoreGateSvc", m_store_gate);
  if ( sc == StatusCode::FAILURE ) {
    log<<MSG::ERROR << "   could not access StoreGate " << endreq;
    return StatusCode::FAILURE;
  }

  StoreGateSvc* detStore;
   sc = service("DetectorStore",detStore);
   if (sc.isFailure()) {
     log << MSG::ERROR
	 << "Unable to get pointer to Detector Store Service" << endreq;
     return sc;
   }
  
   sc = detStore->retrieve(m_calo_helper);
   if (sc.isFailure()) {
     log << MSG::ERROR
	 << "Unable to retrieve calo helper from DetectorStore" << endreq;
     return sc;
   }
   log << MSG::INFO<<" Selecting detector "<< m_detSel<<endreq;

  return StatusCode::SUCCESS;
}


StatusCode LArMuIdAlgorithm::execute()
{  

  MsgStream log(messageService(), name());
  //xAOD::CaloClusterContainer * clusColl = new xAOD::CaloClusterContainer;

  //StatusCode sc = CaloClusterStoreHelper::recordClusters(m_store_gate, clusColl,
  //					   m_clustersOutputName, log);
  xAOD::CaloClusterContainer * clusColl = CaloClusterStoreHelper::makeContainer(m_store_gate,
					   m_clustersOutputName, log);
  if ( clusColl==NULL ) {
    log << MSG::WARNING << "Error registering CaloCluster container in TDS "
	<< endreq;
    return StatusCode::SUCCESS;
  }


  //log << MSG::DEBUG << "in LArMuIdAlgorithm execute() ..." << endreq;

  // retrieve Calo Cell Container
  const CaloCellContainer* cell_Container; 
  StatusCode sc = m_store_gate->retrieve(cell_Container, m_cellContainer); 
  if(sc.isFailure() || !cell_Container) {
    log << MSG::WARNING
	<< "no Calo Cell Container " << m_cellContainer << " found"
	<<endreq;
  }

  if (m_tracks){

    //need a seed to start the clustering algorithm
    //use TrackToCalo in the middle to get it
    
    const ImpactInCaloCollection* impactincalocollec ;
    sc = m_store_gate->retrieve(impactincalocollec, m_ImpactInCaloCollectionName) ; 
    
    if ( sc.isFailure()) {
      log << MSG::WARNING
	  << " No impact in Calo found "
	  << endreq;
      return StatusCode::SUCCESS;
    }
    
    int icluster, ncluster;
    
    for (ImpactInCaloCollection::const_iterator itr  = 
	   (*impactincalocollec).begin(); itr != (*impactincalocollec).end(); itr++){
      
      std:: vector<xAOD::CaloCluster*> LArMuClus;
      
      float eta_seed = (*itr)->etaCaloLocal_2();
      float phi_seed = (*itr)->phiCaloLocal_2();
      //  = (*itr)->trketa_at_2();
      //  = (*itr)->trkphi_at_2();
      

      for(int be=0; be<=1; ++be ){

	CaloCell_ID::CaloSample	samp = CaloCell_ID::EMB2;
	if( be==0 ){
	  int flag = m_detSel & (1<<be) ;
	  if ( flag != 0) {
	    log<<MSG::DEBUG << " selected barrel " << endreq;
	    samp = CaloCell_ID::EMB2;
	  }
	  else 
	   {
	     continue; 
	     }
	}
	else if( be==1 ) {
	  int flag = m_detSel & (1<<be)  ;
	  if( flag != 0 ) {
	    log<<MSG::DEBUG << " selected endcap"  << endreq;
	    samp = CaloCell_ID::EME2;
	  }
	  else 
	    {
	      continue; 
	    }
	}


	// find cells in the middle arround the eta, phi seed 
	CaloCellList* my_list = 0;
      
      
	/* Internal variables needed for the muon clustering */
	float pix[256][256];
	int occupied[256][256];
	
	/* Init all the tables needed by the muon clustering */
	for (int ieta=0;ieta<256;ieta++)
	  {
	    for (int iphi=0;iphi<256;iphi++)
	      {
		occupied[ieta][iphi]=-1;
		pix[ieta][iphi]=0.0;
	      }
	  }
      
	// Construct the list in the middle only 
	my_list = new CaloCellList(cell_Container);
	//float dR = 0.1;
	my_list->select(eta_seed,phi_seed,m_dR,(int)samp);
	
	log<<MSG::DEBUG << "cell selection, eta, phi, dR " << eta_seed << " " << phi_seed << " "  << m_dR << " " << samp << endreq;	
	
	/* Loop over cells */
	// loop on all selected cells
	CaloCellList::list_iterator first = my_list->begin();
	CaloCellList::list_iterator last  = my_list->end();   
	
	int ind = 0;
	for(;first!=last;first++) {	
	  /* This is a middle cell */
	  ind++;
	  if ((*first)->energy() > -50000.)
	    {
	      
	      if ( eta2neta((*first)->eta()) < 256 && eta2neta((*first)->eta()) >= 0 && phi2nphi((*first)->phi()) < 256 && phi2nphi((*first)->phi()) >= 0 )
		pix[eta2neta((*first)->eta())][phi2nphi((*first)->phi())] 
		  = (*first)->energy();
	      //else
	      //  log<<MSG::ERROR << "too much cells for the array " << endreq;
	    }
	}
	
      
	//log<<MSG::DEBUG << "cells in the list : size " << ind << endreq;
	
	/* Start actual clustering */
	icluster=-1;
	for (int ieta=0;ieta<256;ieta++) for (int iphi=0;iphi<256;iphi++)
	  {
	    /* Look for cluster seed */
	    if ((pix[ieta][iphi] > m_thr1) && (occupied[ieta][iphi] < 0))
	      {
		/* We have a valid cluster seed */
		icluster++;
		do_clustering(pix,occupied,ieta,iphi,icluster);
	      }
	  }
	log<<MSG::DEBUG << " Nb of clusters found :  " << icluster+1 << endreq;
	ncluster=icluster+1;
	
	/* Analysis of cluster content */
	int mucand_size = LArMuClus.size();
	for (int iclus=0; iclus <ncluster; iclus++){
	  //CaloCluster *myCluster = new CaloCluster();
          xAOD::CaloCluster *myCluster = CaloClusterStoreHelper::makeCluster(cell_Container);
	  LArMuClus.push_back(myCluster);
	}  
	
	CaloCellList::list_iterator nfirst = my_list->begin();
	CaloCellList::list_iterator nlast  = my_list->end();   
	
	for(;nfirst!=nlast;nfirst++) {
	  int index = -1;
	  
	  if ( (*nfirst)->energy() >= m_thr2 ){
	    if ( eta2neta((*nfirst)->eta()) < 256 && eta2neta((*nfirst)->eta()) >= 0 && phi2nphi((*nfirst)->phi()) < 256 && phi2nphi((*nfirst)->phi()) >= 0 )
	      {
		index = occupied[eta2neta((*nfirst)->eta())][phi2nphi((*nfirst)->phi())]+mucand_size;
	      }
	    else
	      log<<MSG::DEBUG << " too much cells for the array " << endreq;
	    
	    if ( index >= 0 ){
	      LArMuClus[index]->addCell(cell_Container->findIndex(m_calo_helper->calo_cell_hash((*nfirst)->ID())),1.);
	      //LArMuClus[index]->addCell(cell_Container,(*cell_Container)[nfirst],1.);
	    }
	  }
	}
	
	for (int iclus=0; iclus <ncluster; iclus++){
          //update kine at the end
          if(LArMuClus[iclus+mucand_size]==0) continue; 
          CaloClusterKineHelper::calculateKine(LArMuClus[iclus+mucand_size],false,true);
	  clusColl->push_back(LArMuClus[iclus+mucand_size]); 
	  log << MSG::DEBUG << "CaloCluster E = " << LArMuClus[iclus+mucand_size]->e() 
	    << " MeV, Et = " << LArMuClus[iclus+mucand_size]->et() 
	    << " MeV, eta = " << LArMuClus[iclus+mucand_size]->eta() 
	    << " NCells = " << LArMuClus[iclus+mucand_size]->size() << endreq;
	}
	delete my_list;
      }
    }
  }
  else {

    int icluster, ncluster;
    std:: vector<xAOD::CaloCluster*> LArMuClus;
    
    // Construct the list in the middle only 
    CaloCell_ID::CaloSample	samp = CaloCell_ID::EMB2;
    CaloCell_ID::SUBCALO subdet = CaloCell_ID::LAREM;
 
    /* Internal variables needed for the muon clustering */
    float pix[256][256];
    int occupied[256][256];
    
    /* Init all the tables needed by the muon clustering */
    for (int ieta=0;ieta<256;ieta++)
      {
	for (int iphi=0;iphi<256;iphi++)
	  {
	    occupied[ieta][iphi]=-1;
	    pix[ieta][iphi]=0.0;
	  }
      }
    /* Loop over cells */
    // loop on all selected cells
    size_t first = cell_Container->indexFirstCellCalo(subdet);
    size_t last = cell_Container->indexLastCellCalo(subdet);
    
    int ind = 0;
    for(;first<=last;first++) {	
      /* This is a middle cell */
      ind++;
      if ( ((*cell_Container)[first])->energy() > -50000.)
	{
	  if ( m_calo_helper->calo_sample(((*cell_Container)[first])->ID()) == samp){

	    //log<<MSG::DEBUG << "cells in the list : " << (*cell_Container)[first]->eta() << " " << (*cell_Container)[first]->phi() << " " << (*cell_Container)[first]->energy() << " " << m_calo_helper->calo_sample(((*cell_Container)[first])->ID()) << endreq;
	  
	  if ( eta2neta((*cell_Container)[first]->eta()) < 256 && eta2neta((*cell_Container)[first]->eta()) >= 0 && phi2nphi((*cell_Container)[first]->phi()) < 256 && phi2nphi((*cell_Container)[first]->phi()) >= 0 )
	    {
	      pix[eta2neta((*cell_Container)[first]->eta())][phi2nphi((*cell_Container)[first]->phi())] 
		= (*cell_Container)[first]->energy();
	      //log<<MSG::DEBUG << "cells in the list : " << (*cell_Container)[first]->eta() << " " << eta2neta((*cell_Container)[first]->eta()) << " " << (*cell_Container)[first]->phi() << " " << phi2nphi((*cell_Container)[first]->phi()) << " "<< (*cell_Container)[first]->energy() << " " << m_calo_helper->calo_sample(((*cell_Container)[first])->ID()) << endreq;  
	    }
	  else
	    log<<MSG::DEBUG << "too much cells for the array " << endreq;
	  }
	}
    }
    
    //log<<MSG::DEBUG << "cells in the list : size " << ind << endreq;
    
    /* Start actual clustering */
    icluster=-1;
    for (int ieta=0;ieta<256;ieta++) for (int iphi=0;iphi<256;iphi++)
      {
	/* Look for cluster seed */
	if ((pix[ieta][iphi] > m_thr1) && (occupied[ieta][iphi] < 0))
	  {
	    /* We have a valid cluster seed */
	    icluster++;
	    do_clustering(pix,occupied,ieta,iphi,icluster);
	  }
      }
    //log<<MSG::DEBUG << " Nb of clusters found :  " << icluster+1 << endreq;
    ncluster=icluster+1;
    
    /*    for (int ieta=0; ieta<256; ieta++)
	  for (int iphi=0;iphi<256; iphi++){
	  log<<MSG::DEBUG << occupied[ieta][iphi] << endreq;
	  }
	  log<<MSG::DEBUG << "***************************" << endreq;
	  for (int ieta=0; ieta<256; ieta++)
	  for (int iphi=0;iphi<256; iphi++){
	  log<<MSG::DEBUG << pix[ieta][iphi] << endreq;
	  }*/
    
    /* Analysis of cluster content */
    for (int iclus=0; iclus <ncluster; iclus++){
      /*for (int ieta=0; ieta<256; ieta++)
	for (int iphi=0;iphi<256; iphi++)
	  if ( occupied[ieta][iphi]==iclus ){
	    log<<MSG::DEBUG << " Cluster :  " << iclus << " " << ieta << " " << iphi << " " << pix[ieta][iphi] << endreq;
	    }*/
      
      //CaloCluster *myCluster = new CaloCluster();
      xAOD::CaloCluster *myCluster = CaloClusterStoreHelper::makeCluster(cell_Container);
      LArMuClus.push_back(myCluster);
    }  
    
    size_t nfirst = cell_Container->indexFirstCellCalo(subdet);
    size_t nlast = cell_Container->indexLastCellCalo(subdet);
    
    for(;nfirst<=nlast;nfirst++) {
      int index = -1;
      
      if (m_calo_helper->calo_sample((*cell_Container)[nfirst]->ID()) == samp 
	  && (*cell_Container)[nfirst]->energy() >= m_thr2 ){
	if ( eta2neta((*cell_Container)[nfirst]->eta()) < 256 && eta2neta((*cell_Container)[nfirst]->eta()) >= 0 && phi2nphi((*cell_Container)[nfirst]->phi()) < 256 && phi2nphi((*cell_Container)[nfirst]->phi()) >= 0)
	  {
	    index = occupied[eta2neta((*cell_Container)[nfirst]->eta())][phi2nphi((*cell_Container)[nfirst]->phi())];
	  }
	else
	  log<<MSG::DEBUG << " too much cells for the array " << endreq;
	
	if ( index >= 0 ){
	  //LArMuClus[index]->addCell(cell_Container,(*cell_Container)[nfirst],1.);
	  LArMuClus[index]->addCell(nfirst,1.);
	  //log << MSG::DEBUG << "adding cell: " << (*cell_Container)[nfirst]->energy() << " " << m_calo_helper->calo_sample((*cell_Container)[nfirst]->ID()) << endreq;
	}
      }
    }
    
    for (int iclus=0; iclus <ncluster; iclus++){
      //update kine at the end
      if(LArMuClus[iclus]==0) continue; 
      CaloClusterKineHelper::calculateKine(LArMuClus[iclus],false,true);
      clusColl->push_back(LArMuClus[iclus]); 
      /*log << MSG::DEBUG << "CaloCluster E = " << LArMuClus[iclus]->energy() 
	  << " MeV, Et = " << LArMuClus[iclus]->et() 
	  << " MeV, NCells = " << LArMuClus[iclus]->getNumberOfCells() << endreq;*/
    }
  }


  if(m_thr3<1.e6)
    {
      //add strips
      xAOD::CaloClusterContainer::iterator fclu = clusColl->begin();
      xAOD::CaloClusterContainer::iterator lclu = clusColl->end();
      
      for ( ; fclu!=lclu; ++fclu) 
	{
	  //CaloCluster* cluster = (*fclu) ; 
	  //get eta and phi from the cluster and convert them 
	  //float eta_cluster_pos=eta2neta(cluster->eta());
	  //float phi_cluster_pos=phi2nphi(cluster->phi());
	  float eta_cluster_pos=(*fclu)->eta();
	  float phi_cluster_pos=(*fclu)->phi();
	  
	  // loop on cells again
	  size_t mfirst = cell_Container->indexFirstCellCalo(CaloCell_ID::LAREM);
	  size_t mlast = cell_Container->indexLastCellCalo(CaloCell_ID::LAREM);
	  
	  for(;mfirst<=mlast;mfirst++) {	
	    /* This is a strip cell */
	    if ( (m_calo_helper->calo_sample((*cell_Container)[mfirst]->ID()) == CaloCell_ID::EMB1 )
		 ||  (m_calo_helper->calo_sample((*cell_Container)[mfirst]->ID()) == CaloCell_ID::EME1 ) )
	      {
		
		if ( (fabs(((*cell_Container)[mfirst]->eta())-eta_cluster_pos)<m_Deta) && 
		     (fabs(((*cell_Container)[mfirst]->phi())-phi_cluster_pos)<m_Dphi) )
		  {
		    if ( (*cell_Container)[mfirst]->energy() > m_thr3 ) {
		      //(*fclu)->addCell(cell_Container,(*cell_Container)[mfirst],1.);
		      (*fclu)->addCell(cell_Container->findIndex(m_calo_helper->calo_cell_hash(((*cell_Container)[mfirst])->ID())),1.);
                    }
		  }   
	      }
	  }
	}
    }
  // make a Cluster Container 
  // log << MSG::DEBUG << " Finalizing CaloClusterContainer "
  //   << m_clustersOutputName << endreq;
  //sc = m_store_gate->record(clusColl, m_clustersOutputName);
  sc = CaloClusterStoreHelper::finalizeClusters(m_store_gate, clusColl,
					   m_clustersOutputName, log);
  if ( sc.isFailure() || !clusColl) {
    log << MSG::WARNING << "Error finalizing CaloCluster container in TDS "
	<< endreq;
    return StatusCode::SUCCESS;
  }
  
  // log << MSG::DEBUG << "CaloClusterContainer size = " 
  //    << clusColl->size()<< endreq;
  
  sc = m_store_gate->setConst(clusColl);
  if ( sc.isFailure()) {
    log << MSG::WARNING
	<< " Error: collection not properly registered"
	<< endreq;
    return StatusCode::SUCCESS;
  }

  return StatusCode::SUCCESS;
}   

StatusCode LArMuIdAlgorithm::finalize()
{ 
  return StatusCode::SUCCESS;
}

int LArMuIdAlgorithm::eta2neta(float eta){
/* Conversion valid only for middle sampling*/
int ieta;
 if ( eta > 0. )  
   ieta=(int)(eta/0.025)+128;
 else
   ieta=(int)(eta/0.025)+127;

return(ieta);
}

int LArMuIdAlgorithm::phi2nphi(float phi){
/* Conversion valid only for middle sampling*/
int iphi;
 if ( phi >= 0. )
   iphi=(int)(phi/0.02454)+128;
 else 
   iphi=(int)(phi/0.02454)+127;
 
return(iphi);
}

void LArMuIdAlgorithm::do_clustering(float pix[256][256],int occupied[256][256],int ieta,int iphi,int icluster){

  //float THRESHOLD_LOW=50;

  /* This is the main clustering routine */
  
  int next_neighbour;
  int ieta1,iphi1;
 

  occupied[ieta][iphi]=icluster;
  next_neighbour=0; /* Not yet any nearest neighbour found */
  do
    {
      get_next_neighbour(occupied,ieta1,iphi1,ieta,iphi,next_neighbour);

      if ( (pix[ieta1][iphi1] >= m_thr2) && (occupied[ieta1][iphi1] <0) )
	{
	  /* This cell belongs to the cluster */
	  do_clustering(pix,occupied,ieta1,iphi1,icluster);
	}                                                  
    } 
  while (next_neighbour < 8);
  
}

void LArMuIdAlgorithm::get_next_neighbour(int occupied[256][256],int &ieta1, int &iphi1, int ieta, int iphi, int &next_neighbour){

  int neighb_found=0;
  do
    {
      switch(next_neighbour)
	{
	case 0 :
	  {
	    ieta1=ieta-1; iphi1=iphi-1;
	    break;
	  }
	case 1:
	  {
	    ieta1=ieta-1; iphi1=iphi;
	    break;             
	  }
	case 2:
	  {
	    ieta1=ieta-1; iphi1=iphi+1;
	    break;
	  }
	case 3:
	  {
	    ieta1=ieta; iphi1=iphi+1;
	    break;
	  }
	case 4:
	  {
	    ieta1=ieta+1; iphi1=iphi+1;
	    break;
	  }
	case 5:
	  {
	    ieta1=ieta+1; iphi1=iphi;
	    break;
	  }             
	case 6:
	  {
	    ieta1=ieta+1; iphi1=iphi-1;
	    break;
	  }
	case 7:
	  {
	    ieta1=ieta; iphi1=iphi-1;
	    break;
	  }
	}
      /* Protection against wrap-around */
      if (ieta1 < 0) ieta1=0;
      if (ieta1 > 255) ieta1=255;
      if (iphi1 < 0) iphi1=0;
      if (iphi1 > 255) iphi1=255;
      
      (next_neighbour)++;
      
      if (occupied[ieta1][iphi1] < 0) neighb_found=1;    
      
    } while ( (neighb_found == 0) && (next_neighbour < 8)) ;
  return;
}

