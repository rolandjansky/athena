/*
  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
*/

#include "ITkPixelOfflineCalibCondAlg.h"
#include "Identifier/Identifier.h"
#include "Identifier/IdentifierHash.h"
#include "GaudiKernel/EventIDRange.h"
#include "PathResolver/PathResolver.h"
#include <memory>
#include <sstream>

namespace ITk
{

PixelOfflineCalibCondAlg::PixelOfflineCalibCondAlg(const std::string& name, ISvcLocator* pSvcLocator):
  ::AthReentrantAlgorithm(name, pSvcLocator)
{
}


StatusCode PixelOfflineCalibCondAlg::initialize() {
  ATH_MSG_DEBUG("ITkPixelOfflineCalibCondAlg::initialize()");

  ATH_CHECK(service("DetectorStore", m_detStore));
  ATH_CHECK(m_detStore->retrieve(m_pixelid, "PixelID")) ;

  if (m_inputSource==2 && m_readKey.key().empty()) {
    ATH_MSG_ERROR("The database is set to be input source (2) but the ReadKey is empty.");
  }
  ATH_CHECK(m_readKey.initialize());

  ATH_CHECK(m_writeKey.initialize());

  return StatusCode::SUCCESS;
}



StatusCode PixelOfflineCalibCondAlg::execute(const EventContext& ctx) const {
  ATH_MSG_DEBUG("ITkPixelOfflineCalibCondAlg::execute()");

  SG::WriteCondHandle<PixelOfflineCalibData> writeHandle(m_writeKey, ctx);
  if (writeHandle.isValid()) {
    ATH_MSG_DEBUG("CondHandle " << writeHandle.fullKey() << " is already valid.. In theory this should not be called, but may happen if multiple concurrent events are being processed out of order.");
    return StatusCode::SUCCESS;
  }

  // Construct the output Cond Object and fill it in
  std::unique_ptr<PixelOfflineCalibData> writeCdo(std::make_unique<PixelOfflineCalibData>());

  if (m_inputSource==0) {
    ATH_MSG_WARNING("So far do nothing!! return StatusCode::FAILURE");
    return StatusCode::FAILURE;
  }
  else if (m_inputSource==1) {
    ATH_MSG_WARNING("Pixel ITk constants read from text file. Only supported for local developments and debugging!");

    auto calibData = std::make_unique<PixelOfflineCalibData>();
    PixelClusterErrorData* pced = calibData->getClusterErrorData();

    // Find and open the text file
    ATH_MSG_INFO("Load ITkPixelErrorData constants from text file");
    std::string fileName = PathResolver::find_file(m_textFileName, "DATAPATH");
    if (fileName.empty()) { ATH_MSG_WARNING("Input file " << fileName << " not found! Default (hardwired) values to be used!"); }
    else { pced->load(fileName);  }

    ATH_MSG_DEBUG("Get error constants");
    std::vector<float> constants = calibData->getConstants();
    if (!constants.empty()) { ATH_MSG_VERBOSE("constants are defined"); }
    else                  { ATH_MSG_ERROR("constants size is NULL!!!"); }


    const EventIDBase start{EventIDBase::UNDEFNUM, EventIDBase::UNDEFEVT, 0,                       0,                       EventIDBase::UNDEFNUM, EventIDBase::UNDEFNUM};
    const EventIDBase stop {EventIDBase::UNDEFNUM, EventIDBase::UNDEFEVT, EventIDBase::UNDEFNUM-1, EventIDBase::UNDEFNUM-1, EventIDBase::UNDEFNUM, EventIDBase::UNDEFNUM};
    const EventIDRange rangeW{start, stop};

    ATH_MSG_DEBUG("Range of input is " << rangeW);

    if (!constants.empty()) {
      ATH_MSG_DEBUG("Found constants with new-style Identifier key");
      writeCdo->setConstants(constants);
    }

    if (writeHandle.record(rangeW, std::move(writeCdo)).isFailure()) {
      ATH_MSG_FATAL("Could not record PixelCalib::ITkPixelOfflineCalibData " << writeHandle.key() << " with EventRange " << rangeW << " into Conditions Store");
      return StatusCode::FAILURE;
    }
    ATH_MSG_DEBUG("recorded new CDO " << writeHandle.key() << " with range " << rangeW << " into Conditions Store");

    if (m_dump!=0) {
      ATH_MSG_DEBUG("Dump the constants to file");
      calibData->dump();
    }
  }

  else if (m_inputSource==2) {

    SG::ReadCondHandle<CondAttrListCollection> readHandle{m_readKey, ctx};
    const CondAttrListCollection* readCdo{*readHandle};
    if (readCdo==nullptr) {
      ATH_MSG_FATAL("Null pointer to the read conditions object");
      return StatusCode::FAILURE;
    }

    // Get the validitiy range
    EventIDRange rangeW;
    if (not readHandle.range(rangeW)) {
       ATH_MSG_FATAL("Failed to retrieve validity range for " << readHandle.key());
       return StatusCode::FAILURE;
    }

    ATH_MSG_DEBUG("Range of input is " << rangeW);

    std::vector<float> constants;

    for(CondAttrListCollection::const_iterator attrList = readCdo->begin(); attrList != readCdo->end(); ++attrList){

      std::ostringstream attrStr;
      (*attrList).second.toOutputStream(attrStr);
      ATH_MSG_DEBUG( "ChanNum " << (*attrList).first << " Attribute list " << attrStr.str() );

      std::string stringData = (*attrList).second["data_array"].data<std::string>();
      std::string delimiter = "],";
      size_t pos = 0;
      std::vector<std::string> component;
      std::string buffer;
      while ((pos = stringData.find(delimiter)) != std::string::npos) {
	buffer = stringData.substr(0,pos);
	component.push_back(buffer);
	stringData.erase(0, pos + delimiter.length());
      }
      component.push_back(stringData);
      ATH_MSG_INFO("Last component="<<stringData);

      for (size_t i=0; i<component.size(); i++) {
	std::string checkModule = component[i];
	std::vector<std::string> moduleString;
	delimiter = ":[";
	pos = 0;
	while ((pos = checkModule.find(delimiter)) != std::string::npos) {
	  buffer = checkModule.substr(0,pos);
	  moduleString.push_back(buffer);
	  checkModule.erase(0, pos + delimiter.length());
	}
	moduleString.push_back(checkModule);

	if (moduleString.size()!=2) {
	  ATH_MSG_FATAL("String size (moduleString) is not 2. " << moduleString.size() << " in " << component[i] << " channel " <<  (*attrList).first << " read from " << readHandle.fullKey());
	  return StatusCode::FAILURE;
	}

	std::stringstream checkModuleHash(moduleString[0]);
	std::vector<std::string> moduleStringHash;
	while (std::getline(checkModuleHash,buffer,'"')) { moduleStringHash.push_back(buffer); }

	int waferHash   = std::atoi(moduleStringHash[1].c_str());
	IdentifierHash waferID_hash(waferHash);
	Identifier pixelID = m_pixelid->wafer_id(waferID_hash);
	constants.emplace_back( pixelID.get_compact() );

	std::stringstream moduleConstants(moduleString[1]);
	std::vector<float> moduleConstantsVec;
	while (std::getline(moduleConstants,buffer,',')) {  moduleConstantsVec.emplace_back(std::atof(buffer.c_str())); }

	// Format v1 with no incident angle dependance
	if(moduleConstantsVec.size()==4){
	  constants.emplace_back(0); // period_phi
	  constants.emplace_back(0); // period_sinheta
	  constants.emplace_back(0); // delta_x_slope
	  constants.emplace_back(moduleConstantsVec[0]); // delta_x_offset
	  constants.emplace_back(moduleConstantsVec[1]); // delta_error_x
	  constants.emplace_back(0); // delta_y_slope
	  constants.emplace_back(moduleConstantsVec[2]); // delta_y_offset
	  constants.emplace_back(moduleConstantsVec[3]); // delta_error_y
	}

	else if(moduleConstantsVec.size()==7){
	  constants.emplace_back(moduleConstantsVec[0]); // period_phi
	  for( auto& x : moduleConstantsVec ) constants.emplace_back(x);
	}

	// Format v3 with incident angle dependance + different eta-phi periods
	else if(moduleConstantsVec.size()==8){
	  for( auto& x : moduleConstantsVec ) constants.emplace_back(x);
	}

      }

    }

    writeCdo->setConstants(constants);

    if (writeHandle.record(rangeW, std::move(writeCdo)).isFailure()) {
      ATH_MSG_FATAL("Could not record PixelCalib::ITkPixelOfflineCalibData " << writeHandle.key() << " with EventRange " << rangeW << " into Conditions Store");
      return StatusCode::FAILURE;
    }
    ATH_MSG_DEBUG("recorded new CDO " << writeHandle.key() << " with range " << rangeW << " into Conditions Store");

  }

  return StatusCode::SUCCESS;
}

} // namespace ITk
