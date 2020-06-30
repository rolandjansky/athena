/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#include "PixelAthErrorMonAlg.h"

PixelAthErrorMonAlg::PixelAthErrorMonAlg( const std::string& name, ISvcLocator* pSvcLocator ) : 
  AthMonitorAlgorithm(name, pSvcLocator),
  m_pixelid(nullptr)
{
  //jo flags
  // declareProperty("ErrorsTool", m_pixelErrorTool);
  declareProperty("doOnline", m_doOnline = false);
  declareProperty("doModules", m_doModules = false);
  declareProperty("doLumiBlock", m_doLumiBlock = false);
  declareProperty("doLowOccupancy", m_doLowOccupancy = false);
  declareProperty("doHighOccupancy", m_doHighOccupancy = false);
  declareProperty("doHeavyIonMon", m_doHeavyIonMon = false);
}


PixelAthErrorMonAlg::~PixelAthErrorMonAlg() {}


StatusCode PixelAthErrorMonAlg::initialize() {

  ATH_CHECK( detStore()->retrieve(m_pixelid, "PixelID") );
  ATH_CHECK( m_pixelCondSummaryTool.retrieve() );
  if ( !m_pixelErrorTool.empty() ) ATH_CHECK( m_pixelErrorTool.retrieve() );

  return AthMonitorAlgorithm::initialize();
}


