/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
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

LArTBClusterBuilder::LArTBClusterBuilder(const std::string& type, 
					 const std::string& name,
					 const IInterface* parent):
  AthAlgTool(type, name, parent),
  m_deta(0),
  m_dphi(0)
{
  declareProperty("CellContainers",m_cellContainerName);
  declareProperty("EtaSize",m_eta_size = 3.0);
  declareProperty("PhiSize", m_phi_size = 3.0);
  declareProperty("EnergyThreshold", m_ethreshold = 0.0);
  declareProperty("ClusterNbr", m_nbcluster = 1);
  declareProperty("LArCaloRegion", m_caloRegion = "BARREL");
}

LArTBClusterBuilder::~LArTBClusterBuilder() 
{}

StatusCode LArTBClusterBuilder::initialize()
{
  ATH_MSG_INFO( ">>> Initialize"  );
    
  //eta & phi size
  m_deta = (m_eta_size*0.025)/2.;
  m_dphi = (m_phi_size*0.0245436926)/2.;
      

  return StatusCode::SUCCESS;
}

StatusCode
LArTBClusterBuilder::execute(const EventContext& /*ctx*/,
                             xAOD::CaloClusterContainer* clusColl) const
{
  xAOD::CaloClusterContainer& larClusterContainer = *clusColl;
  
  const CaloCellContainer *caloCellContainer = nullptr;
  ATH_CHECK( evtStore()->retrieve(caloCellContainer, m_cellContainerName) );

  CaloCellContainer::const_iterator itrCell=caloCellContainer->beginConstCalo( CaloCell_ID::LAREM);

  if (caloCellContainer->size()==0){
    ATH_MSG_ERROR( " Empty CaloCellContainer "  );
  }

  CellList cell_list;
  CellList middle_cell_list;
  for( ; itrCell!=caloCellContainer->endConstCalo( CaloCell_ID::LAREM); ++itrCell)
    {
      const CaloCell* theCell = (*itrCell);
      CaloSampling::CaloSample sample=(CaloSampling::CaloSample)theCell->caloDDE()->getSampling();
      cell_list.push_back(theCell);
      if(sample==CaloSampling::EMB2 || sample==CaloSampling::EME2) middle_cell_list.push_back(theCell);
    }
  

   //
  ene_sup tri;
  cell_list.sort(tri);
  middle_cell_list.sort(tri);
  

  for(int j=0;j<m_nbcluster;j++)
    {      
      CellList::iterator it_cell = cell_list.begin();
      CellList::iterator it_middle_cell = middle_cell_list.begin();
            
      double layere[8] = {0};
      double layereta[8]= {0};
      double layerphi[8]= {0};  

      double eta_clus = 0.;
      double phi_clus = 0.;
      bool seed = true;
      xAOD::CaloCluster* cluster = 0;
      for(;it_cell!= cell_list.end(); )
	{ 
	  if(seed)
	    {//seed cell position
	      eta_clus = (*it_middle_cell)->eta();
	      phi_clus = (*it_middle_cell)->phi();
	      cluster = larClusterContainer.push_back(CaloClusterStoreHelper::makeCluster(caloCellContainer,eta_clus,phi_clus, xAOD::CaloCluster::CSize_Unknown));
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
			layere[isampling] += ene;
			layereta[isampling] += ene*eta;
			layerphi[isampling] += ene*phi;
			it_cell=cell_list.erase(it_cell); // Removes the cells which are used for the clustering... keeping the others in the container for further clusters...
			if(isampling==CaloSampling::EMB2 || isampling==CaloSampling::EME2)
			  it_middle_cell=middle_cell_list.erase(it_middle_cell);
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

      
      //now put data in TDS
      //for (int iSample=m_layer0; iSample <= m_layer3; ++iSample) 
      for (int iSample=CaloSampling::PreSamplerB; iSample <=CaloSampling::EME3 ; ++iSample) 
	{
	  CaloSampling::CaloSample sampling=static_cast<xAOD::CaloCluster::CaloSample>(iSample);
	  
	  if(layere[sampling]>0.){
	    layereta[sampling]= layereta[sampling]/layere[sampling];
	    layerphi[sampling]= layerphi[sampling]/layere[sampling];
	  }
	  else
	    {
	      layereta[sampling]=-999.;
	      layerphi[sampling]=-999.;
	    }
	  
 	  cluster->setEnergy(sampling, layere[sampling]);
	  cluster->setEta(sampling, layereta[sampling]);
	  cluster->setPhi(sampling, layerphi[sampling]);
	}
    }

  return StatusCode::SUCCESS;
}
