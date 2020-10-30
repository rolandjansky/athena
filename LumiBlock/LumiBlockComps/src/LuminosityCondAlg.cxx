/*
 * Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration.
 */
/**
 * @file LumiBlockComps/src/LuminosityCondAlg.cxx
 * @author scott snyder <snyder@bnl.gov>, from existing LuminosityTool.
 * @date May, 2019
 * @brief Conditions algorithm for luminosity data.
 */


#include "LuminosityCondAlg.h"
#include "AthenaPoolUtilities/CondAttrListCollection.h"
#include "StoreGate/ReadCondHandle.h"
#include "StoreGate/WriteCondHandle.h"
#include "CoolKernel/IObject.h"
#include "CxxUtils/get_unaligned.h"
#include <sstream>


namespace {

const EventIDBase::number_type UNDEFNUM = EventIDBase::UNDEFNUM;
const EventIDBase::event_number_t UNDEFEVT = EventIDBase::UNDEFEVT;
const EventIDRange fullrange (EventIDBase (0, UNDEFEVT, 0, 0, 0),
                              EventIDBase (UNDEFNUM-1, UNDEFEVT, UNDEFNUM-1, 0, 0));

} // anonymous namespace


/**
 * @brief Gaudi initialize method.
 */
StatusCode
LuminosityCondAlg::initialize()
{
  ATH_CHECK( m_luminosityOutputKey.initialize() );

  // May be empty if configured for MC.
  ATH_CHECK( m_luminosityFolderInputKey.initialize(SG::AllowEmpty) );
  ATH_CHECK( m_onlineLumiCalibrationInputKey.initialize(SG::AllowEmpty) );

  // Only used for run1.
  ATH_CHECK( m_bunchLumisInputKey.initialize(SG::AllowEmpty) );
  ATH_CHECK( m_bunchGroupInputKey.initialize(SG::AllowEmpty) );
  ATH_CHECK( m_fillParamsInputKey.initialize(SG::AllowEmpty) );
  return StatusCode::SUCCESS;
}


/**
 * @brief Algorithm execute method.
 * @param ctx Event Context.
 */
StatusCode
LuminosityCondAlg::execute (const EventContext& ctx) const
{
  auto lumi = std::make_unique<LuminosityCondData>();
  EventIDRange range;

  if (m_luminosityFolderInputKey.empty()) {
    // MC case.
    range = fullrange;
  }
  else {
    SG::ReadCondHandle<CondAttrListCollection> luminosityFolder
      (m_luminosityFolderInputKey, ctx);
    ATH_CHECK( luminosityFolder.range (range) );

    unsigned int preferredChannel;
    unsigned int calibChannel;
    const coral::Blob* bunchInstLumiBlob = nullptr;
    ATH_CHECK( updateAvgLumi (**luminosityFolder,
                              *lumi,
                              preferredChannel,
                              calibChannel,
                              bunchInstLumiBlob) );

    ATH_CHECK( updatePerBunchLumi (ctx,
                                   bunchInstLumiBlob,
                                   preferredChannel,
                                   calibChannel,
                                   range,
                                   *lumi) );
  }

  SG::WriteCondHandle<LuminosityCondData> luminosityCondData
    (m_luminosityOutputKey, ctx);
  ATH_CHECK( luminosityCondData.record (range, std::move (lumi)) );
  return StatusCode::SUCCESS;
}


/**
 * @brief Unpack luminosity data from the attribute list.
 * @param lumiData Input luminosity data.
 * @param lumi Output luminosity data being filled.
 * @param preferredChannel[out] Preferred luminosity channel to use.
 * @param calibChannel[out] Calibration luminosity channel to use.
 * @param bunchInstLumiBlob[out] Packed per-bunch luminosity data.
 *                               Set to null for Run 1.
 *
 * Unpacks luminosity data from the attribute list.
 * Fills in the average luminosity fields in @c lumi,
 * and determines the luminosity channels to use.
 * For Run 2 and later, returns the packed luminosity data.
 */
StatusCode
LuminosityCondAlg::updateAvgLumi (const CondAttrListCollection& lumiData,
                                  LuminosityCondData& lumi,
                                  unsigned int& preferredChannel,
                                  unsigned int& calibChannel,
                                  const coral::Blob*& bunchInstLumiBlob) const
                                  
