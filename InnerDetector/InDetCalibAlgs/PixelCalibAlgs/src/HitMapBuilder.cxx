/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

// PixelCalibAlgs
#include "PixelCalibAlgs/HitMapBuilder.h"
#include "PixelCalibAlgs/PixelConvert.h"

// Gaudi
#include "GaudiKernel/ITHistSvc.h"

// EDM
#include "InDetRawData/PixelRDO_Container.h"

// InnerDetector/InDetDetDescr/InDetReadtoutGeometry

#include "PixelReadoutGeometry/PixelDetectorManager.h"
#include "InDetReadoutGeometry/SiDetectorElement.h"
#include "InDetReadoutGeometry/SiDetectorElementCollection.h"

#include "EventInfo/EventInfo.h"
#include "EventInfo/EventID.h"

// geometry
#include "InDetIdentifier/PixelID.h"

// ROOT
#include "TH2.h"
#include "TString.h"

// standard library
#include <string>
#include <sstream>
#include <algorithm>
#include <map>
#include <fstream>
#include <cstdlib>

HitMapBuilder::HitMapBuilder(const std::string& name, ISvcLocator* pSvcLocator) :
  AthAlgorithm(name, pSvcLocator),
  m_tHistSvc("THistSvc", name),
  m_pixman(0),
  m_pixelID(0),
  m_pixelRDOKey("PixelRDOs"),
  m_nEvents(0.),
  m_nEventsHist(nullptr),
  m_nEventsLBHist(nullptr),
  m_hist_lbMax(3001),
  m_evt_lbMin(0),
  m_evt_lbMax(-1) {
  declareProperty("PixelRDOKey", m_pixelRDOKey, "StoreGate key of pixel RDOs");
  declareProperty("nLBmax", m_hist_lbMax, "Maximum number of LB (for histograms binning)");
  declareProperty("LBMin", m_evt_lbMin, "First lumi block to consider");
  declareProperty("LBMax", m_evt_lbMax, "Last lumi block to consider");
  declareProperty("THistSvc", m_tHistSvc, "THistSvc");
}

HitMapBuilder::~HitMapBuilder() {
}

std::string HitMapBuilder::getDCSIDFromPosition(int barrel_ec, int layer, int modPhi, int module_eta) {
  for (unsigned int ii = 0; ii < m_pixelMapping.size(); ii++) {
    if (m_pixelMapping[ii].second.size() != 4) {
      ATH_MSG_FATAL("getDCSIDFromPosition: Vector size is not 4!");
      return std::string("Error!");
    }
    if (m_pixelMapping[ii].second[0] != barrel_ec) continue;
    if (m_pixelMapping[ii].second[1] != layer) continue;
    if (m_pixelMapping[ii].second[2] != modPhi) continue;
    if (m_pixelMapping[ii].second[3] != module_eta) continue;
    return m_pixelMapping[ii].first;
  }
  ATH_MSG_FATAL("DCS ID is not found!");
  return std::string("Error!");
}

const std::string HitMapBuilder::histoSuffix(const int bec, const int layer) {
  std::ostringstream out;
  switch (bec) {
    case 0:
      if (layer==0)  out << "IBL";
      else          out << "B" << layer-1;
      break;
    case +2: out << "Disk" << layer+1 << "A"; break;
    case -2: out << "Disk" << layer+1 << "C"; break;
    case +4: out << "DBM"  << layer+1 << "A"; break;
    case -4: out << "DBM"  << layer+1 << "C"; break;
    default: break;
  }
  return out.str();
}

std::vector<std::string>& HitMapBuilder::splitter(const std::string &str, char delim, std::vector<std::string> &elems) {
  std::stringstream ss(str);
  std::string item;
  while (std::getline(ss, item, delim)) {
    elems.push_back(item);
  }
  return elems;
}

std::vector<std::string> HitMapBuilder::splitter(const std::string &str, char delim) {
  std::vector<std::string> elems;
  splitter(str, delim, elems);
  return elems;
}

