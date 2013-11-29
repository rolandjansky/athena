/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "IdHelpersValidation.h"
#include "GaudiKernel/MsgStream.h"
#include "AthenaKernel/errorcheck.h"    // to add CHECK() macro to derived classes

#include <fstream>

namespace Muon {

  // * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *

  IdHelpersValidation::IdHelpersValidation(const std::string& name, ISvcLocator* pSvcLocator) :
    AthAlgorithm(name, pSvcLocator)
  {
    ATH_MSG_DEBUG ("IdHelpersValidation()");

    declareProperty("FilePrefix",   m_FilePrefix="");
    declareProperty("CheckMdtIds",  m_DoCheckMdtIds=true);
    declareProperty("CheckCscIds",  m_DoCheckCscIds=true);
    declareProperty("CheckRpcIds",  m_DoCheckRpcIds=true);
    declareProperty("CheckTgcIds",  m_DoCheckTgcIds=true);
    declareProperty("CheckMmIds",   m_DoCheckMmIds=true);
    declareProperty("CheckStgcIds", m_DoCheckStgcIds=true);

    m_cscIdHelper  = 0;
    m_mdtIdHelper  = 0;
    m_mmIdHelper   = 0;
    m_rpcIdHelper  = 0;
    m_stgcIdHelper = 0;
    m_tgcIdHelper  = 0;
  }

  // * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *

  IdHelpersValidation::~IdHelpersValidation()
  { 
    ATH_MSG_DEBUG ("~IdHelpersValidation");
  }

  // * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *

  StatusCode IdHelpersValidation::initialize(){
    ATH_MSG_DEBUG ("initialize()");

    CHECK ( detStore()->retrieve(m_mdtIdHelper, "MDTIDHELPER") );
    ATH_MSG_DEBUG("Retrieved MDTIDHELPER " << m_mdtIdHelper);
      
    CHECK ( detStore()->retrieve(m_cscIdHelper, "CSCIDHELPER") );
    ATH_MSG_DEBUG("Retrieved CSCIDHELPER " << m_cscIdHelper);
      
    CHECK ( detStore()->retrieve(m_rpcIdHelper, "RPCIDHELPER") );
    ATH_MSG_DEBUG("Retrieved RPCIDHELPER " << m_rpcIdHelper);
      
    CHECK ( detStore()->retrieve(m_tgcIdHelper, "TGCIDHELPER") );
    ATH_MSG_DEBUG("Retrieved TGCIDHELPER " << m_tgcIdHelper);
      
    CHECK ( detStore()->retrieve(m_mmIdHelper, "MMIDHELPER") );
    ATH_MSG_DEBUG("Retrieved MMIDHELPER " << m_mmIdHelper);
      
    CHECK ( detStore()->retrieve(m_stgcIdHelper, "STGCIDHELPER") );
    ATH_MSG_DEBUG("Retrieved STGCIDHELPER " << m_stgcIdHelper);

    m_MdtIdSanityCheck  = false;
    m_CscIdSanityCheck  = false;
    m_RpcIdSanityCheck  = false;
    m_TgcIdSanityCheck  = false;
    m_MmIdSanityCheck   = false;
    m_sTgcIdSanityCheck = false;

    return StatusCode::SUCCESS;
  }

  // * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *

  StatusCode IdHelpersValidation::execute() {
    
    ATH_MSG_DEBUG ("execute()");

    CHECK ( dumpIds(m_mdtIdHelper, "mdt", "channel") );
    CHECK ( dumpIds(m_mdtIdHelper, "mdt", "detector") );
    CHECK ( dumpIds(m_mdtIdHelper, "mdt", "module") );

    CHECK ( dumpIds(m_cscIdHelper, "csc", "channel") );
    CHECK ( dumpIds(m_cscIdHelper, "csc", "detector") );
    CHECK ( dumpIds(m_cscIdHelper, "csc", "module") );

    CHECK ( dumpIds(m_rpcIdHelper, "rpc", "channel") );
    CHECK ( dumpIds(m_rpcIdHelper, "rpc", "detector") );
    CHECK ( dumpIds(m_rpcIdHelper, "rpc", "module") );

    CHECK ( dumpIds(m_tgcIdHelper, "tgc", "channel") );
    CHECK ( dumpIds(m_tgcIdHelper, "tgc", "detector") );
    CHECK ( dumpIds(m_tgcIdHelper, "tgc", "module") );

    CHECK ( dumpIds(m_mmIdHelper, "mm", "channel") );
    CHECK ( dumpIds(m_mmIdHelper, "mm", "detector") );
    CHECK ( dumpIds(m_mmIdHelper, "mm", "module") );

    CHECK ( dumpIds(m_stgcIdHelper, "stgc", "channel") );
    CHECK ( dumpIds(m_stgcIdHelper, "stgc", "detector") );
    CHECK ( dumpIds(m_stgcIdHelper, "stgc", "module") );

    if (m_DoCheckMdtIds)  CHECK ( checkMdtIds() );
    if (m_DoCheckCscIds)  CHECK ( checkCscIds() );
    if (m_DoCheckRpcIds)  CHECK ( checkRpcIds() );
    if (m_DoCheckTgcIds)  CHECK ( checkTgcIds() );
    if (m_DoCheckStgcIds) CHECK ( checkStgcIds() );
    if (m_DoCheckMmIds)   CHECK ( checkMmIds() );

    return StatusCode::SUCCESS;
  }

