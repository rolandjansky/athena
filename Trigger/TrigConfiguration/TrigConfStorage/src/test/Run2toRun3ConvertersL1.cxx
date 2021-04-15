// Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration

#include "Run2toRun3ConvertersL1.h"
#include <numeric>
#include <unordered_set>
#include <boost/tokenizer.hpp>
#include <tuple>
#include <stdexcept>

#include "TrigConfData/L1BunchGroupSet.h"
#include "TrigConfData/DataStructure.h"
#include "TrigConfData/L1PrescalesSet.h"
#define BOOST_BIND_GLOBAL_PLACEHOLDERS // Needed to silence Boost pragma message
#include <boost/property_tree/json_parser.hpp>
#include "TrigConfL1Data/CTPConfig.h"
#include "TrigConfL1Data/TriggerThreshold.h"
#include "TrigConfL1Data/TriggerItem.h"
#include "TrigConfL1Data/TriggerItemNode.h"
#include "TrigConfL1Data/ClusterThresholdValue.h"
#include "TrigConfL1Data/JetThresholdValue.h"
#include "TrigConfL1Data/HelperFunctions.h"
#include "TrigConfL1Data/CaloInfo.h"

#include "TrigConfL1Data/BunchGroupSet.h"
#include "TrigConfL1Data/BunchGroup.h"

#include "TrigConfIO/JsonFileWriterL1.h"

#include <nlohmann/json.hpp>

using json = nlohmann::json; // to be changed to ordered_json
using ptree = boost::property_tree::ptree;

namespace {
   std::tuple<std::string, std::vector<std::string>> 
   decodeItemDefinition(const TrigConf::TriggerItem * item) {
      std::string definition{""};
      std::vector<std::string> bgps;
      const TrigConf::TriggerItemNode * topNode = item->topNode();
      std::vector<const TrigConf::TriggerItemNode*> nodes;
      topNode->getAllFinalNodes(nodes);
      std::map<unsigned int, const TrigConf::TriggerItemNode*> nodeMap;
      for(auto * node : nodes) {
         nodeMap[node->position()] = node;
      }
      // build tokens with separators ()&|! and <space>. Keeps all separators except <space> in the list of tokens 
      for ( auto & tok : boost::tokenizer<boost::char_separator<char> > (item->definition(), boost::char_separator<char>(" ", "()&|!")) ) {
         if(isdigit(tok[0])) {
            // replace by threshold
            int idx = std::stoi(tok);
            const TrigConf::TriggerItemNode* node = nodeMap[idx];
            if(node->triggerThreshold()->type()=="BGRP") {
               bgps.push_back(node->thresholdName());
               if(definition.back()=='&') {
                  definition.pop_back();
               }
            } else {
               definition += node->thresholdName() + "[x" + std::to_string(node->multiplicity()) + "]";
            }
         } else  {
            // keep token ()&|!
            definition += tok;
         }
      }
      // remove outer parentheses
      if(definition.front()=='(' and definition.back()==')') {
         definition = definition.substr(1,definition.size()-2);
      }
      return make_tuple(definition, bgps);
   }
}


/**
 * Conversion of L1 menu
 **/