{
  preferredChannel = 0;
  calibChannel = 0;
  bunchInstLumiBlob = nullptr;

  const coral::AttributeList& attrList = lumiData.attributeList (m_lumiChannel);
  if (attrList.size() == 0 || attrList["Valid"].isNull()) {
    ATH_MSG_DEBUG ("Can't find luminosity information for channel " << m_lumiChannel);
    return StatusCode::SUCCESS;
  }

  if (msgLvl (MSG::DEBUG)) {
    std::ostringstream attrStr1;
    attrList.toOutputStream( attrStr1 );
    ATH_MSG_DEBUG( "ChanNum " << m_lumiChannel << " Attribute list "
                   << attrStr1.str() );
  }

  // Check data availability
  if (attrList["LBAvInstLumi"].isNull() || attrList["LBAvEvtsPerBX"].isNull()) {
    ATH_MSG_ERROR( " NULL Luminosity information in database " );
    return StatusCode::FAILURE;
  }

  // Check validity (don't bother continuing if invalid)
  uint32_t valid = attrList["Valid"].data<cool::UInt32>();
  lumi.setLbAverageValid (valid);
  if (valid & 0x01) {
    if (m_skipInvalid) {
      if (!m_expectInvalid) {
        ATH_MSG_WARNING( " Invalid LB Average luminosity ... set lumi to 0" );
      }
      return StatusCode::SUCCESS;
    } else {
      ATH_MSG_WARNING( " Invalid LB Average luminosity ... continuing because skipInvalid == FALSE" );
    }
  }

  // Get preferred channel (needed for per-BCID calculation)
  if (m_lumiChannel == 0u) {

    // Check if we have a payload for this (Run2 only)      
    bool hasAlgorithmID = false;
    for (coral::AttributeList::const_iterator attr = attrList.begin();
	 attr != attrList.end(); ++attr) {
      if (attr->specification().name() == "AlgorithmID") {
	hasAlgorithmID = true;
	break;
      }
    }

    if (hasAlgorithmID) {
      // In Run2, channel 0 should be good.  Leave as is
      preferredChannel = m_lumiChannel;
      calibChannel = attrList["AlgorithmID"].data<cool::UInt32>();

    } else {
      // In Run1, we need to recalculate from the actual channel number
      preferredChannel = (valid >> 22);
      calibChannel = preferredChannel;
    }

  } else {
    preferredChannel = m_lumiChannel;
    calibChannel = m_lumiChannel;
  }

  float LBAvInstLumi = attrList["LBAvInstLumi"].data<cool::Float>();   // Lumi
  float LBAvEvtsPerBX = attrList["LBAvEvtsPerBX"].data<cool::Float>(); // Mu

  // Check (and protect for NaN
  if ( std::isnan (LBAvInstLumi) ) {
    ATH_MSG_WARNING( " Luminosity is not a number.. " << LBAvInstLumi << "  ... set it to 0 " );
    LBAvInstLumi=0.;
  }

  if ( std::isnan (LBAvEvtsPerBX) ) {
    ATH_MSG_WARNING( " Luminosity is not a number.. " << LBAvEvtsPerBX << "  ... set it to 0 " );
    LBAvEvtsPerBX=0.;
  }

  lumi.setLbAverageLuminosity (LBAvInstLumi);
  lumi.setLbAverageInteractionsPerCrossing (LBAvEvtsPerBX);

  // Check validity of per-BCID luminosity (will issue warning in recalcPerBCIDLumi
  int perBcidValid = ((valid&0x3ff)/10) % 10;
  if ((perBcidValid > 0) && m_skipInvalid) {
    return StatusCode::SUCCESS;
  }

  // Also save per-BCID blob if it exists
  for (coral::AttributeList::const_iterator attr = attrList.begin();
       attr != attrList.end(); ++attr)
  {
    if (attr->specification().name() == "BunchInstLumi") {
      if (!attrList["BunchInstLumi"].isNull())
	bunchInstLumiBlob = &attrList["BunchInstLumi"].data<coral::Blob>();
      break;
    }
  }

  return StatusCode::SUCCESS;
}


/**
 * @brief Fill in per-bunch luminosity data.
 * @param ctx Event context.
 * @param bunchInstLumiBlob Packed per-bunch luminosity data.
 *                          Null for Run 1.
 * @param preferredChannel Preferred luminosity channel to use.
 * @param calibChannel Calibration luminosity channel to use.
 * @param range Validity range of the conditions data being filled.
 *              Updated if needed.
 * @param lumi Output luminosity data being filled.
 */