  /*******************************************************************
   ** finalize()
   **/
  StatusCode IdHelpersValidation::finalize() {
    ATH_MSG_DEBUG ("finalize()");

    ATH_MSG_INFO("======================================================");
    ATH_MSG_INFO("== Muon IdHelper Validation Summary                 ==");
    ATH_MSG_INFO("======================================================");
    if ( m_DoCheckMdtIds ) {
      if ( m_MdtIdSanityCheck ) {
	ATH_MSG_INFO("== MDT ID Sanity Check : success                    ==");
      } else {
	ATH_MSG_INFO("== MDT ID Sanity Check : failed                     ==");
      }
    } else {
	ATH_MSG_INFO("== MDT ID Sanity Check : not done                   ==");
    }
    if ( m_DoCheckCscIds ) {
      if ( m_CscIdSanityCheck ) {
	ATH_MSG_INFO("== CSC ID Sanity Check : success                    ==");
      } else {
	ATH_MSG_INFO("== CSC ID Sanity Check : failed                     ==");
      }
    } else {
	ATH_MSG_INFO("== CSC ID Sanity Check : not done                   ==");
    }
    if ( m_DoCheckRpcIds ) {
      if ( m_RpcIdSanityCheck ) {
	ATH_MSG_INFO("== RPC ID Sanity Check : success                    ==");
      } else {
	ATH_MSG_INFO("== RPC ID Sanity Check : failed                     ==");
      }
    } else {
	ATH_MSG_INFO("== RPC ID Sanity Check : not done                   ==");
    }
    if ( m_DoCheckTgcIds ) {
      if ( m_TgcIdSanityCheck ) {
	ATH_MSG_INFO("== TGC ID Sanity Check : success                    ==");
      } else {
	ATH_MSG_INFO("== TGC ID Sanity Check : failed                     ==");
      }
    } else {
	ATH_MSG_INFO("== TGC ID Sanity Check : not done                   ==");
    }
    if ( m_DoCheckMmIds ) {
      if ( m_MmIdSanityCheck ) {
	ATH_MSG_INFO("== MM ID Sanity Check : success                     ==");
      } else {
	ATH_MSG_INFO("== MM ID Sanity Check : failed                      ==");
      }
    } else {
	ATH_MSG_INFO("== MM ID Sanity Check : not done                    ==");
    }
    if ( m_DoCheckStgcIds ) {
      if ( m_sTgcIdSanityCheck ) {
	ATH_MSG_INFO("== sTGC ID Sanity Check : success                   ==");
      } else {
	ATH_MSG_INFO("== sTGC ID Sanity Check : failed                    ==");
      }
    } else {
	ATH_MSG_INFO("== sTGC ID Sanity Check : not done                  ==");
    }
    ATH_MSG_INFO("======================================================");

    return StatusCode::SUCCESS;
  }

  // * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *

  StatusCode IdHelpersValidation::dumpIds(const MuonIdHelper* helper, const std::string& name, const std::string& level) {
    ATH_MSG_DEBUG ("dumpIds() " << name << " " << level);

    StatusCode sc = StatusCode::SUCCESS;

    std::string fileName = m_FilePrefix + name + "_" + level + ".txt";

    std::ofstream outfile (fileName.c_str());

    std::vector<Identifier>::const_iterator ib;
    std::vector<Identifier>::const_iterator ie;

    if (level == "channel") {
      ib = helper->channel_begin();
      ie = helper->channel_end();
    } else if (level == "detector") {
      ib = helper->detectorElement_begin();
      ie = helper->detectorElement_end();
    } else if (level == "module") {
      ib = helper->module_begin();
      ie = helper->module_end();
    } else {
      ATH_MSG_WARNING ( "Unkown level requested" );
      return StatusCode::RECOVERABLE;
    }

    int nElements = 0;
    for ( ; ib != ie; ++ib) {
      ++nElements;
      Identifier id = (*ib);
      outfile << id.get_compact() << " " << id.getString() << std::endl;

      bool isMdt  = helper->is_mdt(id);
      bool isCsc  = helper->is_csc(id);
      bool isRpc  = helper->is_rpc(id);
      bool isTgc  = helper->is_tgc(id);
      bool isMm   = helper->is_mm(id);
      bool isStgc = helper->is_stgc(id);

      int n = 0;
      if (isMdt)  n++;
      if (isCsc)  n++;
      if (isRpc)  n++;
      if (isTgc)  n++;
      if (isMm)   n++;
      if (isStgc) n++;
      if ( n > 1 ) {
	ATH_MSG_WARNING ("MDT/CSC/RPC/TGC/MM/sTGC " << isMdt << " " << isCsc << " " << isRpc << " " << isTgc << " " << isMm << " " << isStgc);
	sc = worstStatus(sc, StatusCode::RECOVERABLE);
      }
    }

    if (level == "channel") {
      ATH_MSG_DEBUG (name << " nChannels = " << nElements);
    } else if (level == "detector") {
      ATH_MSG_DEBUG (name << " nDetectorElements = " << nElements);
    } else if (level == "module") {
      ATH_MSG_DEBUG (name << " nModules = " << nElements);
    }

    return sc;
  }

  // * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *

  StatusCode IdHelpersValidation::checkMdtIds() {
    ATH_MSG_DEBUG ("checkMdtIds");

    StatusCode sc = StatusCode::SUCCESS;

    std::string fileName = m_FilePrefix + "mdt_tube.txt";
    std::ofstream outfile (fileName.c_str());

    std::vector<Identifier>::const_iterator ib = m_mdtIdHelper->module_begin();
    std::vector<Identifier>::const_iterator ie = m_mdtIdHelper->module_end();

    std::vector<Identifier> vect;

    int nChannels = 0;
    for ( ; ib != ie; ++ib) {
      Identifier id = (*ib);

      vect.clear();
      m_mdtIdHelper->idChannels(id, vect);
      
      std::vector<Identifier>::const_iterator cib = vect.begin();
      std::vector<Identifier>::const_iterator cie = vect.end();

      for ( ; cib != cie; ++cib) {
	++nChannels;
	const Identifier cid = (*cib);
	bool isValid = m_mdtIdHelper->valid(cid);
	bool isMdt  = m_mdtIdHelper->is_mdt(cid);
	bool isCsc  = m_mdtIdHelper->is_csc(cid);
	bool isRpc  = m_mdtIdHelper->is_rpc(cid);
	bool isTgc  = m_mdtIdHelper->is_tgc(cid);
	bool isMm   = m_mdtIdHelper->is_mm(cid);
	bool isStgc = m_mdtIdHelper->is_stgc(cid);

	int technology    = m_mdtIdHelper->technology(cid);
	int stationRegion = m_mdtIdHelper->stationRegion(cid);
	int stationName = m_mdtIdHelper->stationName(cid);
	int stationEta  = m_mdtIdHelper->stationEta(cid);
	int stationPhi  = m_mdtIdHelper->stationPhi(cid);
	int multilayer  = m_mdtIdHelper->multilayer(cid);
	int tubeLayer   = m_mdtIdHelper->tubeLayer(cid);
	int tube        = m_mdtIdHelper->tube(cid);
	bool check      = true;
	bool isValidChannel(true);

	std::string technologyString = m_mdtIdHelper->technologyString(technology);
	std::string stationNameString = m_mdtIdHelper->stationNameString(stationName);
	int technologyIndex = m_mdtIdHelper->technologyIndex(technologyString);
	int stationNameIndex = m_mdtIdHelper->stationNameIndex(stationNameString);

	std::stringstream out;
	out<< cid.get_compact() << " " << cid.getString() << " " << technology << " " << stationRegion << " " << stationName << " " << stationEta << " " << stationPhi << " " << multilayer << " " << tubeLayer << " " << tube << " " << technologyString << " " << stationNameString << std::endl;
	
	std::string mdtIdString = out.str();

	outfile << mdtIdString << std::endl;

	const Identifier newId = m_mdtIdHelper->channelID(stationName, stationEta, stationPhi, multilayer, tubeLayer, tube, check, &isValidChannel);

	if (!isValid) {
	  ATH_MSG_WARNING ("Not a valid MDT ID: " << mdtIdString);
	  sc = worstStatus(sc, StatusCode::RECOVERABLE);
	}
	if (!isMdt) {
	  ATH_MSG_WARNING ("Not an MDT ID: " << mdtIdString);
	  sc = worstStatus(sc, StatusCode::RECOVERABLE);
	}
	if (isCsc || isRpc || isTgc || isMm || isStgc) {
	  ATH_MSG_WARNING ("Wrong technology ID: " << mdtIdString);
	  ATH_MSG_WARNING ("MDT/CSC/RPC/TGC/MM/sTGC " << isMdt << " " << isCsc << " " << isRpc << " " << isTgc << " " << isMm << " " << isStgc);
	  sc = worstStatus(sc, StatusCode::RECOVERABLE);
	}
	if (!isValidChannel) {
	  ATH_MSG_WARNING ("MDT Id check failed: " << mdtIdString);
	  sc = worstStatus(sc, StatusCode::RECOVERABLE);
	}
	if (cid != newId) {
	  ATH_MSG_WARNING ("MDT Id pack/unpack failed!");
	  ATH_MSG_WARNING ("Old Id:" << mdtIdString);
	  ATH_MSG_WARNING ("New Id:" << newId.get_compact() << " " << newId.getString());
	  sc = worstStatus(sc, StatusCode::RECOVERABLE);
	}
	if (technology != technologyIndex) {
	  ATH_MSG_WARNING ("MDT technology index->string->string failed! " << mdtIdString);
	  ATH_MSG_WARNING ("index=" << technology << " string=" << technologyString << " index=" << technologyIndex);
	}
	if (stationName != stationNameIndex) {
	  ATH_MSG_WARNING ("MDT stationName index->string->string failed! " << mdtIdString);
	  ATH_MSG_WARNING ("index=" << stationName << " string=" << stationNameString << " index=" << stationNameIndex);
	}
      }

    }
    ATH_MSG_DEBUG ("mdt nChannels = " << nChannels);

    if ( sc == StatusCode::SUCCESS ) {
      m_MdtIdSanityCheck = true;
    }

    return sc;
  }

  // * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *

  StatusCode IdHelpersValidation::checkCscIds() {
    ATH_MSG_DEBUG ("checkCscIds");

    StatusCode sc = StatusCode::SUCCESS;

    std::string fileName = m_FilePrefix + "csc_strip.txt";
    std::ofstream outfile (fileName.c_str());

    std::vector<Identifier>::const_iterator ib = m_cscIdHelper->module_begin();
    std::vector<Identifier>::const_iterator ie = m_cscIdHelper->module_end();

    std::vector<Identifier> vect;

    int nChannels = 0;
    for ( ; ib != ie; ++ib) {
      Identifier id = (*ib);

      vect.clear();
      m_cscIdHelper->idChannels(id, vect);
      
      std::vector<Identifier>::const_iterator cib = vect.begin();
      std::vector<Identifier>::const_iterator cie = vect.end();

      for ( ; cib != cie; ++cib) {
	++nChannels;
	const Identifier cid = (*cib);
	bool isValid = m_cscIdHelper->valid(cid);
	bool isMdt  = m_cscIdHelper->is_mdt(cid);
	bool isCsc  = m_cscIdHelper->is_csc(cid);
	bool isRpc  = m_cscIdHelper->is_rpc(cid);
	bool isTgc  = m_cscIdHelper->is_tgc(cid);
	bool isMm   = m_cscIdHelper->is_mm(cid);
	bool isStgc = m_cscIdHelper->is_stgc(cid);

	int stationName  = m_cscIdHelper->stationName(cid);
	int stationEta   = m_cscIdHelper->stationEta(cid);
	int stationPhi   = m_cscIdHelper->stationPhi(cid);
	int chamberLayer = m_cscIdHelper->chamberLayer(cid);
	int wireLayer    = m_cscIdHelper->wireLayer(cid);
	int measuresPhi  = m_cscIdHelper->measuresPhi(cid);
	int strip        = m_cscIdHelper->strip(cid);
	bool check       = true;
	bool isValidChannel(true);

	int technology    = m_cscIdHelper->technology(cid);
	std::string technologyString = m_cscIdHelper->technologyString(technology);
	std::string stationNameString = m_cscIdHelper->stationNameString(stationName);

	outfile << cid.get_compact() << " " << cid.getString() << " " << stationName << " " << stationEta << " " << stationPhi << " " << chamberLayer << " " << wireLayer << " " << measuresPhi << " " << strip << std::endl;

	const Identifier newId = m_cscIdHelper->channelID(stationName, stationEta, stationPhi, chamberLayer, wireLayer, measuresPhi, strip, check, &isValidChannel);

	if (!isValid) {
	  ATH_MSG_WARNING ("Not a valid CSC ID: " << cid.get_compact() << " " << cid.getString() << " " << stationName << " " << stationEta << " " << stationPhi << " " << chamberLayer << " " << wireLayer << " " << measuresPhi << " " << strip);
	  sc = worstStatus(sc, StatusCode::RECOVERABLE);
	}
	if (!isCsc) {
	  ATH_MSG_WARNING ("Not an CSC ID: " << cid.get_compact() << " " << cid.getString() << " " << stationName << " " << stationEta << " " << stationPhi << " " << chamberLayer << " " << wireLayer << " " << measuresPhi << " " << strip);
	  sc = worstStatus(sc, StatusCode::RECOVERABLE);
	}
	if (isMdt || isRpc || isTgc || isMm || isStgc) {
	  ATH_MSG_WARNING ("Wrong technology ID: " << cid.get_compact() << " " << cid.getString() << " " << stationName << " " << stationEta << " " << stationPhi << " " << chamberLayer << " " << wireLayer << " " << measuresPhi << " " << strip);
	  ATH_MSG_WARNING ("MDT/CSC/RPC/TGC/MM/sTGC " << isMdt << " " << isCsc << " " << isRpc << " " << isTgc << " " << isMm << " " << isStgc);
	  sc = worstStatus(sc, StatusCode::RECOVERABLE);
	}
	if (!isValidChannel) {
	  ATH_MSG_WARNING ("CSC Id check failed: " << cid.get_compact() << " " << cid.getString() << " " << stationName << " " << stationEta << " " << stationPhi << " " << chamberLayer << " " << wireLayer << " " << measuresPhi << " " << strip);
	  sc = worstStatus(sc, StatusCode::RECOVERABLE);
	}
	if (cid != newId) {
	  ATH_MSG_WARNING ("CSC Id pack/unpack failed: " << cid.get_compact() << " " << cid.getString() << " " << newId.get_compact() << " " << newId.getString() << " " << stationName << " " << stationEta << " " << stationPhi << " " << chamberLayer << " " << wireLayer << " " << measuresPhi << " " << strip);
	  sc = worstStatus(sc, StatusCode::RECOVERABLE);
	}
      }

    }
    ATH_MSG_DEBUG ("csc nChannels = " << nChannels);

    if ( sc == StatusCode::SUCCESS ) {
      m_CscIdSanityCheck = true;
    }

    return sc;
  }

  // * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *

  StatusCode IdHelpersValidation::checkRpcIds() {
    ATH_MSG_DEBUG ("checkRpcIds");

    StatusCode sc = StatusCode::SUCCESS;

    std::string fileName = m_FilePrefix + "rpc_strip.txt";
    std::ofstream outfile (fileName.c_str());

    std::vector<Identifier>::const_iterator ib = m_rpcIdHelper->module_begin();
    std::vector<Identifier>::const_iterator ie = m_rpcIdHelper->module_end();

    std::vector<Identifier> vect;

    int nChannels = 0;
    for ( ; ib != ie; ++ib) {
      Identifier id = (*ib);

      vect.clear();
      m_rpcIdHelper->idChannels(id, vect);
      
      std::vector<Identifier>::const_iterator cib = vect.begin();
      std::vector<Identifier>::const_iterator cie = vect.end();

      for ( ; cib != cie; ++cib) {
	++nChannels;
	const Identifier cid = (*cib);
	bool isValid = m_rpcIdHelper->valid(cid);
	bool isMdt  = m_rpcIdHelper->is_mdt(cid);
	bool isCsc  = m_rpcIdHelper->is_csc(cid);
	bool isRpc  = m_rpcIdHelper->is_rpc(cid);
	bool isTgc  = m_rpcIdHelper->is_tgc(cid);
	bool isMm   = m_rpcIdHelper->is_mm(cid);
	bool isStgc = m_rpcIdHelper->is_stgc(cid);

	int stationName  = m_rpcIdHelper->stationName(cid);
	int stationEta   = m_rpcIdHelper->stationEta(cid);
	int stationPhi   = m_rpcIdHelper->stationPhi(cid);

	int doubletR    = m_rpcIdHelper->doubletR(cid);
	int doubletZ    = m_rpcIdHelper->doubletZ(cid);
	int doubletPhi  = m_rpcIdHelper->doubletPhi(cid);
	int gasGap      = m_rpcIdHelper->gasGap(cid);
	int measuresPhi = m_rpcIdHelper->measuresPhi(cid);
	int strip       = m_rpcIdHelper->strip(cid);
	bool check      = true;
	bool isValidChannel(true);

	int technology    = m_rpcIdHelper->technology(cid);
	std::string technologyString = m_rpcIdHelper->technologyString(technology);
	std::string stationNameString = m_rpcIdHelper->stationNameString(stationName);

	outfile << cid.get_compact() << " " << cid.getString() << " " << stationName << " " << stationEta << " " << stationPhi << " " << doubletR << " " << doubletZ << " " << doubletPhi << " " << gasGap << " " << measuresPhi << " " << strip << std::endl;

	const Identifier newId = m_rpcIdHelper->channelID(stationName, stationEta, stationPhi, doubletR, doubletZ, doubletPhi, gasGap, measuresPhi, strip, check, &isValidChannel);

	if (!isValid) {
	  ATH_MSG_WARNING ("Not a valid RPC ID: " << cid.get_compact() << " " << cid.getString() << " " << stationName << " " << stationEta << " " << stationPhi << " " << doubletR << " " << doubletZ << " " << doubletPhi << " " << gasGap << " " << measuresPhi << " " << strip);
	  sc = worstStatus(sc, StatusCode::RECOVERABLE);
	}
	if (!isRpc) {
	  ATH_MSG_WARNING ("Not an RPC ID: " << cid.get_compact() << " " << cid.getString() << " " << stationName << " " << stationEta << " " << stationPhi << " " << doubletR << " " << doubletZ << " " << doubletPhi << " " << gasGap << " " << measuresPhi << " " << strip);
	  sc = worstStatus(sc, StatusCode::RECOVERABLE);
	}
	if (isMdt || isCsc || isTgc || isMm || isStgc) {
	  ATH_MSG_WARNING ("Wrong technology ID: " << cid.get_compact() << " " << cid.getString() << " " << stationName << " " << stationEta << " " << stationPhi << " " << doubletR << " " << doubletZ << " " << doubletPhi << " " << gasGap << " " << measuresPhi << " " << strip);
	  ATH_MSG_WARNING ("MDT/CSC/RPC/TGC/MM/sTGC " << isMdt << " " << isCsc << " " << isRpc << " " << isTgc << " " << isMm << " " << isStgc);
	  sc = worstStatus(sc, StatusCode::RECOVERABLE);
	}
	if (!isValidChannel) {
	  ATH_MSG_WARNING ("RPC Id check failed: " << cid.get_compact() << " " << cid.getString() << " " << stationName << " " << stationEta << " " << stationPhi << " " << doubletR << " " << doubletZ << " " << doubletPhi << " " << gasGap << " " << measuresPhi << " " << strip);
	  sc = worstStatus(sc, StatusCode::RECOVERABLE);
	}
	if (cid != newId) {
	  ATH_MSG_WARNING ("RPC Id pack/unpack failed: " << cid.get_compact() << " " << cid.getString() << " " << newId.get_compact() << " " << newId.getString() << " " << stationName << " " << stationEta << " " << stationPhi << " " << doubletR << " " << doubletZ << " " << doubletPhi << " " << gasGap << " " << measuresPhi << " " << strip);
	  sc = worstStatus(sc, StatusCode::RECOVERABLE);
	}
      }

    }
    ATH_MSG_DEBUG ("rpc nChannels = " << nChannels);

    if ( sc == StatusCode::SUCCESS ) {
      m_RpcIdSanityCheck = true;
    }

    return sc;
  }

  // * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *

  StatusCode IdHelpersValidation::checkTgcIds() {
    ATH_MSG_DEBUG ("checkTgcIds");

    StatusCode sc = StatusCode::SUCCESS;

    std::string fileName = m_FilePrefix + "tgc_strip.txt";
    std::ofstream outfile (fileName.c_str());

    std::vector<Identifier>::const_iterator ib = m_tgcIdHelper->module_begin();
    std::vector<Identifier>::const_iterator ie = m_tgcIdHelper->module_end();

    std::vector<Identifier> vect;

    int nChannels = 0;
    for ( ; ib != ie; ++ib) {
      Identifier id = (*ib);

      vect.clear();
      m_tgcIdHelper->idChannels(id, vect);
      
      std::vector<Identifier>::const_iterator cib = vect.begin();
      std::vector<Identifier>::const_iterator cie = vect.end();

      for ( ; cib != cie; ++cib) {
	++nChannels;
	const Identifier cid = (*cib);
	bool isValid = m_tgcIdHelper->valid(cid);
	bool isMdt  = m_tgcIdHelper->is_mdt(cid);
	bool isCsc  = m_tgcIdHelper->is_csc(cid);
	bool isRpc  = m_tgcIdHelper->is_rpc(cid);
	bool isTgc  = m_tgcIdHelper->is_tgc(cid);
	bool isMm   = m_tgcIdHelper->is_mm(cid);
	bool isStgc = m_tgcIdHelper->is_stgc(cid);

	int stationName  = m_tgcIdHelper->stationName(cid);
	int stationEta   = m_tgcIdHelper->stationEta(cid);
	int stationPhi   = m_tgcIdHelper->stationPhi(cid);

	int gasGap = m_tgcIdHelper->gasGap(cid);
	int measuresPhi    = m_tgcIdHelper->measuresPhi(cid);
	int channel  = m_tgcIdHelper->channel(cid);
	bool check       = true;
	bool isValidChannel(true);

	int technology    = m_tgcIdHelper->technology(cid);
	std::string technologyString = m_tgcIdHelper->technologyString(technology);
	std::string stationNameString = m_tgcIdHelper->stationNameString(stationName);

	outfile << cid.get_compact() << " " << cid.getString() << " " << stationName << " " << stationEta << " " << stationPhi << " " << gasGap << " " << measuresPhi << " " << channel << std::endl;

	const Identifier newId = m_tgcIdHelper->channelID(stationName, stationEta, stationPhi, gasGap, measuresPhi, channel, check, &isValidChannel);

	if (!isValid) {
	  ATH_MSG_WARNING ("Not a valid TGC ID: " << cid.get_compact() << " " << cid.getString() << " " << stationName << " " << stationEta << " " << stationPhi << " " << gasGap << " " << measuresPhi << " " << channel);
	  sc = worstStatus(sc, StatusCode::RECOVERABLE);
	}
	if (!isTgc) {
	  ATH_MSG_WARNING ("Not an TGC ID: " << cid.get_compact() << " " << cid.getString() << " " << stationName << " " << stationEta << " " << stationPhi << " " << gasGap << " " << measuresPhi << " " << channel);
	  sc = worstStatus(sc, StatusCode::RECOVERABLE);
	}
	if (isMdt || isCsc || isRpc || isMm || isStgc) {
	  ATH_MSG_WARNING ("Wrong technology ID: " << cid.get_compact() << " " << cid.getString() << " " << stationName << " " << stationEta << " " << stationPhi << " " << gasGap << " " << measuresPhi << " " << channel);
	  ATH_MSG_WARNING ("MDT/CSC/RPC/TGC/MM/sTGC " << isMdt << " " << isCsc << " " << isRpc << " " << isTgc << " " << isMm << " " << isStgc);
	  sc = worstStatus(sc, StatusCode::RECOVERABLE);
	}
	if (!isValidChannel) {
	  ATH_MSG_WARNING ("TGC Id check failed: " << cid.get_compact() << " " << cid.getString() << " " << stationName << " " << stationEta << " " << stationPhi << " " << gasGap << " " << measuresPhi << " " << channel);
	  sc = worstStatus(sc, StatusCode::RECOVERABLE);
	}
	if (cid != newId) {
	  ATH_MSG_WARNING ("TGC Id pack/unpack failed: " << cid.get_compact() << " " << cid.getString() << " " << newId.get_compact() << " " << newId.getString() << " " << stationName << " " << stationEta << " " << stationPhi << " " << gasGap << " " << measuresPhi << " " << channel);
	  sc = worstStatus(sc, StatusCode::RECOVERABLE);
	}
      }

    }
    ATH_MSG_DEBUG ("tgc nChannels = " << nChannels);

    if ( sc == StatusCode::SUCCESS ) {
      m_TgcIdSanityCheck = true;
    }

    return sc;
  }

  // * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *

