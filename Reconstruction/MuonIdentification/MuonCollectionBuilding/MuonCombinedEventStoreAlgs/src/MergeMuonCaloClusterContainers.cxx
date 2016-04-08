/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// MergeCaloClusterContainers.cxx, (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////

#include "CaloEvent/CaloClusterNavigable.h"
#include "CaloEvent/CaloClusterContainer.h"
#include "CaloEvent/CaloCellLinkContainer.h"

//this
#include "MergeMuonCaloClusterContainers.h"

#include "muonEvent/MuonContainer.h"
#include "muonEvent/Muon.h"

/////////////////////////////////////////////////////////////////
// Constructor
/////////////////////////////////////////////////////////////////

Rec :: MergeMuonCaloClusterContainers :: MergeMuonCaloClusterContainers(const std::string& name, ISvcLocator* pSvcLocator) : 
  AthAlgorithm(name, pSvcLocator),
  m_MuonCaloClusterContainerTarget("MuonClusterCollection"),
  m_MuonCaloClusterContainerSource("MuonClusterCollection_NEW"),
  m_NewMuonCollection(1, std::string("StacoMuonCollection"))
{
  declareProperty("MuonCaloClusterContainerTarget", m_MuonCaloClusterContainerTarget);
  declareProperty("MuonCaloClusterContainerSource", m_MuonCaloClusterContainerSource);
  declareProperty("NewMuonCollection", m_NewMuonCollection);
}
	
/////////////////////////////////////////////////////////////////
//Initialize
/////////////////////////////////////////////////////////////////

StatusCode Rec::MergeMuonCaloClusterContainers::initialize() {

  ATH_MSG_INFO ("Initialize done in " << name());
  return StatusCode::SUCCESS;
}
	
/////////////////////////////////////////////////////////////////
// Finalize
/////////////////////////////////////////////////////////////////

StatusCode Rec::MergeMuonCaloClusterContainers::finalize() {
  return StatusCode::SUCCESS;
}


/////////////////////////////////////////////////////////////////
// Execute
/////////////////////////////////////////////////////////////////

StatusCode Rec::MergeMuonCaloClusterContainers::execute() {

  std::vector<Analysis::MuonContainer *> muonCollection(m_NewMuonCollection.size());
  std::vector<std::vector<const CaloCluster *> >clusters_in_muon(m_NewMuonCollection.size());
	
  for(unsigned int i=0; i<m_NewMuonCollection.size(); i++) 
    {
      const Analysis::MuonContainer * muonCollection_const(NULL);
      if(!evtStore()->retrieve(muonCollection_const, m_NewMuonCollection[i]).isSuccess())
	{
	  ATH_MSG_FATAL ("Cannot retrieve " << m_NewMuonCollection[i]);
	  return StatusCode::FAILURE;
	}
      muonCollection[i] = const_cast<Analysis::MuonContainer *>(muonCollection_const);
		
      for(Analysis::MuonContainer :: const_iterator it=muonCollection_const->begin(); it!=muonCollection_const->end(); it++)
	{
	  clusters_in_muon[i].push_back((*it)->cluster());
	}
    }
  
  CaloClusterContainer *muonCaloClusterContainerSource(NULL), *muonCaloClusterContainerTarget(NULL);
  CaloCellLinkContainer *muonCaloCellLinkContainerSource(NULL), *muonCaloCellLinkContainerTarget(NULL);
  if (!retrieveCollections(muonCaloClusterContainerTarget, muonCaloCellLinkContainerTarget, m_MuonCaloClusterContainerTarget))
    {
      return StatusCode::FAILURE;
    }

  if (!retrieveCollections(muonCaloClusterContainerSource, muonCaloCellLinkContainerSource, m_MuonCaloClusterContainerSource))
    {
      return StatusCode::FAILURE;
    }

  for(unsigned int i=0; i<muonCaloCellLinkContainerSource->size(); i++)
    {
      CaloCellLink *link;
      muonCaloCellLinkContainerSource->swapElement(i, NULL, link);
      muonCaloCellLinkContainerTarget->push_back(link);
    }

  for(unsigned int i=0; i<muonCaloClusterContainerSource->size(); i++)
    {
//  CaloCluster *cluster;
//  muonCaloClusterContainerSource->swapElement(i, NULL, cluster);
      muonCaloClusterContainerTarget->push_back( (*muonCaloClusterContainerSource)[i] );
      ElementLink<CaloCellLinkContainer> el =
        (*muonCaloClusterContainerSource)[i]->cellLink();
      el.setStorableObject( *muonCaloCellLinkContainerTarget, true );
      (*muonCaloClusterContainerSource)[i]->resetCellLink (el);
    }

/*	std::cout<<"Own policies:"<<std::endl;
	std::cout<<"muonCaloClusterContainerSource: "<<muonCaloClusterContainerSource->ownPolicy()<<std::endl;
	std::cout<<"muonCaloClusterContainerTarget: "<<muonCaloClusterContainerTarget->ownPolicy()<<std::endl;
	std::cout<<"muonCaloCellLinkContainerSource: "<<muonCaloCellLinkContainerSource->ownPolicy()<<std::endl;
	std::cout<<"muonCaloCellLinkContainerTarget: "<<muonCaloCellLinkContainerTarget->ownPolicy()<<std::endl;*/
  for(unsigned int i=0; i<muonCollection.size(); i++)
    {
      unsigned int count(0);
      for(Analysis::MuonContainer :: iterator it=muonCollection[i]->begin(); it!=muonCollection[i]->end(); it++)
	{
	  Analysis::Muon *muon(*it);
	  bool had_good_cluster_link(true);
	  if (!muon->clusterLink().isValid())
	    {
	      ATH_MSG_INFO ("Muon comes with invalid cluster link!");
	      had_good_cluster_link=false;
	    }
	  if (clusters_in_muon[i][count])
	    {
              ElementLink<CaloClusterContainer> el = muon->clusterLink();
              el.setStorableObject(* muonCaloClusterContainerTarget, true);
	      muon->set_cluster (el);
	    }
	  if(!muon->clusterLink().isValid() && had_good_cluster_link)	
	    {
	      ATH_MSG_WARNING ("Cluster link is broken!");
	    }
			
	  count++;
	}
    }
  return StatusCode::SUCCESS;
}
	
	
inline bool Rec::MergeMuonCaloClusterContainers::retrieveCollections(CaloClusterContainer *& cluster_collection, CaloCellLinkContainer *& cell_link_collection, const std::string & container_name)
{
  const CaloClusterContainer *cluster_collection_const(NULL);
  if(!evtStore()->retrieve(cluster_collection_const, container_name))
    {
      ATH_MSG_FATAL ("Cannot retrieve " << container_name);
      return false;
    }
  cluster_collection=const_cast<CaloClusterContainer *>(cluster_collection_const);
  std::string name=container_name+"_Link";
  const CaloCellLinkContainer *cell_link_collection_const;
  if(!evtStore()->retrieve(cell_link_collection_const, name))
    {
      ATH_MSG_FATAL ("Cannot retrieve " << name);
      return false;
    }
  cell_link_collection=const_cast<CaloCellLinkContainer *>(cell_link_collection_const);
  return true;
}
