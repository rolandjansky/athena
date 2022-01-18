/*
  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
*/

#include <list>
#include <iostream>
#include <memory>
#include <nlohmann/json.hpp>

#include "MuonNSWAsBuilt/StripCalculator.h"
#include "MuonNSWAsBuilt/Element.h"
#include "MuonNSWAsBuilt/ElementModelRigid.h"
#include "MuonNSWAsBuilt/ElementModelScaleSag.h"


using namespace NswAsBuilt;

struct StripCalculator::json_t {
  const nlohmann::json& json;
  json_t(const nlohmann::json& j) : json(j) {}
};

//===============================================================================
StripCalculator::strip_t StripCalculator::getStrip(ParameterClass iclass, stripIdentifier_t strip_id) const {
  int ipcb = (strip_id.istrip-1)/1024 + 1; // MM 1024 channels per PCB, hard-coded convention
  if (std::abs(strip_id.quadruplet.stationEta)==2) {
    ipcb += 5;
  }
  pcbIdentifier_t pcb_id { strip_id.quadruplet, strip_id.ilayer, ipcb };
  auto it = m_pcbMap.find(pcb_id);

  if (it == m_pcbMap.end()) {
    return { INVALID, {0.0,0.0,0.0}, {0.0,0.0,0.0}, {0.0,0.0,0.0} };
  }

  auto strip = it->second.getStrip(iclass, strip_id.istrip);
  return { VALID, strip.center, strip.left, strip.right };
}


//===============================================================================
StripCalculator::position_t StripCalculator::getPositionAlongStrip(ParameterClass iclass, stripIdentifier_t strip_id, double sx, double sy) const {
  int ipcb = (strip_id.istrip-1)/1024 + 1; // MM 1024 channels per PCB, hard-coded convention
  if (std::abs(strip_id.quadruplet.stationEta)==2) {
    ipcb += 5;
  }
  pcbIdentifier_t pcb_id { strip_id.quadruplet, strip_id.ilayer, ipcb };
  auto it = m_pcbMap.find(pcb_id);

  if (it == m_pcbMap.end()) {
    return { INVALID, {0.0,0.0,0.0} };
  }

  return { VALID, it->second.getPositionAlongStrip(iclass, strip_id.istrip, sx, sy) };
}


//===============================================================================
void StripCalculator::parseJSON(std::istream& in) {
  try {
    using json = nlohmann::json;
    json jroot;
    in >> jroot;
    for (const json& jmodule : jroot.at("elementarray")) {
      parseRootElement(jmodule);
    }
  } catch (std::exception& e) {
    throw std::runtime_error(e.what());
  }
}



//===============================================================================
// Follows: private methods for parsing the JSON CLOB
std::unique_ptr<Element> StripCalculator::buildElement(json_t j) const {
  // Build the appropriate deformation model
  std::unique_ptr<ElementModel> deformation_model;
  std::string model = j.json.at("model");
  if (model == "nodefo") {
    deformation_model = std::make_unique<ElementModelRigid>();
  } else if (model == "scalesag") {
    double lenX, lenY;
    Amg::Vector3D defo0;
    const auto& jc = j.json.at("model_constant_pars");
    jc.at("len_x").get_to(lenX);
    jc.at("len_y").get_to(lenY);
    jc.at("defo0_x").get_to(defo0[0]);
    jc.at("defo0_y").get_to(defo0[1]);
    jc.at("defo0_z").get_to(defo0[2]);
    deformation_model = std::make_unique<ElementModelScaleSag>(lenX, lenY, defo0);
  } else {
    throw std::runtime_error("Unknown model: "+model);
  }

  // Build the element
  std::unique_ptr<Element> el = std::make_unique<Element>(std::move(deformation_model));
  el->setAsapId(j.json.at("id_asap"));

  // Set the model parameters
  std::map<std::string, double> correctionPars, nominalPars;
  j.json.at("pars_correction").get_to(correctionPars);
  j.json.at("pars_nominal").get_to(nominalPars);
  el->setParametersFromMap(ParameterClass::CORRECTION, correctionPars);
  el->setParametersFromMap(ParameterClass::NOMINAL, nominalPars);

  return el;
}


