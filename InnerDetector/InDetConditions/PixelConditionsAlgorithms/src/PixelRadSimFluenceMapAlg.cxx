/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#include "PixelRadSimFluenceMapAlg.h"
#include "Identifier/IdentifierHash.h"
#include "GaudiKernel/EventIDRange.h"
#include <memory>
#include <sstream>

#include "PathResolver/PathResolver.h"
#include "TFile.h"

PixelRadSimFluenceMapAlg::PixelRadSimFluenceMapAlg(const std::string& name, ISvcLocator* pSvcLocator):
  ::AthReentrantAlgorithm(name, pSvcLocator)
{
}

StatusCode PixelRadSimFluenceMapAlg::initialize() {
  ATH_MSG_DEBUG("PixelRadSimFluenceMapAlg::initialize()");
  ATH_CHECK(m_moduleDataKey.initialize());
  ATH_CHECK(m_writeFluenceMapKey.initialize());
  return StatusCode::SUCCESS;
}

StatusCode PixelRadSimFluenceMapAlg::execute(const EventContext& ctx) const {
  ATH_MSG_DEBUG("PixelRadSimFluenceMapAlg::execute()");

  SG::WriteCondHandle<PixelRadiationDamageFluenceMapData> writeFluenceMapHandle(m_writeFluenceMapKey, ctx);
  if (writeFluenceMapHandle.isValid()) {
    ATH_MSG_DEBUG("CondHandle " << writeFluenceMapHandle.fullKey() << " is already valid.. In theory this should not be called, but may happen if multiple concurrent events are being processed out of order.");
    return StatusCode::SUCCESS; 
  }

  // Construct the output Cond Object and fill it in
  std::unique_ptr<PixelRadiationDamageFluenceMapData> writeFluenceCdo(std::make_unique<PixelRadiationDamageFluenceMapData>());

  const EventIDBase start{EventIDBase::UNDEFNUM, EventIDBase::UNDEFEVT,                     0,                       
                                              0, EventIDBase::UNDEFNUM, EventIDBase::UNDEFNUM};
  const EventIDBase stop {EventIDBase::UNDEFNUM,   EventIDBase::UNDEFEVT, EventIDBase::UNDEFNUM-1, 
                          EventIDBase::UNDEFNUM-1, EventIDBase::UNDEFNUM, EventIDBase::UNDEFNUM};

  EventIDRange rangeW{start, stop};

  SG::ReadCondHandle<PixelModuleData>moduleData(m_moduleDataKey, ctx);

  // mapping files for radiation damage simulation
  writeFluenceCdo -> setFluenceLayer(moduleData->getFluenceLayer());
  writeFluenceCdo -> setFluenceLayer3D(moduleData->getFluenceLayer3D());

  // Create mapping file for radiation damage simulation
  std::vector<PixelHistoConverter> ramoPotentialMap;
  std::vector<PixelHistoConverter> lorentzMap_e;
  std::vector<PixelHistoConverter> lorentzMap_h;
  std::vector<PixelHistoConverter> distanceMap_e;
  std::vector<PixelHistoConverter> distanceMap_h;
  for (unsigned int i=0; i<moduleData->getRadSimFluenceMapList().size(); i++) {
    ATH_MSG_INFO("Using maps located in: "<< moduleData->getRadSimFluenceMapList().at(i) << " for layer No." << i);
    std::unique_ptr<TFile> mapsFile(TFile::Open((moduleData->getRadSimFluenceMapList().at(i)).c_str(), "READ")); //this is the ramo potential

    if (!mapsFile) {
      ATH_MSG_FATAL("Cannot open file: " << moduleData->getRadSimFluenceMapList().at(i));
      return StatusCode::FAILURE;
    }

    //Setup ramo weighting field map
    std::unique_ptr<TH3F> ramoPotentialMap_hold(mapsFile->Get<TH3F>("hramomap1"));
    if (!ramoPotentialMap_hold) {
      ramoPotentialMap_hold.reset(mapsFile->Get<TH3F>("ramo3d"));
    }
    if (!ramoPotentialMap_hold) {
      ATH_MSG_FATAL("Did not find a Ramo potential map and an approximate form is available yet. Exit...");
      return StatusCode::FAILURE;
    }

    ramoPotentialMap_hold->SetDirectory(nullptr);
    std::unique_ptr<TH2F> lorentzMap_e_hold(mapsFile->Get<TH2F>("lorentz_map_e"));
    std::unique_ptr<TH2F> lorentzMap_h_hold(mapsFile->Get<TH2F>("lorentz_map_h"));
    std::unique_ptr<TH2F> distanceMap_e_hold(mapsFile->Get<TH2F>("edistance"));
    std::unique_ptr<TH2F> distanceMap_h_hold(mapsFile->Get<TH2F>("hdistance"));

    if (!lorentzMap_e_hold || !lorentzMap_h_hold || !distanceMap_e_hold || !distanceMap_h_hold) {
      ATH_MSG_FATAL("Cannot read one of the histograms needed");
      return StatusCode::FAILURE;
    }

    lorentzMap_e_hold->SetDirectory(nullptr);
    lorentzMap_h_hold->SetDirectory(nullptr);
    distanceMap_e_hold->SetDirectory(nullptr);
    distanceMap_h_hold->SetDirectory(nullptr);

    ramoPotentialMap.emplace_back();
    ATH_CHECK(ramoPotentialMap.back().setHisto3D(ramoPotentialMap_hold.get()));
    lorentzMap_e.emplace_back();
    lorentzMap_h.emplace_back();
    distanceMap_e.emplace_back();
    distanceMap_h.emplace_back();
    ATH_CHECK(lorentzMap_e.back().setHisto2D(lorentzMap_e_hold.get()));
    ATH_CHECK(lorentzMap_h.back().setHisto2D(lorentzMap_h_hold.get()));
    ATH_CHECK(distanceMap_e.back().setHisto2D(distanceMap_e_hold.get()));
    ATH_CHECK(distanceMap_h.back().setHisto2D(distanceMap_h_hold.get()));

    mapsFile->Close();
  }
  writeFluenceCdo -> setLorentzMap_e(lorentzMap_e);
  writeFluenceCdo -> setLorentzMap_h(lorentzMap_h);
  writeFluenceCdo -> setDistanceMap_e(distanceMap_e);
  writeFluenceCdo -> setDistanceMap_h(distanceMap_h);
  writeFluenceCdo -> setRamoPotentialMap(ramoPotentialMap);

  // Create mapping file for radiation damage simulation for 3D sensor
  std::vector<PixelHistoConverter> ramoPotentialMap3D;
  std::vector<PixelHistoConverter> eFieldMap3D;
  std::vector<PixelHistoConverter> xPositionMap3D_e;
  std::vector<PixelHistoConverter> xPositionMap3D_h;
  std::vector<PixelHistoConverter> yPositionMap3D_e;
  std::vector<PixelHistoConverter> yPositionMap3D_h;
  std::vector<PixelHistoConverter> timeMap3D_e;
  std::vector<PixelHistoConverter> timeMap3D_h;
  PixelHistoConverter avgChargeMap3D_e;
  PixelHistoConverter avgChargeMap3D_h;

  for (unsigned int i=0; i<moduleData->getRadSimFluenceMapList3D().size(); i++) {
    ATH_MSG_INFO("Using maps located in: "<< moduleData->getRadSimFluenceMapList3D().at(i) << " for 3D sensor layer No." << i);
    std::unique_ptr<TFile> mapsFile3D(TFile::Open((moduleData->getRadSimFluenceMapList3D().at(i)).c_str(), "READ")); //this is the ramo potential

    if (!mapsFile3D) {
      ATH_MSG_FATAL("Cannot open file: " << moduleData->getRadSimFluenceMapList3D().at(i));
      return StatusCode::FAILURE;
    }

    //Setup ramo weighting field map
    std::unique_ptr<TH2F> ramoPotentialMap3D_hold(mapsFile3D->Get<TH2F>("ramo"));
    std::unique_ptr<TH2F> eFieldMap3D_hold(mapsFile3D->Get<TH2F>("efield"));
    if (!ramoPotentialMap3D_hold || !eFieldMap3D_hold) {
      ATH_MSG_FATAL("Did not find a Ramo potential or e-field map for 3D and an approximate form is available yet. Exit...");
      return StatusCode::FAILURE;
    }
    ramoPotentialMap3D_hold->SetDirectory(nullptr);
    eFieldMap3D_hold->SetDirectory(nullptr);
    ramoPotentialMap3D.emplace_back();
    eFieldMap3D.emplace_back();
    ATH_CHECK(ramoPotentialMap3D.back().setHisto2D(ramoPotentialMap3D_hold.get()));
    ATH_CHECK(eFieldMap3D.back().setHisto2D(eFieldMap3D_hold.get()));

    //Now setup the E-field.
    std::unique_ptr<TH3F> xPositionMap3D_e_hold(mapsFile3D->Get<TH3F>("xPosition_e"));
    std::unique_ptr<TH3F> xPositionMap3D_h_hold(mapsFile3D->Get<TH3F>("xPosition_h"));
    std::unique_ptr<TH3F> yPositionMap3D_e_hold(mapsFile3D->Get<TH3F>("yPosition_e"));
    std::unique_ptr<TH3F> yPositionMap3D_h_hold(mapsFile3D->Get<TH3F>("yPosition_h"));
    std::unique_ptr<TH2F> timeMap3D_e_hold(mapsFile3D->Get<TH2F>("etimes"));
    std::unique_ptr<TH2F> timeMap3D_h_hold(mapsFile3D->Get<TH2F>("htimes"));

    if (!xPositionMap3D_e_hold || !xPositionMap3D_h_hold || !yPositionMap3D_e_hold || !yPositionMap3D_h_hold || !timeMap3D_e_hold || !timeMap3D_h_hold) {
      ATH_MSG_FATAL("Cannot find one of the maps.");
      return StatusCode::FAILURE;
    }

    xPositionMap3D_e_hold->SetDirectory(nullptr);
    xPositionMap3D_h_hold->SetDirectory(nullptr);
    yPositionMap3D_e_hold->SetDirectory(nullptr);
    yPositionMap3D_h_hold->SetDirectory(nullptr);
    timeMap3D_e_hold->SetDirectory(nullptr);
    timeMap3D_h_hold->SetDirectory(nullptr);

    //Now, determine the time to reach the electrode and the trapping position.
    xPositionMap3D_e.emplace_back();
    xPositionMap3D_h.emplace_back();
    yPositionMap3D_e.emplace_back();
    yPositionMap3D_h.emplace_back();
    timeMap3D_e.emplace_back();
    timeMap3D_h.emplace_back();
    ATH_CHECK(xPositionMap3D_e.back().setHisto3D(xPositionMap3D_e_hold.get()));
    ATH_CHECK(xPositionMap3D_h.back().setHisto3D(xPositionMap3D_h_hold.get()));
    ATH_CHECK(yPositionMap3D_e.back().setHisto3D(yPositionMap3D_e_hold.get()));
    ATH_CHECK(yPositionMap3D_h.back().setHisto3D(yPositionMap3D_h_hold.get()));
    ATH_CHECK(timeMap3D_e.back().setHisto2D(timeMap3D_e_hold.get()));
    ATH_CHECK(timeMap3D_h.back().setHisto2D(timeMap3D_h_hold.get()));

    std::unique_ptr<TH2F> avgCharge3D_e_hold(mapsFile3D->Get<TH2F>("avgCharge_e"));
    std::unique_ptr<TH2F> avgCharge3D_h_hold(mapsFile3D->Get<TH2F>("avgCharge_h"));

    if (!avgCharge3D_e_hold || !avgCharge3D_h_hold) {
      ATH_MSG_ERROR("Cannot find one of the charge maps.");
      return StatusCode::FAILURE;
    }

    avgCharge3D_e_hold->SetDirectory(nullptr);
    avgCharge3D_h_hold->SetDirectory(nullptr);

    // Get average charge data (for charge chunk effect correction)
    ATH_CHECK(avgChargeMap3D_e.setHisto2D(avgCharge3D_e_hold.get()));
    ATH_CHECK(avgChargeMap3D_h.setHisto2D(avgCharge3D_h_hold.get()));

    mapsFile3D->Close();
  }
  writeFluenceCdo -> setRamoPotentialMap3D(ramoPotentialMap3D);
  writeFluenceCdo -> setEFieldMap3D(eFieldMap3D);
  writeFluenceCdo -> setXPositionMap3D_e(xPositionMap3D_e);
  writeFluenceCdo -> setXPositionMap3D_h(xPositionMap3D_h);
  writeFluenceCdo -> setYPositionMap3D_e(yPositionMap3D_e);
  writeFluenceCdo -> setYPositionMap3D_h(yPositionMap3D_h);
  writeFluenceCdo -> setTimeMap3D_e(timeMap3D_e);
  writeFluenceCdo -> setTimeMap3D_h(timeMap3D_h);
  writeFluenceCdo -> setAvgChargeMap3D_e(avgChargeMap3D_e);
  writeFluenceCdo -> setAvgChargeMap3D_h(avgChargeMap3D_h);

  if (rangeW.stop().isValid() && rangeW.start()>rangeW.stop()) {
    ATH_MSG_FATAL("Invalid intersection rangeW: " << rangeW);
    return StatusCode::FAILURE;
  }
  if (writeFluenceMapHandle.record(rangeW, std::move(writeFluenceCdo)).isFailure()) {
    ATH_MSG_FATAL("Could not record PixelRadiationDamageFluenceMapData " << writeFluenceMapHandle.key() << " with EventRange " << rangeW << " into Conditions Store");
    return StatusCode::FAILURE;
  }
  ATH_MSG_INFO("recorded new CDO " << writeFluenceMapHandle.key() << " with range " << rangeW << " into Conditions Store");

  return StatusCode::SUCCESS;
}


