/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/********************************************************************
    
    NAME:     LArTBClusterBuilder.cxx
    PACKAGE:  offline/LArCalorimeter/LArClusterRec
    
    AUTHORS:  Mohamed AHARROUCHE

    CREATED:  Jul. 10, 2004
    MODIFIED: C. Santamarina (Cibran.Santamarina.Rios@cern.ch)
              Use simultaneously Barrel and End-Cap (for Trigger)

    PURPOSE:  Build (for TB) a cluster 3x3 around a seed cell 
              in the middle.
                
********************************************************************/

// Include files

#include "LArClusterRec/LArTBClusterBuilder.h"
#include "CaloEvent/CaloClusterContainer.h"
#include "xAODCaloEvent/CaloCluster.h"
#include "xAODCaloEvent/CaloClusterKineHelper.h"
#include "CaloEvent/CaloCellContainer.h"
#include "CaloUtils/CaloClusterStoreHelper.h"
#include "StoreGate/StoreGateSvc.h"
#include "GaudiKernel/Property.h"
#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/ToolFactory.h"

LArTBClusterBuilder::LArTBClusterBuilder(const std::string& type, 
					 const std::string& name,
					 const IInterface* parent):
  AthAlgTool(type, name, parent),
  m_storeGateSvc(0),
  m_eta_size(3.),
  m_deta(0),
  m_phi_size(3.),
  m_dphi(0),
  m_nbcluster(1),
  m_layere(),
  m_layereta(),
  m_layerphi(),
  m_caloRegion("BARREL")
{
  declareProperty("CellContainers",m_cellContainerName);
  declareProperty("EtaSize",m_eta_size);
  declareProperty("PhiSize", m_phi_size);
  declareProperty("EnergyThreshold", m_ethreshold);
  declareProperty("ClusterNbr", m_nbcluster);
  declareProperty("LArCaloRegion", m_caloRegion);
}

LArTBClusterBuilder::~LArTBClusterBuilder() 
{}

StatusCode LArTBClusterBuilder::initialize()
{
  StatusCode sc;
  
  // Use the message service
  MsgStream log(msgSvc(), name());
  log << MSG::INFO << ">>> Initialize" << endreq;
    
  // StoreGate service
  sc = service("StoreGateSvc", m_storeGateSvc);
  if (sc.isFailure()) 
    {
      log << MSG::FATAL << " StoreGate service not found " << std::endl;
      sc = StatusCode::FAILURE; 
      return sc;
    }

  //eta & phi size
  m_deta = (m_eta_size*0.025)/2.;
  m_dphi = (m_phi_size*0.0245436926)/2.;
      

  return StatusCode::SUCCESS;
}

