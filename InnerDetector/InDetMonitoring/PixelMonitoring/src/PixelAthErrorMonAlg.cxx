/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#include "PixelAthErrorMonAlg.h"

PixelAthErrorMonAlg::PixelAthErrorMonAlg( const std::string& name, ISvcLocator* pSvcLocator ) : 
  AthMonitorAlgorithm(name, pSvcLocator),
  m_pixelCablingSvc("PixelCablingSvc", name),
  m_pixelid(nullptr)
{
  // jo flags
  declareProperty("doOnline", m_doOnline = false);
  declareProperty("doLumiBlock", m_doLumiBlock = false);
  declareProperty("doLowOccupancy", m_doLowOccupancy = false);
  declareProperty("doHighOccupancy", m_doHighOccupancy = false);
  declareProperty("doHeavyIonMon", m_doHeavyIonMon = false);
}


PixelAthErrorMonAlg::~PixelAthErrorMonAlg() {}


StatusCode PixelAthErrorMonAlg::initialize() {

  ATH_CHECK( detStore()->retrieve(m_pixelid, "PixelID") );
  ATH_CHECK( m_pixelCondSummaryTool.retrieve() );
  ATH_CHECK( m_pixelCablingSvc.retrieve() );

  return AthMonitorAlgorithm::initialize();
}