//=========================================================
//
// initialize
//
//=========================================================
StatusCode HitMapBuilder::initialize() {
  ATH_MSG_INFO("Initializing HitMapBuilder");

  // retrieve THistSvc
  StatusCode sc = m_tHistSvc.retrieve();
  if (!sc.isSuccess()) {
    ATH_MSG_FATAL("Unable to retrieve THistSvc");
    return StatusCode::FAILURE;
  }
  // retrieve PixelDetectorManager
  sc = detStore()->retrieve(m_pixman,"Pixel");
  if (!sc.isSuccess()) {
    ATH_MSG_FATAL("Unable to retrieve PixelDetectorManager");
    return StatusCode::FAILURE;
  }

  // retrieve PixelID helper
  sc = detStore()->retrieve(m_pixelID, "PixelID");
  if (!sc.isSuccess()) {
    ATH_MSG_FATAL("Unable to retrieve PixelID helper");
    return StatusCode::FAILURE;
  }

  // resize vectors of histograms
  const Identifier::size_type maxHash = m_pixelID->wafer_hash_max();
  ATH_MSG_DEBUG("PixelID maxHash = " << maxHash);
  m_occupancyMaps.resize(maxHash+m_nIblFes);
  m_occupancyMapsIBL2dLB.resize(m_perLB_n*m_nIblFes);
  m_TOTdistributions.resize(maxHash+m_nIblFes);
  m_TOTdistributionsIBL2dLB.resize(m_perLB_n*m_nIblFes);
  m_occupancyLB.resize(maxHash+m_nIblFes);

  // Resize vector of variables
  m_nEventsLB.resize(m_hist_lbMax);
  m_nEventsLBCategory.resize(m_perLB_n);

  return (registerHistograms());
}


