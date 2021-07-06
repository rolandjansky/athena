/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

/**
 * @file   SiGlobAlignDBTool.cxx
 * @author Petr Balek <petr.balek@cern.ch>
 * @date   2021-02-23
 *
 * @brief  File with implementation of SiGlobAlignDBTool
 */

#include "AFP_DBTools/SiGlobAlignDBTool.h"
#include "AFP_DBTools/SiGlobAlignDataBuilder.h"
#include "AFP_DBTools/SiGlobAlignData.h"

#define BOOST_BIND_GLOBAL_PLACEHOLDERS // Needed to silence Boost pragma message
#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/json_parser.hpp>

#include <sstream>
#include <string>
#include <list>
#include <map>

namespace AFP
{
  const int SiGlobAlignDBTool::s_numberOfStations = 4;

  SiGlobAlignDBTool::SiGlobAlignDBTool(const std::string& type,
                                       const std::string& name,
                                       const IInterface* parent) :
    AthAlgTool  (type, name, parent),
    m_alignments (s_numberOfStations)
  {
    declareInterface<AFP::ISiGlobAlignDBTool>(this);
    declareProperty( "folderName", m_folderName = "/FWD/AFP/GlobalAlignment/", "Name of the folder in database");
  }

  StatusCode SiGlobAlignDBTool::initialize()
  {
    StatusCode regSC = detStore()->regFcn(&SiGlobAlignDBTool::update, this, m_conditionsData , m_folderName.data(), true);
    if (regSC.isFailure())
      ATH_MSG_WARNING ("Failed to register SiGlobAlignDBTool::update in detStore().");

    return StatusCode::SUCCESS;
  }

  const SiGlobAlignData* SiGlobAlignDBTool::alignment (const int stationID) const
  {
    assert (stationID < s_numberOfStations);

    try {
      return m_alignments[stationID].get();
    }
    catch (const std::out_of_range& excpetion) {
      ATH_MSG_WARNING ("Access to SiGlobAlignDBTool::m_alignments["<<stationID<<"] is out of range.");
      return nullptr;
    }

    return nullptr;
  }


  StatusCode SiGlobAlignDBTool::update (IOVSVC_CALLBACK_ARGS)
  {
    // read database content
    boost::property_tree::ptree inputData;
    const coral::AttributeList& attrList = m_conditionsData->coralList();
    std::stringstream inputJSON (attrList["data"].data<std::string>());
    boost::property_tree::read_json(inputJSON, inputData);
    boost::property_tree::ptree& payload = inputData.get_child("data_array");

    SiGlobAlignDataBuilder builder;
    // configure builder and check for warning messages
    std::string builderWarning = builder.setSpecification(inputData.get<std::string>("folder_payloadspec"));
    if (!builderWarning.empty())
      ATH_MSG_WARNING(builderWarning);

    // Create alignment objects based on parsed information from database. Also find number of stations
    std::list<std::unique_ptr<const SiGlobAlignData> > alignmentsList;
    int maxStationID = 0;
    for (boost::property_tree::ptree::value_type node : payload)
      for (boost::property_tree::ptree::value_type nodeData : node.second) {

        // create new alignment object and read station ID
        std::unique_ptr<const SiGlobAlignData> newAlignment = std::make_unique<const SiGlobAlignData> (builder.build(nodeData));
        const int stationID = newAlignment->stationID();

        // find highest stationID for a given station
        if (maxStationID < stationID) {
          maxStationID = stationID;
        }

        // add alignment object to output list
        alignmentsList.push_back(std::move(newAlignment));
      }

    // rewrite new alignments to the output vector
    for (std::unique_ptr<const SiGlobAlignData>& globAlign : alignmentsList)
      m_alignments[globAlign->stationID()] = std::move(globAlign);

    return StatusCode::SUCCESS;
  }
}