StatusCode PixelAthErrorMonAlg::fillHistograms( const EventContext& ctx ) const {
  using namespace Monitored;

  int lb = GetEventInfo(ctx)->lumiBlock();
  auto errorGroup = getGroup("Error");

  ATH_MSG_DEBUG("Filling Error Monitoring Histograms");

  // Generate a vector of error maps for all different error states.
  std::vector<VecAccumulator2DMap> error_maps_per_state;
  error_maps_per_state.reserve(kNumErrorStatesFEI3 + kNumErrorStatesFEI4);
  for (const auto& state : error_names_stateFEI3) {
    error_maps_per_state.emplace_back( state + std::string("Map") );
  }
  for (const auto& state : error_names_stateFEI4) {
    error_maps_per_state.emplace_back( state + std::string("Map") );
  }
  std::vector<VecAccumulator2DMap> error_maps_per_cat_rodmod; 
  // only first four rodmod histos are unique, others are covered by 
  // the overall, rod/mod-agnostic categories below
  for (unsigned int cat = 0; cat < ErrorCategoryRODMOD::kTruncROD+1; ++cat) {
    error_maps_per_cat_rodmod.emplace_back( error_names_cat_rodmod[cat] );
  }
  std::vector<VecAccumulator2DMap> error_maps_per_cat;
  for (unsigned int cat = 0; cat < ErrorCategory::COUNT; ++cat) {
    error_maps_per_cat.emplace_back( error_names_cat[cat] );
  }

  // containers to keep IBL service records info
  std::vector<int>          flagged_ibl_error_bits;
  std::vector<unsigned int> weights_of_flagged_ibl_error_bits;

  int nActive_layer[PixLayers::COUNT] = {0};

  // Array to count number of errors occurring in all layers.
  float num_errors[PixLayers::COUNT] = {0};
  float num_errors_per_state[std::max(kNumErrorStatesFEI3, kNumErrorStatesFEI4)][PixLayers::COUNT] = {{0}};

  // Counters for erroneous modules in a layer, per
  // error category, w/ and w/o ROD/MOD distinction.
  float num_errormodules_per_cat[ErrorCategory::COUNT][PixLayers::COUNT] = {{0}};
  float num_errormodules_per_cat_rodmod[ErrorCategoryRODMOD::COUNT][PixLayers::COUNT] = {{0}};

  // Generate femcc_errwords, ROB error and per LB maps.
  VecAccumulator2DMap femcc_errwords_maps("FEMCCErrorwords");
  VecAccumulator2DMap trunc_rob_errors_maps("TruncatedROBErrors", true);
  VecAccumulator2DMap masked_rob_errors_maps("MaskedROBErrors", true);
  VecAccumulator2DMap all_errors_maps("ErrorsLB");
  VecAccumulator2DMap modsync_errors_maps("ErrorsModSyncLB");
  VecAccumulator2DMap rodsync_errors_maps("ErrorsRODSyncLB");

  //====================================================================================
  // This is an example how to read the Error informaiton.
  //
  // The Error word is defined in 
  //    InDetConditions/PixelConditionsData/PixelConditionsData/PixelByteStreamErrors.h
  //
  // The IDCInDetBSErrContainer can be accessed through 
  //    m_pixelCondSummaryTool->getBSErrorWord(i,ctx) 
  // where
  //    i= [    0,  2047] : module error
  //        ( [0, 11] - DBMC, [12, 155] - ECC, [156, 435] - IBL,
  //         [436, 721] - B0, [722, 1215] - B1, [1216, 1891] - B2,
  //         [1892, 2035] - ECA, [2036, 2047] - DBMA )    
  //
  //  for PIXEL(FEI3):
  //     = [ 2048,  4095] :   FE-0 error
  //     = [ 4096,  6143] :   FE-1 error
  //     = [ 6144,  8191] :   FE-2 error
  //          ...    ...      ...
  //          ...    ...      ...
  //     = [30720, 32767] :  FE-14 error
  //     = [32768, 34815] :  FE-15 error
  //
  //  for IBL(FEI4):
  //     = [ 2048,  4095] :   FE-0 error
  //     = [ 4096,  6143] :   FE-1 error
  //     = [34816, 35375] :  Error counter in bit#=0 from ServiceRecords (shift: modHash*nFE+iFE)
  //     = [35376, 35935] :  Error counter in bit#=1 from ServiceRecords
  //          ...    ...      ...
  //          ...    ...      ...
  //     = [52176, 52735] :  Error counter in bit#=31 from ServiceRecords
  //
  //====================================================================================
  //

  const int maxHash = m_pixelid->wafer_hash_max(); // 2048

  // Loop over modules
  for (int modHash=0; modHash<maxHash; modHash++) {
    Identifier waferID = m_pixelid->wafer_id(modHash);
    int pixlayer = getPixLayersID(m_pixelid->barrel_ec(waferID), m_pixelid->layer_disk(waferID) );
    int nFE;
    bool is_fei4;
    if (pixlayer == PixLayers::kDBMC || pixlayer == PixLayers::kDBMA) {
      nFE = 1;  
      is_fei4 = true;
    } else if (pixlayer == PixLayers::kIBL) {
      nFE = 1; // IBL 3D
      if (m_pixelid->eta_module(waferID)>-7 && m_pixelid->eta_module(waferID)<6) nFE = 2; //IBL Planar
      is_fei4 = true;
    } else { // for fei3 Pixel layers
      nFE = 16;
      is_fei4 = false;
    }
    // flagging/counting categorized errors per module.
    bool has_err_cat[ErrorCategory::COUNT][nFEIBL2D] = {{false}};
    int nerrors_cat_rodmod[ErrorCategoryRODMOD::COUNT][nFEIBL2D] = {{0}};

    // count number of words w/ MCC/FE flags per module
    unsigned int num_femcc_errwords = 0;

    uint64_t mod_errorword = m_pixelCondSummaryTool->getBSErrorWord(modHash, ctx);

    // extracting ROB error information
    //
    if ( PixelByteStreamErrors::hasError(mod_errorword, PixelByteStreamErrors::TruncatedROB) ) {
      trunc_rob_errors_maps.add(pixlayer, waferID, m_pixelid, 1.0);
    }
    if ( PixelByteStreamErrors::hasError(mod_errorword, PixelByteStreamErrors::MaskedROB) ) {
      masked_rob_errors_maps.add(pixlayer, waferID, m_pixelid, 1.0);
    }
    // getting module_error information (only fei3 layers)
    //
    if (!is_fei4) {
      std::bitset<kNumErrorStatesFEI3> stateFEI3 = getErrorStateFEI3Mod(mod_errorword);  
      num_errors[pixlayer]+=stateFEI3.count();
      for (unsigned int state = 0; state < stateFEI3.size(); ++state) {
	if (stateFEI3[state]) {
	  num_errors_per_state[state][pixlayer]++;
	  error_maps_per_state[state].add(pixlayer, waferID, m_pixelid, 1.0);
	}
      }
      fillErrorCatRODmod(mod_errorword, nerrors_cat_rodmod);
    }

    // getting fe_error information (all pixel layers)
    //
    for (int iFE=0; iFE<nFE; iFE++) {

      int offsetFE = (1+iFE)*maxHash + modHash;    // (FE index+1)*2048 + moduleHash
      uint64_t fe_errorword = m_pixelCondSummaryTool->getBSErrorWord(offsetFE, ctx);

      fillErrorCatRODmod(fe_errorword, is_fei4, nerrors_cat_rodmod, iFE);

      if (!is_fei4) {
	std::bitset<kNumErrorStatesFEI3> stateFEI3 = getErrorStateFE(fe_errorword, is_fei4);  
	num_errors[pixlayer]+=stateFEI3.count();
	if ( stateFEI3.any() ) num_femcc_errwords++;
	for (unsigned int state = 0; state < stateFEI3.size(); ++state) {
	  if (stateFEI3[state]) {
	    num_errors_per_state[state][pixlayer]++;
	    error_maps_per_state[state].add(pixlayer, waferID, m_pixelid, 1.0);
	  }
	}
      } else {
	std::bitset<kNumErrorStatesFEI3> stateFEI4 = getErrorStateFE(fe_errorword, is_fei4);  
	num_errors[pixlayer]+=stateFEI4.count();
	Identifier pixelIDperFEI4 = m_pixelCablingSvc->getPixelIdfromHash(modHash, iFE, 1, 1);
	for (unsigned int state = 0; state < stateFEI4.size(); ++state) {
	  if (stateFEI4[state]) {
	    num_errors_per_state[state][pixlayer]++;
	    error_maps_per_state[state+kNumErrorStatesFEI3].add(pixlayer, pixelIDperFEI4, m_pixelid, 1.0);
	  }
	}
      }

    } // FE loop
    if (!is_fei4) femcc_errwords_maps.add(pixlayer, waferID, m_pixelid, num_femcc_errwords);

    // getting error information from service records (only IBL)
    //
    int state_offset(8); // serviceCode part starts from state 8 of kNumErrorStatesFEI4
    const int serviceRecordFieldOffset = 17*maxHash;
    if (pixlayer == PixLayers::kIBL) {
      int moduleOffset = (modHash-156)*nFE;
      for (int serviceCode=0; serviceCode<32; serviceCode++) {
	// skip irrelevant SR's (as in rel21)
	if ( (serviceCode >= 9 && serviceCode <=14) || (serviceCode >=17 && serviceCode <=23) ) { 
	  state_offset--;
	  continue;
	}
	int serviceCodeOffset = serviceCode*280*nFE;
	for (int iFE=0; iFE<nFE; iFE++) {
	  Identifier pixelIDperFEI4 = m_pixelCablingSvc->getPixelIdfromHash(modHash, iFE, 1, 1);
	  // index = offset + (serviceCode)*(#IBL*nFE) + (moduleHash-156)*nFE + FE
          int serviceCodeCounterIndex = serviceRecordFieldOffset + serviceCodeOffset + moduleOffset + iFE;
          uint64_t serviceCodeCounter = m_pixelCondSummaryTool->getBSErrorWord(serviceCodeCounterIndex, ctx);
	  if (serviceCodeCounter>0) {

	    float payload = serviceCodeCounter; // NB: + 1, as in rel 21, is now added upstream
	    flagged_ibl_error_bits.push_back(serviceCode);
	    weights_of_flagged_ibl_error_bits.push_back(payload);

	    int state = serviceCode + state_offset;
	    num_errors[pixlayer] += payload;
	    num_errors_per_state[state][pixlayer] += payload;
	    error_maps_per_state[state+kNumErrorStatesFEI3].add(pixlayer, pixelIDperFEI4, m_pixelid, payload);
	   
	    fillErrorCatRODmod(serviceCode, payload, nerrors_cat_rodmod, iFE);
	  }

        } // over FE
      } //over service codes
    } // IBL modules

    // access categorized error information per module
    // access categorized error information per module (for IBL - FE)
    // it is only flagged - the actual number of errors per category is not tracked
    for (int iFE=0; iFE<nFE; iFE++) {

      if (pixlayer != PixLayers::kIBL && iFE>0) continue;
      Identifier pixID = waferID;
      if (pixlayer == PixLayers::kIBL) {
	pixID = m_pixelCablingSvc->getPixelIdfromHash(modHash, iFE, 1, 1);
      }
      if (pixID.is_valid()) {
	for (int i = 0; i < ErrorCategoryRODMOD::COUNT; i++) {
	  if (nerrors_cat_rodmod[i][iFE]) {
	    if (getErrorCategory(i+1)!=99) has_err_cat[getErrorCategory(i+1)][iFE] = true;
	    num_errormodules_per_cat_rodmod[i][pixlayer]++;
	    if (!m_doOnline) {
	      all_errors_maps.add(pixlayer, pixID, m_pixelid, nerrors_cat_rodmod[i][iFE]);
	      if (i<ErrorCategoryRODMOD::kTruncROD+1) {
		error_maps_per_cat_rodmod[i].add(pixlayer, pixID, m_pixelid, 1.0);
		if (i==0) modsync_errors_maps.add(pixlayer, pixID, m_pixelid, 1.0);
		if (i==1) rodsync_errors_maps.add(pixlayer, pixID, m_pixelid, 1.0);
	      }
	    }
	  }
	}
	for (int i = 0; i < ErrorCategory::COUNT; i++) {
	  if (has_err_cat[i][iFE]) {
	    num_errormodules_per_cat[i][pixlayer]++;
	    if (!m_doOnline) {
	      error_maps_per_cat[i].add(pixlayer, pixID, m_pixelid, 1.0);
	    }
	  }
	}
	
	// filling nActive modules per layer for later normalization
	// for IBL (and DBM) normalization isdone by number of FEI4 
	if ( (pixlayer != PixLayers::kIBL && m_pixelCondSummaryTool->isActive(modHash) == true) ||
	     (pixlayer == PixLayers::kIBL && m_pixelCondSummaryTool->isActive(modHash, pixID) == true) ) {
	  nActive_layer[pixlayer]++;
	}
      } else {
	ATH_MSG_ERROR("PixelMonitoring: got invalid pixID " << pixID);
      }
    } // loop over FEs

  } // loop over modules

  // normalization by active modules (or FE's in IBL/DBM case)
  //
  for (int i = 0; i < PixLayers::COUNT; i++) {
    if (nActive_layer[i]>0) {
      for (int state = 0; state < numErrorStatesLayer[i]; state++) {
	num_errors_per_state[state][i] /= nActive_layer[i];
      }
      for (int cat = 0; cat < ErrorCategoryRODMOD::COUNT; cat++) {
	num_errormodules_per_cat_rodmod[cat][i] /= nActive_layer[i];
      }
      for (int cat = 0; cat < ErrorCategory::COUNT; cat++) {
	num_errormodules_per_cat[cat][i] /= nActive_layer[i];
      }
    }
  }

  for (unsigned int state = 0; state < kNumErrorStatesFEI3+kNumErrorStatesFEI4; state++) {
    if ( state < kNumErrorStatesFEI3 ) {
      fill2DProfLayerAccum(error_maps_per_state[state]);
      fill1DProfLumiLayers(error_names_stateFEI3[state] + std::string("PerLumi"), lb, num_errors_per_state[state], PixLayers::NFEI3LAYERS);
    } else {
      fill2DProfLayerAccum(error_maps_per_state[state]);
      fill1DProfLumiLayers(error_names_stateFEI4[state-kNumErrorStatesFEI3] + std::string("PerLumi"), lb, num_errors_per_state[state-kNumErrorStatesFEI3], PixLayers::COUNT - PixLayers::NFEI3LAYERS);
    }
  }
  // Fill the accumulated maps
  fill2DProfLayerAccum(femcc_errwords_maps);
  fill2DProfLayerAccum(trunc_rob_errors_maps);
  fill2DProfLayerAccum(masked_rob_errors_maps);
  fill2DProfLayerAccum(all_errors_maps);
  fill2DProfLayerAccum(modsync_errors_maps);
  fill2DProfLayerAccum(rodsync_errors_maps);

  if (!m_doOnline) {
    for (unsigned int cat = 0; cat < ErrorCategoryRODMOD::kTruncROD+1; ++cat) {
      fill2DProfLayerAccum(error_maps_per_cat_rodmod[cat]);
    }
    for (unsigned int cat = 0; cat < ErrorCategory::COUNT; ++cat) {
      fill2DProfLayerAccum(error_maps_per_cat[cat]);
    }
  }
  // Fill the luminosity error profiles for all layers.
  fill1DProfLumiLayers("ErrorsPerLumi", lb, num_errors);

  auto vals = Monitored::Collection( "ServiceRecord_val", flagged_ibl_error_bits );
  auto wgts = Monitored::Collection( "ServiceRecord_wgt", weights_of_flagged_ibl_error_bits );
  fill( errorGroup, vals, wgts);

  // Fill 2D luminosity error profiles per error bit and cat split by ROD/MOD for all layers.
  fill2DProfLumiLayers("ErrorStatePerLumi", lb, num_errors_per_state, numErrorStatesLayer);
  fill2DProfLumiLayers("ErrorCatRODModPerLumi", lb, num_errormodules_per_cat_rodmod, numErrorCatRODModsLayer);

  // Fill 1D luminosity error profiles for error catergory for all layers.
  for (unsigned int cat = 0; cat < error_names_cat_rodmod_norm.size(); ++cat) {
    fill1DProfLumiLayers(error_names_cat_rodmod_norm[cat], lb, num_errormodules_per_cat_rodmod[cat]);
  }
  for (unsigned int cat = 0; cat < ErrorCategory::COUNT; ++cat) {
    fill1DProfLumiLayers(error_names_cat_norm[cat], lb, num_errormodules_per_cat[cat]);
  }

  return StatusCode::SUCCESS;
}

