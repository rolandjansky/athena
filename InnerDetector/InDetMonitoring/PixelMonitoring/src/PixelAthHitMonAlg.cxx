/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#include "PixelAthHitMonAlg.h"
#include "PixelCabling/IPixelCablingSvc.h"

PixelAthHitMonAlg::PixelAthHitMonAlg( const std::string& name, ISvcLocator* pSvcLocator ) : 
  AthMonitorAlgorithm(name, pSvcLocator),
  m_pixelCableSvc("PixelCablingSvc", name),
  m_pixelid(nullptr)
{
  //jo flags
  declareProperty("doOnline", m_doOnline = false);
  declareProperty("doModules", m_doModules = false);
  declareProperty("doLumiBlock", m_doLumiBlock = false);
  declareProperty("doLowOccupancy", m_doLowOccupancy = false);
  declareProperty("doHighOccupancy", m_doHighOccupancy = false);
  declareProperty("doHeavyIonMon", m_doHeavyIonMon = false);
}


PixelAthHitMonAlg::~PixelAthHitMonAlg() {}


StatusCode PixelAthHitMonAlg::initialize() {

  ATH_CHECK( detStore()->retrieve(m_pixelid, "PixelID") );
  ATH_CHECK( m_pixelCondSummaryTool.retrieve() );
  ATH_CHECK( m_pixelCableSvc.retrieve() );
  ATH_CHECK( m_pixelRDOName.initialize() );

  return AthMonitorAlgorithm::initialize();
}