StatusCode
LuminosityCondAlg::updatePerBunchLumi (const EventContext& ctx,
                                       const coral::Blob* bunchInstLumiBlob,
                                       unsigned int preferredChannel,
                                       unsigned int calibChannel,
                                       EventIDRange& range,
                                       LuminosityCondData& lumi) const
{
  if (lumi.lbAverageLuminosity() <= 0.) {
    if (!m_expectInvalid) {
      ATH_MSG_WARNING( "LBAvInstLumi is zero or negative in updatePerBunchLumi():"
                       << lumi.lbAverageLuminosity());
    }
    range = EventIDRange::intersect (range, fullrange);
    return StatusCode::SUCCESS;
  }

  bool isValid = true;
  ATH_CHECK( updateMuToLumi (ctx, calibChannel, range, lumi, isValid) );

  // Check here if we want to do this the Run1 way (hard) or the Run2 way (easy)

  if (!isValid) {
    // Skip if not valid.
  }
  else if (bunchInstLumiBlob != nullptr) { // Run2 way, easy
    ATH_CHECK( updatePerBunchLumiRun2 (*bunchInstLumiBlob,
                                       preferredChannel,
                                       lumi) );
  }
  else { // Run1 way, hard!
    ATH_CHECK( updatePerBunchLumiRun1 (ctx,
                                       preferredChannel,
                                       range,
                                       lumi) );
  }

  ATH_MSG_DEBUG( "finished updatePerBunchLumi() for alg: "
                 << preferredChannel );
  return StatusCode::SUCCESS;
}


/**
 * @brief Fill in mu-to-lumi calibration.
 * @param ctx Event context.
 * @param calibChannel Calibration luminosity channel to use.
 * @param range Validity range of the conditions data being filled.
 *              Updated if needed.
 * @param lumi Output luminosity data being filled.
 * @param isValid Set to false if data are not valid.
 */
StatusCode
LuminosityCondAlg::updateMuToLumi (const EventContext& ctx,
                                   unsigned int calibChannel,
                                   EventIDRange& range,
                                   LuminosityCondData& lumi,
                                   bool& isValid) const
{
  SG::ReadCondHandle<OnlineLumiCalibrationCondData> onlineLumiCalibration
    ( m_onlineLumiCalibrationInputKey, ctx );
  EventIDRange range2;
  ATH_CHECK( onlineLumiCalibration.range (range2) );
  range = EventIDRange::intersect (range, range2);

  // This is the only correct way to do this!
  // The division below gives average mu (over all bunches) to total lumi
  float muToLumi = onlineLumiCalibration->getMuToLumi (calibChannel);

  // Check if this is reasonable
  if (muToLumi < 0.) {
    ATH_MSG_WARNING(" Found muToLumi = " << muToLumi << " for channel " << calibChannel << ". Try backup channel..." );
    muToLumi = onlineLumiCalibration->getMuToLumi(m_calibBackupChannel);
    ATH_MSG_WARNING(" Found muToLumi = " << muToLumi << " for backup channel " << m_calibBackupChannel);
  }

  lumi.setMuToLumi (muToLumi);

  // Check validity
  isValid = true;
  int perBcidValid = ((lumi.lbAverageValid()&0x3ff)/10) % 10;
  if ((lumi.lbAverageValid() & 0x03) || (perBcidValid > 0)) {  // Skip if either per-BCID or LBAv is invalid
    isValid = false;
    if (m_skipInvalid) {
      ATH_MSG_WARNING( " Invalid per-BCID luminosity found: "
                       << lumi.lbAverageValid() << "!" );
      return StatusCode::SUCCESS;
    } else {
      ATH_MSG_WARNING( " Invalid per-BCID luminosity found: "
                       << lumi.lbAverageValid()
                       << " continuing because skipInvalid == FALSE" );
    }
  }

  // Now check muToLumi and report depending upon whether lumi is valid or not
  if (muToLumi < 0.) {
    if (isValid) {
      ATH_MSG_ERROR(" Found invalid muToLumi = " << muToLumi << " for backup channel " << m_calibBackupChannel << "!");
    } else {
      ATH_MSG_WARNING(" Found invalid muToLumi = " << muToLumi << " for backup channel " << m_calibBackupChannel << "!");
    }

    // Don't keep negative values
    muToLumi = 0.;
    lumi.setMuToLumi (muToLumi);
  }

  ATH_MSG_DEBUG(" Found muToLumi = " << muToLumi << " for channel "
                << calibChannel );

  return StatusCode::SUCCESS;
}


/**
 * @brief Fill in per-bunch luminosity data, run 2 and later.
 * @param bunchInstLumiBlob Packed per-bunch luminosity data.
 * @param preferredChannel Preferred luminosity channel to use.
 * @param lumi Output luminosity data being filled.
 */