int PixelAthErrorMonAlg::getErrorCategory(int error_cat_rodmod) const {
  int error_cat = 99;
  if (error_cat_rodmod == 1 || error_cat_rodmod == 2) error_cat = ErrorCategory::kSync;
  if (error_cat_rodmod == 3 || error_cat_rodmod == 4) error_cat = ErrorCategory::kTrunc;
  if (error_cat_rodmod == 5) error_cat = ErrorCategory::kOpt;
  if (error_cat_rodmod == 6) error_cat = ErrorCategory::kSeu;
  if (error_cat_rodmod == 7) error_cat = ErrorCategory::kTout;
  return error_cat;
}

std::bitset<kNumErrorStatesFEI3> PixelAthErrorMonAlg::getErrorStateFEI3Mod(uint64_t errorword) const {
  std::bitset<kNumErrorStatesFEI3> result(0);
  if ( PixelByteStreamErrors::hasError(errorword, PixelByteStreamErrors::TimeOut) )            result|= 1;
  if ( PixelByteStreamErrors::hasError(errorword, PixelByteStreamErrors::BCID) )               result|= 1<<1;
  if ( PixelByteStreamErrors::hasError(errorword, PixelByteStreamErrors::LVL1ID) )             result|= 1<<2;
  if ( PixelByteStreamErrors::hasError(errorword, PixelByteStreamErrors::Preamble) )           result|= 1<<3;
  if ( PixelByteStreamErrors::hasError(errorword, PixelByteStreamErrors::RODTrailerBitError) ) result|= 1<<14;
  if ( PixelByteStreamErrors::hasError(errorword, PixelByteStreamErrors::RODHeaderLimit) )     result|= 1<<15;
  if ( PixelByteStreamErrors::hasError(errorword, PixelByteStreamErrors::RODDataOVerflow) )    result|= 1<<16;
  return result;
}

