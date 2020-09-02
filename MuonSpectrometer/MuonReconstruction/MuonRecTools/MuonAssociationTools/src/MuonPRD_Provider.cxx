/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#include "MuonPRD_Provider.h"

#include "MuonPrepRawData/MdtPrepData.h"
#include "MuonPrepRawData/TgcPrepData.h"
#include "MuonPrepRawData/RpcPrepData.h"
#include "MuonPrepRawData/CscPrepData.h"

namespace Muon {
  MuonPRD_Provider::MuonPRD_Provider(const std::string& t, const std::string& n, const IInterface* p):
    AthAlgTool(t,n,p)
  {
    declareInterface<Trk::IPRD_Provider>(this);
    // PRD container name
    declareProperty("CscPrepDataContainer", m_keyCsc = "CSC_Clusters");
    declareProperty("MdtPrepDataContainer", m_keyMdt = "MDT_DriftCircles");
    declareProperty("RpcPrepDataContainer", m_keyRpc = "RPC_Measurements");
    declareProperty("TgcPrepDataContainer", m_keyTgc = "TGC_Measurements");
    declareProperty("sTgcPrepDataContainer", m_keysTgc = "STGC_Measurements");
    declareProperty("MMPrepDataContainer",  m_keyMM  = "MM_Measurements");
  }


  StatusCode MuonPRD_Provider::initialize()
  {
    ATH_MSG_VERBOSE("Initializing ...");           
    ATH_CHECK(m_idHelperSvc.retrieve());
    return StatusCode::SUCCESS;
  }   
       
  StatusCode MuonPRD_Provider::retrieveCollection() {
    if (m_keyMdt!="" && evtStore()->retrieve(m_mdtPrds,m_keyMdt).isFailure() ){
      ATH_MSG_ERROR("Could not retrieve " << m_keyMdt << ". Aborting." );
      return StatusCode::FAILURE;
    }
    if (m_keyTgc!="" && evtStore()->retrieve(m_tgcPrds,m_keyTgc).isFailure() ){
      ATH_MSG_ERROR("Could not retrieve " << m_keyTgc << ". Aborting." );
      return StatusCode::FAILURE;
    }
    if (m_keyRpc!="" && evtStore()->retrieve(m_rpcPrds,m_keyRpc).isFailure() ){
      ATH_MSG_ERROR("Could not retrieve " << m_keyRpc << ". Aborting." );
      return StatusCode::FAILURE;
    }
    if (m_keyCsc!="" && evtStore()->retrieve(m_cscPrds,m_keyCsc).isFailure() ){
      ATH_MSG_ERROR("Could not retrieve " << m_keyCsc << ". Aborting." );
      return StatusCode::FAILURE;
    }
    if (m_keyMM!="" && evtStore()->retrieve(m_mmPrds,m_keyMM).isFailure() ){
      ATH_MSG_ERROR("Could not retrieve " << m_keyMM << ". Aborting." );
      return StatusCode::FAILURE;
    }
    if (m_keysTgc!="" && evtStore()->retrieve(m_stgcPrds,m_keysTgc).isFailure() ){
      ATH_MSG_ERROR("Could not retrieve " << m_keysTgc << ". Aborting." );
      return StatusCode::FAILURE;
    }
    return StatusCode::SUCCESS; 
  }
       
       
  /** return the Prd given the Identifier - make a HashId out of the Id and return the associated PRD */
  const Trk::PrepRawData* MuonPRD_Provider::prdFromIdentifier(const Identifier& ide, size_t& ndof ) const {


    // check validity of the Identifier
    if (!ide.is_valid() || !m_idHelperSvc->isMuon(ide) ){
      ATH_MSG_VERBOSE("The identifier is not valid ! Return 0.");
      return 0;
    }
    ndof = 1;
    if ( m_mdtPrds && m_idHelperSvc->isMdt(ide) ){
      // get the Identifier Hash
      IdentifierHash ideHash;
      m_idHelperSvc->mdtIdHelper().get_module_hash(ide,ideHash);

      if (!ideHash.is_valid()){
	ATH_MSG_VERBOSE("The hash identifier is not valid ! Return 0.");
	return 0;
      }
      return prdFromIdentifierContainer<MdtPrepData>(*m_mdtPrds,ide,ideHash);
    }

    if ( m_cscPrds && m_idHelperSvc->isCsc(ide) ){
      // get the Identifier Hash
      IdentifierHash ideHash;
      m_idHelperSvc->cscIdHelper().get_module_hash(ide,ideHash);

      if (!ideHash.is_valid()){
	ATH_MSG_VERBOSE("The hash identifier is not valid ! Return 0.");
	return 0;
      }
      return prdFromIdentifierContainer<CscPrepData>(*m_cscPrds,ide,ideHash);
    }

    if ( m_tgcPrds && m_idHelperSvc->isTgc(ide) ){
      // get the Identifier Hash
      IdentifierHash ideHash;
      m_idHelperSvc->tgcIdHelper().get_module_hash(ide,ideHash);

      if (!ideHash.is_valid()){
	ATH_MSG_VERBOSE("The hash identifier is not valid ! Return 0.");
	return 0;
      }
      return prdFromIdentifierContainer<TgcPrepData>(*m_tgcPrds,ide,ideHash);
    }

    if ( m_rpcPrds && m_idHelperSvc->isRpc(ide) ){
      // get the Identifier Hash
      IdentifierHash ideHash;
      m_idHelperSvc->rpcIdHelper().get_module_hash(ide,ideHash);

      if (!ideHash.is_valid()){
	ATH_MSG_VERBOSE("The hash identifier is not valid ! Return 0.");
	return 0;
      }
      return prdFromIdentifierContainer<RpcPrepData>(*m_rpcPrds,ide,ideHash);
    }
    if ( m_mmPrds && m_idHelperSvc->isMM(ide) ){
      // get the Identifier Hash
      IdentifierHash ideHash;
      m_idHelperSvc->mmIdHelper().get_module_hash(ide,ideHash);

      if (!ideHash.is_valid()){
	ATH_MSG_VERBOSE("The hash identifier is not valid ! Return 0.");
	return 0;
      }
      const Trk::PrepRawData* prd =  prdFromIdentifierContainer<MMPrepData>(*m_mmPrds,ide,ideHash);
    
      return prd;
    }

    if ( m_stgcPrds && m_idHelperSvc->issTgc(ide) ){
      // get the Identifier Hash
      IdentifierHash ideHash;
      m_idHelperSvc->stgcIdHelper().get_module_hash(ide,ideHash);

      if (!ideHash.is_valid()){
	ATH_MSG_VERBOSE("The hash identifier is not valid ! Return 0.");
	return 0;
      }
      return prdFromIdentifierContainer<sTgcPrepData>(*m_stgcPrds,ide,ideHash);
    }
    return 0;
  }
}
