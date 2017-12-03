/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/**
 * @file   SiLocAlignDBTool.cxx
 * @author Grzegorz Gach <grzegorz.gach@cern.ch>
 * @date   2017-12-03
 * 
 * @brief  File with implementation of SiLocAlignDBTool
 */

#include "AFP_DBTools/SiLocAlignDBTool.h"

namespace AFP
{
  const int SiLocAlignDBTool::s_numberOfStations = 4;
  
  SiLocAlignDBTool::SiLocAlignDBTool(const std::string& type,
				     const std::string& name,
				     const IInterface* parent) : 
    ::AthAlgTool  (type, name, parent),
    m_alignments (s_numberOfStations)
  {
    declareProperty( "folderName", m_folderName = "/FWD/AFP/LocalAlignment/SiliconPlanes", "Name of the folder in database");
  }

  StatusCode SiLocAlignDBTool::initialize()
  {
    StatusCode regSC = detStore()->regFcn(&SiLocAlignDBTool::update, this, m_conditionsData , m_folderName.data());
    if (regSC.isFailure())
      ATH_MSG_WARNING ("Failed to register SiLocAlignDBTool::update in detStore().");

    return StatusCode::SUCCESS;
  }

  const AlignmentData* SiLocAlignDBTool::alignment (const int stationID, const int planeID) const
  {
    assert (stationID < s_numberOfStations);
    
    try {
      return m_alignments[stationID].at(planeID);
    }
    catch (const std::out_of_range& excpetion) {
      ATH_MSG_WARNING ("Access to SiLocAlignDBTool::m_alignments["<<stationID<<"].at("<<planeID<<") is out of range.");
      return nullptr;
    }

    return nullptr;
  }

  StatusCode SiLocAlignDBTool::update (IOVSVC_CALLBACK_ARGS)
  {
    // clear old alignments
    clearAlignments();

    // find layer with the highest ID for each station
    int stationsN = m_alignments.size();
    std::vector<int> maxLayers (stationsN, 0);
    for (const auto& chanAndAttList : *m_conditionsData) {
      const coral::AttributeList& attrList = chanAndAttList.second;
      const int stationID = attrList["stationID"].data<int>();
      const int layerID = attrList["layerID"].data<int>();

      // check if there are more stations in database than in vector
      if (stationID >= stationsN)
	maxLayers.resize(stationID + 1, 0);
				  
      if (layerID >= maxLayers[stationID])
	maxLayers[stationID] = layerID+1;
    }

    {
      int stationID = 0;
      for (int layersN : maxLayers)
	m_alignments[stationID++].resize(layersN, nullptr);
    }

    // set new alignments
    for (const auto& chanAndAttList : *m_conditionsData) {
      const coral::AttributeList& attrList = chanAndAttList.second;
      const int stationID = attrList["stationID"].data<int>();
      const int layerID = attrList["layerID"].data<int>();

      const double xShift = attrList["xShift"].data<double>();
      const double yShift = attrList["yShift"].data<double>();
      const double zShift = attrList["zShift"].data<double>();
      const double alpha = attrList["alpha"].data<double>();
      const double beta = attrList["beta"].data<double>();
      const double gamma = attrList["gamma"].data<double>();

      m_alignments[stationID][layerID] = new AlignmentData (xShift, yShift, zShift, alpha, beta, gamma);
    }

    return StatusCode::SUCCESS;
  }

  void SiLocAlignDBTool::clearAlignments ()
  {
    for (std::vector<const AlignmentData*>& dataVector : m_alignments)
      for (const AlignmentData*& data : dataVector) {
	delete data;
	data = nullptr;
      }
  }

}