StatusCode
LuminosityCondAlg::updatePerBunchLumiRun2 (const coral::Blob& bunchInstLumiBlob,
                                           unsigned int preferredChannel,
                                           LuminosityCondData& lumi) const
{
  ATH_MSG_DEBUG( "starting updatePerBunchLumiRun2() for alg: " << preferredChannel );

  // Check that the length isn't zero
  if (bunchInstLumiBlob.size() == 0) {
    ATH_MSG_ERROR("BunchInstLumi blob found with zero length!");
    return StatusCode::FAILURE;
  }

  // Hardcode the Run2 BLOB decoding (should use CoolLumiUtilities...)
  const uint8_t* ATH_RESTRICT pchar =
    static_cast<const uint8_t*>(bunchInstLumiBlob.startingAddress()); // First byte holds storage size and mode
  unsigned int bss = ((*pchar) % 100) / 10;  // Byte storage size
  unsigned int smod = ((*pchar) % 10);       // Storage mode
  ++pchar;  // Points to next char after header

  ATH_MSG_DEBUG( "BunchInstLumi blob found with storage mode " << smod
                 << " and byte storage size " << bss );

  // Make sure we have what we think we have
  if (bss != 4 || smod != 1) {
    ATH_MSG_ERROR( "BunchInstLumi blob found with storage mode " << smod << " and byte storage size " << bss << " - Unknown!");
    return StatusCode::FAILURE;
  }

  unsigned int nbcids = LuminosityCondData::TOTAL_LHC_BCIDS;
  unsigned int bloblength = bss * nbcids + 1;

  if (static_cast<cool::UInt32>(bunchInstLumiBlob.size()) != bloblength) {
    ATH_MSG_ERROR( "BunchRawInstLumi blob found with length"
                   << bunchInstLumiBlob.size() << "in storage mode" << smod
                   <<  ", expecting " << bloblength << "!" );
    return StatusCode::FAILURE;
  }

  // Length is correct, read raw data according to packing scheme
  // This is absolute luminosity, so just unpack values into our array
    
  ATH_MSG_DEBUG( "Unpacking lumi value from blob");
  std::vector<float> instLumi (nbcids);
  for (unsigned int i=0; i<nbcids; i++) {
    // Can't use assignment directly because source may be misaligned.
    instLumi[i] = CxxUtils::get_unaligned_float (pchar);
  }

  if (msgLvl (MSG::DEBUG)) {
    for (unsigned int i=0; i<nbcids; i++) {
      ATH_MSG_DEBUG( "Bcid: " << i << " Lumi: " << instLumi[i] );
    }
  }

  lumi.setLbLuminosityPerBCIDVector (std::move (instLumi));

  return StatusCode::SUCCESS;
}


/**
 * @brief Fill in per-bunch luminosity data, run 1.
 * @param preferredChannel Preferred luminosity channel to use.
 * @param range Validity range of the conditions data being filled.
 *              Updated if needed.
 * @param lumi Output luminosity data being filled.
 */