void
convertRun2L1MenuToRun3(const TrigConf::CTPConfig* ctpConfig, const TXC::L1TopoMenu* topoMenu, 
                        const std::string& filename, bool writeTmpFile)
{

   if( !ctpConfig ) {
      std::cout << "No CTPConfig, no L1Menu file will be produced" << std::endl;
      return;
   }

   unsigned int run = ctpConfig->ctpVersion() <=3 ? 1 : 2;

   // items
   json items = json::object_t{};
   for( const TrigConf::TriggerItem * sourceItem: ctpConfig->menu().items() ) {
      json item({});
      item["name"] = sourceItem->name();
      item["legacy"] = true;
      item["ctpid"] = sourceItem->ctpId();
      auto [definition, bunchgroups] = decodeItemDefinition(sourceItem);
      item["definition"] = definition;
      item["bunchgroups"] = bunchgroups;
      item["triggerType"] = TrigConf::uint2bin(sourceItem->triggerType(),8);
      item["partition"] = sourceItem->partition();
      std::string smon("LF:");
      smon += (sourceItem->monitor() & 0x04 ? '1' : '0');
      smon += (sourceItem->monitor() & 0x02 ? '1' : '0');
      smon += (sourceItem->monitor() & 0x01 ? '1' : '0');
      smon += "|HF:";
      smon += (sourceItem->monitor() & 0x20 ? '1' : '0');
      smon += (sourceItem->monitor() & 0x10 ? '1' : '0');
      smon += (sourceItem->monitor() & 0x08 ? '1' : '0');
      item["monitor"] = smon;
      items[sourceItem->name()] = item;
   };

   // thresholds
   json thresholds = json::object_t{};
   std::unordered_set<std::string> legacyThrTypes{"EM", "TAU", "JET", "JB", "JE", "JF", "XE", "XS", "TE", "ZB", "R2TOPO"};
   for( const TrigConf::TriggerThreshold * sourceThr : ctpConfig->menu().thresholdVector()) {
      std::string thrType = sourceThr->type();
      if(thrType=="BGRP" || thrType=="RNDM") {
         continue;
      }
      if(thrType=="MUON") {
         thrType = "MU";
      } 
      else if(thrType=="TOPO") {
         thrType = "R2TOPO";
      }

      bool isLegacyCalo = legacyThrTypes.count(thrType) > 0;
      json &jThisType = isLegacyCalo ? thresholds["legacyCalo"][thrType] : thresholds[thrType];
      bool firstOfItsType = jThisType.empty();
      if (firstOfItsType)
      {
         jThisType["type"] = thrType;
      }
      json thr;
      size_t mapping = sourceThr->mapping();
      thr["mapping"] = mapping;
      if(thrType=="MU") {
         if (firstOfItsType)
         {
            jThisType["type"] = "MU";
            jThisType["exclusionLists"] = json::object_t{};
            jThisType["roads"]["rpc"] = json::object_t{};
            jThisType["roads"]["tgc"] = json::object_t{};
         }
         int ptCut = sourceThr->triggerThresholdValue(0,0)->ptCutCount();
         thr["baThr"] = ptCut;
         thr["ecThr"] = ptCut;
         thr["fwThr"] = ptCut;
         thr["baIdx"] = mapping;
         thr["ecIdx"] = mapping;
         thr["fwIdx"] = mapping;
         thr["tgcFlags"] = "";
         thr["rpcFlags"] = "";
         thr["region"] = "ALL";
         jThisType["roads"]["rpc"][std::to_string(ptCut)] = mapping;
         jThisType["roads"]["tgc"][std::to_string(ptCut)] = mapping;
      }
      else if (thrType == "EM")
      {
         thr["thrValues"] = json::array_t{};
         for (const TrigConf::TriggerThresholdValue *tv : sourceThr->thresholdValueVector())
         {
            auto cl = dynamic_cast<const TrigConf::ClusterThresholdValue *>(tv);
            json jtv;
            jtv["value"] = static_cast<unsigned int>(tv->ptcut());
            std::string isobits = "00000";
            auto isomask = cl->isolationMask();
            for (size_t b = 0; b < 5; ++b)
            {
               if (isomask & (1 << b))
               {
                  isobits[4 - b] = '1';
               }
            }
            jtv["isobits"] = isobits;
            jtv["etamin"] = tv->etamin();
            jtv["etamax"] = tv->etamax();
            jtv["phimin"] = tv->phimin();
            jtv["phimax"] = tv->phimax();
            jtv["priority"] = static_cast<unsigned int>(tv->priority());
            thr["thrValues"] += jtv;
         }
      }
      else if (thrType == "TAU")
      {
         auto cl = dynamic_cast<const TrigConf::ClusterThresholdValue*>(sourceThr->triggerThresholdValue(0,0));
         int ptCut = (int)cl->ptcut();
         thr["value"] = ptCut;
         std::string isobits = "00000";
         auto isomask = cl->isolationMask();
         for (size_t b = 0; b < 5; ++b)
         {
            if (isomask & (1 << b))
            {
               isobits[4 - b] = '1';
            }
         }
         thr["isobits"] = isobits;
      }
      else if (thrType == "JET" || thrType == "JB" ||  thrType == "JF")
      {
         thr["thrValues"] = json::array_t{};
         for (const TrigConf::TriggerThresholdValue *tv : sourceThr->thresholdValueVector())
         {
            auto jetThrVal = dynamic_cast<const TrigConf::JetThresholdValue *>(sourceThr->triggerThresholdValue(0, 0));
            json jtv;
            jtv["value"] = static_cast<unsigned int>(tv->ptcut());
            jtv["etamin"] = tv->etamin();
            jtv["etamax"] = tv->etamax();
            jtv["phimin"] = tv->phimin();
            jtv["phimax"] = tv->phimax();
            jtv["window"] = jetThrVal->windowSize() == TrigConf::JetWindowSize::SMALL ? 4 : 8; // 4 and 8 are the two possible jet window sizes
            jtv["priority"] = static_cast<unsigned int>(tv->priority());
            thr["thrValues"] += jtv;
         }
      }
      else if (thrType == "XE" || thrType == "TE" ||  thrType == "XS" || thrType == "JE")
      {
         thr["value"] = static_cast<unsigned int>(sourceThr->thresholdValueVector().at(0)->ptcut());
      }
      else if (thrType == "ZB")
      {
         thr["seed"] = sourceThr->zbSeedingThresholdName();
         thr["seedMultiplicity"] = sourceThr->zbSeedingThresholdMulti();
         thr["seedBcdelay"] = sourceThr->bcDelay();
      }
      jThisType["thresholds"][sourceThr->name()] = thr;
   }

   // extra info for thresholds
   const TrigConf::CaloInfo& ci = ctpConfig->menu().caloInfo();
   thresholds["legacyCalo"]["EM"]["ptMinToTopo"] = ci.minTobEM().ptmin;
   thresholds["legacyCalo"]["EM"]["resolutionMeV"] = (int)(1000/ci.globalEmScale());
   thresholds["legacyCalo"]["TAU"]["ptMinToTopo"] = ci.minTobTau().ptmin;
   thresholds["legacyCalo"]["JET"]["ptMinToTopoSmallWindow"] = ci.minTobJetSmall().ptmin;
   thresholds["legacyCalo"]["JET"]["ptMinToTopoLargeWindow"] = ci.minTobJetLarge().ptmin;
   if(run==2) {
      json isoHAforEM{ {"thrtype", "HAIsoForEMthr"}, {"Parametrization", json::array_t{}} };
      json isoEMforEM{ {"thrtype", "EMIsoForEMthr"}, {"Parametrization", json::array_t{}} };
      json isoEMforTAU{ {"thrtype", "EMIsoForTAUthr"}, {"Parametrization", json::array_t{}} };
      for(const TrigConf::IsolationParam & iso : ci.isolationHAIsoForEMthr()) {
         json p{ {"etamax", iso.etamax()}, {"etamin", iso.etamin()}, {"isobit", iso.isobit()}, {"mincut", iso.mincut()}, 
               {"offset", iso.offset()}, {"priority", iso.priority()}, {"slope", iso.slope()}, {"upperlimit", iso.upperlimit()} };
         isoHAforEM["Parametrization"] += p;
      }
      for(const TrigConf::IsolationParam & iso : ci.isolationEMIsoForEMthr()) {
         json p{ {"etamax", iso.etamax()}, {"etamin", iso.etamin()}, {"isobit", iso.isobit()}, {"mincut", iso.mincut()}, 
               {"offset", iso.offset()}, {"priority", iso.priority()}, {"slope", iso.slope()}, {"upperlimit", iso.upperlimit()} };
         isoEMforEM["Parametrization"] += p;
      }
      for(const TrigConf::IsolationParam & iso : ci.isolationEMIsoForTAUthr()) {
         json p{ {"etamax", iso.etamax()}, {"etamin", iso.etamin()}, {"isobit", iso.isobit()}, {"mincut", iso.mincut()}, 
               {"offset", iso.offset()}, {"priority", iso.priority()}, {"slope", iso.slope()}, {"upperlimit", iso.upperlimit()} };
         isoEMforTAU["Parametrization"] += p;
      }
      thresholds["legacyCalo"]["EM"]["isolation"]["HAIsoForEMthr"] = isoHAforEM;
      thresholds["legacyCalo"]["EM"]["isolation"]["EMIsoForEMthr"] = isoEMforEM;
      thresholds["legacyCalo"]["TAU"]["isolation"]["EMIsoForTAUthr"] = isoEMforTAU;
   }
   const TrigConf::METSigParam &xs = ci.metSigParam();
   thresholds["legacyCalo"]["XS"]["significance"] = json::object_t{
       {"xsSigmaScale", xs.xsSigmaScale()}, {"xsSigmaOffset", xs.xsSigmaOffset()}, {"xeMin", xs.xeMin()}, {"xeMax", xs.xeMax()}, {"teSqrtMin", xs.teSqrtMin()}, {"teSqrtMax", xs.teSqrtMax()}};

   // boards
   json boards = json::object_t{};
   boards["Ctpin7"] = json::object_t{ {"type", "CTPIN"}, {"legacy", true},
                                      {"connectors", std::vector<std::string>{"EM1", "EM2", "TAU1", "TAU2"}} };
   boards["Ctpin8"] = json::object_t{ {"type", "CTPIN"}, {"legacy", true},
                                      {"connectors", std::vector<std::string>{"JET1", "JET2", "EN1", "EN2"}} };
   boards["Ctpin9"] = json::object_t{ {"type", "CTPIN"}, {"legacy", true},
                                      {"connectors", std::vector<std::string>{"MUCTPI", "CTPCAL", "NIM1", "NIM2"}} };
   boards["LegacyTopo0"] = json::object_t{ {"type", "TOPO"}, {"legacy", true},
                                           {"connectors", std::vector<std::string>{"LegacyTopo0"}} };
   boards["LegacyTopo1"] = json::object_t{ {"type", "TOPO"}, {"legacy", true},
                                           {"connectors", std::vector<std::string>{"LegacyTopo1"}} };

   // connectors
   json connectors = json::object_t{};
   std::map<std::string,std::vector<const TrigConf::TriggerThreshold*>> triggerlinesMap;
   for( const TrigConf::TriggerThreshold * sourceThr : ctpConfig->menu().thresholdVector()) {
      if(sourceThr->isInternal()) {
         continue;
      }
      std::string cableName = sourceThr->cableName();
      if(cableName=="ALFA") {
         cableName = "AlfaCtpin";
      }
      else if(cableName=="TOPO1") {
         cableName = "LegacyTopo0";
      }
      else if(cableName=="TOPO2") {
         cableName = "LegacyTopo1";
      }
      triggerlinesMap[cableName].push_back(sourceThr);
   }
   for( auto & [type, triggerlines] : triggerlinesMap) {
      std::sort(std::begin(triggerlines), std::end(triggerlines),
                [](const TrigConf::TriggerThreshold *a, const TrigConf::TriggerThreshold *b) { return a->cableStart() < b->cableStart(); });
      for( const TrigConf::TriggerThreshold * thr : triggerlines) {
         if(!connectors.contains(type)) {
            if(thr->input()=="ctpcore") {
               connectors[type]["type"] =  "electrical";
               if(type=="AlfaCtpin") {
                  connectors[type]["triggerlines"]["clock0"] = json::array_t{};
                  connectors[type]["triggerlines"]["clock1"] = json::array_t{};
               } else {
                  connectors[type]["triggerlines"]["fpga0"]["clock0"] = json::array_t{};
                  connectors[type]["triggerlines"]["fpga0"]["clock1"] = json::array_t{};
                  connectors[type]["triggerlines"]["fpga1"]["clock0"] = json::array_t{};
                  connectors[type]["triggerlines"]["fpga1"]["clock1"] = json::array_t{};
               }
            } else if(thr->input()=="ctpin") {
               connectors[type]["type"] =  "ctpin";
               connectors[type]["triggerlines"] = json::array_t{};
            } else {
               throw std::runtime_error("Unknown connector type" + thr->input());
            }
            connectors[type]["legacy"] = true;
         }
         size_t start = thr->cableStart();
         size_t nbits = thr->cableEnd() - thr->cableStart() + 1;
         if(thr->input()=="ctpcore") {
            unsigned int clock = thr->clock();
            if(type=="AlfaCtpin") {
               json tl{{"name", thr->name()}, {"startbit", start}, {"nbits", nbits}};
               connectors[type]["triggerlines"]["clock" + std::to_string(clock)] += tl;
            } else {
               size_t fpga = 0;
               if(start>=16) {
                  start -= 16;
                  fpga++;
               }
               json tl{{"name", "R2TOPO_" + thr->name()}, {"startbit", start}, {"nbits", nbits}};
               connectors[type]["triggerlines"]["fpga" + std::to_string(fpga)]["clock" + std::to_string(clock)] += tl;
            }
         } else {
            json tl{{"name", thr->name()}, {"startbit", start}, {"nbits", nbits}};
            connectors[type]["triggerlines"] += tl;
         }
      }
   }

   // algorithms
   json::object_t decAlgos;
   json::object_t sortAlgos;
   for(const TXC::L1TopoConfigAlg & alg : topoMenu->getL1TopoConfigAlgs()) {
      json jAlg;
      jAlg["algId"] = alg.algoID();
      jAlg["klass"] = alg.type();
      jAlg["fixedParameters"]["generics"] = json::object_t{};
      size_t pos{0};
      for(const TXC::FixedParameter & fixP : alg.getFixedParameters()) {
         try
         {
            auto vInt = std::stoi(fixP.value);
            jAlg["fixedParameters"]["generics"][fixP.name] = json::object_t{{"value", vInt}, {"position", pos++}};
         }
         catch (std::invalid_argument &)
         {
            jAlg["fixedParameters"]["generics"][fixP.name] = json::object_t{{"value", fixP.value}, {"position", pos++}};
         }
      }
      jAlg["variableParameters"] = json::array_t{};
      for(const TXC::RegisterParameter & regP : alg.getParameters()) {
         // Work around overflow in the database...
         if (regP.name == "MaxMSqr") {
           unsigned val = std::stoul(regP.value);
           if (val >= 1u<<31) {
             val = 999;
           }
           jAlg["variableParameters"] += json::object_t{{"name", regP.name}, {"selection", regP.selection}, {"value", val}};
         }
         else {
           jAlg["variableParameters"] += json::object_t{{"name", regP.name}, {"selection", regP.selection}, {"value", std::stoi(regP.value)}};
         }
      }
      if(alg.isSortAlg()) {
         jAlg["input"] = alg.getInputNames()[0];
         jAlg["output"] = alg.output();
         sortAlgos[alg.name()] = jAlg;
      } else if(alg.isDecAlg()) {
         jAlg["input"] = alg.getInputNames();
         jAlg["output"] = alg.getOutputNames();
         decAlgos[alg.name()] = jAlg;
      }
   }
   json topo = json::object_t{};
   if(run==2) {
      topo["R2TOPO"]["decisionAlgorithms"] = decAlgos;
      topo["R2TOPO"]["sortingAlgorithms"] = sortAlgos;
   }

   // ctp
   json ctp = json::object_t{};
   ctp["inputs"]["ctpin"]["slot7"] = json::object_t{{"connector0", "EM1"}, {"connector1", "EM2"}, 
                                                    {"connector2", "TAU1"}, {"connector3", "TAU2"}};
   ctp["inputs"]["ctpin"]["slot8"] = json::object_t{{"connector0", "JET1"}, {"connector1", "JET2"}, 
                                                    {"connector2", "EN1"}, {"connector3", "EN2"}};
   ctp["inputs"]["ctpin"]["slot9"] = json::object_t{{"connector0", "MUCTPI"}, {"connector1", "CTPCAL"}, 
                                                    {"connector2", "NIM1"}, {"connector3", "NIM2"}};
   if(run==2) {
      ctp["inputs"]["electrical"] = json::object_t{{"connector0", "AlfaCtpin"}, {"connector1", "LegacyTopo0"}, {"connector2", "LegacyTopo1"}};
   } else {
      ctp["inputs"]["electrical"] = json::object_t{};
   }
   ctp["inputs"]["optical"] = json::object_t{};
   ctp["monitoring"]["ctpmon"] = json::object_t{};

   // putting the menu together
   json menu = json::object_t{};
   menu["filetype"] = "l1menu";
   menu["run"] = run;
   menu["name"] = ctpConfig->name();
   menu["items"] = items;
   menu["thresholds"] = thresholds;
   menu["topoAlgorithms"] = topo;
   menu["boards"] = boards;
   menu["connectors"] = connectors;
   menu["ctp"] = ctp;


   if(writeTmpFile) {
      std::ofstream outfile("tmp" + filename);
      outfile << std::setw(4) << menu << std::endl;
      std::cout << "Wrote tmp" << filename << std::endl;
   }
   std::stringstream ss;
   ss << menu;
   ptree top;
   boost::property_tree::read_json(ss, top);
   TrigConf::L1Menu l1menu(std::move(top));
   TrigConf::JsonFileWriterL1 writer;
   writer.writeJsonFile(filename, l1menu);
}