  StatusCode IdHelpersValidation::checkMmIds() {
    ATH_MSG_DEBUG ("checkMmIds");

    StatusCode sc = StatusCode::SUCCESS;

    std::string fileName = m_FilePrefix + "mm_strip.txt";
    std::ofstream outfile (fileName.c_str());

    std::vector<Identifier>::const_iterator ib = m_mmIdHelper->module_begin();
    std::vector<Identifier>::const_iterator ie = m_mmIdHelper->module_end();

    std::vector<Identifier> vect;

    int nChannels = 0;
    for ( ; ib != ie; ++ib) {
      Identifier id = (*ib);

      vect.clear();
      m_mmIdHelper->idChannels(id, vect);
      
      std::vector<Identifier>::const_iterator cib = vect.begin();
      std::vector<Identifier>::const_iterator cie = vect.end();

      for ( ; cib != cie; ++cib) {
	++nChannels;
	const Identifier cid = (*cib);
	bool isValid = m_mmIdHelper->valid(cid);
	bool isMdt  = m_mmIdHelper->is_mdt(cid);
	bool isCsc  = m_mmIdHelper->is_csc(cid);
	bool isRpc  = m_mmIdHelper->is_rpc(cid);
	bool isTgc  = m_mmIdHelper->is_tgc(cid);
	bool isMm   = m_mmIdHelper->is_mm(cid);
	bool isStgc = m_mmIdHelper->is_stgc(cid);

	int stationName  = m_mmIdHelper->stationName(cid);
	int stationEta   = m_mmIdHelper->stationEta(cid);
	int stationPhi   = m_mmIdHelper->stationPhi(cid);

	int multilayer   = m_mmIdHelper->multilayer(cid);
	int gasGap      = m_mmIdHelper->gasGap(cid);
	int channel     = m_mmIdHelper->channel(cid);
	bool check      = true;
	bool isValidChannel(true);

	int technology    = m_mmIdHelper->technology(cid);
	std::string technologyString = m_mmIdHelper->technologyString(technology);
	std::string stationNameString = m_mmIdHelper->stationNameString(stationName);

	outfile << cid.get_compact() << " " << cid.getString() << " " << stationName << " " << stationEta << " " << stationPhi << " " << multilayer << " " << gasGap << " " << channel << " " << technologyString << " " << stationNameString << std::endl;

	const Identifier newId = m_mmIdHelper->channelID(stationName, stationEta, stationPhi, multilayer, gasGap, channel, check, &isValidChannel);

	if (!isValid) {
	  ATH_MSG_WARNING ("Not a valid MM ID: " << cid.get_compact() << " " << cid.getString() << " " << stationName << " " << stationEta << " " << stationPhi << " " << multilayer << " " << gasGap << " " << channel << " " << technologyString << " " << stationNameString);
	  sc = worstStatus(sc, StatusCode::RECOVERABLE);
	}
	if (!isMm) {
	  ATH_MSG_WARNING ("Not an MM ID: " << cid.get_compact() << " " << cid.getString() << " " << stationName << " " << stationEta << " " << stationPhi << " " << multilayer << " " << gasGap << " " << channel << " " << technologyString << " " << stationNameString);
	  sc = worstStatus(sc, StatusCode::RECOVERABLE);
	}
	if (isMdt || isCsc || isRpc || isTgc || isStgc) {
	  ATH_MSG_WARNING ("Wrong technology ID: " << cid.get_compact() << " " << cid.getString() << " " << stationName << " " << stationEta << " " << stationPhi << " " << multilayer << " " << gasGap << " " << channel << " " << technologyString << " " << stationNameString);
	  ATH_MSG_WARNING ("MDT/CSC/RPC/TGC/MM/sTGC " << isMdt << " " << isCsc << " " << isRpc << " " << isTgc << " " << isMm << " " << isStgc);
	  sc = worstStatus(sc, StatusCode::RECOVERABLE);
	}
	if (!isValidChannel) {
	  ATH_MSG_WARNING ("MM Id check failed: " << cid.get_compact() << " " << cid.getString() << " " << stationName << " " << stationEta << " " << stationPhi << " " << multilayer << " " << gasGap << " " << channel << " " << technologyString << " " << stationNameString);
	  sc = worstStatus(sc, StatusCode::RECOVERABLE);
	}
	if (cid != newId) {
	  ATH_MSG_WARNING ("MM Id pack/unpack failed: " << cid.get_compact() << " " << cid.getString() << " " << newId.get_compact() << " " << newId.getString() << " " << stationName << " " << stationEta << " " << stationPhi << " " << multilayer << " " << gasGap << " " << channel << " " << technologyString << " " << stationNameString);
	  sc = worstStatus(sc, StatusCode::RECOVERABLE);
	}
      }

    }
    ATH_MSG_DEBUG ("mm nChannels = " << nChannels);

    if ( sc == StatusCode::SUCCESS ) {
      m_MmIdSanityCheck = true;
    }

    return sc;
  }

  // * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *

