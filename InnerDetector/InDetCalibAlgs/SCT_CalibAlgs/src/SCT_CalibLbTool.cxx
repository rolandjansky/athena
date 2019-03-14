/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/


/**
 * @file SCT_CalibLbTool.cxx
 * Implementation file for the SCT_CalibLbTool class
 * @author Shaun Roe
 **/

#include "SCT_CalibLbTool.h"
#include "SCT_CalibUtilities.h"
#include "SCT_CalibNumbers.h"
#include "SCT_CalibEventInfo.h"

//InnerDetector
#include "InDetReadoutGeometry/SiDetectorElement.h"
#include "InDetReadoutGeometry/SCT_DetectorManager.h"
#include "InDetRawData/SCT3_RawData.h"
#include "InDetRawData/InDetRawDataCLASS_DEF.h"

#include "Identifier/Identifier.h"
#include "Identifier/IdentifierHash.h"

#include "StoreGate/ReadHandle.h"

#include "GaudiKernel/ITHistSvc.h"

#include "TH1I.h"
#include "TH1F.h"
#include "TH2F.h"
#include "TFile.h"
#include "TMath.h"

#include <boost/lexical_cast.hpp>

using namespace std;
using namespace SCT_CalibAlgs;

static const string pathRoot{"/LB/"};
static const int n_chipsPerSide{6};
static const int n_stripsPerChip{128};
static const string detectorNames[]{"negativeEndcap", "barrel", "positiveEndcap"};
static const string detectorPaths[]{"SCTEC/", "SCTB/","SCTEA/"};

SCT_CalibLbTool::SCT_CalibLbTool(const std::string& type, const std::string& name, const IInterface* parent):
  base_class(type, name, parent),
  m_pSCTHelper{nullptr},
  m_pManager{nullptr},
  m_sct_waferHash{0},
  m_sct_firstStrip{0},
  m_sct_rdoGroupSize{0},
  m_lumiBlock{0},
  m_LbRange{0},
  m_LbsToMerge{0},
  m_rdoContainerKey{"SCT_RDOs"}
{
  //nop
}

StatusCode
SCT_CalibLbTool::initialize() {
  ATH_MSG_INFO("Initialize of " << PACKAGE_VERSION);
  ATH_CHECK(service("THistSvc", m_thistSvc));
  ATH_CHECK(detStore()->retrieve(m_pSCTHelper, "SCT_ID"));
  ATH_CHECK(detStore()->retrieve(m_pManager, "SCT"));
  std::pair<std::string, bool> msgCode{retrievedTool(m_evtInfo)};
  if (not msgCode.second) {
    ATH_MSG_ERROR(msgCode.first);
    return StatusCode::FAILURE;
  }
  //
  m_waferItrBegin = m_pSCTHelper->wafer_begin();
  m_waferItrEnd = m_pSCTHelper->wafer_end();

  m_LbRange = numberOfLb();

  // Read Handle Key
  ATH_CHECK(m_rdoContainerKey.initialize());

  return StatusCode::SUCCESS;
}

StatusCode
SCT_CalibLbTool::finalize() {
  ATH_MSG_INFO("Finalize of " << PACKAGE_VERSION);
  return StatusCode::SUCCESS;
}