std::bitset<kNumErrorStatesFEI3> PixelAthErrorMonAlg::getErrorStateFE(uint64_t errorword, bool is_fei4) const {
  std::bitset<kNumErrorStatesFEI3> result(0);
  if (!is_fei4) {
    if ( PixelByteStreamErrors::hasError(errorword, PixelByteStreamErrors::MCCLVL1IDEoECheck) ) result|= 1<<4;
    if ( PixelByteStreamErrors::hasError(errorword, PixelByteStreamErrors::MCCBCIDEoECheck) )   result|= 1<<5;
    if ( PixelByteStreamErrors::hasError(errorword, PixelByteStreamErrors::MCCLVL1IDCheck) )    result|= 1<<6;
    if ( PixelByteStreamErrors::hasError(errorword, PixelByteStreamErrors::MCCEoEOverflow) )    result|= 1<<7;
    if ( PixelByteStreamErrors::hasError(errorword, PixelByteStreamErrors::MCCHitOverflow) )    result|= 1<<8;
    if ( PixelByteStreamErrors::hasError(errorword, PixelByteStreamErrors::FEWarning) )         result|= 1<<9;
    if ( PixelByteStreamErrors::hasError(errorword, PixelByteStreamErrors::FEHitParity) )       result|= 1<<10;
    if ( PixelByteStreamErrors::hasError(errorword, PixelByteStreamErrors::FERegisterParity) )  result|= 1<<11;
    if ( PixelByteStreamErrors::hasError(errorword, PixelByteStreamErrors::FEHammingCode) )     result|= 1<<12;
    if ( PixelByteStreamErrors::hasError(errorword, PixelByteStreamErrors::FEEoCOverflow) )     result|= 1<<13;
  } else {
    if ( PixelByteStreamErrors::hasError(errorword, PixelByteStreamErrors::TimeOut) )           result|= 1;
    if ( PixelByteStreamErrors::hasError(errorword, PixelByteStreamErrors::BCID) )              result|= 1<<1;
    if ( PixelByteStreamErrors::hasError(errorword, PixelByteStreamErrors::LVL1ID) )            result|= 1<<2;
    if ( PixelByteStreamErrors::hasError(errorword, PixelByteStreamErrors::Preamble) )          result|= 1<<3;
    if ( PixelByteStreamErrors::hasError(errorword, PixelByteStreamErrors::Trailer) )           result|= 1<<4;
    if ( PixelByteStreamErrors::hasError(errorword, PixelByteStreamErrors::Invalid) )           result|= 1<<5;
    if ( PixelByteStreamErrors::hasError(errorword, PixelByteStreamErrors::LinkMaskedByPPC) )   result|= 1<<6;
    if ( PixelByteStreamErrors::hasError(errorword, PixelByteStreamErrors::Limit) )             result|= 1<<7;
  }
  return result;
}

