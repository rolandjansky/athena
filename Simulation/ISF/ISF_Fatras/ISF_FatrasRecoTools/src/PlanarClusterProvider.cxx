/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// PlanarClusterProvider.cxx, (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////

#include "ISF_FatrasRecoTools/PlanarClusterProvider.h"
// ISF includes
#include "ISF_FatrasEvent/PlanarCluster.h"
// AtlasDetDescr  
#include "AtlasDetDescr/AtlasDetectorID.h"
// InDetIdentifier
#include "InDetIdentifier/PixelID.h"
#include "InDetIdentifier/SCT_ID.h"

iFatras::PlanarClusterProvider::PlanarClusterProvider(const std::string& t, const std::string& n, const IInterface* p):
  AthAlgTool(t,n,p),
  m_idHelper(0),
  m_pixIdHelper(0),
  m_sctIdHelper(0),
  m_planarClusterContainerName(""),
  m_planarClusterContainer(0)
{
  declareInterface<Trk::IPRD_Provider>(this);
  // PRD container name
  declareProperty("PlanarClusterContainer",        m_planarClusterContainerName);
}



StatusCode iFatras::PlanarClusterProvider::initialize()
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
     
     return StatusCode::SUCCESS;
}           



StatusCode iFatras::PlanarClusterProvider::finalize()
{
     ATH_MSG_VERBOSE("Initializing ...");
     return StatusCode::SUCCESS;
}       


StatusCode iFatras::PlanarClusterProvider::retrieveCollection() {
  
  if (m_planarClusterContainerName!="" && evtStore()->retrieve(m_planarClusterContainer,m_planarClusterContainerName).isFailure() ){
    ATH_MSG_ERROR("Could not retrieve " << m_planarClusterContainerName << ". Aborting." );
    return StatusCode::FAILURE;
  }
  
  return StatusCode::SUCCESS; 
}

/** return the Prd given the Identifier - make a HashId out of the Id and return the associated PRD */
const Trk::PrepRawData* iFatras::PlanarClusterProvider::prdFromIdentifier(const Identifier& ide, size_t& ndof ) const {

    // check validity of the Identifier
    if (!ide.is_valid()){
        ATH_MSG_VERBOSE("The identifier is not valid ! Return 0.");
        return 0;
    }

    // is pixel case 
    if ( m_idHelper->is_pixel(ide) &&  m_planarClusterContainer){
      ndof = 2;
      // get the Identifier Hash
      Identifier idewafer =  m_pixIdHelper->wafer_id(ide);
      IdentifierHash ideHash = m_pixIdHelper->wafer_hash(idewafer);
      ATH_MSG_VERBOSE("Pixel Identifier found as transformed to hash identifier " << (unsigned int)ideHash );
      if (!ideHash.is_valid()){
	ATH_MSG_VERBOSE("The hash identifier is not valid ! Return 0.");
	return 0;
      }
      ATH_MSG_DEBUG("Returning the PRD for PlananrCluster for Pixel Identifier " << (unsigned int)ideHash );
      return prdFromIdentifierContainer<iFatras::PlanarCluster>(*m_planarClusterContainer,ide,ideHash);
    }

    if ( m_idHelper->is_sct(ide) &&  m_planarClusterContainer ){
      // get the Identifier Hash
      ndof = 1;
      Identifier idewafer =  m_sctIdHelper->wafer_id(ide);
      IdentifierHash ideHash = m_sctIdHelper->wafer_hash(idewafer);
      ATH_MSG_VERBOSE("SCT Identifier found as transformed to hash identifier " << (unsigned int)ideHash );
      if (!ideHash.is_valid()){
	ATH_MSG_VERBOSE("The hash identifier is not valid ! Return 0.");
	return 0;
      }
      ATH_MSG_DEBUG("Returning the PRD for PlananrCluster for SCT Identifier " << (unsigned int)ideHash );
      return prdFromIdentifierContainer<iFatras::PlanarCluster>(*m_planarClusterContainer,ide,ideHash);
    }
    
    return 0;
}