bool
SCT_CalibLbTool::book() {
  ATH_MSG_INFO("book() method of " << PACKAGE_VERSION);
  bool result{true};
  //pointers to the histos are deleted by m_thistSvc methods
  m_phistoVector.clear();
  m_phistoVector2D.clear();
  string histoName{pathRoot+"GENERAL/"};
  m_LbRange = numberOfLb();
  m_numberOfEventsHisto = new TH1I{"events", "Events", m_LbRange, 0.5, m_LbRange+0.5};

  m_LbsToMerge = LbToMerge();
  int yAxisBins{static_cast<int>(TMath::Ceil(1.0*m_LbRange/m_LbsToMerge))};

  if (m_thistSvc->regHist( histoName.c_str(), m_numberOfEventsHisto ).isFailure()) {
    ATH_MSG_ERROR("Error in booking EventNumber histogram");
  }
  //histograms for each wafer
  SCT_ID::const_id_iterator waferItr{m_waferItrBegin};
  ////
  std::string hitmapPaths[3];
  for (int i{0}; i<3; ++i) {
    hitmapPaths[i] = pathRoot+detectorPaths[i];
  }
  ////
  for (; waferItr !=m_waferItrEnd; ++waferItr) {
    const Identifier& waferId{*waferItr};
    const int bec{m_pSCTHelper->barrel_ec(waferId)};
    const string formattedPosition{formatPosition(waferId, m_pSCTHelper)+"_"};
    ////
    std::string histotitle{string{"SCT "} + detectorNames[bec2Index(bec)] + string{" Hitmap: plane "} + formattedPosition};
    std::string formattedPosition2D{formattedPosition + "_2D"};
    std::string name2D{hitmapPaths[bec2Index(m_pSCTHelper->barrel_ec(waferId))] + formattedPosition + "_2D"};
    TH2F* hitmapHistoLB_tmp2D{new TH2F{TString{formattedPosition2D}, TString{histotitle}, nbins, firstStrip-0.5, lastStrip+0.5, yAxisBins, 0.5, m_LbsToMerge*yAxisBins+0.5}};
    if (m_thistSvc->regHist(name2D.c_str(), hitmapHistoLB_tmp2D).isFailure()) {
      ATH_MSG_ERROR("Error in booking 2D Hitmap histogram");
    } else {
      m_phistoVector2D.push_back(hitmapHistoLB_tmp2D);
    }
    ////
    for (int iChip(0); iChip!=n_chipsPerSide; ++iChip) {
      int chipId{m_pSCTHelper->side(waferId)==0 ? iChip:iChip+n_chipsPerSide};
      const string formattedChipPosition{formattedPosition + boost::lexical_cast<string>(chipId)};
      const string hname{pathRoot + detectorPaths[bec2Index(bec)] + "/" + formattedChipPosition};
      const string histTitle{string{"SCT"} + detectorNames[bec2Index(bec)] + string{" LB: chip "} + formattedChipPosition};
      TH1F* hist_tmp{new TH1F{TString{formattedChipPosition}, TString{histTitle}, m_LbRange, 0.5, m_LbRange+0.5}};
      if (m_thistSvc->regHist(hname.c_str(), hist_tmp).isFailure()) ATH_MSG_ERROR("Error in booking LB histogram");
      m_phistoVector.push_back(hist_tmp);
    }
  }
  return result;
}

bool
SCT_CalibLbTool::read(const std::string& fileName) {
  ATH_MSG_INFO("read() method of " << PACKAGE_VERSION);
  bool result{true};
  m_LbRange = numberOfLb();
  //pointers to the histos are deleted by m_thistSvc methods
  m_phistoVector.clear();
  m_phistoVector2D.clear();
  TFile* fileLB{TFile::Open(fileName.c_str())};
  ATH_MSG_INFO("opening LB file : " << fileName.c_str());

  if (fileLB) {
    m_numberOfEventsHisto = static_cast<TH1I*>(fileLB->Get("GENERAL/events"));
  } else {
    ATH_MSG_ERROR("can not open LB file : " << fileName.c_str());
    return result;
  }

  if (m_numberOfEventsHisto==nullptr) {
    ATH_MSG_ERROR("Error in reading EventNumber histogram");
  }
  //histograms for each wafer
  SCT_ID::const_id_iterator waferItr{m_waferItrBegin};
  for (; waferItr !=m_waferItrEnd; ++waferItr) {
    const Identifier& waferId{*waferItr};
    const int bec{m_pSCTHelper->barrel_ec(waferId)};
    const string formattedPosition{formatPosition(waferId, m_pSCTHelper)+"_"};
    ////
    std::string name2D=detectorPaths[bec2Index(m_pSCTHelper->barrel_ec( waferId ))] + formattedPosition + "_2D";
    TH2F* hitmapHistoLB_tmp2D = (TH2F*) fileLB->Get(name2D.c_str());

    if (hitmapHistoLB_tmp2D==nullptr) {
      ATH_MSG_ERROR("Error in reading Hitmap histogram");
    } else {
      m_phistoVector2D.push_back(hitmapHistoLB_tmp2D);
    }
    ////
    for (int iChip{0}; iChip!=n_chipsPerSide; ++iChip) {
      int chipId{m_pSCTHelper->side(waferId)==0 ? iChip : iChip+n_chipsPerSide};
      const string hname{detectorPaths[bec2Index(bec)] + "/" + formattedPosition + boost::lexical_cast<string>(chipId)};
      TH1F* hist_tmp{static_cast<TH1F*>(fileLB->Get(hname.c_str()))};
      if (hist_tmp==nullptr) {
        ATH_MSG_ERROR("Error in reading LB histogram");
      } else {
        m_phistoVector.push_back(hist_tmp);
      }
    }
  }
  return result;
}