void PixelAthErrorMonAlg::fillErrorCatRODmod(uint64_t errorword, int (&nerrors_cat_rodmod)[ErrorCategoryRODMOD::COUNT][nFEIBL2D]) const {
  if ( PixelByteStreamErrors::hasError(errorword, PixelByteStreamErrors::TimeOut) )            nerrors_cat_rodmod[6][0]++; // timeout errors
  if ( PixelByteStreamErrors::hasError(errorword, PixelByteStreamErrors::BCID) )               nerrors_cat_rodmod[1][0]++; // ROD synchronization
  if ( PixelByteStreamErrors::hasError(errorword, PixelByteStreamErrors::LVL1ID) )             nerrors_cat_rodmod[1][0]++; // ROD synchronization
  if ( PixelByteStreamErrors::hasError(errorword, PixelByteStreamErrors::Preamble) )           nerrors_cat_rodmod[4][0]++; // optical errors
  if ( PixelByteStreamErrors::hasError(errorword, PixelByteStreamErrors::RODHeaderLimit) )     nerrors_cat_rodmod[3][0]++; // ROD truncation
  if ( PixelByteStreamErrors::hasError(errorword, PixelByteStreamErrors::RODDataOVerflow) )    nerrors_cat_rodmod[3][0]++; // ROD truncation
}

