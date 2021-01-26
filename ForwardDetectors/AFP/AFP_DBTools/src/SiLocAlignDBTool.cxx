/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

/**
 * @file   SiLocAlignDBTool.cxx
 * @author Grzegorz Gach <grzegorz.gach@cern.ch>
 * @date   2017-12-03
 *
 * @brief  File with implementation of SiLocAlignDBTool
 */

#include "../AFP_DBTools/SiLocAlignDBTool.h"
#include "../AFP_DBTools/SiLocAlignDataBuilder.h"
#include "../AFP_DBTools/SiLocAlignData.h"

#define BOOST_BIND_GLOBAL_PLACEHOLDERS // Needed to silence Boost pragma message
#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/json_parser.hpp>

#include <sstream>
#include <string>
#include <list>
#include <map>

namespace AFP
{
  const int SiLocAlignDBTool::s_numberOfStations = 4;

  SiLocAlignDBTool::SiLocAlignDBTool(const std::string& type,
				     const std::string& name,
				     const IInterface* parent) :
    AthAlgTool  (type, name, parent),
    m_alignments (s_numberOfStations)
  {
    declareInterface<AFP::ISiLocAlignDBTool>(this);
    declareProperty( "folderName", m_folderName = "/FWD/AFP/LocalAlignment/SiliconPlanes", "Name of the folder in database");
  }

  StatusCode SiLocAlignDBTool::initialize()
  {
    StatusCode regSC = detStore()->regFcn(&SiLocAlignDBTool::update, this, m_conditionsData , m_folderName.data(), true);
    if (regSC.isFailure())
      ATH_MSG_WARNING ("Failed to register SiLocAlignDBTool::update in detStore().");

    return StatusCode::SUCCESS;
  }

  const SiLocAlignData* SiLocAlignDBTool::alignment (const int stationID, const int planeID) const
  {
    assert (stationID < s_numberOfStations);

    try {
      return m_alignments[stationID].at(planeID).get();
    }
    catch (const std::out_of_range& excpetion) {
      ATH_MSG_WARNING ("Access to SiLocAlignDBTool::m_alignments["<<stationID<<"].at("<<planeID<<") is out of range.");
      return nullptr;
    }

    return nullptr;
  }

  void SiLocAlignDBTool::resizeAlignments (const std::vector<int>& maxLayers)
  {
    m_alignments.resize (maxLayers.size());

    int stationID = 0;
    for (int layersN : maxLayers)
      m_alignments[stationID++].resize(layersN);
  }


  StatusCode SiLocAlignDBTool::update (IOVSVC_CALLBACK_ARGS)
  {
    // read database content
    boost::property_tree::ptree inputData;
    const coral::AttributeList& attrList = m_conditionsData->coralList();
    std::stringstream inputJSON (attrList["data"].data<std::string>());
    boost::property_tree::read_json(inputJSON, inputData);
    boost::property_tree::ptree& payload = inputData.get_child("data_array");

    SiLocAlignDataBuilder builder;
    // configure builder and check for warning messages
    std::string builderWarning = builder.setSpecification(inputData.get<std::string>("folder_payloadspec"));
    if (!builderWarning.empty())
      ATH_MSG_WARNING(builderWarning);

    // Create alignment objects based on parsed information from database. Also find number of stations and layers
    std::list<std::unique_ptr<const SiLocAlignData> > alignmentsList;
    int maxStationID = 0;
    std::vector<int> maxLayers (maxStationID+1, 0);
    for (boost::property_tree::ptree::value_type node : payload)
      for (boost::property_tree::ptree::value_type nodeData : node.second) {

	// create new alignment object and read station and layer ID
	std::unique_ptr<const SiLocAlignData> newAlignment = std::make_unique<const SiLocAlignData> (builder.build(nodeData));
	const int stationID = newAlignment->stationID();
	const int layerID = newAlignment->layerID();

	// find highest stationID and highest layerID for a given station
	if (maxStationID < stationID) {
	  maxStationID = stationID;
	  maxLayers.resize(maxStationID + 1, 0);
	}

	if (layerID >= maxLayers[stationID])
	  maxLayers[stationID] = layerID+1;

	// add alignment object to output list
	alignmentsList.push_back(std::move(newAlignment));
      }

    resizeAlignments (maxLayers);

    // rewrite new alignments to the output vector
    for (std::unique_ptr<const SiLocAlignData>& locAlign : alignmentsList)
      m_alignments[locAlign->stationID()][locAlign->layerID()] = std::move(locAlign);

    return StatusCode::SUCCESS;
  }
}
