/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#include "TrigConfIO/JsonFileWriter.h"

#include <iomanip>
#include <fstream>
#include <algorithm>

#include <nlohmann/json.hpp>
using json = nlohmann::json;

using namespace std;

TrigConf::JsonFileWriter::JsonFileWriter() : 
   TrigConfMessaging( "JsonFileWriter")
{}


bool
TrigConf::JsonFileWriter::writeJsonFile(const std::string & filename, const L1Menu & l1menu) const
{

   json items({});
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
 
   json thresholds({});
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

         try {
            if(thr->hasAttribute("value")) {
               auto & caloThr = dynamic_cast<const TrigConf::L1Threshold_Calo &>(*thr); // for MBTSII
               jThr["value"] = int(caloThr.thrValue());
            }
         } catch(std::bad_cast&) {};

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
            if(const std::string & roiExcl = muThr.rpcExclROIList(); !roiExcl.empty()) {
               jThr["rpcExclROIList"] = roiExcl;
            }
         } catch(std::bad_cast&) {};

         try {
            auto tauThr = dynamic_cast<const TrigConf::L1Threshold_TAU &>(*thr);
            std::string isobits = "00000";
            auto isomask = tauThr.isolationMask();
            for(size_t b=0; b<5; ++b) {
               if(isomask & (1<<b)) { isobits[4-b] = '1'; }
            }
            jThr["isobits"] = isobits;
         } catch(std::bad_cast&) {};

         try {
            auto EMThr = dynamic_cast<const TrigConf::L1Threshold_EM &>(*thr);
            jThr["thrValues"] = json::array_t({});
            for(auto & rv : EMThr.thrValues()) {
               json jRV({});
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
               jRV["value"] = (unsigned int)rv.value();
               jThr["thrValues"] += jRV;
            }
         } catch(std::bad_cast&) {};

         try {
            auto JThr = dynamic_cast<const TrigConf::L1Threshold_JET &>(*thr);
            jThr["thrValues"] = json::array_t({});
            for(auto & rv : JThr.thrValues()) {
               json jRV({});
               jRV["etamin"] = rv.etaMin();
               jRV["etamax"] = rv.etaMax();
               jRV["phimin"] = 0; // never used, so not read 
               jRV["phimax"] = 64; // never used, so not read
               jRV["priority"] = rv.priority();
               jRV["window"] = JThr.window(0);
               jRV["value"] = (unsigned int)rv.value();
               jThr["thrValues"] += jRV;
            }
         } catch(std::bad_cast&) {};

         try {
            auto teThr = dynamic_cast<const TrigConf::L1Threshold_TE &>(*thr);
            //jThr["thrValues"] = json::array_t({});
            for(auto & rv : teThr.thrValues()) {
               json jRV({});
               jRV["etamin"] = rv.etaMin();
               jRV["etamax"] = rv.etaMax();
               jRV["priority"] = rv.priority();
               jRV["value"] = (unsigned int)rv.value();
               jThr["thrValues"] += jRV;
            }
         } catch(std::bad_cast&) {};

         try {
            auto zbThr = dynamic_cast<const TrigConf::L1Threshold_ZB &>(*thr);
            jThr["seed"] = zbThr.seed();
            jThr["seedBcdelay"] = zbThr.seedBcdelay();
            jThr["seedMultiplicity"] = zbThr.seedMultiplicity();
         } catch(std::bad_cast&) {};


         try {
            auto eEMThr = dynamic_cast<const TrigConf::L1Threshold_eEM &>(*thr);
            jThr["reta"] = TrigConf::Selection::wpToString(eEMThr.reta());
            jThr["rhad"] = TrigConf::Selection::wpToString(eEMThr.rhad());
            jThr["wstot"] = TrigConf::Selection::wpToString(eEMThr.wstot());
            jThr["thrValues"] = json::array_t({});
            for(auto & rv : eEMThr.thrValues()) {
               json jRV({});
               jRV["etamin"] = rv.etaMin();
               jRV["etamax"] = rv.etaMax();
               jRV["priority"] = rv.priority();
               jRV["value"] = (unsigned int)rv.value();
               jThr["thrValues"] += jRV;
            }
         } catch(std::bad_cast&) {};

         try {
            auto jJThr = dynamic_cast<const TrigConf::L1Threshold_jJ &>(*thr);
            jThr["ranges"] = json::array_t({});
            for(auto & rv : jJThr.thrValues()) {
               json jRV({});
               jRV["etamin"] = rv.etaMin();
               jRV["etamax"] = rv.etaMax();
               jThr["ranges"] += jRV;
               jThr["value"] = int(jJThr.thrValue(rv.etaMin()));
            }
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

      if(thrType == "EM") {
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

      if(thrType == "TAU") {
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
         for( auto wp : {TrigConf::Selection::WP::LOOSE, TrigConf::Selection::WP::MEDIUM, TrigConf::Selection::WP::TIGHT} ) {
            auto wpstr = TrigConf::Selection::wpToString(wp);
            jThrType["workingPoints"][wpstr] = json::array_t({});
            for(auto & iso : eeminfo.isolation(wp)) {
               json jWPIso({});
               jWPIso["reta"] = iso.value().reta_d();
               jWPIso["rhad"] = iso.value().rhad_d();
               jWPIso["wstot"] = iso.value().wstot_d();
               jWPIso["maxEt"] = iso.value().maxEt();
               jThrType["workingPoints"][wpstr] += jWPIso;
            }
         }
      }

      if(thrType == "eTAU") {
         auto & eeminfo = l1menu.thrExtraInfo().eTAU();
         for( auto wp : {TrigConf::Selection::WP::LOOSE, TrigConf::Selection::WP::MEDIUM, TrigConf::Selection::WP::TIGHT} ) {
            auto wpstr = TrigConf::Selection::wpToString(wp);
            jThrType["workingPoints"][wpstr] = json::array_t({});
            for(auto & iso : eeminfo.isolation(wp)) {
               json jWPIso({});
               jWPIso["isolation"] = (int)iso.value().isolation_d();
               jWPIso["maxEt"] = iso.value().maxEt();
               jThrType["workingPoints"][wpstr] += jWPIso;
            }
         }
      }

      if(thrType == "jJ") {
         auto & ei = l1menu.thrExtraInfo().jJ();
         jThrType["ptMinToTopo"] = json::array_t({});
         for(auto & x : ei.ptMinToTopoMeV() ) {
            jThrType["ptMinToTopo"] += json({
                  {"etamin",x.etaMin()},
                  {"etamax",x.etaMax()},
                  {"small",int(x.value().first/1000.)},
                  {"large",int(x.value().second/1000.)}
               });
         }
      }

      std::vector<std::string> legacyCalo = {"EM", "JET", "TAU", "XE", "TE", "XS", "ZB", "R2TOPO"};
      if( std::any_of(begin(legacyCalo), end(legacyCalo), [&thrType](const std::string &c) { return c==thrType; }) ) {
         thresholds["legacyCalo"][thrType] = jThrType;
      } else {
         thresholds[thrType] = jThrType;
      }
   };


   json boards{};
   for( auto & bname : l1menu.boardNames() ) {
      auto & bdef = l1menu.board(bname);
      boards[bname] = json{ {"connectors", bdef.connectorNames()}, {"type", bdef.type()} };
      if(bdef.legacy())
         boards[bname]["legacy"] = true;
   };


   json connectors{};
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

   json ctp({});
   {
      for(size_t slot=7; slot<=9; ++slot) {
         std::string sName = "slot" + std::to_string(slot);
         ctp["inputs"]["ctpin"][sName] = json({});
         for(size_t conn=0; conn<=3; ++conn) {
            ctp["inputs"]["ctpin"][sName]["connector" + std::to_string(conn)] = l1menu.ctp().ctpin(slot,conn);
         }
      }
      for(size_t conn=0; conn<3; ++conn) {
         if(l1menu.ctp().electrical(conn)=="")
            continue;
         ctp["inputs"]["electrical"]["connector" + std::to_string(conn)] = l1menu.ctp().electrical(conn);
      }
      for(size_t conn=0; conn<12; ++conn) {
         if(l1menu.ctp().optical(conn)=="")
            continue;
         ctp["inputs"]["optical"]["connector" + std::to_string(conn)] = l1menu.ctp().optical(conn);
      }
      auto ctpmon = json({});
      for(auto & mon : l1menu.ctp().ctpMon()) {
         ctpmon[mon.first] = json({{"multiplicity",mon.second.first},{"thr",mon.second.second}});
      }
      ctp["monitoring"] = json({{"ctpmon",ctpmon}});
   }

   json jtopo({});
   {
      std::map<L1TopoAlgorithm::AlgorithmType,std::string> algTypeNames = {
         {L1TopoAlgorithm::AlgorithmType::SORTING, "sortingAlgorithms"},
         {L1TopoAlgorithm::AlgorithmType::DECISION, "decisionAlgorithms"},
         {L1TopoAlgorithm::AlgorithmType::MULTIPLICITY, "multiplicityAlgorithms"}
      };

      for( const std::string topoCat : {"TOPO", "MUTOPO", "MULTTOPO", "R2TOPO"} ) {
         for(auto & algName : l1menu.topoAlgorithmNames(topoCat)) {
            json jalg = json::object_t({});
            auto & alg = l1menu.algorithm(algName,topoCat);
            jalg["algId"]     = alg.algId();
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

   json j({});
   j["filetype"] = "l1menu";
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