StatusCode HitMapBuilder::registerHistograms() {
  const std::string mapFile = "PixelMapping_Run2.dat";

  std::vector<std::string> paths = splitter(std::getenv("DATAPATH"), ':');
  bool found(false);
  for (const auto& x : paths) {
    std::ifstream infile((x+"/"+mapFile).c_str());
    if (infile.is_open()) {
      ATH_MSG_INFO("Mapping file '" << mapFile << "' found in " << x);

      int tmp_barrel_ec; int tmp_layer; int tmp_modPhi; int tmp_module_eta; std::string tmp_module_name;
      std::vector<int> tmp_position;
      tmp_position.resize(4);
      while(infile >> tmp_barrel_ec >> tmp_layer >> tmp_modPhi >> tmp_module_eta >> tmp_module_name) {
        tmp_position[0] = tmp_barrel_ec;
        tmp_position[1] = tmp_layer;
        tmp_position[2] = tmp_modPhi;
        tmp_position[3] = tmp_module_eta;
        m_pixelMapping.push_back(std::make_pair(tmp_module_name, tmp_position));
      }

      found=true;
      infile.close();
      break;
    }
  }

  if (!found) {
    ATH_MSG_FATAL("Mapping file '" << mapFile << "' not found in DATAPATH !!!");
    return StatusCode::FAILURE;
  }

  // Register event histogram
  m_nEventsHist = std::make_unique<TH1F>("NEvents", "NEvents;;# events", 1, 0, 1);
  m_nEventsLBHist = std::make_unique<TH1F>("NEventsLB", "NEventsLB;LB;# events", m_hist_lbMax, -0.5, m_hist_lbMax+0.5);

  // Regist TDirectory first
  // HARD CODING, TODO do in a smarter way
  std::vector<std::pair<int, int>> temp_bec_layer{std::make_pair(2, 3), std::make_pair(-2, 3), std::make_pair(0, 4)};
  std::vector<std::string> temp_dir{"Occupancy2d", "OccupancyLb", "ToT"};
  for (auto const& bl : temp_bec_layer) {
    for (int l=0; l<bl.second; l++) {
      for (auto const& d: temp_dir) {
        std::string temp_str = "/histfile/All/" + d + "/" + histoSuffix(bl.first, l) + "/" + d + histoSuffix(bl.first, l);
        std::unique_ptr<TH1F>temp_h1d = std::make_unique<TH1F>((d+histoSuffix(bl.first, l)).c_str(), (d+histoSuffix(bl.first, l)).c_str(), 1, 0, 1);
        ATH_CHECK(m_tHistSvc->regHist(temp_str.c_str(), std::move(temp_h1d)));
        ATH_CHECK(m_tHistSvc->deReg(temp_str));
      }
    }
  }

  for (InDetDD::SiDetectorElementCollection::const_iterator iter=m_pixman->getDetectorElementBegin(); iter!=m_pixman->getDetectorElementEnd(); ++iter) {
    const InDetDD::SiDetectorElement* element = *iter;
    if (!element) continue;

    Identifier ident = element->identify();
    if (!m_pixelID->is_pixel(ident)) continue;

    int bec        = m_pixelID->barrel_ec(ident);
    int layer      = m_pixelID->layer_disk(ident);
    int modPhi = m_pixelID->phi_module(ident);
    int modEta = m_pixelID->eta_module(ident);
    int modHash = m_pixelID->wafer_hash(ident);
    const Identifier::size_type maxHash = m_pixelID->wafer_hash_max();

    if (abs(bec) == 4) continue; // Skip DBM

    int iblFeHash = -99;
    // Use special fe chip index for IBL
    // {0..3}: 3D C, C8_2, C8_1, C7_2, C7_1
    // {4..15}: 2D C, C6_2, C6_1, ..., C1_2, C1_1
    // {16..27}: 2D A, A1_1, A1_2, ..., A6_1, A6_2
    // {28..31}: 3D A, A7_1, A7_2, A8_1, A8_2
    if (bec == 0 && layer == 0) {
      if (modEta <= -7)                 iblFeHash = modPhi*32 + (modEta+10); // 3D C
      else if (-6<=modEta && modEta<=5) iblFeHash = modPhi*32 + ((modEta+6)*2+4); // 2D
      else                              iblFeHash = modPhi*32 + (modEta+22); // 3D A
      // iblFeHash, here, is even
    }

    std::string onlineID = getDCSIDFromPosition(bec,layer,modPhi,modEta);

    std::ostringstream name;
    std::string axisTitle;

    // Construct IBL Planar later
    std::string LBCategory;
    std::string occ2dDir = "Occupancy2d";
    std::string occLBDir = "OccupancyLb";
    std::string totDir = "ToT";

    // Category: All
    LBCategory = "All";
    if (bec == 0 && layer == 0 && -6 <= modEta && modEta <= 5) { // IBL 2D
      for (unsigned chipId=0; chipId<=1; chipId++) { // FE chips in double module
        int ibl2dHash;
        if (chipId) {
          if (modEta <= -1) ibl2dHash = maxHash + iblFeHash; // C side, '_2'
          else ibl2dHash = maxHash + iblFeHash + chipId; // A side, '_2'
        }
        else {
          if (modEta <= -1) ibl2dHash = maxHash + iblFeHash + 1; // C side, '_1'
          else ibl2dHash = maxHash + iblFeHash; // A side, '_1'
        }
        std::string histTitle = onlineID + "_" + std::to_string(chipId+1);
        // Occupancy Map
        axisTitle = ";pixel_eta;pixel_phi;# hits/pixel/event";
        m_occupancyMaps[ibl2dHash] = std::make_unique<TH2F>(histTitle.c_str(), (histTitle+axisTitle).c_str(), 80, -0.5, 79.5, 336, -0.5, 335.5);
        name << "/histfile/" << LBCategory << "/" << occ2dDir << "/" << histoSuffix(bec,layer) << "/" << onlineID << "_" << chipId+1;
        name.str(""); name.clear();
        m_occupancyMaps[ibl2dHash]->SetOption("colz");

        // Occupancy vs. LB
        axisTitle = ";LB;# hits/pixel/event";
        m_occupancyLB[ibl2dHash] = std::make_unique<TH1F>(histTitle.c_str(), (histTitle+axisTitle).c_str(), m_hist_lbMax, -0.5, m_hist_lbMax-0.5);
        name << "/histfile/" << LBCategory << "/" << occLBDir << "/" << histoSuffix(bec,layer) << "/" << onlineID << "_" << chipId+1;
        name.str(""); name.clear();

        // ToT
        axisTitle = ";ToT;# hits";
        m_TOTdistributions[ibl2dHash] = std::make_unique<TH1F>(histTitle.c_str(), (histTitle+axisTitle).c_str(), 19, -0.5, 18.5);
        name << "/histfile/" << LBCategory << "/" << totDir << "/" << histoSuffix(bec,layer) << "/" << onlineID << "_" << chipId+1;
        name.str(""); name.clear();
      }
    }
    else { // None IBL 2D
      // Occupancy Map
      axisTitle = ";pixel_eta;pixel_phi;# hits/pixel/event";
      if (bec == 0 && layer == 0) {
        m_occupancyMaps[modHash] = std::make_unique<TH2F>(onlineID.c_str(), (onlineID+axisTitle).c_str(),  80, -0.5,  79.5, 336, -0.5, 335.5); // IBL 3D
      }
      else {
        m_occupancyMaps[modHash] = std::make_unique<TH2F>(onlineID.c_str(), (onlineID+axisTitle).c_str(), 144, -0.5, 143.5, 328, -0.5, 327.5);
      }
      name << "/histfile/" << LBCategory << "/" << occ2dDir << "/" << histoSuffix(bec,layer) << "/" << onlineID;
      name.str(""); name.clear();
      m_occupancyMaps[modHash].get()->SetOption("colz");

      // Occupancy vs. LB
      axisTitle = ";LB;# hits/pixel/event";
      m_occupancyLB[modHash] = std::make_unique<TH1F>(onlineID.c_str(), (onlineID+axisTitle).c_str(), m_hist_lbMax, -0.5, m_hist_lbMax-0.5);
      name << "/histfile/" << LBCategory << "/" << occLBDir << "/" << histoSuffix(bec,layer) << "/" << onlineID;
      name.str(""); name.clear();

      // ToT
      axisTitle = ";ToT;# hits";
      if (bec == 0 && layer == 0) {
        m_TOTdistributions[modHash] = std::make_unique<TH1F>(onlineID.c_str(), (onlineID+axisTitle).c_str(),  19, -0.5,  18.5); // IBL 3D
      }
      else {
        m_TOTdistributions[modHash] = std::make_unique<TH1F>(onlineID.c_str(), (onlineID+axisTitle).c_str(), 256, -0.5, 255.5);
      }
      name << "/histfile/" << LBCategory << "/" << totDir << "/" << histoSuffix(bec,layer) << "/" << onlineID;
      name.str(""); name.clear();
    } // End of if ibl 2d

    // Category: LB1-100, 101-200, ...
    if (0 <= iblFeHash) { // IBL
      for (int i=0; i<m_perLB_n; i++) {
        LBCategory = "LB" + std::to_string(m_perLB_min+m_perLB_step*i+1) + "-" + std::to_string(m_perLB_min+m_perLB_step*(i+1));
        if (-6 <= modEta && modEta <= 5) { // IBL 2D
          for (int chipId=0; chipId<=1; chipId++) { // FE chips in double module
            int ibl2dHash;
            if (chipId) {
              if (modEta <= -1) ibl2dHash = m_nIblFes*i + iblFeHash; // C side, '_2'
              else ibl2dHash = m_nIblFes*i + iblFeHash + chipId; // A side, '_2'
            }
            else {
              if (modEta <= -1) ibl2dHash = m_nIblFes*i + iblFeHash + 1; // C side, '_1'
              else ibl2dHash = m_nIblFes*i + iblFeHash; // A side, '_1'
            }
            std::string histTitle = onlineID + "_" + std::to_string(chipId+1);
            // Occupancy Map
            axisTitle = ";pixel_eta;pixel_phi;# hits/pixel/event";
            m_occupancyMapsIBL2dLB[ibl2dHash] = std::make_unique<TH2F>(histTitle.c_str(), (histTitle+axisTitle).c_str(), 80, -0.5, 79.5, 336, -0.5, 335.5);
            name << "/histfile/" << LBCategory << "/" << occ2dDir << "/" << histoSuffix(bec,layer) << "/" << histTitle;
            name.str(""); name.clear();
            m_occupancyMapsIBL2dLB[ibl2dHash]->SetOption("colz");

            // ToT
            axisTitle = ";ToT;# hits";
            m_TOTdistributionsIBL2dLB[ibl2dHash] = std::make_unique<TH1F>(histTitle.c_str(), (histTitle+axisTitle).c_str(), 19, -0.5, 18.5);
            name << "/histfile/" << LBCategory << "/" << totDir << "/" << histoSuffix(bec,layer) << "/" << histTitle;
            name.str(""); name.clear();
          }
        }
        else { // IBL 3D
          int ibl2dHash = m_nIblFes*i + iblFeHash;
          // Occupancy Map
          axisTitle = ";pixel_eta;pixel_phi;# hits/pixel/event";
          m_occupancyMapsIBL2dLB[ibl2dHash] = std::make_unique<TH2F>(onlineID.c_str(), (onlineID+axisTitle).c_str(), 80, -0.5, 79.5, 336, -0.5, 335.5);
          name << "/histfile/" << LBCategory << "/" << occ2dDir << "/" << histoSuffix(bec,layer) << "/" << onlineID;
          name.str(""); name.clear();
          m_occupancyMapsIBL2dLB[ibl2dHash]->SetOption("colz");

          // ToT
          axisTitle = ";ToT;# hits";
          m_TOTdistributionsIBL2dLB[ibl2dHash] = std::make_unique<TH1F>(onlineID.c_str(), (onlineID+axisTitle).c_str(), 19, -0.5, 18.5);
          name << "/histfile/" << LBCategory << "/" << totDir << "/" << histoSuffix(bec,layer) << "/" << onlineID;
          name.str(""); name.clear();
        } // End of if ibl 2d
      } // End of loop of category: LB
    } // End of if IBL
  } // end loop in detector elements

  return StatusCode::SUCCESS;
}