StatusCode PixelAthHitMonAlg::fillHistograms( const EventContext& ctx ) const {
  using namespace Monitored;

  int lb = GetEventInfo(ctx)->lumiBlock();
  auto lbval    = Monitored::Scalar<int>( "pixhitsmontool_lb", lb );
  auto hitGroup = getGroup("Hit");

  unsigned int bcid = GetEventInfo(ctx)->bcid();
  auto bcidval      = Monitored::Scalar<unsigned int>( "pixhitsmontool_bcid", bcid );
  auto rdocontainer = SG::makeHandle(m_pixelRDOName, ctx);
  if ( !(rdocontainer.isValid()) ) {
    ATH_MSG_ERROR("Pixel Monitoring: Pixel RDO container "<< m_pixelRDOName << " could not be found.");
    return StatusCode::RECOVERABLE;
  } else {
    ATH_MSG_DEBUG("Pixel Monitoring: Pixel RDO container "<< rdocontainer.name() <<" is found.");
  }

  int nhits = 0;
  float nhits_layer[PixLayers::COUNT] = {0};

  int phiMod(-99);
  int etaMod(-99);
  bool copyFEval(false);

  AccumulatorArrays hitsPerEventArray = { {{0}}, {{0}}, {{0}}, {{0}}, {{0}}, {{0}} };
  // for inactive or bad modules init corresponding arrays entries to -1
  for (auto idIt = m_pixelid->wafer_begin(); idIt!=m_pixelid->wafer_end(); ++idIt) {
    Identifier     waferID = *idIt;
    IdentifierHash id_hash = m_pixelid->wafer_hash(waferID);
    int pixlayer = getPixLayersID(m_pixelid->barrel_ec(waferID), m_pixelid->layer_disk(waferID) );
    if (pixlayer == 99) continue;
    if (m_pixelCondSummaryTool->isActive(id_hash) == false) { 
      getPhiEtaMod(m_pixelid, waferID, phiMod, etaMod, copyFEval);
      switch (pixlayer) {
      case PixLayers::kECA : 
	hitsPerEventArray.DA[phiMod][etaMod] = -1;
	break;
      case PixLayers::kECC : 
	hitsPerEventArray.DC[phiMod][etaMod] = -1;
	break;
      case PixLayers::kB0 : 
	hitsPerEventArray.B0[phiMod][etaMod] = -1;
	break;
      case PixLayers::kB1 : 
	hitsPerEventArray.B1[phiMod][etaMod] = -1;
	break;
      case PixLayers::kB2 : 
	hitsPerEventArray.B2[phiMod][etaMod] = -1;
	break;
      case PixLayers::kIBL : 
	hitsPerEventArray.IBL[phiMod][etaMod] = -1;
	if (copyFEval) hitsPerEventArray.IBL[phiMod][++etaMod] = -1; 
	break;
      }
    }
  }

  int nGood_layer[PixLayers::COUNT] = {0};
  int nActive_layer[PixLayers::COUNT] = {0};
  float avgocc_active_layer[PixLayers::COUNT] = {0};
  float avgocc_good_layer[PixLayers::COUNT] = {0};
  float avgocc_ratio_toIBL_layer[PixLayers::COUNT] = {0};

  PixelID::const_id_iterator idIt = m_pixelid->wafer_begin();
  PixelID::const_id_iterator idItEnd = m_pixelid->wafer_end();
  for (; idIt != idItEnd; ++idIt) {
    Identifier waferID = *idIt;
    IdentifierHash id_hash = m_pixelid->wafer_hash(waferID);
    int pixlayer = getPixLayersID(m_pixelid->barrel_ec(waferID), m_pixelid->layer_disk(waferID) );
    if (pixlayer == 99) continue;

    if (m_pixelCondSummaryTool->isActive(id_hash) == true) {
      if (pixlayer == PixLayers::kIBL && m_pixelid->eta_module(waferID)>-7 && m_pixelid->eta_module(waferID)<6) nActive_layer[pixlayer]+=2;
      else nActive_layer[pixlayer]++;
 
      if (m_pixelCondSummaryTool->isGood(id_hash) == true) {
	if (pixlayer == PixLayers::kIBL && m_pixelid->eta_module(waferID)>-7 && m_pixelid->eta_module(waferID)<6) nGood_layer[pixlayer]+=2;
	else nGood_layer[pixlayer]++;
      }
    }
  }

  const int nChannels_mod[PixLayers::COUNT] = {46080, 46080, 46080, 46080, 46080, 26880, 26880, 26880};
  float nGoodChannels_total = 0.;
  float nGoodChannels_layer[PixLayers::COUNT];
  float nActiveChannels_layer[PixLayers::COUNT];
  for (int i = 0; i < PixLayers::COUNT; i++) {
    nGoodChannels_layer[i]   = 1.0 * nChannels_mod[i] * nGood_layer[i];
    nActiveChannels_layer[i] = 1.0 * nChannels_mod[i] * nActive_layer[i];
    nGoodChannels_total     += nGoodChannels_layer[i];
  }

  //*******************************************************************************
  //************************** Begin of filling Hit Histograms ********************
  //*******************************************************************************

  ATH_MSG_DEBUG("Filling Raw Hit (RDO) Monitoring Histograms");


  VecAccumulator2DMap HitMap("HitMap");
  std::vector<int> hitLvl1a;
  std::unordered_map<int, std::vector<int>> hitLvl1aLayer;


  Identifier rdoID;
  PixelRDO_Container::const_iterator colNext = rdocontainer->begin();
  PixelRDO_Container::const_iterator lastCol = rdocontainer->end();
  DataVector<PixelRDORawData>::const_iterator p_rdo;

  for (; colNext != lastCol; ++colNext) {
   
    const InDetRawDataCollection<PixelRDORawData>* PixelCollection(*colNext);
    if (!PixelCollection) {
      ATH_MSG_DEBUG("Pixel Monitoring: Pixel Hit container is empty.");
      continue;
    }

    for (p_rdo = PixelCollection->begin(); p_rdo != PixelCollection->end(); ++p_rdo) {
      rdoID = (*p_rdo)->identify();
      int pixlayer = getPixLayersID(m_pixelid->barrel_ec(rdoID), m_pixelid->layer_disk(rdoID) );
      if (pixlayer == 99) continue;
      HitMap.add(pixlayer, rdoID, m_pixelid, 1.0);
      nhits++;
      nhits_layer[pixlayer]++;
      hitLvl1a.push_back( (*p_rdo)->getLVL1A() );
      hitLvl1aLayer[pixlayer].push_back( (*p_rdo)->getLVL1A() );
      getPhiEtaMod(m_pixelid, rdoID, phiMod, etaMod, copyFEval);
      switch (pixlayer) {
      case PixLayers::kECA : 
	hitsPerEventArray.DA[phiMod][etaMod]++;
	break;
      case PixLayers::kECC : 
	hitsPerEventArray.DC[phiMod][etaMod]++;
	break;
      case PixLayers::kB0 : 
	hitsPerEventArray.B0[phiMod][etaMod]++;
	break;
      case PixLayers::kB1 : 
	hitsPerEventArray.B1[phiMod][etaMod]++;
	break;
      case PixLayers::kB2 : 
	hitsPerEventArray.B2[phiMod][etaMod]++;
	break;
      case PixLayers::kIBL : 
	hitsPerEventArray.IBL[phiMod][etaMod]++;
	break;
      }
    }
  }

  fill2DProfLayerAccum( HitMap );

  auto vals = Monitored::Collection( "Hit_LVL1A_pixel", hitLvl1a );
  fill( hitGroup, vals);
  for ( const auto& itr : hitLvl1aLayer ) {
    int layer = itr.first;
    auto vals = Monitored::Collection( "Hit_LVL1A_layer", hitLvl1aLayer.at(layer) );
    fill( pixLayersLabel[layer], vals);
  }


  auto nhitsval = Monitored::Scalar<int>( "nhits_per_event", nhits );
  fill( hitGroup, lbval, nhitsval );
  fill1DProfLumiLayers( "Hits_per_lumi", lb, nhits_layer );

  fillFromArrays( "Hit_Occupancy_PP0", hitsPerEventArray, "Occupancy_per_pixel_event");

  for (int i = 0; i < PixLayers::COUNT; i++) {
    if (nGoodChannels_layer[i]>0)   avgocc_good_layer[i] = nhits_layer[i] / nGoodChannels_layer[i];
    auto val = Monitored::Scalar<float>( "AvgOcc_per_BCID_val", avgocc_good_layer[i]);
    fill( pixLayersLabel[i], bcidval, val );
    if (nActiveChannels_layer[i]>0) avgocc_active_layer[i] = nhits_layer[i] / nActiveChannels_layer[i];
  }
  fill1DProfLumiLayers( "AvgOcc_active_per_lumi", lb, avgocc_active_layer );
  fill1DProfLumiLayers( "AvgOcc_good_per_lumi", lb, avgocc_good_layer );

  if (m_doOnline && avgocc_good_layer[PixLayers::kIBL]>0) {
    for (int i = 0; i < PixLayers::COUNT; i++) {
      avgocc_ratio_toIBL_layer[i] = avgocc_good_layer[i] / avgocc_good_layer[PixLayers::kIBL];
    }
    fill1DProfLumiLayers( "AvgOcc_ratio_toIBL_per_lumi", lb, avgocc_ratio_toIBL_layer );
  }
  //*******************************************************************************
  //************************** End of filling Hit Histograms **********************
  //*******************************************************************************

  return StatusCode::SUCCESS;
}

