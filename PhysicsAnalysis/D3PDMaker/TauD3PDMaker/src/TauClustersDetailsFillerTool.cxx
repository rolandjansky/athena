/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id$
/**
 * @file TauD3PDMaker/src/TauCellsDetailsFillerTool.cxx
 * @author Dugan O'Neil <doneil@sfu.ca>
 * @date December, 2009
 * @brief Block filler tool for tau track details.
 */

#include "TauClustersDetailsFillerTool.h"
#include "tauEvent/TauJet.h"
#include "tauEvent/TauJetContainer.h"
#include "AthenaKernel/errorcheck.h"
#include "CaloEvent/CaloClusterContainer.h"
#include "CaloEvent/CaloCluster.h"
#include "AnalysisUtils/AnalysisMisc.h"
#include "VxVertex/VxContainer.h"
#include "CLHEP/Geometry/Vector3D.h"
//#include "CaloUtils/CaloClusterVertexCorrection.h"


namespace D3PD {
    
    
/**
 * @brief Standard Gaudi tool constructor.
 * @param type The name of the tool type.
 * @param name The tool name.
 * @param parent The tool's Gaudi parent.
 */
    TauClustersDetailsFillerTool::TauClustersDetailsFillerTool (const std::string& type,
								const std::string& name,
								const IInterface* parent)
	: BlockFillerTool<Analysis::TauJet> (type, name, parent),
	  m_clusterenergy(0), m_clustereta(0), m_clusterphi(0), m_clustereta_atTJVA(0), m_clusterphi_atTJVA(0), m_clusterPSSF(0), m_clusterEM2F(0), m_clusterEM3F(0)
    {
      book().ignore(); // Avoid coverity warnings.
    }
    
    
/**
 * @brief Book variables for this block.
 */
    StatusCode TauClustersDetailsFillerTool::book()
    {
	CHECK( addVariable ("cluster_E",  m_clusterenergy )  );
	CHECK( addVariable ("cluster_eta",  m_clustereta )  );
	CHECK( addVariable ("cluster_phi",  m_clusterphi )  );
	CHECK( addVariable ("cluster_eta_atTJVA",  m_clustereta_atTJVA )  );
	CHECK( addVariable ("cluster_phi_atTJVA",  m_clusterphi_atTJVA )  );
	CHECK( addVariable ("cluster_PreSamplerStripF", m_clusterPSSF) );
	CHECK( addVariable ("cluster_EMLayer2F", m_clusterEM2F) );
	CHECK( addVariable ("cluster_EMLayer3F", m_clusterEM3F) );
	CHECK( addVariable ("cluster_n",  m_numclusters )  );
	
	
	return StatusCode::SUCCESS;
    }
    
    
/**
 * @brief Fill one block --- type-safe version.
 * @param p The input object.
 *
 * This is called once per object.  The caller
 * is responsible for arranging that all the pointers for booked variables
 * are set appropriately upon entry.
 */
    StatusCode TauClustersDetailsFillerTool::fill (const Analysis::TauJet& p)
    {
	NavigationToken<CaloCluster> nt;
	std::vector<const CaloCluster*> Clusters;
	// loop over all clusters of the tau (placed there by the tauSeedBuilder)
	p.fillToken( nt ); 
	NavigationToken<CaloCluster>::const_iterator firstcluster = nt.begin();
	NavigationToken<CaloCluster>::const_iterator lastcluster = nt.end();
	unsigned int num_clusters = 0;
	for ( ; firstcluster != lastcluster; firstcluster++ )
	{
	    ++num_clusters;
	    Clusters.push_back(*firstcluster);
	}
	*m_numclusters = num_clusters;
	
	// now insert clusters into event ordered by energy
	// this makes it much faster to recalculate cluster-based
	// variables in macros later
	
	if(Clusters.size() > 0)
	{
	    //Retrieve TJVA vertex
	    const Trk::RecVertex* myVtx = p.origin();
            Amg::Vector3D TauJetVertex;
	    if (myVtx!=0)
	    {
                const Amg::Vector3D& pos = myVtx->position();
		float vtx_x = pos.x();
		float vtx_y = pos.y();
		float vtx_z = pos.z();
		TauJetVertex = Amg::Vector3D(vtx_x, vtx_y, vtx_z);
	    }
	    
	    AnalysisUtils::Sort::e (&Clusters);
	    
	    for (unsigned int i=0; i<num_clusters; ++i )
	    {
		// Simplified Sampling information
		float PreSampler = Clusters[i]->eSample(CaloSampling::PreSamplerB) + Clusters[i]->eSample(CaloSampling::PreSamplerE);
		float EMLayer1   = Clusters[i]->eSample(CaloSampling::EMB1)        + Clusters[i]->eSample(CaloSampling::EME1);
		float EMLayer2   = Clusters[i]->eSample(CaloSampling::EMB2)        + Clusters[i]->eSample(CaloSampling::EME2);
		float EMLayer3   = Clusters[i]->eSample(CaloSampling::EMB3)        + Clusters[i]->eSample(CaloSampling::EME3);
		
		float Energy   = Clusters[i]->getBasicEnergy();
		
		float PSSF = (PreSampler + EMLayer1)/Energy;
		float EM2F = EMLayer2/Energy;
		float EM3F = EMLayer3/Energy;
		
		if(PSSF < 0.) PSSF = 0.;
		if(PSSF > 1.) PSSF = 1.;
		
		if(EM2F < 0.) EM2F = 0.;
		if(EM2F > 1.) EM2F = 1.;
		
		if(EM3F < 0.) EM3F = 0.;
		if(EM3F > 1.) EM3F = 1.;
		
		m_clusterenergy->push_back(Clusters[i]->e());
		m_clustereta->push_back(Clusters[i]->eta());
		m_clusterphi->push_back(Clusters[i]->phi());
		m_clusterPSSF->push_back(PSSF);
		m_clusterEM2F->push_back(EM2F);
		m_clusterEM3F->push_back(EM3F);
		
		// Corrected cluster direction information
                // modified by FF, April 2014
                ATH_MSG_WARNING("VertexCorrection not supported anymore! need to migrate to xAOD and use CaloVertexedCluster as done in tauRec");
		/*
                CaloClusterVertexCorrection::setVertex(Clusters[i], &TauJetVertex);
		m_clustereta_atTJVA->push_back(Clusters[i]->eta());
		m_clusterphi_atTJVA->push_back(Clusters[i]->phi());
		
		// Reset Cluster vertex
		CaloClusterVertexCorrection::resetVertex(Clusters[i]);
                */
	    }
	}
	return StatusCode::SUCCESS;
    } // end fill method
    
} // namespace D3PD
