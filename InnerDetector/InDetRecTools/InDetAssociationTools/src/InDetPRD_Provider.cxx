/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// InDetPRD_Provider.cxx, (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////

#include "InDetAssociationTools/InDetPRD_Provider.h"
#include "InDetPrepRawData/PixelCluster.h"
#include "InDetPrepRawData/SCT_Cluster.h"
#include "InDetPrepRawData/TRT_DriftCircle.h"

// AtlasDetDescr  
#include "AtlasDetDescr/AtlasDetectorID.h"
// InDetIdentifier
#include "InDetIdentifier/PixelID.h"
#include "InDetIdentifier/SCT_ID.h"
#include "InDetIdentifier/TRT_ID.h"


InDet::InDetPRD_Provider::InDetPRD_Provider(const std::string& t, const std::string& n, const IInterface* p):
  AthAlgTool(t,n,p),
  m_idHelper(0),
  m_pixIdHelper(0),
  m_pixClusterContainerName(""),
  m_pixClusterContainer(0),  
  m_sctIdHelper(0),
  m_sctClusterContainerName(""),
  m_sctClusterContainer(0), 
  m_trtIdHelper(0),
  m_trtDriftCircleContainerName(""),
  m_trtDriftCircleContainer(0)
{
    declareInterface<Trk::IPRD_Provider>(this);
    // PRD container name
    declareProperty("PixelClusterContainer",         m_pixClusterContainerName);
    declareProperty("SCT_ClusterContainer",          m_sctClusterContainerName);
    declareProperty("TRT_DriftCircleContainer",      m_trtDriftCircleContainerName);
}


StatusCode InDet::InDetPRD_Provider::initialize()
{
     ATH_MSG_VERBOSE("Initializing ...");           
     // Set up ATLAS ID helper to be able to identify the PRD's det-subsystem
     if (detStore()->retrieve(m_idHelper, "AtlasID").isFailure()) {
         ATH_MSG_ERROR ("Could not get AtlasDetectorID helper. Aborting ...");
         return StatusCode::FAILURE;
     }     
     // Get Pixel helpers                                                                                                                         
     if (detStore()->retrieve(m_pixIdHelper, "PixelID").isFailure()) { 
         ATH_MSG_ERROR ("Could not get PixelID helper. Aborting ...");
        return StatusCode::FAILURE; 
     }     
     // Get SCT helpers                                                                                                                         
     if (detStore()->retrieve(m_sctIdHelper, "SCT_ID").isFailure()) { 
         ATH_MSG_ERROR ("Could not get SCT_ID helper. Aborting ...");
        return StatusCode::FAILURE; 
     }     
     // Get TRT helpers
     if (detStore()->retrieve(m_trtIdHelper, "TRT_ID").isFailure()) { 
         ATH_MSG_ERROR ("Could not get TRT_ID helper. Aborting ...");
        return StatusCode::FAILURE; 
     }     
     return StatusCode::SUCCESS;
}           

StatusCode InDet::InDetPRD_Provider::finalize()
{
     ATH_MSG_VERBOSE("Initializing ...");
     return StatusCode::SUCCESS;
}       
       
StatusCode InDet::InDetPRD_Provider::retrieveCollection() {
   if (m_pixClusterContainerName!="" && evtStore()->retrieve(m_pixClusterContainer,m_pixClusterContainerName).isFailure() ){
       ATH_MSG_ERROR("Could not retrieve " << m_pixClusterContainerName << ". Aborting." );
       return StatusCode::FAILURE;
   }
   if (m_sctClusterContainerName!="" && evtStore()->retrieve(m_sctClusterContainer,m_sctClusterContainerName).isFailure() ){
       ATH_MSG_ERROR("Could not retrieve " << m_sctClusterContainerName << ". Aborting." );
       return StatusCode::FAILURE;
   }
   if (m_trtDriftCircleContainerName!="" && evtStore()->retrieve(m_trtDriftCircleContainer,m_trtDriftCircleContainerName).isFailure() ){
       ATH_MSG_ERROR("Could not retrieve " << m_trtDriftCircleContainerName << ". Aborting." );
       return StatusCode::FAILURE;
   }
   return StatusCode::SUCCESS; 
}
       
       
/** return the Prd given the Identifier - make a HashId out of the Id and return the associated PRD */
const Trk::PrepRawData* InDet::InDetPRD_Provider::prdFromIdentifier(const Identifier& ide, size_t& ndof ) const {

    // check validity of the Identifier
    if (!ide.is_valid()){
        ATH_MSG_VERBOSE("The identifier is not valid ! Return 0.");
        return 0;
    }
    // is pixel case 
    if ( m_idHelper->is_pixel(ide) &&  m_pixClusterContainer ){
        ndof = 2;
        // get the Identifier Hash
        Identifier idewafer =  m_pixIdHelper->wafer_id(ide);
        IdentifierHash ideHash = m_pixIdHelper->wafer_hash(idewafer);
        ATH_MSG_VERBOSE("Pixel Identifier found as transformed to hash identifier " << (unsigned int)ideHash );
	    if (!ideHash.is_valid()){
		    ATH_MSG_VERBOSE("The hash identifier is not valid ! Return 0.");
		    return 0;
	    }
        return prdFromIdentifierContainer<InDet::PixelCluster>(*m_pixClusterContainer,ide,ideHash);
    }

    if ( m_idHelper->is_sct(ide) &&  m_sctClusterContainer ){
        // get the Identifier Hash
        ndof = 1;
        Identifier idewafer =  m_sctIdHelper->wafer_id(ide);
        IdentifierHash ideHash = m_sctIdHelper->wafer_hash(idewafer);
        ATH_MSG_VERBOSE("SCT Identifier found as transformed to hash identifier " << (unsigned int)ideHash );
        if (!ideHash.is_valid()){
		    ATH_MSG_VERBOSE("The hash identifier is not valid ! Return 0.");
		    return 0;
	    }
        return prdFromIdentifierContainer<InDet::SCT_Cluster>(*m_sctClusterContainer,ide,ideHash);
    }

    if ( m_idHelper->is_trt(ide) &&  m_trtDriftCircleContainer ){
        // get the Identifier Hash
        ndof = 1;
        Identifier idestrawlayer  = m_trtIdHelper->layer_id(m_trtIdHelper->barrel_ec(ide),
                                                            m_trtIdHelper->phi_module(ide),
                                                            m_trtIdHelper->layer_or_wheel(ide),
                                                            m_trtIdHelper->straw_layer(ide));
        IdentifierHash ideHash = m_trtIdHelper->straw_layer_hash(idestrawlayer); 
        ATH_MSG_VERBOSE("TRT Identifier found as transformed to hash identifier " << (unsigned int)ideHash );
        if (!ideHash.is_valid()){
		    ATH_MSG_VERBOSE("The hash identifier is not valid ! Return 0.");
		    return 0;
	    }
        return prdFromIdentifierContainer<InDet::TRT_DriftCircle>(*m_trtDriftCircleContainer,ide,ideHash);
    }
    return 0;
}