/**
 * Conversion of bunchgroup set
 **/
std::vector<std::pair<int, int>>
toRanges(const std::vector<int>& bunches) {
   // converts sequence of bunches into the pairs of continous ranges
   if ( bunches.empty() ) {
      return {};
   }
   if (bunches.size() == 1) {
      return { {bunches.front(), bunches.front()} };
   }

   // nontrival ranges
   std::vector<std::pair<int, int>> ranges;
   std::vector<int> sorted = bunches;
   std::sort(sorted.begin(), sorted.end());

   std::vector<int> differences;
   std::adjacent_difference( sorted.begin(), sorted.end(), std::back_inserter(differences));

   int start = sorted.front();

   for (size_t i = 1; i < differences.size(); ++i) {
      if (differences[i] == 1) continue;
      ranges.emplace_back( std::pair(start, sorted[i-1]) );
      start = sorted[i];
   }
   ranges.emplace_back( std::pair(start, sorted.back()) );
   return ranges;
}

void
convertRun2BunchGroupsToRun3(const TrigConf::CTPConfig* ctpConfig, const std::string& filename, bool writeTmpFile)
{
   const TrigConf::BunchGroupSet & bgs = ctpConfig->bunchGroupSet();
   if(bgs.bunchGroups().size()==0) {
      std::cout << "BunchgroupSet is empty, no file will be produced" << std::endl;
      return;
   }

   json bgset;
   bgset["filetype"] ="bunchgroupset";
   bgset["name"] = bgs.name();

   json jGroups;
   const std::vector<TrigConf::BunchGroup>& bgVec = bgs.bunchGroups();
   for (auto & group : bgVec) {
      auto ranges = toRanges(group.bunches());
      json jGroup;
      jGroup["name"] =group.name();
      jGroup["id"] =group.internalNumber();
      jGroup["info"] = std::to_string(group.bunches().size()) + " bunches, " + std::to_string(ranges.size()) + " groups";

      json jBCIDS = json::array_t{};
      for ( auto [start, end] : ranges) {
         jBCIDS += json{{"first", start}, {"length", 1+end-start}};
      }
      jGroup["bcids"] = jBCIDS;

      jGroups[std::string("BGRP")+std::to_string(group.internalNumber())] = jGroup;
   }
   bgset["bunchGroups"] = jGroups;

   if(writeTmpFile) {
      std::ofstream outfile("tmp" + filename);
      outfile << std::setw(4) << bgset << std::endl;
      std::cout << "Wrote tmp" << filename << std::endl;
   }
   std::stringstream ss;
   ss << bgset;
   ptree top;
   boost::property_tree::read_json(ss, top);
   TrigConf::L1BunchGroupSet newbgs(std::move(top));
   TrigConf::JsonFileWriterL1 writer;
   writer.writeJsonFile(filename, newbgs);
}