StatusCode PixelAthErrorMonAlg::fillHistograms( const EventContext& ctx ) const {
  using namespace Monitored;

  int lb = GetEventInfo(ctx)->lumiBlock();
  auto errorGroup = getGroup("Error");

  ATH_MSG_DEBUG("Filling Error Monitoring Histograms");

  // Array to count number of errors occurring in all components.
  float num_errors[PixLayers::COUNT] = {0};

  // taking the largest - FEI4's - error bit/state sizes 
  float num_errors_per_bit[kNumErrorBitsFEI4][PixLayers::COUNT] = {{0}};
  float num_errors_per_state[error_names_stateFEI4.size()][PixLayers::COUNT] = {{0}};

  // Counters for erroneous modules in a layer, per
  // error category, w/ and w/o ROD/MOD distinction.
  float num_errormodules_per_cat[ErrorCategory::COUNT][PixLayers::COUNT] = {{0}};
  float num_errormodules_per_cat_rodmod[ErrorCategoryRODMOD::COUNT][PixLayers::COUNT] = {{0}};

  const auto& kFeErrorWords = m_pixelErrorTool->getAllFeErrors();

  // const auto& kFeSvcRecord  = m_pixelErrorTool->getAllServiceCodes(); //REVIEW n.y. implemented
  // temporary kFeSvcRecord
  std::map<IdentifierHash, std::map<unsigned int, std::vector<std::pair<int, unsigned int>>>> kFeSvcRecord;
 
  // Generate a vector of error maps for all different error states.
  std::vector<VecAccumulator2DMap> error_maps_per_state;
  error_maps_per_state.reserve(error_names_stateFEI3.size() + error_names_stateFEI4.size());
  for (const auto& state : error_names_stateFEI3) {
    error_maps_per_state.emplace_back(state + std::string("_Map"), true);
  }
  for (const auto& state : error_names_stateFEI4) {
    error_maps_per_state.emplace_back(state + std::string("_Map"), true);
  }
  std::vector<VecAccumulator2DMap> error_maps_per_cat_rodmod; 
  // only first four rodmod histos are unique, others are covered by 
  // the overall, rod/mod-agnostic categories below
  for (unsigned int cat = 0; cat < ErrorCategoryRODMOD::kTruncROD+1; ++cat) {
    error_maps_per_cat_rodmod.emplace_back(error_names_cat_rodmod[cat], true);
  }
  std::vector<VecAccumulator2DMap> error_maps_per_cat;
  for (unsigned int cat = 0; cat < ErrorCategory::COUNT; ++cat) {
    error_maps_per_cat.emplace_back(error_names_cat[cat], true);
  }
  // Generate femcc_errwords and per LB maps.
  VecAccumulator2DMap femcc_errwords_maps("femcc_errorwords", true); //simple counting per _module_
  VecAccumulator2DMap all_errors_maps("Errors_LB", true);
  VecAccumulator2DMap modsync_errors_maps("Errors_ModSync_LB", true);
  VecAccumulator2DMap rodsync_errors_maps("Errors_RODSync_LB", true);

  // containers to keep IBL service records info
  std::vector<int> flagged_ibl_error_bits;
  std::vector<unsigned int> weights_of_flagged_ibl_error_bits;

  int nActive_layer[PixLayers::COUNT] = {0};

  // Perform a loop over all pixel IDs to fill the error-bit maps.
  PixelID::const_id_iterator idIt = m_pixelid->wafer_begin();
  PixelID::const_id_iterator idItEnd = m_pixelid->wafer_end();
  for (; idIt != idItEnd; ++idIt) {
    Identifier waferID = *idIt;
    IdentifierHash id_hash = m_pixelid->wafer_hash(waferID);
    const auto& kErrorWord = m_pixelErrorTool->getModuleErrors(id_hash);

    bool is_fei4 = false;
    if (m_pixelErrorTool->isActive(id_hash) &&  // isActive from PixelBytestreamErrorTool to REVIEW
	( (m_pixelid->barrel_ec(waferID) == 0 && m_pixelid->layer_disk(waferID) == 0) ||
	  abs(m_pixelid->barrel_ec(waferID)) == 4) ) is_fei4 = true;

    int pixlayer = getPixLayersID(m_pixelid->barrel_ec(waferID), m_pixelid->layer_disk(waferID) );

    // Boolean whether current module has a categorized error.
    bool has_err_cat[ErrorCategory::COUNT] = {false};
    bool has_err_cat_rodmod[ErrorCategoryRODMOD::COUNT] = {false};

    if (!is_fei4) {
      // Bit-shifting over module errors, only for ROD-type errors for FE-I3
      for (unsigned int bit = 0; bit < kNumErrorBitsFEI3; bit++) {
	if ((kErrorWord & (static_cast<uint64_t>(1) << bit)) != 0) {
	  if (bit >=4 && bit <=16) continue;
	  num_errors[pixlayer]++;
	  num_errors_per_bit[bit][pixlayer]++;
	  
	  int error_cat_rodmod = 0;
	  if (bit == 20 || bit == 21)              error_cat_rodmod = 2;  // ROD synchronization errors      (20: BCID, 21: LVL1ID)
	  if (bit == 0  || bit == 1)               error_cat_rodmod = 4;  // ROD truncation errors           (0: FIFO Overflow, 1: H/T Limit)
	  if (bit == 23)                           error_cat_rodmod = 5;  // optical errors                  (23: preamble (bitflip))
	  if (bit == 22)                           error_cat_rodmod = 7;  // timeout errors                  (22: timeout on ROD formatter)
	  if (error_cat_rodmod) {
	    has_err_cat_rodmod[error_cat_rodmod - 1] = true;
	    if (!m_doOnline) {
	      all_errors_maps.add(pixlayer, waferID, m_pixelid, 1.0);
	    }
	    if (getErrorCategory(error_cat_rodmod)!=99) has_err_cat[getErrorCategory(error_cat_rodmod)] = true;
	  }
	  if (getErrorState(bit, is_fei4) != 99) {
	    num_errors_per_state[getErrorState(bit, is_fei4)][pixlayer]++;
	    error_maps_per_state[getErrorState(bit, is_fei4)].add(pixlayer, waferID, m_pixelid, 1.0);
	  }
	}
      }
    } //end of FEI3-only part
     
    // Bit-shifting over FE errors, kFeErrorWords are common to both FEI3 and FEI4
    // but bits are different, see 
    // InnerDetector/InDetEventCnv/PixelRawDataByteStreamCnv/src/PixelRodDecoder.cxx
    unsigned int num_femcc_errwords = 0;
    if (kFeErrorWords.find(id_hash) != kFeErrorWords.end()) {
      // Collection of: FE ID, associated error word
      std::map<unsigned int, unsigned int> fe_errorword_map = kFeErrorWords.find(id_hash)->second;
      
      for (const auto& [fe_id, fe_errorword] : fe_errorword_map) {
	bool has_femcc_errbits(false);
	
	for (unsigned int bit = 0; bit < kNumErrorBitsFEI3; bit++) {
	  if ((fe_errorword & (static_cast<uint64_t>(1) << bit)) != 0) {
            if (is_fei4 && bit > 7) continue; // For FE-I4 we are interested only in trailer errors, which are the first 8 bits, service records are taken separately
	    // For FE-I3, no double-counting w/ other bits? REVIEW
	    
	    num_errors[pixlayer]++;
	    num_errors_per_bit[bit][pixlayer]++;
	    // This error word contains FE/MCC related errors. 
	    if (bit >=4 && bit <=16) has_femcc_errbits = true;
	    
	    int error_cat_rodmod = 0;
	    if (!is_fei4) {
	      if (bit == 14 || bit == 15 || bit == 16) error_cat_rodmod = 1;  // module synchronization errors   (14: BCID, 15: BCID. 16: LVL1ID)
	      if (bit == 4  || bit == 12 || bit == 13) error_cat_rodmod = 3;  // module truncation errors        (4: EOC, 12: hit overflow, 13: EoE overflow)
	      if (bit >= 5  && bit <= 7)               error_cat_rodmod = 6;  // SEU (single event upset) errors (5,6,7: hit parity, register parity, hammingcode)
	    } else {
              if (bit == 3 || bit == 4)                error_cat_rodmod = 2;  // synchronization error   (3:LVL1ID, 4:BCID)
              if (bit == 0 || bit == 1)                error_cat_rodmod = 4;  // ROD truncation error    (0:Row/Column error, 1:Limit error)
              if (bit == 5)                            error_cat_rodmod = 5;  // optical error           (5:Preable error)
              if (bit == 2 || bit == 7)                error_cat_rodmod = 7;  // Timeout error           (2:Trailer timeout error, 7:Timeout error)
	    }
	    if (error_cat_rodmod) {
	      has_err_cat_rodmod[error_cat_rodmod - 1] = true;
	      if (!m_doOnline) {
		all_errors_maps.add(pixlayer, waferID, m_pixelid, 1.0);
	      }
	      if (getErrorCategory(error_cat_rodmod)!=99) has_err_cat[getErrorCategory(error_cat_rodmod)] = true;
	    }
	    if (getErrorState(bit, is_fei4) != 99) {
	      if (!is_fei4) {
		num_errors_per_state[getErrorState(bit, is_fei4)][pixlayer]++;
	      } else {
		num_errors_per_state[getErrorState(bit, is_fei4)-error_names_stateFEI3.size()][pixlayer]++;
	      }
	      error_maps_per_state[getErrorState(bit, is_fei4)].add(pixlayer, waferID, m_pixelid, 1.0);
	    }
	  }
	}
	// If we have bits generated by FE/MCC, count this error word.
	if (has_femcc_errbits) num_femcc_errwords++;
      }
    }

    // Loop over IBL service records, FE-I4 only
    if (is_fei4 && kFeSvcRecord.find(id_hash) != kFeSvcRecord.end()) {
      // map of FE ID and a vector of pairs of associated service record and payload
      std::map<unsigned int, std::vector<std::pair<int, unsigned int>>> map_svc_record = kFeSvcRecord.find(id_hash)->second;

      for (const auto& [fe_id, svc_record_vec] : map_svc_record) {
	for (const auto& [svc_record, payload_read] : svc_record_vec) {
          const unsigned int payload = payload_read + 1;

	  // svc_record starts from bit 8 of kNumErrorBitsIBL
	  int bit = svc_record+8;
	  
	  num_errors[pixlayer] += payload;
	  num_errors_per_bit[bit][pixlayer] += payload;

	  int error_cat_rodmod = 0;
	  if (bit == 8)                     error_cat_rodmod = 1;  // synchronization error   (8:BCID counter)
	  if (bit == 24)                    error_cat_rodmod = 3;  // truncation error        (24:Truncated event)
	  if (bit == 9 || bit == 10 ||                             // (9:Hamming code 0, 10:Hamming code 1,
	      bit == 11 || bit == 32 ||                            // 12:Hamming code 2, 32:Triple redundant CNFGMEM,
	      bit == 36 || bit == 37 ||                            // 36:Bit flip in CMD, 37:Triple redundant CMD,
	      bit == 39)                                           // 39:Triple redundant EFUSE)
	                                    error_cat_rodmod = 6;  // SEU error
	  if (error_cat_rodmod) {
	    has_err_cat_rodmod[error_cat_rodmod - 1] = true;
	    if (!m_doOnline) {
	      all_errors_maps.add(pixlayer, waferID, m_pixelid, 1.0);
	    }
	    if (getErrorCategory(error_cat_rodmod)!=99) has_err_cat[getErrorCategory(error_cat_rodmod)] = true;
	  }
	  if (getErrorState(bit, is_fei4) != 99) {
	    num_errors_per_state[getErrorState(bit, is_fei4)-error_names_stateFEI3.size()][pixlayer] += payload;

	    error_maps_per_state[getErrorState(bit, is_fei4)].add(pixlayer, waferID, m_pixelid, payload);
	  }
	  if (pixlayer==PixLayers::kIBL) {
	    flagged_ibl_error_bits.push_back(bit);
	    weights_of_flagged_ibl_error_bits.push_back(payload);
	  }
	}
      } // end loop over service records
    }

    femcc_errwords_maps.add(pixlayer, waferID, m_pixelid, num_femcc_errwords);

    // access categorized error information per module
    // it is only flagged - the actual number of errors in the same category 
    // is not tracked for those particular arrays
    for (int i = 0; i < ErrorCategoryRODMOD::COUNT; i++) {
      if (has_err_cat_rodmod[i]) {
	num_errormodules_per_cat_rodmod[i][pixlayer]++;
	if (!m_doOnline && i<ErrorCategoryRODMOD::kTruncROD+1) {
	  error_maps_per_cat_rodmod[i].add(pixlayer, waferID, m_pixelid, 1.0);
	  if (i==0) modsync_errors_maps.add(pixlayer, waferID, m_pixelid, 1.0);
	  if (i==1) rodsync_errors_maps.add(pixlayer, waferID, m_pixelid, 1.0);
	}
      }
    }
    for (int i = 0; i < ErrorCategory::COUNT; i++) {
      if (has_err_cat[i]) {
	num_errormodules_per_cat[i][pixlayer]++;
	if (!m_doOnline) {
	  error_maps_per_cat[i].add(pixlayer, waferID, m_pixelid, 1.0);
	}
      }
    }
    // filling nActive modules per layer for later normalization
    // for IBL (and DBM) normalization is effectively done by number of FEI4 
    // REVIEW once per-FE active info is available
    //
    if (m_pixelCondSummaryTool->isActive(id_hash) == true) {
      if (pixlayer == PixLayers::kIBL && m_pixelid->eta_module(waferID)>-7 && m_pixelid->eta_module(waferID)<6) nActive_layer[pixlayer]+=2;
      else nActive_layer[pixlayer]++;
    }   
  }  // Loop over all pixel IDs (i.e. modules)

  // Fill the accumulated maps.
  for (unsigned int state = 0; state < error_names_stateFEI3.size()+error_names_stateFEI4.size(); state++) {
    fill2DProfLayerAccum(error_maps_per_state[state]);
    if (state < error_names_stateFEI3.size()) {
      for (int i = 0; i < PixLayers::kIBL; i++) {
	if (nActive_layer[i]>0) num_errors_per_state[state][i] /= nActive_layer[i];
      }
      fill1DProfLumiLayers(error_names_stateFEI3[state] + std::string("_per_lumi"), lb, num_errors_per_state[state]);
    } else {
      for (int i = PixLayers::kIBL; i < PixLayers::COUNT; i++) {
	if (nActive_layer[i]>0) num_errors_per_state[state-error_names_stateFEI3.size()][i] /= nActive_layer[i];
      }
      fill1DProfLumiLayers(error_names_stateFEI4[state-error_names_stateFEI3.size()] + std::string("_per_lumi"), lb, num_errors_per_state[state-error_names_stateFEI3.size()]);
    }
  }
  fill2DProfLayerAccum(femcc_errwords_maps);
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
  fill1DProfLumiLayers("errors_per_lumi", lb, num_errors);

  // Fill the luminosity error profiles per error cat_rodmod for all layers.

  auto vals = Monitored::Collection( "ServiceRecord_val", flagged_ibl_error_bits );
  auto wgts = Monitored::Collection( "ServiceRecord_wgt", weights_of_flagged_ibl_error_bits );
  fill( errorGroup, vals, wgts);

  // NORMALIZATION by active modules (or FE's in IBL/DBM case)
  //
  // Normalize error bit and cat histograms by active modules
  for (int i = 0; i < PixLayers::COUNT; i++) {
    if (nActive_layer[i]>0) {
      for (int bit = 0; bit < numErrorBitsLayer[i]; bit++) {
	num_errors_per_bit[bit][i] /= nActive_layer[i];
      }
      for (int cat = 0; cat < ErrorCategoryRODMOD::COUNT; cat++) {
	num_errormodules_per_cat_rodmod[cat][i] /= nActive_layer[i];
      }
      for (int cat = 0; cat < ErrorCategory::COUNT; cat++) {
	num_errormodules_per_cat[cat][i] /= nActive_layer[i];
      }
    }
  }
  // Fill 2D luminosity error profiles per error bit and cat split by ROD/MOD for all layers.
  fill2DProfLumiLayers("ErrorBit_per_lumi", lb, num_errors_per_bit, numErrorBitsLayer);
  fill2DProfLumiLayers("ErrorCatRODMod_per_lumi", lb, num_errormodules_per_cat_rodmod, numErrorCatRODModsLayer);

  // Fill 1D luminosity error profiles for error catergory for all layers.
  for (unsigned int cat = 0; cat < error_names_cat_rodmod_norm.size(); ++cat) {
    fill1DProfLumiLayers(error_names_cat_rodmod_norm[cat], lb, num_errormodules_per_cat_rodmod[cat]);
  }
  for (unsigned int cat = 0; cat < ErrorCategory::COUNT; ++cat) {
    fill1DProfLumiLayers(error_names_cat_norm[cat], lb, num_errormodules_per_cat[cat]);
  }
  
  return StatusCode::SUCCESS;
}

