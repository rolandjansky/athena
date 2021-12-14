// Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration

#include "TrigConfIO/JsonFileWriterL1.h"

#include <iomanip>
#include <fstream>
#include <algorithm>
#include <sstream>

#include <nlohmann/json.hpp>
using json = nlohmann::json;

using namespace std;

TrigConf::JsonFileWriterL1::JsonFileWriterL1() : 
   TrigConfMessaging( "JsonFileWriterL1")
{}


bool
TrigConf::JsonFileWriterL1::writeJsonFile(const std::string & filename, const L1Menu & l1menu) const
{

   json items = json::object_t{};
   for( auto & item : l1menu ) {
      json jItem({});
      jItem["name"] = item.name();
      jItem["ctpid"] = item.ctpId();
      jItem["definition"] = item.definition();
      jItem["monitor"] = item.monitor();
      jItem["partition"] = item.partition();
      jItem["triggerType"] = item.triggerType();
      jItem["bunchgroups"] = json(item.bunchgroups());
      if(auto legacy = item.legacy() )
         jItem["legacy"] = *legacy;
      items[item.name()] = jItem;
   };

   json thresholds = json::object_t{};
   for(const std::string & thrType : l1menu.thresholdTypes()) {
      json jThresholsByType({});
      // first the thresholds of this type
      for(auto & thr : l1menu.thresholds(thrType)) {
         json jThr({});
         jThr["mapping"] = thr->mapping();

         if(thr->hasChild("sectors")) { // for MBTS_A and MBTS_C
            std::vector<std::string> sectors; 
            for(auto & s : thr->getList("sectors")) {
               sectors.push_back(s.getValue());
            }
            jThr["sectors"] = sectors;
         } 
         if(thr->hasAttribute("voltage")) { // for MBTSII
            jThr["voltage"] = thr->getAttribute<float>("voltage");
         }

         // value
         try {
            if(thr->hasAttribute("value")) {
               auto & caloThr = dynamic_cast<const TrigConf::L1Threshold_Calo &>(*thr); // for MBTSII
               jThr["value"] = int(caloThr.thrValue());
            }
         } catch(std::bad_cast&) {};

         // MU
         try {
            auto muThr = dynamic_cast<const TrigConf::L1Threshold_MU &>(*thr);
            jThr["baThr"] = muThr.ptBarrel();
            jThr["ecThr"] = muThr.ptEndcap();
            jThr["fwThr"] = muThr.ptForward(); 
            jThr["baIdx"] = muThr.idxBarrel();
            jThr["ecIdx"] = muThr.idxEndcap();
            jThr["fwIdx"] = muThr.idxForward();
            jThr["region"] = muThr.region();
            jThr["tgcFlags"] = muThr.tgcFlags();
            jThr["rpcFlags"] = muThr.rpcFlags();
            if(const std::string & roiExcl = muThr.rpcExclROIList(); !roiExcl.empty()) {
               jThr["rpcExclROIList"] = roiExcl;
            }
         } catch(std::bad_cast&) {};

         // TAU
         try {
            auto tauThr = dynamic_cast<const TrigConf::L1Threshold_TAU &>(*thr);
            std::string isobits = "00000";
            auto isomask = tauThr.isolationMask();
            for(size_t b=0; b<5; ++b) {
               if(isomask & (1<<b)) { isobits[4-b] = '1'; }
            }
            jThr["isobits"] = isobits;
            jThr["thrValues"] = json::array_t({});
            for(auto & rv : tauThr.thrValues()) {
               json jRV({});
               jRV["value"] = static_cast<unsigned int>(rv.value());
               jRV["etamin"] = rv.etaMin();
               jRV["etamax"] = rv.etaMax();
               jRV["phimin"] = 0; // never used, so not read 
               jRV["phimax"] = 64; // never used, so not read
               jRV["priority"] = rv.priority();
               jThr["thrValues"] += jRV;
            }
         } catch(std::bad_cast&) {};

         // EM
         try {
            auto EMThr = dynamic_cast<const TrigConf::L1Threshold_EM &>(*thr);
            jThr["thrValues"] = json::array_t({});
            for(auto & rv : EMThr.thrValues()) {
               json jRV({});
               jRV["value"] = static_cast<unsigned int>(rv.value());
               jRV["etamin"] = rv.etaMin();
               jRV["etamax"] = rv.etaMax();
               jRV["phimin"] = 0; // never used, so not read 
               jRV["phimax"] = 64; // never used, so not read
               std::string isobits = "00000";
               auto isomask = EMThr.isolationMask(rv.etaMin());
               for(size_t b=0; b<5; ++b) {
                  if(isomask & (1<<b)) { isobits[4-b] = '1'; }
               }
               jRV["isobits"] = isobits;
               jRV["priority"] = rv.priority();
               jThr["thrValues"] += jRV;
            }
         } catch(std::bad_cast&) {};

         // JET
         try {
            auto JThr = dynamic_cast<const TrigConf::L1Threshold_JET &>(*thr);
            jThr["thrValues"] = json::array_t({});
            for(auto & rv : JThr.thrValues()) {
               json jRV({});
               jRV["value"] = static_cast<unsigned int>(rv.value());
               jRV["etamin"] = rv.etaMin();
               jRV["etamax"] = rv.etaMax();
               jRV["phimin"] = 0; // never used, so not read 
               jRV["phimax"] = 64; // never used, so not read
               jRV["window"] = JThr.window(0);
               jRV["priority"] = rv.priority();
               jThr["thrValues"] += jRV;
            }
         } catch(std::bad_cast&) {};

         // TE
         try {
            auto teThr = dynamic_cast<const TrigConf::L1Threshold_TE &>(*thr);
            //jThr["thrValues"] = json::array_t({});
            for(auto & rv : teThr.thrValues()) {
               json jRV({});
               jRV["value"] = static_cast<unsigned int>(rv.value());
               jRV["etamin"] = rv.etaMin();
               jRV["etamax"] = rv.etaMax();
               jRV["priority"] = rv.priority();
               jThr["thrValues"] += jRV;
            }
         } catch(std::bad_cast&) {};

         // ZB
         try {
            auto zbThr = dynamic_cast<const TrigConf::L1Threshold_ZB &>(*thr);
            jThr["seed"] = zbThr.seed();
            jThr["seedBcdelay"] = zbThr.seedBcdelay();
            jThr["seedMultiplicity"] = zbThr.seedMultiplicity();
         } catch(std::bad_cast&) {};

         // eEM
         try {
            auto eEMThr = dynamic_cast<const TrigConf::L1Threshold_eEM &>(*thr);
            jThr["reta"] = TrigConf::Selection::wpToString(eEMThr.reta());
            jThr["rhad"] = TrigConf::Selection::wpToString(eEMThr.rhad());
            jThr["wstot"] = TrigConf::Selection::wpToString(eEMThr.wstot());
            jThr["thrValues"] = json::value_t::array;
            for(auto & rv : eEMThr.thrValues()) {
               json jRV({});
               jRV["value"] = static_cast<unsigned int>(rv.value());
               jRV["etamin"] = rv.etaMin();
               jRV["etamax"] = rv.etaMax();
               jRV["priority"] = rv.priority();
               jThr["thrValues"] += jRV;
            }
         } catch(std::bad_cast&) {};

         // jEM
         try {
            auto jEMThr = dynamic_cast<const TrigConf::L1Threshold_jEM &>(*thr);
            jThr["iso"] = TrigConf::Selection::wpToString(jEMThr.iso());
            jThr["frac"] = TrigConf::Selection::wpToString(jEMThr.frac());
            jThr["frac2"] = TrigConf::Selection::wpToString(jEMThr.frac2());
            jThr["thrValues"] = json::value_t::array;
            for(auto & rv : jEMThr.thrValues()) {
               json jRV({});
               jRV["value"] = static_cast<unsigned int>(rv.value());
               jRV["etamin"] = rv.etaMin();
               jRV["etamax"] = rv.etaMax();
               jRV["priority"] = rv.priority();
               jThr["thrValues"] += jRV;
            }
         } catch(std::bad_cast&) {};

         // eTAU
         try {
            auto eTAUThr = dynamic_cast<const TrigConf::L1Threshold_eTAU &>(*thr);
            jThr["rCore"] = TrigConf::Selection::wpToString(eTAUThr.rCore());
            jThr["rHad"] = TrigConf::Selection::wpToString(eTAUThr.rHad());
            jThr["thrValues"] = json::array_t({});
            for(auto & rv : eTAUThr.thrValues()) {
               json jRV({});
               jRV["value"] = static_cast<unsigned int>(rv.value());
               jRV["etamin"] = rv.etaMin();
               jRV["etamax"] = rv.etaMax();
               jRV["phimin"] = 0; // never used, so not read 
               jRV["phimax"] = 64; // never used, so not read
               jRV["priority"] = rv.priority();
               jThr["thrValues"] += jRV;
            }
         } catch(std::bad_cast&) {};
         
         // jTAU
         try {
            auto jTAUThr = dynamic_cast<const TrigConf::L1Threshold_jTAU &>(*thr);
            jThr["isolation"] = TrigConf::Selection::wpToString(jTAUThr.isolation());
            jThr["thrValues"] = json::array_t({});
            for(auto & rv : jTAUThr.thrValues()) {
               json jRV({});
               jRV["value"] = static_cast<unsigned int>(rv.value());
               jRV["etamin"] = rv.etaMin();
               jRV["etamax"] = rv.etaMax();
               jRV["phimin"] = 0; // never used, so not read 
               jRV["phimax"] = 64; // never used, so not read
               jRV["priority"] = rv.priority();
               jThr["thrValues"] += jRV;
            }
         } catch(std::bad_cast&) {};         

         // cTAU
         try {
            auto cTAUThr = dynamic_cast<const TrigConf::L1Threshold_cTAU &>(*thr);
            jThr["isolation"] = TrigConf::Selection::wpToString(cTAUThr.isolation());
            jThr["thrValues"] = json::array_t({});
            for(auto & rv : cTAUThr.thrValues()) {
               json jRV({});
               jRV["value"] = static_cast<unsigned int>(rv.value());
               jRV["etamin"] = rv.etaMin();
               jRV["etamax"] = rv.etaMax();
               jRV["phimin"] = 0; // never used, so not read 
               jRV["phimax"] = 64; // never used, so not read
               jRV["priority"] = rv.priority();
               jThr["thrValues"] += jRV;
            }
         } catch(std::bad_cast&) {};

         // jJ
         try {
            auto jJThr = dynamic_cast<const TrigConf::L1Threshold_jJ &>(*thr);
            jThr["thrValues"] = json::array_t({});
            for(auto & rv : jJThr.thrValues()) {
               json jRV({});
               jRV["value"] = static_cast<unsigned int>(rv.value());
               jRV["etamin"] = rv.etaMin();
               jRV["etamax"] = rv.etaMax();
               jRV["priority"] = rv.priority();
               jThr["thrValues"] += jRV;
            }
         } catch(std::bad_cast&) {};

         // jLJ
         try {
            auto jLJThr = dynamic_cast<const TrigConf::L1Threshold_jLJ &>(*thr);
            jThr["thrValues"] = json::array_t({});
            for(auto & rv : jLJThr.thrValues()) {
               json jRV({});
               jRV["value"] = static_cast<unsigned int>(rv.value());
               jRV["etamin"] = rv.etaMin();
               jRV["etamax"] = rv.etaMax();
               jRV["priority"] = rv.priority();
               jThr["thrValues"] += jRV;
            }
         } catch(std::bad_cast&) {};

         // gJ
         try {
            auto gJThr = dynamic_cast<const TrigConf::L1Threshold_gJ &>(*thr);
            jThr["value"] = int(gJThr.thrValue());
         } catch(std::bad_cast&) {};

         // gLJ
         try {
            auto gLJThr = dynamic_cast<const TrigConf::L1Threshold_gLJ &>(*thr);
            jThr["value"] = int(gLJThr.thrValue());
         } catch(std::bad_cast&) {};         

         // jXE
         try {
            auto jXEThr = dynamic_cast<const TrigConf::L1Threshold_jXE &>(*thr);
            jThr["value"] = int(jXEThr.thrValue());
         } catch(std::bad_cast&) {};

         // jTE
         try {
            auto jTEThr = dynamic_cast<const TrigConf::L1Threshold_jTE &>(*thr);
            jThr["value"] = int(jTEThr.thrValue());
         } catch(std::bad_cast&) {};         
         
         // gXE
         try {
            auto gXEThr = dynamic_cast<const TrigConf::L1Threshold_gXE &>(*thr);
            jThr["value"] = int(gXEThr.thrValue());
         } catch(std::bad_cast&) {};         

         // gTE
         try {
            auto gTEThr = dynamic_cast<const TrigConf::L1Threshold_gTE &>(*thr);
            jThr["value"] = int(gTEThr.thrValue());
         } catch(std::bad_cast&) {};

         jThresholsByType[thr->name()] = jThr;
      };
      json jThrType({});
      if(thrType != "internal") {
         jThrType["thresholds"] = jThresholsByType;
      }
      jThrType["type"] = thrType;

      // extra info 
      auto & extraInfo = l1menu.thrExtraInfo().thrExtraInfo(thrType);
      if(extraInfo.hasAttribute("ptMinToTopo")) { // for MBTSII
         jThrType["ptMinToTopo"] = extraInfo.getAttribute<unsigned int>("ptMinToTopo");
      }
      if(extraInfo.hasAttribute("resolutionMeV")) { // for MBTSII
         jThrType["resolutionMeV"] = extraInfo.getAttribute<unsigned int>("resolutionMeV");
      }

      // extra info using type specific accessors
      if(thrType == "internal") {
         jThrType["names"] = json::array_t({});
         for(auto & thr : l1menu.thresholds(thrType)) {
            jThrType["names"] += thr->name();
         }
         auto & extInfo = l1menu.thrExtraInfo().thrExtraInfo("internal");
         if(auto randoms = extInfo.getExtraInfo("randoms")) {
            for(size_t rc=0; rc<4; ++rc) {
               jThrType["randoms"]["RNDM" + std::to_string(rc)]["cut"] = 
                  randoms->get().getAttribute<unsigned int>("RNDM" + std::to_string(rc)+".cut");
            }
         }
      }

      if(thrType == "MU") {
         auto & muinfo = l1menu.thrExtraInfo().MU();
         jThrType["exclusionLists"] = json::value_t::object;
         for(auto & listName : muinfo.exclusionListNames()) {
            jThrType["exclusionLists"][listName] = json::array_t({});
            for(auto & x : muinfo.exclusionList(listName)) {
               jThrType["exclusionLists"][listName] += json({ {"sectorName", x.first}, {"rois", x.second}});
            }
         }
         for(auto & rpcPt : muinfo.knownRpcPtValues()) {
            jThrType["roads"]["rpc"][std::to_string(rpcPt)] = muinfo.rpcIdxForPt(rpcPt);
         }
         for(auto & tgcPt : muinfo.knownTgcPtValues()) {
            jThrType["roads"]["tgc"][std::to_string(tgcPt)] = muinfo.tgcIdxForPt(tgcPt);
         }
      }

      if(thrType == "EM" && l1menu.run()>1) {
         auto & eminfo = l1menu.thrExtraInfo().EM();
         for(const std::string isoSet : { "EMIsoForEMthr", "HAIsoForEMthr" }) {
            jThrType["isolation"][isoSet]["thrtype"] = isoSet;
            jThrType["isolation"][isoSet]["Parametrization"] = json::array_t({});
            for(size_t bit = 1; bit<=5; ++bit) {
               auto & iso = eminfo.isolation(isoSet,bit);
               json jIso({});
               jIso["etamax"] = iso.etamax();
               jIso["etamin"] = iso.etamin();
               jIso["isobit"] = iso.isobit();
               jIso["mincut"] = iso.mincut();
               jIso["offset"] = iso.offset();
               jIso["priority"] = iso.priority();
               jIso["slope"] = iso.slope();
               jIso["upperlimit"] = iso.upperlimit();
               jThrType["isolation"][isoSet]["Parametrization"] += jIso;
            }
         }
      }

      if(thrType == "TAU" && l1menu.run()>1) {
         auto & tauinfo = l1menu.thrExtraInfo().TAU();
         const std::string isoSet{ "EMIsoForTAUthr" };
         jThrType["isolation"][isoSet]["thrtype"] = isoSet;
         jThrType["isolation"][isoSet]["Parametrization"] = json::array_t({});
         for(size_t bit = 1; bit<=5; ++bit) {
            auto & iso = tauinfo.isolation(isoSet,bit);
            json jIso({});
            jIso["etamax"] = iso.etamax();
            jIso["etamin"] = iso.etamin();
            jIso["isobit"] = iso.isobit();
            jIso["mincut"] = iso.mincut();
            jIso["offset"] = iso.offset();
            jIso["priority"] = iso.priority();
            jIso["slope"] = iso.slope();
            jIso["upperlimit"] = iso.upperlimit();
            jThrType["isolation"][isoSet]["Parametrization"] += jIso;
         }
      }

      if(thrType == "JET") {
         auto & jetinfo = l1menu.thrExtraInfo().JET();
         jThrType["ptMinToTopoSmallWindow"] = (int)jetinfo.ptMinToTopoSmallWindow();
         jThrType["ptMinToTopoLargeWindow"] = (int)jetinfo.ptMinToTopoLargeWindow();
      }

      if(thrType == "XS") {
         auto & xsinfo = l1menu.thrExtraInfo().XS();
         jThrType["significance"]["xeMin"] = xsinfo.xeMin();
         jThrType["significance"]["xeMax"] = xsinfo.xeMax();
         jThrType["significance"]["teSqrtMin"] = xsinfo.teSqrtMin();
         jThrType["significance"]["teSqrtMax"] = xsinfo.teSqrtMax();
         jThrType["significance"]["xsSigmaScale"] = xsinfo.xsSigmaScale();
         jThrType["significance"]["xsSigmaOffset"] = xsinfo.xsSigmaOffset();
      }

      if(thrType == "eEM") {
         auto & eeminfo = l1menu.thrExtraInfo().eEM();
         jThrType["maxEt"] = (int)eeminfo.maxEt();
         for( auto wp : {TrigConf::Selection::WP::LOOSE, TrigConf::Selection::WP::MEDIUM, TrigConf::Selection::WP::TIGHT} ) {
            auto wpstr = TrigConf::Selection::wpToString(wp);
            jThrType["workingPoints"][wpstr] = json::array_t({});
            for(auto & iso : eeminfo.isolation(wp)) {
               json jWPIso({});
               std::stringstream stream;
               stream << std::fixed << std::setprecision(3) << iso.value().reta_d();
               jWPIso["reta"] = std::stod(stream.str());
               jWPIso["reta_fw"] = iso.value().reta_fw();
               stream.str("");
               stream.clear();
               stream << std::fixed << std::setprecision(3) << iso.value().rhad_d();
               jWPIso["rhad"] = std::stod(stream.str());
               jWPIso["rhad_fw"] = iso.value().rhad_fw();
               stream.str(""); 
               stream.clear();
               stream << std::fixed << std::setprecision(3) << iso.value().wstot_d();               
               jWPIso["wstot"] = std::stod(stream.str());
               jWPIso["wstot_fw"] = iso.value().wstot_fw();
               jWPIso["etamin"] = iso.etaMin();
               jWPIso["etamax"] = iso.etaMax();
               jWPIso["priority"] = iso.priority();
               jThrType["workingPoints"][wpstr] += jWPIso;
            }
         }
      }

      if(thrType == "jEM") {
         auto & jeminfo = l1menu.thrExtraInfo().jEM();
         jThrType["maxEt"] = (int)jeminfo.maxEt();
         jThrType["ptMinToTopo1"] = (int)jeminfo.ptMinToTopo("1A");
         jThrType["ptMinToTopo2"] = (int)jeminfo.ptMinToTopo("2A");
         jThrType["ptMinToTopo3"] = (int)jeminfo.ptMinToTopo("3A");
         jThrType["ptMinxTOB1"] = (int)jeminfo.ptMinxTOB("1A");
         jThrType["ptMinxTOB2"] = (int)jeminfo.ptMinxTOB("2A");
         jThrType["ptMinxTOB3"] = (int)jeminfo.ptMinxTOB("3A");
         for( auto wp : {TrigConf::Selection::WP::LOOSE, TrigConf::Selection::WP::MEDIUM, TrigConf::Selection::WP::TIGHT} ) {
            auto wpstr = TrigConf::Selection::wpToString(wp);
            jThrType["workingPoints"][wpstr] = json::array_t({});
            for(auto & iso : jeminfo.isolation(wp)) {
               json jWPIso({});
               std::stringstream stream;
               stream << std::fixed << std::setprecision(3) << iso.value().iso_d();
               jWPIso["iso"] = std::stod(stream.str());
               jWPIso["iso_fw"] = iso.value().iso_fw();
               stream.str("");
               stream.clear();
               stream << std::fixed << std::setprecision(3) << iso.value().frac_d();
               jWPIso["frac"] = std::stod(stream.str());
               jWPIso["frac_fw"] = iso.value().frac_fw();
               stream.str("");
               stream.clear();
               stream << std::fixed << std::setprecision(3) << iso.value().frac2_d();
               jWPIso["frac2"] = std::stod(stream.str());
               jWPIso["frac2_fw"] = iso.value().frac2_fw();
               jWPIso["etamin"] = iso.etaMin();
               jWPIso["etamax"] = iso.etaMax();
               jWPIso["priority"] = iso.priority();
               jThrType["workingPoints"][wpstr] += jWPIso;
            }
         }
      }

      if(thrType == "eTAU") {
         auto & eeminfo = l1menu.thrExtraInfo().eTAU();
         jThrType["maxEt"] = (int)eeminfo.maxEt();
         for( auto wp : {TrigConf::Selection::WP::LOOSE, TrigConf::Selection::WP::MEDIUM, TrigConf::Selection::WP::TIGHT, 
                         TrigConf::Selection::WP::HADLOOSE, TrigConf::Selection::WP::HADMEDIUM, TrigConf::Selection::WP::HADTIGHT} ) {
            auto wpstr = TrigConf::Selection::wpToString(wp);
            jThrType["workingPoints"][wpstr] = json::array_t({});
            for(auto & iso : eeminfo.isolation(wp)) {
               json jWPIso({});
               std::stringstream stream;
               stream << std::fixed << std::setprecision(3) << iso.value().rCore_d();
               jWPIso["rCore"] = std::stod(stream.str());
               jWPIso["rCore_fw"] = iso.value().rCore_fw();
               stream.str("");
               stream.clear();
               stream << std::fixed << std::setprecision(3) << iso.value().rHad_d();
               jWPIso["rHad"] = std::stod(stream.str());
               jWPIso["rHad_fw"] = iso.value().rHad_fw();
               jThrType["workingPoints"][wpstr] += jWPIso;
            }
         }
      }

      if(thrType == "jTAU") {
         auto & jtauinfo = l1menu.thrExtraInfo().jTAU();
         jThrType["maxEt"] = (int)jtauinfo.maxEt();
         jThrType["ptMinToTopo1"] = (int)jtauinfo.ptMinToTopo("1A");
         jThrType["ptMinToTopo2"] = (int)jtauinfo.ptMinToTopo("2A");
         jThrType["ptMinToTopo3"] = (int)jtauinfo.ptMinToTopo("3A");
         jThrType["ptMinxTOB1"] = (int)jtauinfo.ptMinxTOB("1A");
         jThrType["ptMinxTOB2"] = (int)jtauinfo.ptMinxTOB("2A");
         jThrType["ptMinxTOB3"] = (int)jtauinfo.ptMinxTOB("3A");
         for( auto wp : {TrigConf::Selection::WP::LOOSE, TrigConf::Selection::WP::MEDIUM, TrigConf::Selection::WP::TIGHT} ) {
            auto wpstr = TrigConf::Selection::wpToString(wp);
            jThrType["workingPoints"][wpstr] = json::array_t({});
            for(auto & iso : jtauinfo.isolation(wp)) {
               json jWPIso({});
               std::stringstream stream;
               stream << std::fixed << std::setprecision(3) << iso.value().isolation_d();
               jWPIso["isolation"] = std::stod(stream.str());
               jWPIso["isolation_fw"] = iso.value().isolation_fw();
               jThrType["workingPoints"][wpstr] += jWPIso;
            }
         }
      }

      if(thrType == "cTAU") {
         auto & eeminfo = l1menu.thrExtraInfo().cTAU();
         for( auto wp : {TrigConf::Selection::WP::LOOSE, TrigConf::Selection::WP::MEDIUM, TrigConf::Selection::WP::TIGHT} ) {
            auto wpstr = TrigConf::Selection::wpToString(wp);
            jThrType["workingPoints"][wpstr] = json::array_t({});
            for(auto & iso : eeminfo.isolation(wp)) {
               json jWPIso({});
               std::stringstream stream;
               stream << std::fixed << std::setprecision(3) << iso.value().isolation_d();
               jWPIso["isolation"] = std::stod(stream.str());
               jWPIso["isolation_fw"] = iso.value().isolation_fw();
               jThrType["workingPoints"][wpstr] += jWPIso;
            }
         }
      }

      if(thrType == "jJ") {
         auto & jjinfo = l1menu.thrExtraInfo().jJ();
         jThrType["ptMinToTopo1"] = (int)jjinfo.ptMinToTopo("1A");
         jThrType["ptMinToTopo2"] = (int)jjinfo.ptMinToTopo("2A");
         jThrType["ptMinToTopo3"] = (int)jjinfo.ptMinToTopo("3A");
         jThrType["ptMinxTOB1"] = (int)jjinfo.ptMinxTOB("1A");
         jThrType["ptMinxTOB2"] = (int)jjinfo.ptMinxTOB("2A");
         jThrType["ptMinxTOB3"] = (int)jjinfo.ptMinxTOB("3A");
      }

      if(thrType == "jLJ") {
         auto & jljinfo = l1menu.thrExtraInfo().jLJ();
         jThrType["ptMinToTopo1"] = (int)jljinfo.ptMinToTopo("1A");
         jThrType["ptMinToTopo2"] = (int)jljinfo.ptMinToTopo("2A");
         jThrType["ptMinToTopo3"] = (int)jljinfo.ptMinToTopo("3A");
         jThrType["ptMinxTOB1"] = (int)jljinfo.ptMinxTOB("1A");
         jThrType["ptMinxTOB2"] = (int)jljinfo.ptMinxTOB("2A");
         jThrType["ptMinxTOB3"] = (int)jljinfo.ptMinxTOB("3A");
      }

      if(thrType == "gJ") {
         auto & gjinfo = l1menu.thrExtraInfo().gJ();
         jThrType["ptMinToTopo1"] = (int)gjinfo.ptMinToTopo(1);
         jThrType["ptMinToTopo2"] = (int)gjinfo.ptMinToTopo(2);
      }

      if(thrType == "gLJ") {
         auto & gljinfo = l1menu.thrExtraInfo().gLJ();
         jThrType["ptMinToTopo1"] = (int)gljinfo.ptMinToTopo(1);
         jThrType["ptMinToTopo2"] = (int)gljinfo.ptMinToTopo(2);
         jThrType["seedThrA"] = (int)gljinfo.seedThr('A');
         jThrType["seedThrB"] = (int)gljinfo.seedThr('B');
         jThrType["seedThrC"] = (int)gljinfo.seedThr('C');
         std::stringstream stream;
         stream << std::fixed << std::setprecision(3) << gljinfo.rhoTowerMin('A');
         jThrType["rhoTowerMinA"] = std::stod(stream.str());
         stream.str("");
         stream.clear(); 
         stream << std::fixed << std::setprecision(3) << gljinfo.rhoTowerMin('B');
         jThrType["rhoTowerMinB"] = std::stod(stream.str());
         stream.str("");
         stream.clear();      
         stream << std::fixed << std::setprecision(3) << gljinfo.rhoTowerMin('C');
         jThrType["rhoTowerMinC"] = std::stod(stream.str());
         stream.str("");
         stream.clear(); 
         stream << std::fixed << std::setprecision(3) << gljinfo.rhoTowerMax('A');
         jThrType["rhoTowerMaxA"] = std::stod(stream.str());
         stream.str("");
         stream.clear();      
         stream << std::fixed << std::setprecision(3) << gljinfo.rhoTowerMax('B');
         jThrType["rhoTowerMaxB"] = std::stod(stream.str());
         stream.str("");
         stream.clear();
         stream << std::fixed << std::setprecision(3) << gljinfo.rhoTowerMax('C');
         jThrType["rhoTowerMaxC"] = std::stod(stream.str());
      }

      if(thrType == "jXE") {
          // nothing to do for now...
      }

      if(thrType == "jTE") {
         auto & ei = l1menu.thrExtraInfo().jTE();
         jThrType["etaBoundary1"] = ei.etaBoundary("1A");
         jThrType["etaBoundary1_fw"] = ei.etaBoundary_fw("1A");
         jThrType["etaBoundary2"] = ei.etaBoundary("2A");
         jThrType["etaBoundary2_fw"] = ei.etaBoundary_fw("2A");
         jThrType["etaBoundary3"] = ei.etaBoundary("3A");
         jThrType["etaBoundary3_fw"] = ei.etaBoundary_fw("3A");   
      }

      if(thrType == "gXE") {
         auto & ei = l1menu.thrExtraInfo().gXE();
         jThrType["seedThrA"] = (int)ei.seedThr('A');
         jThrType["seedThrB"] = (int)ei.seedThr('B');
         jThrType["seedThrC"] = (int)ei.seedThr('C');
         jThrType["XERHO_sigmaPosA"] = ei.XERHO_param('A',true);
         jThrType["XERHO_sigmaPosB"] = ei.XERHO_param('B',true);
         jThrType["XERHO_sigmaPosC"] = ei.XERHO_param('C',true);
         jThrType["XERHO_sigmaNegA"] = ei.XERHO_param('A',false);
         jThrType["XERHO_sigmaNegB"] = ei.XERHO_param('B',false);
         jThrType["XERHO_sigmaNegC"] = ei.XERHO_param('C',false);
         jThrType["XEJWOJ_a_A"] = ei.JWOJ_param('A','a');
         jThrType["XEJWOJ_a_B"] = ei.JWOJ_param('B','a');
         jThrType["XEJWOJ_a_C"] = ei.JWOJ_param('C','a');
         jThrType["XEJWOJ_b_A"] = ei.JWOJ_param('A','b');
         jThrType["XEJWOJ_b_B"] = ei.JWOJ_param('B','b');
         jThrType["XEJWOJ_b_C"] = ei.JWOJ_param('C','b');
         jThrType["XEJWOJ_c_A"] = ei.JWOJ_param('A','c');
         jThrType["XEJWOJ_c_B"] = ei.JWOJ_param('B','c');
         jThrType["XEJWOJ_c_C"] = ei.JWOJ_param('C','c');
      }

      if(thrType == "gTE") {
          // nothing to do for now...
      }

      std::vector<std::string> legacyCalo = {"EM", "JET", "TAU", "XE", "TE", "XS", "ZB", "JB", "JF", "JE", "R2TOPO"};
      if( std::any_of(begin(legacyCalo), end(legacyCalo), [&thrType](const std::string &c) { return c==thrType; }) ) {
         thresholds["legacyCalo"][thrType] = jThrType;
      } else {
         thresholds[thrType] = jThrType;
      }
   };

   json boards = json::object_t{};
   for( auto & bname : l1menu.boardNames() ) {
      auto & bdef = l1menu.board(bname);
      boards[bname] = json{ {"connectors", bdef.connectorNames()}, {"type", bdef.type()} };
      if(bdef.legacy())
         boards[bname]["legacy"] = true;
   };

   json connectors = json::object_t{};
   for( auto & cname : l1menu.connectorNames() ) {
      auto jConn = json{};
      auto & cdef = l1menu.connector(cname);
      jConn["type"] = cdef.type();
      if(cdef.legacy())
         jConn["legacy"] = true;
      if(cdef.maxFpga() == 2) {
         for(size_t fpga = 0; fpga<2; ++fpga) {
            std::string fName = "fpga" + std::to_string(fpga);
            for(size_t clock = 0; clock<2; ++clock) {
               std::string cName = "clock" + std::to_string(clock);
               jConn["triggerlines"][fName][cName] = json::array_t();
               for(auto & tl : cdef.triggerLines(fpga, clock)) {
                  jConn["triggerlines"][fName][cName] += json({ {"name", tl.name()}, {"nbits",tl.nbits()}, {"startbit", tl.startbit()} });
               }
            }
         }
      } else {
         if(cdef.maxClock() == 2) {
            // merger boards
            for(size_t clock = 0; clock<cdef.maxClock(); ++clock) {
               std::string cName = "clock" + std::to_string(clock);
               jConn["triggerlines"][cName] = json::array_t();
               for(auto & tl : cdef.triggerLines(0, clock)) {
                  jConn["triggerlines"][cName] += json({ {"name", tl.name()}, {"nbits",tl.nbits()}, {"startbit", tl.startbit()} });
               }
            }            
         } else {
            jConn["triggerlines"] = json::array_t();
            for(auto & tl : cdef.triggerLines()) {
               jConn["triggerlines"] += json({ {"name", tl.name()}, {"nbits",tl.nbits()}, {"startbit", tl.startbit()} });
            }
         }
      }
      connectors[cname] = jConn;
   }

   json ctp = json::object_t{};
   {
      for(size_t slot=7; slot<=9; ++slot) {
         std::string sName = "slot" + std::to_string(slot);
         ctp["inputs"]["ctpin"][sName] = json({});
         for(size_t conn=0; conn<=3; ++conn) {
            ctp["inputs"]["ctpin"][sName]["connector" + std::to_string(conn)] = l1menu.ctp().ctpin(slot,conn);
         }
      }
      ctp["inputs"]["electrical"] = json::object_t{};
      for(size_t conn=0; conn<3; ++conn) {
         if(l1menu.ctp().electrical(conn)=="")
            continue;
         ctp["inputs"]["electrical"]["connector" + std::to_string(conn)] = l1menu.ctp().electrical(conn);
      }
      ctp["inputs"]["optical"] = json::object_t{};
      for(size_t conn=0; conn<12; ++conn) {
         if(l1menu.ctp().optical(conn)=="")
            continue;
         ctp["inputs"]["optical"]["connector" + std::to_string(conn)] = l1menu.ctp().optical(conn);
      }
      json ctpmon = json::object_t{};
      for(auto & mon : l1menu.ctp().ctpMon()) {
         ctpmon[mon.first] = json({{"multiplicity",mon.second.first},{"thr",mon.second.second}});
      }
      json ctpin = json::object_t{};
      for(auto & mon : l1menu.ctp().ctpinMon()) {
         ctpin[mon.first] = json({{"multiplicity", mon.second.first},{"thr", mon.second.second}});
      }
      ctp["monitoring"] = json({{"ctpmon", ctpmon}, {"ctpin", ctpin}});
   }

   json jtopo = json::object_t{};
   if(l1menu.run()>1) {
      std::map<L1TopoAlgorithm::AlgorithmType,std::string> algTypeNames = {
         {L1TopoAlgorithm::AlgorithmType::SORTING, "sortingAlgorithms"},
         {L1TopoAlgorithm::AlgorithmType::DECISION, "decisionAlgorithms"},
         {L1TopoAlgorithm::AlgorithmType::MULTIPLICITY, "multiplicityAlgorithms"}
      };
      auto topoCategories = l1menu.isRun2() ? std::vector<std::string> {"R2TOPO"} : std::vector<std::string> {"TOPO", "MUTOPO", "MULTTOPO", "R2TOPO"};
      for( const std::string& topoCat : topoCategories ) {
         for(auto & algName : l1menu.topoAlgorithmNames(topoCat)) {
            json jalg = json::object_t({});
            auto & alg = l1menu.algorithm(algName,topoCat);
            jalg["klass"]     = alg.klass();
            // input
            if(alg.type()==L1TopoAlgorithm::AlgorithmType::MULTIPLICITY) {
               if(alg.inputs().size()>0) {
                  jalg["input"] = alg.inputs()[0];
               } else {
                  jalg["input"] = nullptr;
               }
            } else if(alg.type()==L1TopoAlgorithm::AlgorithmType::SORTING) {
               jalg["input"] = alg.inputs()[0];
            } else {
               jalg["input"] = alg.inputs();
            }
            // output
            if( alg.type() == L1TopoAlgorithm::AlgorithmType::DECISION ) {
               jalg["output"] = alg.outputs();
            } else {
               jalg["output"] = alg.outputs()[0];
            }
            // generic parameters
            if(topoCat == "MULTTOPO") {
               jalg["nbits"] = alg.getAttribute<unsigned int>("nbits");
               jalg["threshold"] = alg.getAttribute("threshold");
               if(alg.klass()=="EnergyThreshold"){
                  for(const L1TopoAlgorithm::VariableParameter & vpar : alg.parameters()) { 
                     if(vpar.name()=="flavour") jalg["flavour"] = XEFlavour::flavourIntToStr(vpar.value());
                  }
               }
            } else {
               auto ds = alg.generics();
               for(auto & gpname : ds.getKeys()) {
                  auto gp = ds.getObject(gpname);
                  {
                     if(gp.hasAttribute("position")) {
                        jalg["fixedParameters"]["generics"][gpname]["position"] = gp.getAttribute<unsigned int>("position");
                     }
                     std::string pval = alg.genericParameter(gpname);
                     try{
                        int pvali = std::stoi(pval);
                        jalg["fixedParameters"]["generics"][gpname]["value"] = pvali;
                     } catch(std::invalid_argument &) {
                        jalg["fixedParameters"]["generics"][gpname]["value"] = pval;
                     }
                  }
               }
            }
            // variable parameters
            if(topoCat != "MULTTOPO") {
               jalg["variableParameters"] = json::array_t({});
               for(const L1TopoAlgorithm::VariableParameter & vpar : alg.parameters()) {
                  json jVPar({});
                  jVPar["name"] = vpar.name();
                  jVPar["value"] = vpar.value();
                  if(auto sel = vpar.selection_optional()) { jVPar["selection"] = *sel; }
                  jalg["variableParameters"] += jVPar;
               }
            }
            jtopo[topoCat][algTypeNames[alg.type()]][algName] = jalg;
         }
      }
   }

   json j = json::object_t{};
   j["filetype"] = "l1menu";
   j["run"] = l1menu.run();
   j["name"] = l1menu.name();
   j["items"] = items;
   j["thresholds"] = thresholds;
   j["topoAlgorithms"] = jtopo;
   j["boards"] = boards;
   j["connectors"] = connectors;
   j["ctp"] = ctp;

   std::ofstream outfile(filename);
   outfile << std::setw(4) << j << std::endl;

   TRG_MSG_INFO("Saved file " << filename);
   return true;
}