/**
 * Conversion of L1 prescales set
 **/
void
convertRun2L1PrescalesToRun3(const TrigConf::CTPConfig* ctpConfig, const std::string& filename, bool writeTmpFile) {
   const TrigConf::PrescaleSet & l1pss = ctpConfig->prescaleSet();
   if(l1pss.isNull()) {
      std::cout << "L1PrescaleSet is empty, no file will be produced" << std::endl;
      return;
   }

   json psset;
   psset["filetype"] = "l1prescale";
   psset["name"] = l1pss.name();
   json jCuts;
   for (size_t id = 0; id < l1pss.prescales_float().size(); ++id) {
      json jCut;
      auto itemPtr = ctpConfig->menu().item(id);
      if ( itemPtr != nullptr ) {
         int32_t cut = l1pss.cuts().at(id);
         jCut["cut"] = abs(cut);
         jCut["enabled"] = cut > 0;
         double ps = static_cast<double>(0xFFFFFF) / ( 0x1000000 - cut );
         jCut["info"] = "prescale: "+std::to_string(ps);
         jCuts[itemPtr->name()] = jCut;
      }
   }
   psset["cutValues"] = jCuts;

   if(writeTmpFile) {
      std::ofstream outfile("tmp" + filename);
      outfile << std::setw(4) << psset << std::endl;
      std::cout << "Wrote tmp" << filename << std::endl;
   }
   std::stringstream ss;
   ss << psset;
   ptree top;
   boost::property_tree::read_json(ss, top);
   TrigConf::L1PrescalesSet ps(std::move(top));
   TrigConf::JsonFileWriterL1 writer;
   writer.writeJsonFile(filename, ps);
}