//=========================================================
//
// execute
//
//=========================================================
StatusCode HitMapBuilder::execute() {
  ATH_MSG_DEBUG( "Executing HitMapBuilder" );

  // retrieve EventInfo
  const EventInfo* eventInfo;
  StatusCode sc = sgSvc()->retrieve(eventInfo);
  if (!sc.isSuccess()) {
    ATH_MSG_FATAL("Unable to retrieve event info");
    return StatusCode::FAILURE;
  } ATH_MSG_DEBUG("Event info retrieved");

  // check LB is in allowed range
  int LB =  static_cast<int>(eventInfo->event_ID()->lumi_block());
  if ((LB < m_evt_lbMin) || (m_evt_lbMax >= m_evt_lbMin && LB > m_evt_lbMax)) {
    ATH_MSG_VERBOSE("Event in lumiblock " << eventInfo->event_ID()->lumi_block() << " not in selected range [" << m_evt_lbMin << "," << m_evt_lbMax << "] => skipped");
    return StatusCode::SUCCESS;
  }

  // Get max LB range
  if (m_LBrange_max < LB) m_LBrange_max = LB;

  // retrieve PixelRDO container
  const DataHandle<PixelRDO_Container> pixelRDOs;
  sc = sgSvc()->retrieve(pixelRDOs, m_pixelRDOKey);
  if (!sc.isSuccess()) {
    ATH_MSG_FATAL( "Unable to retrieve pixel RDO container at " << m_pixelRDOKey );
    return StatusCode::FAILURE;
  } ATH_MSG_DEBUG( "Pixel RDO container retrieved" );
  // loop in RDO container
  for (const auto pPixelRDOCollection: *pixelRDOs) {
    if (pPixelRDOCollection) {
      Identifier moduleID = pPixelRDOCollection->identify();
      IdentifierHash modHash = m_pixelID->wafer_hash(moduleID);
      ATH_MSG_VERBOSE("moduleID, modHash = " << moduleID << " , " << modHash);

      for (DataVector<PixelRDORawData>::const_iterator rdo=pPixelRDOCollection->begin(); rdo!=pPixelRDOCollection->end(); ++rdo) {
        Identifier rdoID = (*rdo)->identify();
        int bec       = m_pixelID->barrel_ec(rdoID);
        //
        if (std::abs(bec) == 4) continue; // Skip DBM
        //
        unsigned int pixel_eta = m_pixelID->eta_index(rdoID);
        unsigned int pixel_phi = m_pixelID->phi_index(rdoID);
        int layer     = m_pixelID->layer_disk(rdoID);
        int modPhi    = m_pixelID->phi_module(rdoID);
        int modEta    = m_pixelID->eta_module(rdoID);
        const Identifier::size_type maxHash = m_pixelID->wafer_hash_max();

        

        int iblFeHash = -99;
        if (bec==0 && layer==0) { // IBL FE order: {0..3}: 3D C, {4..15}: 2D C, {16..27}: 2D A, {28..31}: 3D A
          if (modEta <= -7)                 iblFeHash = modPhi*32 + (modEta+10); // 3D C
          else if (-6<=modEta && modEta<=5) iblFeHash = modPhi*32 + ((modEta+6)*2+4); // 2D
          else                              iblFeHash = modPhi*32 + (modEta+22); // 3D A
          if (pixel_eta/80) iblFeHash++; // For 2D, C side '_1', or A side '_2' for pixel_eta 80~159
        }

        int TOT = (*rdo)->getToT(); // it returns a 8 bits "word"
  
        // Category: All
        if (bec == 0 && layer == 0 && -6 <= modEta && modEta <= 5) { // IBL 2D
          int ibl2dHash = maxHash + iblFeHash;
          m_occupancyMaps[ibl2dHash]->Fill(pixel_eta%80, pixel_phi);
          m_occupancyLB[ibl2dHash]->Fill(LB);
          m_TOTdistributions[ibl2dHash]->Fill(TOT);
        }
        else { // None IBL 2D
          m_occupancyMaps[modHash]->Fill(pixel_eta, pixel_phi);
          m_occupancyLB[modHash]->Fill(LB);
          m_TOTdistributions[modHash]->Fill(TOT);
        }

        // Category: LB
        if (0 <= iblFeHash) { // IBL
          for (int i=0; i<m_perLB_n; i++) {
            if ((m_perLB_min+m_perLB_step*i < LB) && (LB <= m_perLB_min+m_perLB_step*(i+1))) { // LB
              int ibl2dHash = m_nIblFes*i + iblFeHash;
              m_occupancyMapsIBL2dLB[ibl2dHash]->Fill(pixel_eta%80, pixel_phi);
              m_TOTdistributionsIBL2dLB[ibl2dHash]->Fill(TOT);
            }
          }
        }
      }
    }
  }
  m_nEvents++;
  m_nEventsHist->Fill(0.5);
  m_nEventsLBHist->Fill(LB);

  m_nEventsLB[LB]++;
  m_nEventsLBCategory[(LB-m_perLB_min)/m_perLB_step]++;

  return StatusCode::SUCCESS;
}