bool 
TrigConf::JsonFileWriterL1::writeJsonFile(const std::string & filename, const TrigConf::L1BunchGroupSet & l1bgs) const {

   json j;
   j["filetype"] = "bunchgroupset";
   j["name"]  = l1bgs.name();

   json groups;
   for (size_t i = 0 ; i< l1bgs.size(); ++i) {
      auto group = l1bgs.getBunchGroup(i);
      json jgroup({});
      jgroup["name"] = group->name();
      jgroup["id"] = group->id();
      jgroup["info"] = std::to_string(group->size()) + " bunches, " + std::to_string(group->nGroups()) + " groups";
      json trains = json::array();
      for (auto [first, len]: group->trains()) {
         json train({});
         train["first"] = first;
         train["length"] = len;
         trains.push_back(train);
      }
      jgroup["bcids"] = trains;
      groups["BGRP"+std::to_string(group->id())] = jgroup;
   }
   j["bunchGroups"] = groups;
   std::ofstream outfile(filename);
   outfile << std::setw(4) << j << std::endl;
   TRG_MSG_INFO("Saved file " << filename);
   return true;
}

bool TrigConf::JsonFileWriterL1::writeJsonFile(const std::string & filename, const TrigConf::L1PrescalesSet & l1ps) const {
   json j;
   j["filetype"] = "l1prescale";
   j["name"]  = l1ps.name();
   json cuts;
   for ( auto [itemName, ps]: l1ps.prescales()){
      json cut;
      cut["cut"] = ps.cut;
      cut["enabled"] = ps.enabled;
      cut["info"] = "prescale: " + std::to_string(ps.prescale);
      cuts[itemName] = cut;
   }
   j["cutValues"] = cuts;
   std::ofstream outfile(filename);
   outfile << std::setw(4) << j << std::endl;
   TRG_MSG_INFO("Saved file " << filename);
   return true;
}
