// This file's extension implies that it's C, but it's really -*- C++ -*-.

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id$
/**
 * @file EventCommonD3PDMaker/src/SimMetadataTool.cxx
 * @author Thomas Gillam <thomas.gillam@cern.ch>
 * @date May, 2012
 * @brief Add simulation flavor metadata to D3PD.
 */


#include "SimMetadataTool.h"
#include "D3PDMakerInterfaces/ID3PD.h"
#include "AthenaKernel/errorcheck.h"

#include "IOVDbDataModel/IOVMetaDataContainer.h"

#include "TString.h"

namespace D3PD {


/**
 * @brief Standard Gaudi tool constructor.
 * @param type The name of the tool type.
 * @param name The tool name.
 * @param parent The tool's Gaudi parent.
 */
SimMetadataTool::SimMetadataTool (const std::string& type,
                                const std::string& name,
                                const IInterface* parent)
  : AthAlgTool (type, name, parent), m_metaDataStore("StoreGateSvc/MetaDataStore", name)
{
  declareProperty ("Metakey", m_metakey = "SimMetadata", "Key for output metadata.");
}


/**
 * @brief Standard Gaudi initialize method.
 */
StatusCode SimMetadataTool::initialize()
{
  if (m_metaDataStore.retrieve().isFailure()) {
    return StatusCode::FAILURE;
  }

  return AthAlgTool::initialize();
}


/**
 * @brief Standard Gaudi @c queryInterface method.
 */
StatusCode
SimMetadataTool::queryInterface( const InterfaceID& riid, void** ppvIf )
{
  if ( riid == IMetadataTool::interfaceID() )  {
    *ppvIf = static_cast<IMetadataTool*> (this);
    addRef();
    return StatusCode::SUCCESS;
  }

  return AlgTool::queryInterface( riid, ppvIf );
}


void SimMetadataTool::getValueMap(const std::string &metadataPath, const std::vector<std::string> &inKeys, std::map<std::string, std::string> &outMap)
{
  const DataHandle<IOVMetaDataContainer> param;
  if (m_metaDataStore->retrieve(param, metadataPath).isFailure()) {
    msg() << MSG::WARNING << "Couldn't retrieve " << metadataPath << endreq;
  } else {
    msg() << MSG::INFO << "Retrieved " << metadataPath << endreq;
    const IOVPayloadContainer *payloadCont = param->payloadContainer();
    
    // As far as I can see, all these elements are the same...
    IOVPayloadContainer::const_iterator itColl = payloadCont->begin();
    const CondAttrListCollection *coll = *itColl;

    CondAttrListCollection::const_iterator itAtt = coll->begin();
    CondAttrListCollection::const_iterator itAttEnd = coll->end();
    for (; itAtt != itAttEnd; ++itAtt) {
      coral::AttributeList::const_iterator itr = itAtt->second.begin();
      coral::AttributeList::const_iterator itrEnd = itAtt->second.end();
      for (; itr != itrEnd; ++itr) {
        std::vector<std::string>::const_iterator key = inKeys.begin();
        std::vector<std::string>::const_iterator keyEnd = inKeys.end();
        for (; key != keyEnd; ++key) {
          if (itr->specification().name() == *key) {
            outMap[*key] = itr->data<std::string>();
          }
        }
      }
    }
  }
}

void SimMetadataTool::printMap(const std::vector<std::string> &intendedKeys, const std::map<std::string, std::string> &arg)
{
  std::vector<std::string>::const_iterator itv = intendedKeys.begin();
  std::vector<std::string>::const_iterator itvEnd = intendedKeys.end();
  for (; itv != itvEnd; ++itv) {
    if (arg.count(*itv) == 0) {
      msg() << MSG::WARNING << *itv << " was not retrieved!" << endreq;
    } else {
      msg() << MSG::INFO << *itv << ": " << arg.find(*itv)->second << endreq;
    }
  }
}

StatusCode SimMetadataTool::addMapToD3PD(ID3PD *d3pd, const std::vector<std::string> &intendedKeys, const std::map<std::string, std::string> &arg)
{
  std::vector<std::string>::const_iterator itv = intendedKeys.begin();
  std::vector<std::string>::const_iterator itvEnd = intendedKeys.end();
  for (; itv != itvEnd; ++itv) {
    if (arg.count(*itv) == 0) {
      msg() << MSG::INFO << "Adding: " << *itv << " " << endreq;

      CHECK( d3pd->addMetadata (m_metakey+"_"+*itv, new TString("")) );
    } else {
      msg() << MSG::INFO << "Adding: " << *itv << ": " << arg.find(*itv)->second << endreq;

      TString value(arg.find(*itv)->second);
      CHECK( d3pd->addMetadata (m_metakey+"_"+*itv, &value) );
      //CHECK( d3pd->addMetadata (*itv, &value) );
    }
  }

  return StatusCode::SUCCESS;
}

/**
 * @brief Write metadata to a D3PD.
 * @param d3pd The D3PD to which to write.
 */
StatusCode SimMetadataTool::writeMetadata (ID3PD* d3pd)
{
  // Paths to metadata in AOD
  const std::string MC_DIGI_PARAM = "/Digitization/Parameters"; 
  const std::string MC_SIM_PARAM = "/Simulation/Parameters"; 

  std::map<std::string, std::string> digiMap;
  std::vector<std::string> digiKeys;
  digiKeys.push_back("N_beamGasInputFiles");
  digiKeys.push_back("N_cavernInputFiles");
  digiKeys.push_back("N_HighPtMinBiasInputFiles");
  digiKeys.push_back("N_LowPtMinBiasInputFiles");
  digiKeys.push_back("N_beamHaloInputFiles");
  digiKeys.push_back("IOVDbGlobalTag");
  digiKeys.push_back("intraTrainBunchSpacing");

  std::map<std::string, std::string> simMap;
  std::vector<std::string> simKeys;
  simKeys.push_back("SimulationFlavour");
  simKeys.push_back("InitFunctions");
  simKeys.push_back("PhysicsList");
  simKeys.push_back("BeamPipeSimMode");
  simKeys.push_back("LArParameterization");
  simKeys.push_back("EventFilter");
  simKeys.push_back("DoLArBirk");
  simKeys.push_back("G4Version");
  simKeys.push_back("SimulationFlavourDetails");
  simKeys.push_back("NeutronTimeCut");
  simKeys.push_back("SimLayout");

  getValueMap(MC_DIGI_PARAM, digiKeys, digiMap);
  printMap(digiKeys, digiMap);

  getValueMap(MC_SIM_PARAM, simKeys, simMap);
  printMap(simKeys, simMap);

  // Are we in MC11? Otherwise assume MC12...
  bool inMC11 = false;
  bool inData = false;
  if (simMap.count("SimulationFlavour") == 0) inMC11 = true;
  if (simMap.count("G4Version") == 0) inData = true;

  // If we're in data, bail out now!
  if (inData) {
    msg() << MSG::INFO << "In data! Quitting..." << endreq;
    return StatusCode::SUCCESS;
  }

  // Determine sim type and save, if in MC11
  if (inMC11) {
    std::string simType = "";
    if (simMap["LArParameterization"] == "0") {
      if (digiMap["IOVDbGlobalTag"] == "OFLCOND-SDR-BS7T-05-22" || digiMap["IOVDbGlobalTag"] == "OFLCOND-SDR-BS7T-05-17") {
        simType = "AtlfastII";
      } else {
        simType = "default";
      }
    } else if (simMap["LArParameterization"] == "3" && digiMap["IOVDbGlobalTag"] != "OFLCOND-SDR-BS7T-05-22" && digiMap["IOVDbGlobalTag"] != "OFLCOND-SDR-BS7T-05-17") {
      simType = "FullSimulation+FCalFrozenShowers";
    }
    msg() << MSG::INFO << "Deduced SimType: " << simType << endreq;
    simMap["SimulationFlavour"] = simType;
  }
  
  // Join the N_* variables. Make a comma separated list of names
  // Include the name if the value is retrieved and is greater than 0
  std::string inputFileTypes;
  std::map<std::string, std::string> fileTypesNameMap;
  fileTypesNameMap["N_beamGasInputFiles"] = "beamGas";
  fileTypesNameMap["N_cavernInputFiles"] = "cavern";
  fileTypesNameMap["N_HighPtMinBiasInputFiles"] = "HighPtMinBias";
  fileTypesNameMap["N_LowPtMinBiasInputFiles"] = "LowPtMinBias";
  fileTypesNameMap["N_beamHaloInputFiles"] = "beamHalo";

  std::map<std::string, std::string>::const_iterator it = fileTypesNameMap.begin();
  std::map<std::string, std::string>::const_iterator itEnd = fileTypesNameMap.end();
  for (; it != itEnd; ++it) {
    if (digiMap.count(it->first) != 0) {
      if (atoi(digiMap[it->first].c_str()) > 0) {
        inputFileTypes += it->second + ",";
      }
    }
  }

  // Remove trailing comma
  if (inputFileTypes.size() > 0)
    inputFileTypes = inputFileTypes.substr(0, inputFileTypes.size()-1);

  msg() << MSG::INFO << "InputFileTypes: " << inputFileTypes << endreq;

  // Build a new digiMap without N_*, and with InputFileTypes
  std::map<std::string, std::string> newDigiMap;
  if (digiMap.count("IOVDbGlobalTag") > 0) newDigiMap["IOVDbGlobalTag"] = digiMap["IOVDbGlobalTag"];
  if (digiMap.count("intraTrainBunchSpacing") > 0) newDigiMap["intraTrainBunchSpacing"] = digiMap["intraTrainBunchSpacing"];
  newDigiMap["InputFileTypes"] = inputFileTypes;
  std::vector<std::string> newDigiKeys;
  newDigiKeys.push_back("IOVDbGlobalTag");
  newDigiKeys.push_back("intraTrainBunchSpacing");
  newDigiKeys.push_back("InputFileTypes");

  // Write to D3PD
  CHECK( addMapToD3PD(d3pd, newDigiKeys, newDigiMap) );
  CHECK( addMapToD3PD(d3pd, simKeys, simMap) );

  return StatusCode::SUCCESS;
}


} // namespace D3PD