int PixelAthErrorMonAlg::getErrorState(int bit, bool isibl) const {
  int erstate = 99;
  if (!isibl) {
    switch (bit) {
      case 14:
        erstate = 0;  // FE/MCC BCID1 Sync
        break;
      case 15:
        erstate = 1;  // FE/MCC BCID2 Sync
        break;
      case 16:
        erstate = 2;  // FE/MCC LVL1ID Sync
        break;
      case 20:
        erstate = 3;  // ROD BCID Sync
        break;
      case 21:
        erstate = 4;  // ROD LVL1ID Sync
        break;
      case 4:
        erstate = 5;  // FE/MCC EoC Trunc
        break;
      case 12:
        erstate = 6;  // FE/MCC Hit Overflow Trunc
        break;
      case 13:
        erstate = 7;  // FE/MCC EoE Overflow Trunc
        break;
      case 1:
        erstate = 8;  // ROD H/T Limit Trunc
        break;
      case 0:
        erstate = 9;  // ROD Overflow Trunc
        break;
      case 23:
        erstate = 10; // Preamble/Header
        break;
      case 5:
        erstate = 11; // SEU Hit Parity
        break;
      case 6:
        erstate = 12; // SEU Register Parity
        break;
      case 7:
        erstate = 13; // SEU Hamming Code
        break;
      case 22:
        erstate = 14; // ROD Formatter Timeout
        break;
      case 8:
        erstate = 15; // FE Warning
        break;
      default:
        erstate = 99;
        break;
    }
  } else {
    switch (bit) {
      case 3:
        erstate = 16;  // BCID, ROD Synch
        break;
      case 4:
        erstate = 17;  // LVL1ID, ROD Synch
        break;
      case 8:
        erstate = 18;  // BCID counter, FE Synch
        break;
      case 12:
        erstate = 19;  // L1 trigger input in EODCL counter (write pointer), FE Synch
        break;
      case 13:
        erstate = 20;  // L1 trigger request counter to EODCL (read pointer), FE Synch
        break;
      case 14:
        erstate = 21;  // L1 register, register is full, FE Synch
        break;
      case 15:
        erstate = 22;  // L1 trigger ID in BC register, FE Synch
        break;
      case 23:
        erstate = 23;  // Skipped trigger because the L1 register is full, FE Synch
        break;
      case 0:
        erstate = 24;  // Row/Column, ROD Trunc
        break;
      case 1:
        erstate = 25;  // Limit error, ROD Trunc
        break;
      case 24:
        erstate = 26; // Truncated event, FE Trunc
        break;
      case 5:
        erstate = 27;  // Preamble error, ROD Optical
        break;
      case 9:
        erstate = 28;  // Hamming code in word 0 in EOCHL, FE SEU
        break;
      case 10:
        erstate = 29;  // Hamming code in word 1 in EOCHL, FE SEU
        break;
      case 11:
        erstate = 30;  // Hamming code in word 2 in EOCHL, FE SEU
        break;
      case 32:
        erstate = 31;  // Triple redundant mismatch in Global Configuration Memory (CNFGMEM,) FE SEU
        break;
      case 36:
        erstate = 32;  // Bit flip in CMD, FE SEU
        break;
      case 37:
        erstate = 33;  // Triple redundant mismatch in CMD, FE SEU
        break;
      case 39:
        erstate = 34;  // Triple redundant mismatch in EFUSE, FE SEU
        break;
      case 2:
        erstate = 35;  // Trailer error, ROD
        break;
      case 7:
        erstate = 36;  // Timeout error, ROD
        break;
      case 6:
        erstate = 37;  // Masked link, ROD
        break;
      case 16:
        erstate = 38;  // Readout process error, FE
        break;
      case 33:
        erstate = 39;  // Write register data error, FE
        break;
      case 34:
        erstate = 40;  // Address error, FE
        break;
      case 35:
        erstate = 41;  // Other CMD decoder error, FE
        break;
      case 38:
        erstate = 42;  // Data bus address, FE
        break;
      default:
        erstate = 99;
        break;
    }
  }
  return erstate;
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