bool
SCT_CalibLbTool::fill(const bool fromData) {
  if (fromData) {
    return fillFromData();
  }
  m_numberOfEventsHisto->Fill(m_lumiBlock);
  bool result{true};
  int maxEntry{static_cast<int>(m_sct_waferHash->size())};
  for (int i{0}; i != maxEntry; ++i) {
    int theFirstStrip{(*m_sct_firstStrip)[i]};
    //
    int endStrip{(*m_sct_rdoGroupSize)[i] + theFirstStrip};
    int index{(*m_sct_waferHash)[i]};
    TH2F* pThisHisto2D{m_phistoVector2D[index]};

    for (int strip{theFirstStrip}; strip !=endStrip; ++strip) {
      pThisHisto2D->Fill(strip, m_lumiBlock);
    }
    //
    int rdoGroupSize{(*m_sct_rdoGroupSize)[i]};
    IdentifierHash waferHash{static_cast<unsigned int>((*m_sct_waferHash)[i])};
    fillLbForWafer(waferHash, theFirstStrip, rdoGroupSize);
  }

  return result;
}

bool
SCT_CalibLbTool::fillFromData() {
  if (!m_evtInfo) {
    ATH_MSG_ERROR("The evtInfo pointer is NULL");
    return false;
  }
  m_lumiBlock=m_evtInfo->lumiBlock();
  m_numberOfEventsHisto->Fill(m_lumiBlock);
  bool result{true};
  //--- Retrieve the RDO container
  typedef SCT_RDORawData SCTRawDataType;
  SG::ReadHandle<SCT_RDO_Container> p_rdoContainer{m_rdoContainerKey};
  if (not p_rdoContainer.isValid()) {
    ATH_MSG_ERROR("Failed to retrieve SCT RDO container");
  }

  SCT_RDO_Container::const_iterator itr{p_rdoContainer->begin()};
  SCT_RDO_Container::const_iterator itrE{p_rdoContainer->end()};
  for (; itr != itrE; ++itr) {
    const InDetRawDataCollection<SCTRawDataType>* SCT_Collection{*itr};
    if (SCT_Collection==nullptr) continue;// select only SCT RDOs
    Identifier waferId{SCT_Collection->identify()};
    IdentifierHash waferHash{m_pSCTHelper->wafer_hash(waferId)};
    ////
    TH2F* pThisHisto2D{m_phistoVector2D[static_cast<int>(waferHash)]};
    ////
    DataVector<SCTRawDataType>::const_iterator rdoItr{SCT_Collection->begin()};
    DataVector<SCTRawDataType>::const_iterator rdoItrE{SCT_Collection->end()};
    for (; rdoItr != rdoItrE; ++rdoItr ) {
      int theFirstStrip{m_pSCTHelper->strip((*rdoItr)->identify())};
      ////
      int strip{m_pSCTHelper->strip((*rdoItr)->identify())};
      const int endStrip{(*rdoItr)->getGroupSize() + strip};
      for (; strip != endStrip; ++strip) {
        pThisHisto2D->Fill( strip, m_lumiBlock);
      }
      ////
      int rdoGroupSize{(*rdoItr)->getGroupSize()};
      fillLbForWafer(waferHash, theFirstStrip, rdoGroupSize);
    }
  }

  return result;
}

void
SCT_CalibLbTool::fillLbForWafer(const IdentifierHash& waferHash, const int theFirstStrip, const int groupSize) {
  const InDetDD::SiDetectorElement* pElement{m_pManager->getDetectorElement(waferHash)};

  if (pElement) {
    int stripNumber{pElement->swapPhiReadoutDirection() ? lastStrip - theFirstStrip : theFirstStrip};
    int index{static_cast<int>(waferHash)*n_chipsPerSide + stripNumber/n_stripsPerChip};
    //--- Fill LB histograms
    for (int j{0}; j != groupSize; ++j) {
      m_phistoVector[index]->Fill(m_lumiBlock);
    }

  } else {
    ATH_MSG_FATAL("Element pointer is NULL");
  }

}