void PixelAthErrorMonAlg::fillErrorCatRODmod(uint64_t errorword, bool is_fei4, 
					     int (&nerrors_cat_rodmod)[ErrorCategoryRODMOD::COUNT][nFEIBL2D], int ife) const {
  if (!is_fei4) {
    if ( PixelByteStreamErrors::hasError(errorword, PixelByteStreamErrors::MCCLVL1IDEoECheck) ) nerrors_cat_rodmod[0][0]++;  // module synchronization
    if ( PixelByteStreamErrors::hasError(errorword, PixelByteStreamErrors::MCCBCIDEoECheck) )   nerrors_cat_rodmod[0][0]++;  // ---
    if ( PixelByteStreamErrors::hasError(errorword, PixelByteStreamErrors::MCCLVL1IDCheck) )    nerrors_cat_rodmod[0][0]++;  // ---
    if ( PixelByteStreamErrors::hasError(errorword, PixelByteStreamErrors::MCCEoEOverflow) )    nerrors_cat_rodmod[2][0]++;  // module truncation
    if ( PixelByteStreamErrors::hasError(errorword, PixelByteStreamErrors::MCCHitOverflow) )    nerrors_cat_rodmod[2][0]++;  // ---
    if ( PixelByteStreamErrors::hasError(errorword, PixelByteStreamErrors::FEEoCOverflow) )     nerrors_cat_rodmod[2][0]++;  // ---
    if ( PixelByteStreamErrors::hasError(errorword, PixelByteStreamErrors::FEHitParity) )       nerrors_cat_rodmod[5][0]++;  // SEU (single event upset)
    if ( PixelByteStreamErrors::hasError(errorword, PixelByteStreamErrors::FERegisterParity) )  nerrors_cat_rodmod[5][0]++;  // ---
    if ( PixelByteStreamErrors::hasError(errorword, PixelByteStreamErrors::FEHammingCode) )     nerrors_cat_rodmod[5][0]++;  // ---
  } else {
    if ( PixelByteStreamErrors::hasError(errorword, PixelByteStreamErrors::TimeOut) )           nerrors_cat_rodmod[6][ife]++; // timeout errors
    if ( PixelByteStreamErrors::hasError(errorword, PixelByteStreamErrors::BCID) )              nerrors_cat_rodmod[1][ife]++; // ROD synchronization
    if ( PixelByteStreamErrors::hasError(errorword, PixelByteStreamErrors::LVL1ID) )            nerrors_cat_rodmod[1][ife]++; // ROD synchronization
    if ( PixelByteStreamErrors::hasError(errorword, PixelByteStreamErrors::Preamble) )          nerrors_cat_rodmod[4][ife]++; // optical errors
    if ( PixelByteStreamErrors::hasError(errorword, PixelByteStreamErrors::Trailer) )           nerrors_cat_rodmod[6][ife]++; // timeout errors
    if ( PixelByteStreamErrors::hasError(errorword, PixelByteStreamErrors::Invalid) )           nerrors_cat_rodmod[3][ife]++; // ROD truncation
    if ( PixelByteStreamErrors::hasError(errorword, PixelByteStreamErrors::Limit) )             nerrors_cat_rodmod[3][ife]++; // ROD truncation
  }
}

void PixelAthErrorMonAlg::fillErrorCatRODmod(int sc, int payload, 
					     int (&nerrors_cat_rodmod)[ErrorCategoryRODMOD::COUNT][nFEIBL2D], int ife) const {

  if (sc == 0)                  nerrors_cat_rodmod[0][ife]+=payload;  // synchronization error   (SR0:BCID counter)
  if (sc == 16)                 nerrors_cat_rodmod[2][ife]+=payload;  // truncation error        (SR16:Truncated event)
  if (sc == 1 || sc == 2 ||                                      // (SR1:Hamming code 0, SR2:Hamming code 1,
      sc == 3 || sc == 24 ||                                     // SR3:Hamming code 2, SR24:Triple redundant CNFGMEM,
      sc == 28 || sc == 29 ||                                    // SR28:Bit flip in CMD, SR29:Triple redundant CMD,
      sc == 31)                                                  // SR31:Triple redundant EFUSE)
    nerrors_cat_rodmod[5][ife]+=payload;                              // SEU error
}
