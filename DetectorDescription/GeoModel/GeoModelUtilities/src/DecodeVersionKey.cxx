/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "GeoModelUtilities/DecodeVersionKey.h"
#include "GeoModelInterfaces/IGeoModelSvc.h"
#include "GeoModelInterfaces/IGeoDbTagSvc.h"
#include "GaudiKernel/ISvcLocator.h"
#include "GaudiKernel/Bootstrap.h"
#include <string>
#include <iostream>

DecodeVersionKey::DecodeVersionKey(const IGeoModelSvc * geoModel, const std::string & node)
{
  defineTag<IGeoModelSvc>(geoModel,node);
}

DecodeVersionKey::DecodeVersionKey(const IGeoDbTagSvc * geoDbTag, const std::string & node)
{
  defineTag<IGeoDbTagSvc>(geoDbTag,node);
}

DecodeVersionKey::DecodeVersionKey(const std::string & node)
{
  ISvcLocator* svcLocator = Gaudi::svcLocator();
  IGeoDbTagSvc* geoDbTag{nullptr};
  if(svcLocator->service("GeoDbTagSvc",geoDbTag).isFailure())
    throw std::runtime_error("DecodeVersionKey constructor: cannot access GeoDbTagSvc");
  defineTag<IGeoDbTagSvc>(geoDbTag,node);
}

template <class T>
void DecodeVersionKey::defineTag(const T* svc, const std::string & node)
{
  std::string nodeOverrideTag;
  std::string indetOverrideTag; // Indet has two levels.
  if (node == "ATLAS") {
    nodeOverrideTag = "";
  } else if (node == "InnerDetector") {
    nodeOverrideTag = svc->inDetVersionOverride();
  } else if (node == "Pixel") {
    indetOverrideTag = svc->inDetVersionOverride();
    nodeOverrideTag  = svc->pixelVersionOverride();
  } else if (node == "SCT") {
    indetOverrideTag = svc->inDetVersionOverride();
    nodeOverrideTag  = svc->SCT_VersionOverride();
  } else if (node == "TRT") {
    indetOverrideTag = svc->inDetVersionOverride();
    nodeOverrideTag  = svc->TRT_VersionOverride();
  } else if (node == "LAr") {
    nodeOverrideTag  = svc->LAr_VersionOverride();
  } else if (node == "TileCal") {
    nodeOverrideTag  = svc->tileVersionOverride();
  } else if (node == "MuonSpectrometer") {
    nodeOverrideTag  = svc->muonVersionOverride();
  } else if (node == "Calorimeter") {
    nodeOverrideTag  = svc->caloVersionOverride();
  } else if (node == "ForwardDetectors") {
    nodeOverrideTag  = svc->forwardDetectorsVersionOverride();
  } else {
    std::cout << "DecodeVersionKey passed an unknown node:" << node << std::endl; 
    nodeOverrideTag = "";
  }

  // Default to atlas version
  m_tag = svc->atlasVersion();
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
    if (inputTag.compare(0,6,"CUSTOM") == 0) {
      custom = true;
      // If its CUSTOM-something skip the next character and get the something
      outputTag = inputTag.substr(6);
      if (!outputTag.empty()) outputTag = outputTag.substr(1);
    }
  }
  return custom;
}