//===============================================================================
quadrupletIdentifier_t StripCalculator::getQuadrupletIdentifier(json_t j) const {
  quadrupletIdentifier_t ret;
  std::string stationName;
  j.json.at("station_name").get_to(stationName);
  if (stationName == "MMS") {
    ret.stationName = quadrupletIdentifier_t::MMS;
  } else if (stationName == "MML") {
    ret.stationName = quadrupletIdentifier_t::MML;
  } else {
    throw std::runtime_error("StripCalculator: stationName not implemented: "+stationName);
  }
  j.json.at("station_eta").get_to(ret.stationEta);
  j.json.at("station_phi").get_to(ret.stationPhi);
  j.json.at("multilayer").get_to(ret.multilayer);
  return ret;
}


//===============================================================================
pcbIdentifier_t StripCalculator::getPcbIdentifier(quadrupletIdentifier_t quad_id, json_t j) const {
  pcbIdentifier_t ret;
  ret.quadruplet = quad_id;
  j.json.at("ilayerath").get_to(ret.ilayer);
  j.json.at("iboard").get_to(ret.ipcb);
  return ret;
}


//===============================================================================
PcbElement::stripConfiguration_t StripCalculator::getStripConfiguration(json_t j) const {
  auto getPoint = [](const nlohmann::json& j) {
    double xpos, ypos, xpitch, ypitch;
    j.at("pos").at("x").get_to(xpos);
    j.at("pos").at("y").get_to(ypos);
    j.at("pitchvec").at("x").get_to(xpitch);
    j.at("pitchvec").at("y").get_to(ypitch);
    return PcbElement::stripPoint_t{{xpos,ypos,0.0},{xpitch,ypitch,0.0}};
  };
  PcbElement::stripConfiguration_t ret;
  j.json.at("central_strip_number").get_to(ret.centralStripNumber);
  ret.centerPoint = getPoint(j.json.at("strip_center"));
  ret.leftPoint   = getPoint(j.json.at("strip_left"));
  ret.rightPoint  = getPoint(j.json.at("strip_right"));
  return ret;
}


//===============================================================================
void StripCalculator::collectStrip(quadrupletIdentifier_t quad_id, Element& element, json_t j) {
  if (j.json.contains("strip_configuration") && j.json.contains("identifier")) {
    pcbIdentifier_t pcb_id = getPcbIdentifier(quad_id, j.json.at("identifier"));
    PcbElement::stripConfiguration_t config = getStripConfiguration(j.json.at("strip_configuration"));
    m_pcbMap.insert(std::make_pair(pcb_id, PcbElement(config, element)));
  }
}


//===============================================================================
void StripCalculator::parseRootElement(json_t j) {
  using json = nlohmann::json;
  struct tree_t {
    json::const_iterator it;
    json::const_iterator end;
  };

  if (!j.json.contains("identifier")) {
    // No athena identifier given for this element, skip
    return;
  }

  std::unique_ptr<Element> root = buildElement(j);
  quadrupletIdentifier_t quad_id = getQuadrupletIdentifier(j.json.at("identifier"));
  Element* mom = root.get();
  const std::string KEY = "zdaughters";

  // Depth-traversal of json tree, collect elements along the way
  std::list<tree_t> jtree;
  jtree.push_back({j.json.at(KEY).begin(), j.json.at(KEY).end()});
  while (!jtree.empty()) {
    auto& it = jtree.back().it;
    if (it != jtree.back().end) {
      std::unique_ptr<Element> daughter = buildElement(*it);
      collectStrip(quad_id, *daughter, *it);
      Element* daugref = daughter.get();
      mom->addDaughter(std::move(daughter));
      if (it->contains(KEY)) {
        jtree.push_back({it->at(KEY).begin(), it->at(KEY).end()});
        mom = daugref;
      }
      ++it;
    } else {
      jtree.pop_back();
      mom = mom->mother();
    }
  }

  m_rootElements.push_back(std::move(root));
}