StatusCode
LuminosityCondAlg::updatePerBunchLumiRun1 (const EventContext& ctx,
                                           unsigned int preferredChannel,
                                           EventIDRange& range,
                                           LuminosityCondData& lumi) const
{
  ATH_MSG_DEBUG( "starting updatePerBunchLumiRun1() for alg: " << preferredChannel );
    
  if (preferredChannel == 0) {
    return StatusCode::SUCCESS;
  }

  // Nothing to do if we don't have the ingredients
  if (m_onlineLumiCalibrationInputKey.empty()) {
    ATH_MSG_ERROR( "OnlineLumiCalibrationInputKey.empty() is TRUE, skipping..." );
    return StatusCode::FAILURE;
  }
  if (m_bunchLumisInputKey.empty()) {
    ATH_MSG_DEBUG( "BunchLumisInputKey.empty() is TRUE, skipping..." );
    return StatusCode::FAILURE;
  }
  if (m_bunchGroupInputKey.empty()) {
    ATH_MSG_DEBUG( "BunchGroupTool.empty() is TRUE, skipping..." );
    return StatusCode::FAILURE;
  }
  if (m_fillParamsInputKey.empty()) {
    ATH_MSG_ERROR( "FillParamsInputKey.empty() is TRUE, skipping..." );
    return StatusCode::FAILURE;
  }

  SG::ReadCondHandle<OnlineLumiCalibrationCondData> onlineLumiCalibration
    (m_onlineLumiCalibrationInputKey, ctx);
  SG::ReadCondHandle<BunchLumisCondData> bunchLumis (m_bunchLumisInputKey, ctx);
  SG::ReadCondHandle<BunchGroupCondData> bunchGroup (m_bunchGroupInputKey, ctx);
  SG::ReadCondHandle<FillParamsCondData> fillParams (m_fillParamsInputKey, ctx);

  EventIDRange range2;
  ATH_CHECK( onlineLumiCalibration.range (range2) );
  range = EventIDRange::intersect (range, range2);
  ATH_CHECK( bunchLumis.range (range2) );
  range = EventIDRange::intersect (range, range2);
  ATH_CHECK( bunchGroup.range (range2) );
  range = EventIDRange::intersect (range, range2);
  ATH_CHECK( fillParams.range (range2) );
  range = EventIDRange::intersect (range, range2);

  const std::vector<unsigned int>& luminousBunches = fillParams->luminousBunches();
  ATH_MSG_DEBUG( "N LuminousBunches:" << luminousBunches.size() );

  // Get the raw data for the preferred channel
  const std::vector<float>& rawLumiVec = bunchLumis->rawLuminosity(preferredChannel);
  if (rawLumiVec.empty()) {
    ATH_MSG_DEBUG( "Empty raw luminosity vector" );
    return StatusCode::SUCCESS;
  }

  //
  // Calibration step
  //
    
  //  Here we want to go through and calibrate raw values in the luminous bunches only.
  // This is what the OL adds up, and since these are online calibrations, we want to rescale the total
  // to agree to whatever offline tag we are using.
  std::vector<float> calLumiVec (LuminosityCondData::TOTAL_LHC_BCIDS, 0.);
    
  // Update muToLumi while we are at it (also check that calibration exists)
  float muToLumi = onlineLumiCalibration->getMuToLumi (preferredChannel);
  if (muToLumi <= 0.) {
    ATH_MSG_ERROR( " dont have calibration information for preferred channel "
                   << preferredChannel << "!" );
    return StatusCode::FAILURE;
  }
  lumi.setMuToLumi (muToLumi);

  double lumiSum = 0.;
  for (unsigned int bcid : luminousBunches) {
    // Don't waste time on zero lumi 
    if (rawLumiVec.at(bcid) <= 0.) {
      ATH_MSG_DEBUG( "Calibrate BCID " << bcid << " with raw "
                     << rawLumiVec.at(bcid) << " -> skipping" );
      continue;
    }

    // Calibrate
    if (!onlineLumiCalibration->calibrateLumi(preferredChannel,
                                              rawLumiVec[bcid],
                                              calLumiVec[bcid]))
    {
      ATH_MSG_DEBUG( "Calibrate BCID " << bcid << " with raw " << rawLumiVec[bcid] << " -> calibration failed!" );
      ATH_MSG_WARNING( "Per-BCID calibration failed for bcid " << bcid << " with raw lumi = " << rawLumiVec[bcid] );
      continue;
    }
      
    lumiSum += calLumiVec[bcid];
      
    ATH_MSG_DEBUG( "Calibrate BCID " << bcid << " with raw " << rawLumiVec[bcid] << " -> " << calLumiVec[bcid] );
  }

  // Work out scale factor between offline and online estimate
  float offlineOnlineRatio = 1.;
  if (lumiSum > 0.) offlineOnlineRatio = lumi.lbAverageLuminosity() / lumiSum;
    
  ATH_MSG_DEBUG( " Offline/Online scale factor: " << lumi.lbAverageLuminosity()
                 << " / " << lumiSum << " = " << offlineOnlineRatio );

  // Make sure we have values for all BCIDs in the physics bunch group
  for (unsigned int bcid : bunchGroup->bunchGroup (1)) {
    // Don't repeat if value already exists
    if (calLumiVec[bcid] > 0.) continue;
    if (rawLumiVec[bcid] <= 0.) continue;
      
    // Calibrate
    if (!onlineLumiCalibration->calibrateLumi(preferredChannel,
                                              rawLumiVec[bcid],
                                              calLumiVec[bcid]))
    {
      ATH_MSG_DEBUG( " -> Calibration failed!" );
      ATH_MSG_WARNING( "Per-BCID calibration failed for bcid " << bcid
                       << " with raw lumi = " << rawLumiVec[bcid] );
      continue;
    }
  }

  // Almost done, now we apply the scale factor to all BCIDs
  for (float& lumi : calLumiVec) {
    lumi *= offlineOnlineRatio;
  }

  lumi.setLbLuminosityPerBCIDVector (std::move (calLumiVec));

  return StatusCode::SUCCESS;
}
