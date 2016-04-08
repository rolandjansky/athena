/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "GeoModelUtilities/DecodeVersionKey.h"
#include "GeoModelInterfaces/IGeoModelSvc.h"
#include <string>
#include <iostream>

DecodeVersionKey::DecodeVersionKey(const IGeoModelSvc * geoModel, const std::string & node)
{
  std::string nodeOverrideTag;
  std::string indetOverrideTag; // Indet has two levels.
  if (node == "ATLAS") {
    nodeOverrideTag = "";
  } else if (node == "InnerDetector") {
    nodeOverrideTag = geoModel->inDetVersionOverride();
  } else if (node == "Pixel") {
    indetOverrideTag = geoModel->inDetVersionOverride();
    nodeOverrideTag  = geoModel->pixelVersionOverride();
  } else if (node == "SCT") {
    indetOverrideTag = geoModel->inDetVersionOverride();
    nodeOverrideTag  = geoModel->SCT_VersionOverride();
  } else if (node == "TRT") {
    indetOverrideTag = geoModel->inDetVersionOverride();
    nodeOverrideTag  = geoModel->TRT_VersionOverride();
  } else if (node == "LAr") {
    nodeOverrideTag  = geoModel->LAr_VersionOverride();
  } else if (node == "TileCal") {
    nodeOverrideTag  = geoModel->tileVersionOverride();
  } else if (node == "MuonSpectrometer") {
    nodeOverrideTag  = geoModel->muonVersionOverride();
  } else if (node == "Calorimeter") {
    nodeOverrideTag  = geoModel->caloVersionOverride();
  } else if (node == "ForwardDetectors") {
    nodeOverrideTag  = geoModel->forwardDetectorsVersionOverride();
  } else {
    std::cout << "DecodeVersionKey passed an unknown node:" << node << std::endl; 
    nodeOverrideTag = "";
  }

  // Default to atlas version
  m_tag = geoModel->atlasVersion();
  m_node = "ATLAS";
  
  // If indetOverrideTag is specified (and is not just "CUSTOM") then override with the indet tag.
  std::string indetTag;
  if (!indetOverrideTag.empty()) {
    // We dont care about the return value (custom = true/false). We only take notice of the custom 
    // flag if the override is at the node we have selected. Ie we only look at nodeOverrideTag 
    // in order to set m_custom. At any rate, we have to remove the CUSTOM string if it is present.
    getCustomTag(indetOverrideTag, indetTag);
  }
  if (!indetTag.empty()) {
    m_tag = indetTag;
    m_node = "InnerDetector";
  }

  // Finally if subsystem tag is overriden then use that.
  std::string outputTag;
  m_custom = getCustomTag(nodeOverrideTag, outputTag);

  if (!outputTag.empty()) {
    m_tag  = outputTag;
    m_node = node;
  }
}


const std::string &
DecodeVersionKey::tag() const
{
  return m_tag;
}

const std::string &
DecodeVersionKey::node() const
{
  return m_node;
}

bool
DecodeVersionKey::custom() const
{
  return m_custom;
}



bool 
DecodeVersionKey::getCustomTag(const std::string & inputTag, std::string & outputTag)
{
  //
  // Check if CUSTOM is specified. If it is not specified then outputTag = inputTag.
  // If the tag is just "CUSTOM" then set output tag to "" so that we use the higher level tag.
  // If the tag is of the form CUSTOM-XXXXX use the XXXXX as the tag
  // The separating character (in this example a '-') can be any character. 
  //
  bool custom = false;
  outputTag = inputTag;
  if (!inputTag.empty()) {
    if (inputTag.substr(0,6) == "CUSTOM") {
      custom = true;
      // If its CUSTOM-something skip the next character and get the something
      outputTag = inputTag.substr(6);
      if (!outputTag.empty()) outputTag = outputTag.substr(1);
    }
  }
  return custom;
}