  StatusCode IdHelpersValidation::checkStgcIds() {
    ATH_MSG_DEBUG ("checkStgcIds");

    StatusCode sc = StatusCode::SUCCESS;

    std::string fileName = m_FilePrefix + "stgc_strip.txt";
    std::ofstream outfile (fileName.c_str());

    std::vector<Identifier>::const_iterator ib = m_stgcIdHelper->module_begin();
    std::vector<Identifier>::const_iterator ie = m_stgcIdHelper->module_end();

    std::vector<Identifier> vect;

    int nChannels = 0;
    for ( ; ib != ie; ++ib) {
      Identifier id = (*ib);

      vect.clear();
      m_stgcIdHelper->idChannels(id, vect);
      
      std::vector<Identifier>::const_iterator cib = vect.begin();
      std::vector<Identifier>::const_iterator cie = vect.end();

      for ( ; cib != cie; ++cib) {
	++nChannels;
	const Identifier cid = (*cib);
	bool isValid = m_stgcIdHelper->valid(cid);
	bool isMdt  = m_stgcIdHelper->is_mdt(cid);
	bool isCsc  = m_stgcIdHelper->is_csc(cid);
	bool isRpc  = m_stgcIdHelper->is_rpc(cid);
	bool isTgc  = m_stgcIdHelper->is_tgc(cid);
	bool isMm   = m_stgcIdHelper->is_mm(cid);
	bool isStgc = m_stgcIdHelper->is_stgc(cid);

	int stationName  = m_stgcIdHelper->stationName(cid);
	int stationEta   = m_stgcIdHelper->stationEta(cid);
	int stationPhi   = m_stgcIdHelper->stationPhi(cid);

	int multilayer    = m_stgcIdHelper->multilayer(cid);
	int gasGap       = m_stgcIdHelper->gasGap(cid);
	int channelType  = m_stgcIdHelper->channelType(cid);
	int channel      = m_stgcIdHelper->channel(cid);
	bool check       = true;
	bool isValidChannel(true);

	int technology    = m_stgcIdHelper->technology(cid);
	std::string technologyString = m_stgcIdHelper->technologyString(technology);
	std::string stationNameString = m_stgcIdHelper->stationNameString(stationName);

	outfile << cid.get_compact() << " " << cid.getString() << " " << stationName << " " << stationEta << " " << stationPhi << " " << multilayer << " " << gasGap << " " << channelType << " " << channel << " " << technologyString << " " << stationNameString << std::endl;

	const Identifier newId = m_stgcIdHelper->channelID(stationName, stationEta, stationPhi, multilayer, gasGap, channelType, channel, check, &isValidChannel);

	if (!isValid) {
	  ATH_MSG_WARNING ("Not a valid STGC ID: " << cid.get_compact() << " " << cid.getString() << " " << stationName << " " << stationEta << " " << stationPhi << " " << multilayer << " " << gasGap << " " << channelType << " " << channel << " " << technologyString << " " << stationNameString);
	  sc = worstStatus(sc, StatusCode::RECOVERABLE);
	}
	if (!isStgc) {
	  ATH_MSG_WARNING ("Not an STGC ID: " << cid.get_compact() << " " << cid.getString() << " " << stationName << " " << stationEta << " " << stationPhi << " " << multilayer << " " << gasGap << " " << channelType << " " << channel << " " << technologyString << " " << stationNameString);
	  sc = worstStatus(sc, StatusCode::RECOVERABLE);
	}
	if (isMdt || isCsc || isRpc || isTgc || isMm) {
	  ATH_MSG_WARNING ("Wrong technology ID: " << cid.get_compact() << " " << cid.getString() << " " << stationName << " " << stationEta << " " << stationPhi << " " << multilayer << " " << gasGap << " " << channelType << " " << channel << " " << technologyString << " " << stationNameString);
	  ATH_MSG_WARNING ("MDT/CSC/RPC/TGC/MM/sTGC " << isMdt << " " << isCsc << " " << isRpc << " " << isTgc << " " << isMm << " " << isStgc);
	  sc = worstStatus(sc, StatusCode::RECOVERABLE);
	}
	if (!isValidChannel) {
	  ATH_MSG_WARNING ("STGC Id check failed: " << cid.get_compact() << " " << cid.getString() << " " << stationName << " " << stationEta << " " << stationPhi << " " << multilayer << " " << gasGap << " " << channelType << " " << channel << " " << technologyString << " " << stationNameString);
	  sc = worstStatus(sc, StatusCode::RECOVERABLE);
	}
	if (cid != newId) {
	  ATH_MSG_WARNING ("STGC Id pack/unpack failed: " << cid.get_compact() << " " << cid.getString() << " " << newId.get_compact() << " " << newId.getString() << " " << stationName << " " << stationEta << " " << stationPhi << " " << multilayer << " " << gasGap << " " << channelType << " " << channel << " " << technologyString << " " << stationNameString);
	  sc = worstStatus(sc, StatusCode::RECOVERABLE);
	}
      }

    }
    ATH_MSG_DEBUG ("stgc nChannels = " << nChannels);

    if ( sc == StatusCode::SUCCESS ) {
      m_sTgcIdSanityCheck = true;
    }

    return sc;
  }

  // * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *

  StatusCode IdHelpersValidation::worstStatus(StatusCode c1, StatusCode c2) {
    if (c1.getCode() == c2.getCode()) {
      return c1;
    } else if (c1.isFailure()) {
      return c1;
    } else if (c2.isFailure()) {
      return c2;
    } else if (c1.isRecoverable()) {
      return c1;
    } else if (c2.isRecoverable()) {
      return c2;
    } else {
      return StatusCode::SUCCESS;
    }
  }

} // end namespace
