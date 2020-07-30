/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#include "HIClusterSubtraction.h"
#include "xAODHIEvent/HIEventShapeContainer.h"
#include "xAODCaloEvent/CaloClusterContainer.h"
#include "HIEventUtils/HIEventShapeMap.h"
#include "CaloUtils/CaloClusterStoreHelper.h"
#include "HIJetRec/HIJetRecDefs.h"
#include "xAODBase/IParticleHelpers.h"
#include "xAODTracking/Vertex.h"
#include "xAODTracking/VertexContainer.h"
#include "xAODCaloEvent/CaloClusterAuxContainer.h"
#include "HIEventUtils/HIEventShapeMapTool.h"

#include "StoreGate/ReadHandle.h"
#include "StoreGate/WriteHandle.h"

//**********************************************************************

HIClusterSubtraction::HIClusterSubtraction(std::string name) : asg::AsgTool(name)//,
{
}

//**********************************************************************

StatusCode HIClusterSubtraction::initialize()
{
	//Keys initialization
	ATH_CHECK( m_eventShapeKey.initialize() );
	ATH_CHECK( m_inClusterKey.initialize() );
	ATH_CHECK( m_outClusterKey.initialize() );
	//Vertex container needs to be initialized only if origin correction will take place
	ATH_CHECK( m_vertexContainer.initialize( m_originCorrection ) );

  for (auto tool :  m_clusterCorrectionTools)
  {
    StatusCode sc = tool.retrieve();
    if (sc.isFailure()) ATH_MSG_ERROR("Failed to retrieve correction tool " << tool);
    else ATH_MSG_DEBUG("Successfully retrieved correction tool " << tool);
  }
  return StatusCode::SUCCESS;
}

bool HIClusterSubtraction::doOriginCorrection( xAOD::CaloCluster* cl, const xAOD::Vertex* origin, xAOD::IParticle::FourMom_t& p4_cl ) const{
	//made boolean to return what was "missingMoment" in HIJetConstituentSubtractionTool
	bool missingMoment = false;
	float mag = 0;
	if(cl->isAvailable<float>("HIMag")) mag=cl->auxdataConst<float>("HIMag");
	else
	{
		double cm_mag=0;
		if(cl->retrieveMoment (xAOD::CaloCluster::CENTER_MAG, cm_mag)) mag=cm_mag;
	}
	if(mag!=0.)
	{
		float eta0=cl->eta0();
		float phi0=cl->phi0();
		float radius=mag/std::cosh(eta0);
		xAOD::IParticle::FourMom_t p4_pos;
		p4_pos.SetX(radius*std::cos(phi0)-origin->x());
		p4_pos.SetY(radius*std::sin(phi0)-origin->y());
		p4_pos.SetZ(radius*std::sinh(eta0)-origin->z());

		double deta=p4_pos.Eta()-eta0;
		double dphi=p4_pos.Phi()-phi0;
		//adjust in case eta/phi are flipped in case of neg E clusters
		//this method is agnostic wrt convention
		if(p4_cl.Eta()*eta0 <0.) deta*=-1;

		double eta_prime=p4_cl.Eta()+deta;
		double phi_prime=p4_cl.Phi()+dphi;
		double e_subtr=p4_cl.E();
		p4_cl.SetPtEtaPhiE(e_subtr/std::cosh(eta_prime),eta_prime,phi_prime,e_subtr);
	}
	else missingMoment=true;

	return missingMoment;
}

