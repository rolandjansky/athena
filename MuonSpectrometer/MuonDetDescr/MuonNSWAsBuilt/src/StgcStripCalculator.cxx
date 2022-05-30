/*
  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
*/

#include <list>
#include <iostream>
#include <memory>
#include <nlohmann/json.hpp>

#include "MuonNSWAsBuilt/StgcStripCalculator.h"
#include "MuonNSWAsBuilt/Element.h"
#include "MuonNSWAsBuilt/ElementModelRigid.h"
#include "MuonNSWAsBuilt/ElementModelSTGC.h"

using namespace NswAsBuilt;

struct StgcStripCalculator::json_t {
  const nlohmann::json& json;
  json_t(const nlohmann::json& j) : json(j) {}
};

StgcStripCalculator::stgcStrip_t StgcStripCalculator::getStgcStrip(ParameterClass iclass, stripIdentifier_t strip_id) const {
  int ipcb = 9; // since sTGC strip boards are identical  
  pcbIdentifier_t pcb_id { strip_id.quadruplet, strip_id.ilayer, ipcb };
  auto it = m_pcbMap.find(pcb_id);

  if (it == m_pcbMap.end()) {
    return { INVALID, {0.0,0.0,0.0}, {0.0,0.0,0.0}, {0.0,0.0,0.0} };
  }

  auto strip = it->second.getStgcStrip(iclass, strip_id.istrip);
  return { VALID, strip.center, strip.left, strip.right };
}
StgcStripCalculator::position_t StgcStripCalculator::getPositionAlongStgcStrip(ParameterClass iclass, stripIdentifier_t strip_id, double s) const {
  int ipcb = 9; // since sTGC strip boards are identical  
  pcbIdentifier_t pcb_id { strip_id.quadruplet, strip_id.ilayer, ipcb };
  auto it = m_pcbMap.find(pcb_id);

  if (it == m_pcbMap.end()) {
    return { INVALID, {0.0,0.0,0.0} };
  }

  return { VALID, it->second.getPositionAlongStgcStrip(iclass, strip_id.istrip, s) };
}

void StgcStripCalculator::parseJSON(std::string& in) {
  try {
    using json = nlohmann::json;
    json jroot = json::parse(in);
    for (const json& jmodule : jroot.at("elementarray")) {
      parseRootElement(jmodule);
    }
  } catch (std::exception& e) {
    throw std::runtime_error(e.what());
  }
}

//
// Follows: private methods for parsing the JSON CLOB
//
std::unique_ptr<Element> StgcStripCalculator::buildElement(json_t j) const {
  // Build the appropriate deformation model
  std::unique_ptr<ElementModel> deformation_model;
  std::string model = j.json.at("model");
  if (model == "nodefo") {
    deformation_model = std::make_unique<ElementModelRigid>();
  } 
  else if (model == "stgc") {
    double lenX, lenY;
    Amg::Vector3D defo0;
    const auto& jc = j.json.at("model_constant_pars");
    jc.at("len_x").get_to(lenX);
    jc.at("len_y").get_to(lenY);
    jc.at("defo0_x").get_to(defo0[0]);
    jc.at("defo0_y").get_to(defo0[1]);
    jc.at("defo0_z").get_to(defo0[2]);
    deformation_model = std::make_unique<ElementModelSTGC>(lenX, lenY, defo0);
  } 
  else {
    throw std::runtime_error("Unknown model: "+model);
  }

  // Build the element
  std::unique_ptr<Element> el = std::make_unique<Element>(std::move(deformation_model));

  // Set the model parameters
  std::map<std::string, double> correctionPars, nominalPars;
  j.json.at("pars_corrected").get_to(correctionPars);
  j.json.at("pars_nominal").get_to(nominalPars);
  el->setParametersFromMap(ParameterClass::CORRECTION, correctionPars);
  el->setParametersFromMap(ParameterClass::NOMINAL, nominalPars);

  return el;
}

quadrupletIdentifier_t StgcStripCalculator::getQuadrupletIdentifier(json_t j) const {
  quadrupletIdentifier_t ret;
  std::string stationName;
  j.json.at("station_name").get_to(stationName);
  if (stationName == "STS") {
    ret.stationName = quadrupletIdentifier_t::STS;
  } else if (stationName == "STL") {
    ret.stationName = quadrupletIdentifier_t::STL;
  } else {
    throw std::runtime_error("StgcStripCalculator: stationName not implemented: "+stationName);
  }
  j.json.at("station_eta").get_to(ret.stationEta);
  j.json.at("station_phi").get_to(ret.stationPhi);
  j.json.at("multilayer").get_to(ret.multilayer);
  return ret;
}
pcbIdentifier_t StgcStripCalculator::getPcbIdentifier(quadrupletIdentifier_t quad_id, json_t j) const {
  pcbIdentifier_t ret;
  ret.quadruplet = quad_id;
  j.json.at("ilayer").get_to(ret.ilayer);
  j.json.at("ipcb").get_to(ret.ipcb);
  return ret;
}
CathodeBoardElement::stgcStripConfiguration_t StgcStripCalculator::getStgcStripConfiguration(json_t j) const {
  auto getPoint = [](const nlohmann::json& j) {
    double xpos, ypos, xpitch, ypitch;
    j.at("pos").at("x").get_to(xpos);
    j.at("pos").at("y").get_to(ypos);
    j.at("pitch").at("x").get_to(xpitch);
    j.at("pitch").at("y").get_to(ypitch);
    return CathodeBoardElement::stgcStripPoint_t{{xpos,ypos,0.0},{xpitch,ypitch,0.0}};
  };
  CathodeBoardElement::stgcStripConfiguration_t ret;
  j.json.at("nstrip").get_to(ret.lastStripNumber);
  ret.fCenterPoint = getPoint(j.json.at("first_strip_center"));
  ret.fLeftPoint   = getPoint(j.json.at("first_strip_left"));
  ret.fRightPoint  = getPoint(j.json.at("first_strip_right"));
  ret.sCenterPoint = getPoint(j.json.at("second_strip_center"));
  ret.sLeftPoint   = getPoint(j.json.at("second_strip_left"));
  ret.sRightPoint  = getPoint(j.json.at("second_strip_right"));
  ret.lCenterPoint = getPoint(j.json.at("last_strip_center"));
  ret.lLeftPoint   = getPoint(j.json.at("last_strip_left"));
  ret.lRightPoint  = getPoint(j.json.at("last_strip_right"));
  return ret;
}

void StgcStripCalculator::collectStrip(quadrupletIdentifier_t quad_id, Element& element, json_t j) {
  if (j.json.contains("strip_configuration") && j.json.contains("identifier")) {
    pcbIdentifier_t pcb_id = getPcbIdentifier(quad_id, j.json.at("identifier"));
    CathodeBoardElement::stgcStripConfiguration_t config = getStgcStripConfiguration(j.json.at("strip_configuration"));
    m_pcbMap.insert(std::make_pair(pcb_id, CathodeBoardElement(config, element)));
  }
}

void StgcStripCalculator::parseRootElement(json_t j) {
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