//=========================================================
//
// finalize
//
//=========================================================
StatusCode HitMapBuilder::finalize() {
  ATH_MSG_INFO("Finalizing HitMapBuilder");

  ATH_CHECK(m_tHistSvc->regHist("/histfile/NEvents", std::move(m_nEventsHist)));
  ATH_CHECK(m_tHistSvc->regHist("/histfile/NEventsLB", std::move(m_nEventsLBHist)));

  //
  // loop in detector elements
  //
  for (InDetDD::SiDetectorElementCollection::const_iterator iter=m_pixman->getDetectorElementBegin(); iter!=m_pixman->getDetectorElementEnd(); ++iter) {
    const InDetDD::SiDetectorElement* element = *iter;
    if (element == 0) continue;

    Identifier ident = element->identify();
    if (!m_pixelID->is_pixel(ident)) continue;

    int bec     = m_pixelID->barrel_ec (ident);
    int layer   = m_pixelID->layer_disk(ident);
    int modPhi  = m_pixelID->phi_module(ident);
    int modEta  = m_pixelID->eta_module(ident);
    int modHash = m_pixelID->wafer_hash(ident);
    const Identifier::size_type maxHash = m_pixelID->wafer_hash_max();

    if (abs(bec) == 4) continue; // Skip DBM

    int iblFeHash = -99;
    if (bec==0 && layer==0) { // IBL FE order: {0..3}: 3D C, {4..15}: 2D C, {16..27}: 2D A, {28..31}: 3D A
      if (modEta <= -7)                 iblFeHash = modPhi*32 + (modEta+10); // 3D C
      else if (-6<=modEta && modEta<=5) iblFeHash = modPhi*32 + ((modEta+6)*2+4); // 2D
      else                              iblFeHash = modPhi*32 + (modEta+22); // 3D A
    }

    // Divide by events for Occupancy Map
    // Category: All
    if (bec == 0 && layer == 0 && -6 <= modEta && modEta <= 5) { // IBL 2D
      int ibl2dHash = maxHash + iblFeHash;
      for (int chipId=0; chipId<2; chipId++) { // 2 Fes in double module
        //yosuke m_occupancyMaps[ibl2dHash+chipId]->Scale(1.0/(m_nEvents));
        // Occupancy LB
        int entries = m_occupancyLB[ibl2dHash+chipId]->GetEntries();
        for (int lb=0; lb<m_hist_lbMax; lb++) {
          if (m_nEventsLB[lb] == 0) continue;
          m_occupancyLB[ibl2dHash+chipId]->SetBinContent(lb+1, m_occupancyLB[ibl2dHash+chipId]->GetBinContent(lb+1)/(m_nEventsLB[lb]*m_fei4bPixels)); // Also divide by fei4 pixels
        }
        m_occupancyLB[ibl2dHash+chipId]->SetEntries(entries);
        m_occupancyLB[ibl2dHash+chipId]->SetBins(m_LBrange_max+100, -0.5, m_LBrange_max+99.5);
      }
    }
    else { // None IBL 2D
      //yosuke m_occupancyMaps[modHash]->Scale(1.0/(m_nEvents));
      // Occupancy LB
      int entries = m_occupancyLB[modHash]->GetEntries();
      for (int lb=0; lb<m_hist_lbMax; lb++) {
        if (m_nEventsLB[lb] == 0) continue;
        if (0 <= iblFeHash) m_occupancyLB[modHash]->SetBinContent(lb+1, m_occupancyLB[modHash]->GetBinContent(lb+1)/(m_nEventsLB[lb]*m_fei4bPixels)); // IBL 3D, divide by fei4 pixels
        else m_occupancyLB[modHash]->SetBinContent(lb+1, m_occupancyLB[modHash]->GetBinContent(lb+1)/(m_nEventsLB[lb]*m_pixModPixels)); // Pixel module, divide by pixel module pixels
      }
      m_occupancyLB[modHash]->SetEntries(entries);
      m_occupancyLB[modHash]->SetBins(m_LBrange_max+100, -0.5, m_LBrange_max+99.5);
    }

    std::string onlineID = getDCSIDFromPosition(bec,layer,modPhi,modEta);

    std::ostringstream name;
    std::string axisTitle;

    // Construct IBL Planar later
    std::string LBCategory;
    std::string occ2dDir = "Occupancy2d";
    std::string occLBDir = "OccupancyLb";
    std::string totDir = "ToT";

    // Category: All
    LBCategory = "All";
    if (bec == 0 && layer == 0 && -6 <= modEta && modEta <= 5) { // IBL 2D
      for (unsigned chipId=0; chipId<=1; chipId++) { // FE chips in double module
        int ibl2dHash;
        if (chipId) {
          if (modEta <= -1) ibl2dHash = maxHash + iblFeHash; // C side, '_2'
          else ibl2dHash = maxHash + iblFeHash + chipId; // A side, '_2'
        }
        else {
          if (modEta <= -1) ibl2dHash = maxHash + iblFeHash + 1; // C side, '_1'
          else ibl2dHash = maxHash + iblFeHash; // A side, '_1'
        }
        // Occupancy Map
        axisTitle = ";pixel_eta;pixel_phi;# hits/pixel/event";
        name << "/histfile/" << LBCategory << "/" << occ2dDir << "/" << histoSuffix(bec,layer) << "/" << onlineID << "_" << chipId+1;
        ATH_CHECK(m_tHistSvc->regHist(name.str(), std::move(m_occupancyMaps[ibl2dHash])));
        name.str(""); name.clear();

        // Occupancy vs. LB
        axisTitle = ";LB;# hits/pixel/event";
        name << "/histfile/" << LBCategory << "/" << occLBDir << "/" << histoSuffix(bec,layer) << "/" << onlineID << "_" << chipId+1;
        ATH_CHECK(m_tHistSvc->regHist(name.str(), std::move(m_occupancyLB[ibl2dHash])));
        name.str(""); name.clear();

        // ToT
        axisTitle = ";ToT;# hits";
        name << "/histfile/" << LBCategory << "/" << totDir << "/" << histoSuffix(bec,layer) << "/" << onlineID << "_" << chipId+1;
        ATH_CHECK(m_tHistSvc->regHist(name.str(), std::move(m_TOTdistributions[ibl2dHash])));
        name.str(""); name.clear();
      }
    }
    else { // None IBL 2D
      // Occupancy Map
      axisTitle = ";pixel_eta;pixel_phi;# hits/pixel/event";
      name << "/histfile/" << LBCategory << "/" << occ2dDir << "/" << histoSuffix(bec,layer) << "/" << onlineID;
      ATH_CHECK(m_tHistSvc->regHist(name.str(), std::move(m_occupancyMaps[modHash])));
      name.str(""); name.clear();

      // Occupancy vs. LB
      axisTitle = ";LB;# hits/pixel/event";
      name << "/histfile/" << LBCategory << "/" << occLBDir << "/" << histoSuffix(bec,layer) << "/" << onlineID;
      ATH_CHECK(m_tHistSvc->regHist(name.str(), std::move(m_occupancyLB[modHash])));
      name.str(""); name.clear();

      // ToT
      axisTitle = ";ToT;# hits";
      name << "/histfile/" << LBCategory << "/" << totDir << "/" << histoSuffix(bec,layer) << "/" << onlineID;
      ATH_CHECK(m_tHistSvc->regHist(name.str(), std::move(m_TOTdistributions[modHash])));
      name.str(""); name.clear();
    } // End of if ibl 2d

    // Category: LB1-100, 101-200, ...
    if (0 <= iblFeHash) { // IBL
      for (int i=0; i<m_perLB_n; i++) {
        LBCategory = "LB" + std::to_string(m_perLB_min+m_perLB_step*i+1) + "-" + std::to_string(m_perLB_min+m_perLB_step*(i+1));
        if (-6 <= modEta && modEta <= 5) { // IBL 2D
          for (int chipId=0; chipId<=1; chipId++) { // FE chips in double module
            int ibl2dHash;
            if (chipId) {
              if (modEta <= -1) ibl2dHash = m_nIblFes*i + iblFeHash; // C side, '_2'
              else ibl2dHash = m_nIblFes*i + iblFeHash + chipId; // A side, '_2'
            }
            else {
              if (modEta <= -1) ibl2dHash = m_nIblFes*i + iblFeHash + 1; // C side, '_1'
              else ibl2dHash = m_nIblFes*i + iblFeHash; // A side, '_1'
            }
            std::string histTitle = onlineID + "_" + std::to_string(chipId+1);
            // Occupancy Map
            axisTitle = ";pixel_eta;pixel_phi;# hits/pixel/event";
            name << "/histfile/" << LBCategory << "/" << occ2dDir << "/" << histoSuffix(bec,layer) << "/" << histTitle;
            ATH_CHECK(m_tHistSvc->regHist(name.str(), std::move(m_occupancyMapsIBL2dLB[ibl2dHash])));
            name.str(""); name.clear();

            // ToT
            axisTitle = ";ToT;# hits";
            name << "/histfile/" << LBCategory << "/" << totDir << "/" << histoSuffix(bec,layer) << "/" << histTitle;
            ATH_CHECK(m_tHistSvc->regHist(name.str(), std::move(m_TOTdistributionsIBL2dLB[ibl2dHash])));
            name.str(""); name.clear();
          }
        }
        else { // IBL 3D
          int ibl2dHash = m_nIblFes*i + iblFeHash;
          // Occupancy Map
          axisTitle = ";pixel_eta;pixel_phi;# hits/pixel/event";
          name << "/histfile/" << LBCategory << "/" << occ2dDir << "/" << histoSuffix(bec,layer) << "/" << onlineID;
          ATH_CHECK(m_tHistSvc->regHist(name.str(), std::move(m_occupancyMapsIBL2dLB[ibl2dHash])));
          name.str(""); name.clear();

          // ToT
          axisTitle = ";ToT;# hits";
          name << "/histfile/" << LBCategory << "/" << totDir << "/" << histoSuffix(bec,layer) << "/" << onlineID;
          ATH_CHECK(m_tHistSvc->regHist(name.str(), std::move(m_TOTdistributionsIBL2dLB[ibl2dHash])));
          name.str(""); name.clear();
        } // End of if ibl 2d
      } // End of loop of category: LB
    } // End of if IBL
  }  // end loop in detector elements

  return StatusCode::SUCCESS;

} // end finalize