StatusCode LArTBClusterBuilder::execute(xAOD::CaloClusterContainer* clusColl)
{
  MsgStream log(msgSvc(), name());

  StatusCode sc;

  xAOD::CaloClusterContainer& larClusterContainer = *clusColl;
  
  const CaloCellContainer *caloCellContainer;

  sc = m_storeGateSvc->retrieve(caloCellContainer, m_cellContainerName) ; 
  if(sc!=StatusCode::SUCCESS)
    {
      log<< MSG::ERROR
	 << "could not get caloCellContainer with name "
	 << m_cellContainerName
	 << endreq;
      return StatusCode::RECOVERABLE;
    }

  CaloCellContainer::const_iterator itrCell=caloCellContainer->beginConstCalo( CaloCell_ID::LAREM);

  if (caloCellContainer->size()==0){
    log << MSG::ERROR << " Empty CaloCellContainer " 
	<< endreq ;
  }
  
  for( ; itrCell!=caloCellContainer->endConstCalo( CaloCell_ID::LAREM); ++itrCell)
    {
      const CaloCell* theCell = (*itrCell);
      CaloSampling::CaloSample sample=(CaloSampling::CaloSample)theCell->caloDDE()->getSampling();
      m_cell_list.push_back(theCell);
      if(sample==CaloSampling::EMB2 || sample==CaloSampling::EME2) m_middle_cell_list.push_back(theCell);
    }
  

   //
  ene_sup tri;
  m_cell_list.sort(tri);
  m_middle_cell_list.sort(tri);
  

  for(int j=0;j<m_nbcluster;j++)
    {      
      CellList::iterator it_cell = m_cell_list.begin();
      CellList::iterator it_middle_cell = m_middle_cell_list.begin();
            
      double eta_clus = 0.;
      double phi_clus = 0.;
      bool seed = true;
      xAOD::CaloCluster* cluster = 0;
      for(unsigned int k=0;k<8;k++)
	{
	  m_layere[k]=0.;
	  m_layereta[k]=0.;
	  m_layerphi[k]=0.;
	}
      for(;it_cell!= m_cell_list.end(); )
	{ 
	  if(seed)
	    {//seed cell position
	      eta_clus = (*it_middle_cell)->eta();
	      phi_clus = (*it_middle_cell)->phi();
	      cluster = CaloClusterStoreHelper::makeCluster(caloCellContainer,eta_clus,phi_clus, xAOD::CaloCluster::CSize_Unknown);
	      seed = false;
	    }
	  
	  double eta = (*it_cell)->eta();
	  double phi = (*it_cell)->phi();
	  double ene = (*it_cell)->energy();
	  int isampling = (*it_cell)->caloDDE()->getSampling();
	  
	  //select the cells into our cluster
	  if( fabs(eta - eta_clus) <= m_deta + 0.001)
	    {
	      if (fabs(phi-phi_clus) <= m_dphi
		  || ((isampling==CaloSampling::PreSamplerB || isampling==CaloSampling::PreSamplerE) && fabs(phi-phi_clus)<=0.062501 )
		  || ((isampling==CaloSampling::EMB1 || isampling==CaloSampling::EME1) && fabs(phi-phi_clus)<=0.062501))
		{
		  if(ene!=-999.&& ene > m_ethreshold)
		    {
		      size_t cell_idx=caloCellContainer->findIndex((*it_cell)->caloDDE()->calo_hash());
		      if (cell_idx>0) {
			cluster->addCell(cell_idx,1.0);
			// This could produce an error anyhow if the ENDCAP is included... the arrays are redimensioned to 8...
			m_layere[isampling] += ene;
			m_layereta[isampling] += ene*eta;
			m_layerphi[isampling] += ene*phi;
			it_cell=m_cell_list.erase(it_cell); // Removes the cells which are used for the clustering... keeping the others in the container for further clusters...
			if(isampling==CaloSampling::EMB2 || isampling==CaloSampling::EME2)
			  it_middle_cell=m_middle_cell_list.erase(it_middle_cell);
		      }
		    }
		  else{
		    ++it_cell;
		    if(isampling==CaloSampling::EMB2 || isampling==CaloSampling::EME2) ++it_middle_cell;
		  }
		}
	      else{
		++it_cell;
		if(isampling==CaloSampling::EMB2 || isampling==CaloSampling::EME2) ++it_middle_cell;
	      }
	    }
	  else{
	    ++it_cell;
	    if(isampling==CaloSampling::EMB2 || isampling==CaloSampling::EME2) ++it_middle_cell;
	  }
	}
      
      //DR if no cluster so far quit
      if (cluster==0) break;
      CaloClusterKineHelper::calculateKine(cluster,false,true);

      larClusterContainer.push_back(cluster);
      
      //now put data in TDS
      //for (int iSample=m_layer0; iSample <= m_layer3; ++iSample) 
      for (int iSample=CaloSampling::PreSamplerB; iSample <=CaloSampling::EME3 ; ++iSample) 
	{
	  CaloSampling::CaloSample sampling=static_cast<xAOD::CaloCluster::CaloSample>(iSample);
	  
	  if(m_layere[sampling]>0.){
	    m_layereta[sampling]= m_layereta[sampling]/m_layere[sampling];
	    m_layerphi[sampling]= m_layerphi[sampling]/m_layere[sampling];
	  }
	  else
	    {
	      m_layereta[sampling]=-999.;
	      m_layerphi[sampling]=-999.;
	    }
	  
 	  cluster->setEnergy(sampling, m_layere[sampling]);
	  cluster->setEta(sampling, m_layereta[sampling]);
	  cluster->setPhi(sampling, m_layerphi[sampling]);
	}
    }
  m_cell_list.clear();
  m_middle_cell_list.clear();

  return StatusCode::SUCCESS;
}