int HIClusterSubtraction::execute() const
{
  //const jet::cellset_t & badcells = badCellMap.cells() ;
  //retrieve UE
	const xAOD::HIEventShapeContainer* shape = 0;
	SG::ReadHandle<xAOD::HIEventShapeContainer>  readHandleEvtShape ( m_eventShapeKey );
  shape = readHandleEvtShape.cptr();
  const HIEventShapeIndex* es_index = m_eventShapeMapTool->getIndexFromShape( shape );
	if(es_index == nullptr) ATH_MSG_FATAL("The HIEventShapeMapTool returned a null pointer. Binning scheme not coherent");
  const xAOD::HIEventShape* eshape = nullptr;
  CHECK(m_modulatorTool->getShape(eshape), 1);

  //New implementation: make a deep copy of original HIClusters and apply subtraction to clusters in the new container
	SG::ReadHandle<xAOD::CaloClusterContainer>  readHandleClusters ( m_inClusterKey );
  // Now a handle to write the deep Copy
  SG::WriteHandle<xAOD::CaloClusterContainer> writeHandleDeepCopyClusters ( m_outClusterKey );
  // Preparing keys and container to perfrom the origin correction
	const xAOD::Vertex* primVertex=nullptr;
	const xAOD::VertexContainer* vertices=nullptr;
  // Boolean to flag that at least a vertex is present in the vertex container
	bool isOriginPossible = true;
	// Finding the primary vertex in case origin correction has to be performed
	if(m_originCorrection)
  {
		// ReadHandle to retrieve the vertex container
		SG::ReadHandle<xAOD::VertexContainer>  readHandleVertexContainer ( m_vertexContainer );
    vertices = readHandleVertexContainer.get();
    for ( size_t iVertex = 0; iVertex < vertices->size(); ++iVertex )
    {
      if(vertices->at(iVertex)->vertexType() == xAOD::VxType::PriVtx)
      {
        	primVertex=vertices->at(iVertex);
        	break;
      }
    }
    if(!primVertex && vertices->size() > 0)
    {
      ATH_MSG_WARNING("No primary vertices found, using first in container");
      primVertex=vertices->at(0);
    }
		if(!primVertex && vertices->size() == 0)
    {
      ATH_MSG_WARNING("No primary vertices found, and vertex container empty. Abortin Origin correction for this event.");
      isOriginPossible = false;
		}
  }
	bool missingMoment=false;

	auto originalCluster = readHandleClusters.cptr();
	// Create the new container and its auxiliary store.
	xAOD::CaloClusterContainer* copyClusters = new xAOD::CaloClusterContainer();
  xAOD::AuxContainerBase* copyClustersAux = new xAOD::AuxContainerBase();
  copyClusters->setStore(copyClustersAux);
  copyClusters->reserve (originalCluster->size());

	for (const xAOD::CaloCluster* oldCluster : *originalCluster) {
	     xAOD::CaloCluster* newClu=new xAOD::CaloCluster();
	     copyClusters->push_back (newClu);
	     *newClu=*oldCluster;
	}

  for(xAOD::CaloClusterContainer::iterator itr=copyClusters->begin(); itr!=copyClusters->end(); itr++)
  {
    xAOD::CaloCluster* cl= *itr;
		xAOD::IParticle::FourMom_t p4;
		//Unsubtracted state record done by the subtractor tool functions.
    if(m_setMoments)
		{
			//This flag is set to false for HIJetClustersSubtractorTool and true for HIJetCellSubtractorTool,
			// but for the second we don't do origin correction. In principle the code is structured to do the same as the
			//else for m_setMoments=true and HIJetClustersSubtractorTool, therefore we add the code for origin correction also here
			m_subtractorTool->subtractWithMoments(cl, shape, es_index, m_modulatorTool, eshape);
			if(isOriginPossible && m_originCorrection)
			{
				missingMoment = HIClusterSubtraction::doOriginCorrection( cl, primVertex, p4 );
				HIJetRec::setClusterP4(p4,cl,HIJetRec::subtractedOriginCorrectedClusterState());
			}
		}
    else
    {
			m_subtractorTool->subtract(p4,cl,shape,es_index,m_modulatorTool,eshape);
			HIJetRec::setClusterP4(p4,cl,HIJetRec::subtractedClusterState());

			if(isOriginPossible)
			{
				ATH_MSG_DEBUG("Applying origin correction"
							<< std::setw(12) << "Before:"
							<< std::setw(10) << std::setprecision(3) << p4.Pt()*1e-3
							<< std::setw(10) << std::setprecision(3) << p4.Eta()
							<< std::setw(10) << std::setprecision(3) << p4.Phi()
							<< std::setw(10) << std::setprecision(3) << p4.E()*1e-3
							<< std::setw(10) << std::setprecision(3) << p4.M()*1e-3);

				missingMoment = HIClusterSubtraction::doOriginCorrection( cl, primVertex, p4 );
				HIJetRec::setClusterP4(p4,cl,HIJetRec::subtractedOriginCorrectedClusterState());

				ATH_MSG_DEBUG("Applying origin correction"
							<< std::setw(12) << "After:"
							<< std::setw(10) << std::setprecision(3) << p4.Pt()*1e-3
							<< std::setw(10) << std::setprecision(3) << p4.Eta()
							<< std::setw(10) << std::setprecision(3) << p4.Phi()
							<< std::setw(10) << std::setprecision(3) << p4.E()*1e-3
							<< std::setw(10) << std::setprecision(3) << p4.M()*1e-3);
			}
		}
  }//End of iterator over CaloClusterContainer

    for(ToolHandleArray<CaloClusterCollectionProcessor>::const_iterator toolIt=m_clusterCorrectionTools.begin();
	      toolIt != m_clusterCorrectionTools.end(); toolIt++)
    {
      ATH_MSG_DEBUG(" Applying correction = " << (*toolIt)->name() );
			CHECK((*toolIt)->execute(Gaudi::Hive::currentContext(), copyClusters), 1);
    }//End loop over correction tools

		if(missingMoment) ATH_MSG_WARNING("No origin correction applied, CENTERMAG missing");

  // Make sure that memory is managed safely
  std::unique_ptr<xAOD::CaloClusterContainer> outClusters(copyClusters);
  std::unique_ptr<xAOD::AuxContainerBase> deepAux(copyClustersAux);

	if(writeHandleDeepCopyClusters.record ( std::move(outClusters), std::move(deepAux)).isFailure() ){
			ATH_MSG_ERROR("Unable to write DeepCopy Copy containers for subtracted clusters with key: " << m_outClusterKey.key());
			return 1;
	}
  return 0;
}
