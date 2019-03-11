/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef XAOD_ANALYSIS

#include "LumiBlockComps/LuminosityTool.h"

#include "CoolKernel/IObject.h"
#include "AthenaPoolUtilities/CondAttrListCollection.h"
#include "AthenaPoolUtilities/AthenaAttributeList.h"

#include "CoolLumiUtilities/BunchLumisUtil.h"

#include "AthenaKernel/errorcheck.h"

// Total number of BCIDs in one turn - this must match value used in /TDAQ/OLC/BUNCHLUMIS for storage mode 1
const unsigned int TOTAL_LHC_BCIDS = 3564;

//--------------------------------------------------

LuminosityTool::LuminosityTool(const std::string& type,
				 const std::string& name,
				 const IInterface* parent)
  : AthAlgTool(type, name, parent),
    m_lumiFolderName(""), // "/TRIGGER/OFLLUMI/LBLESTOFL"),
    m_lumiChannel(0),
    m_fillParamsTool(""), //FillParamsTool"),
    m_bunchLumisTool(""), //BunchLumisTool"),
    m_bunchGroupTool(""), //BunchGroupTool"),
    m_onlineLumiCalibrationTool(""), //OnlineLumiCalibrationTool"),
    m_lblbFolderName(""), ///TRIGGER/LUMI/LBLB"),
    m_recalcPerBCIDLumi(true),
    m_preferredChannel(0),
    m_luminousBunches(0),
    m_bunchInstLumiBlob(NULL),
    m_calibChannel(0),
    m_calibBackupChannel(112),
    m_skipInvalid(true)
{
  declareInterface<ILuminosityTool>(this);
  declareProperty("LumiFolderName", m_lumiFolderName);
  declareProperty("LumiChannelNumber", m_lumiChannel);
  declareProperty("FillParamsTool", m_fillParamsTool);
  declareProperty("BunchLumisTool", m_bunchLumisTool);
  declareProperty("BunchGroupTool", m_bunchGroupTool);
  declareProperty("OnlineLumiCalibrationTool", m_onlineLumiCalibrationTool);
  declareProperty("LBLBFolderName", m_lblbFolderName);
  declareProperty("CalibBackupChannel", m_calibBackupChannel);
  declareProperty("SkipInvalid", m_skipInvalid);

  m_LBAvInstLumi = 0.;
  m_LBAvEvtsPerBX = 0.;
  m_Valid = 0xFFFFFFFF;

  m_LBDuration = 0.;

  m_MuToLumi = 0.;
  m_LBInstLumi.assign(TOTAL_LHC_BCIDS, 0.);

  m_luminousBunchesVec.clear();
}

StatusCode
LuminosityTool::initialize()
{
  ATH_MSG_DEBUG("LuminosityTool::initialize() begin");

  ATH_CHECK(m_eventInfoKey.initialize());

  ATH_MSG_INFO("LuminosityTool::initialize() registering " << m_lumiFolderName);

  // In addition to local, private callback functions, also set up callbacks to updateCache any time
  // the local cached data changes.
  // This must be done with the interface (ILuminosityTool) so external code can use this to trigger
  // their own callbacks.

  // Setup callback
  if (m_lumiFolderName.empty()) {
    ATH_MSG_INFO( "LumiFolderName is empty, skipping" );

  } else if (detStore()->contains<CondAttrListCollection>(m_lumiFolderName)) {

    const DataHandle<CondAttrListCollection> aptr;

    CHECK(detStore()->regFcn(&LuminosityTool::updateAvgLumi, this , aptr, m_lumiFolderName));
    CHECK(detStore()->regFcn(&LuminosityTool::updateAvgLumi, this, &ILuminosityTool::updateCache, dynamic_cast<ILuminosityTool*>(this)));

    ATH_MSG_INFO( " Registered a callback for " << m_lumiFolderName << " COOL folder " );
  } else {
    ATH_MSG_ERROR( " cannot find " << m_lumiFolderName << " in DetectorStore" );
  }

  //
  // Calibration folder
  if (m_onlineLumiCalibrationTool.empty()) {
    // May not be configured, could be OK
    ATH_MSG_INFO( "OnlineLumiCalibrationTool.empty() is TRUE, skipping..." );
  } else {
    ATH_MSG_INFO( "Retrieving OnlineLumiCalibrationTool handle" );
    CHECK(m_onlineLumiCalibrationTool.retrieve());

    // Setup callback on OnlineLumiCalibrationTool change
    ATH_MSG_INFO( "Registering callback on IOnlineLumiCalibrationTool::updateCache" );
    CHECK(detStore()->regFcn(&IOnlineLumiCalibrationTool::updateCache, dynamic_cast<IOnlineLumiCalibrationTool*>(&(*m_onlineLumiCalibrationTool)), &ILuminosityTool::updateCache, dynamic_cast<ILuminosityTool*>(this)));
  }

  //
  // Fillparams folder
  if (m_fillParamsTool.empty()) {
    // May not be configured, could be OK
    ATH_MSG_INFO( "FillParamsTool.empty() is TRUE, skipping..." );
  } else {
    ATH_MSG_INFO( "Retrieving FillParamsTool handle" );
    CHECK(m_fillParamsTool.retrieve());

    // Setup callback on FillParamsTool change
    ATH_MSG_INFO( "Registering callback on IFillParamsTool::updateCache" );
    CHECK(detStore()->regFcn(&IFillParamsTool::updateCache, dynamic_cast<IFillParamsTool*>(&(*m_fillParamsTool)), &ILuminosityTool::updateCache, dynamic_cast<ILuminosityTool*>(this)));
  }

  //
  // Bunchlumis folder
  if (m_bunchLumisTool.empty()) {
    // May not be configured, could be OK
    ATH_MSG_INFO( "BunchLumisTool.empty() is TRUE, skipping..." );
  } else {
    ATH_MSG_INFO( "Retrieving BunchLumisTool handle" );
    CHECK(m_bunchLumisTool.retrieve());

    // Setup callback on BunchLumisTool change
    ATH_MSG_INFO( "Registering callback on IBunchLumisTool::updateCache" );
    CHECK(detStore()->regFcn(&IBunchLumisTool::updateCache, dynamic_cast<IBunchLumisTool*>(&(*m_bunchLumisTool)), &ILuminosityTool::updateCache, dynamic_cast<ILuminosityTool*>(this)));
  }

  //
  // BunchGroup folder
  if (m_bunchGroupTool.empty()) {
    // May not be configured, could be OK
    ATH_MSG_INFO( "BunchGroupTool.empty() is TRUE, skipping..." );
  } else {
    ATH_MSG_INFO( "Retrieving BunchGroupTool handle" );
    CHECK(m_bunchGroupTool.retrieve());

    // Setup callback on BunchGroupTool change
    ATH_MSG_INFO( "Registering callback on IBunchGroupTool::updateCache" );
    CHECK(detStore()->regFcn(&IBunchGroupTool::updateCache, dynamic_cast<IBunchGroupTool*>(&(*m_bunchGroupTool)), &ILuminosityTool::updateCache, dynamic_cast<ILuminosityTool*>(this)));
  }

  //
  // LBLB folder
  if (m_lblbFolderName.empty()) {
    ATH_MSG_INFO("LBLBFolderName is empty, skipping...");

  } else if (detStore()->contains<AthenaAttributeList>(m_lblbFolderName)) {

    const DataHandle<AthenaAttributeList> aptr;

    CHECK(detStore()->regFcn(&LuminosityTool::updateLBLB, this , aptr, m_lblbFolderName));
    CHECK(detStore()->regFcn(&LuminosityTool::updateLBLB, this , &ILuminosityTool::updateCache, dynamic_cast<ILuminosityTool*>(this)));

    ATH_MSG_INFO( " Registered a callback for " << m_lblbFolderName << " COOL folder " );
  } else {
    ATH_MSG_ERROR( " cannot find " << m_lblbFolderName << " in DetectorStore" );
  }

  ATH_MSG_DEBUG( "LuminosityTool::initialize() end" );
  return StatusCode::SUCCESS;
}

StatusCode
LuminosityTool::finalize()
{
  ATH_MSG_DEBUG( "LuminosityTool::finalize()" );
  return StatusCode::SUCCESS;
}

float
LuminosityTool::lbAverageLuminosity() const {
  return m_LBAvInstLumi;
}

float
LuminosityTool::lbAverageInteractionsPerCrossing() const {
  return m_LBAvEvtsPerBX;
}

unsigned int
LuminosityTool::lbAverageValid() const {
  return m_Valid;
}

double
LuminosityTool::lbDuration() const {
  return m_LBDuration;
}

const std::vector<float>&
LuminosityTool::lbLuminosityPerBCIDVector() {
  if (m_recalcPerBCIDLumi) recalculatePerBCIDLumi();
  return m_LBInstLumi;
}

float
LuminosityTool::lbLuminosityPerBCID() {

  SG::ReadHandle<xAOD::EventInfo> eventInfo(m_eventInfoKey);  
  // check is only useful for serial running; remove when MT scheduler used
  if(!eventInfo.isValid()) {
    ATH_MSG_FATAL("Failed to retrieve "<< m_eventInfoKey.key());
    return 0;
  }

  float lumi = lbLuminosityPerBCID(eventInfo->bcid());

  ATH_MSG_DEBUG( "From DB, LB " << eventInfo->lumiBlock() << " bcid " << eventInfo->bcid() << " -> " << lumi << " ub-1 s-1" );

  return lumi;
}

float
LuminosityTool::lbLuminosityPerBCID(unsigned int bcid) {
  if (bcid >= TOTAL_LHC_BCIDS) {
    ATH_MSG_WARNING( "Request for lumi with bcid " << bcid << " > " << TOTAL_LHC_BCIDS << "!" );
    return 0.;
  }

  if (m_recalcPerBCIDLumi) recalculatePerBCIDLumi();
  return m_LBInstLumi[bcid];
}

float
LuminosityTool::muToLumi() {
  // Make sure this is up to date
  if (m_recalcPerBCIDLumi) recalculatePerBCIDLumi();
  return m_MuToLumi;
}

//
// Callback functions
//
StatusCode
LuminosityTool::updateCache( IOVSVC_CALLBACK_ARGS_P(/*idx*/, /*keys*/) )
{
  ATH_MSG_DEBUG( "in updateCache() " );
  m_recalcPerBCIDLumi = true;  
  return StatusCode::SUCCESS;
}

StatusCode
LuminosityTool::updateAvgLumi( IOVSVC_CALLBACK_ARGS_P(/*idx*/, /*keys*/) )
{
  ATH_MSG_DEBUG( "in updateAvgLumi() " );

  // Ensure zero on error
  m_LBAvInstLumi = 0.;
  m_LBAvEvtsPerBX = 0.;
  m_Valid = 0xFFFFFFFF;
  m_preferredChannel = 0;
  m_calibChannel = 0;
  m_bunchInstLumiBlob = NULL;

  // Check if we have anything to do
  // Shouldn't actually get a callback if this folder doesn't exist...
  if (m_lumiFolderName.empty()) return StatusCode::SUCCESS;

  const CondAttrListCollection* attrListColl = 0;
  CHECK(detStore()->retrieve(attrListColl, m_lumiFolderName));

  // Loop over collection and find the requested channel
  CondAttrListCollection::const_iterator first = attrListColl->begin();
  CondAttrListCollection::const_iterator last  = attrListColl->end();
  for (; first != last; ++first) 
    if ((*first).first == m_lumiChannel) break;

  // Make sure we found it
  if (first == last) {
    ATH_MSG_WARNING( "Channel: " << m_lumiChannel << " not found in " << m_lumiFolderName << "!" );
    return StatusCode::SUCCESS;
  }

  // OK, get luminosity 
  std::ostringstream attrStr1;
  (*first).second.toOutputStream( attrStr1 );
  ATH_MSG_DEBUG( "ChanNum " << (*first).first << " Attribute list " << attrStr1.str() );

  const coral::AttributeList& attrList = (*first).second;

  // Check data availability
  if (attrList["LBAvInstLumi"].isNull() || attrList["LBAvEvtsPerBX"].isNull()) {
    ATH_MSG_WARNING( " NULL Luminosity information in database ... set it to 0 " );
    return StatusCode::SUCCESS;
  }

  if (attrList["Valid"].isNull()) {
      ATH_MSG_WARNING( " NULL validity information ... set lumi to 0" );
      return StatusCode::SUCCESS;
  }

  // Check validity (don't bother continuing if invalid)
  m_Valid = attrList["Valid"].data<cool::UInt32>();
  if (m_Valid & 0x01) {
    if (m_skipInvalid) {
      ATH_MSG_WARNING( " Invalid LB Average luminosity ... set lumi to 0" );
      return StatusCode::SUCCESS;
    } else {
      ATH_MSG_WARNING( " Invalid LB Average luminosity ... continuing because skipInvalid == FALSE" );
    }
  }

  // Get preferred channel (needed for per-BCID calculation)
  if (m_lumiChannel == 0) {

    // Check if we have a payload for this (Run2 only)      
    bool hasAlgorithmID = false;
    for (coral::AttributeList::const_iterator attr = attrList.begin();
	 attr != attrList.end(); ++attr) {
      //const std::string& name = attr->specification().name();
      if (attr->specification().name() == "AlgorithmID") {
	hasAlgorithmID = true;
	break;
      }
    }

    if (hasAlgorithmID) {
      // In Run2, channel 0 should be good.  Leave as is
      m_preferredChannel = m_lumiChannel;
      m_calibChannel = attrList["AlgorithmID"].data<cool::UInt32>();

    } else {
      // In Run1, we need to recalculate from the actual channel number
      m_preferredChannel = (m_Valid >> 22);
      m_calibChannel = m_preferredChannel;
    }

  } else {
    m_preferredChannel = m_lumiChannel;
    m_calibChannel = m_lumiChannel;
  }

  m_LBAvInstLumi = attrList["LBAvInstLumi"].data<cool::Float>();   // Lumi
  m_LBAvEvtsPerBX = attrList["LBAvEvtsPerBX"].data<cool::Float>(); // Mu

  // Check (and protect for NaN
  if ( !(m_LBAvInstLumi == m_LBAvInstLumi) ) {
    ATH_MSG_WARNING( " Luminosity is not a number.. " << m_LBAvInstLumi << "  ... set it to 0 " );
    m_LBAvInstLumi=0.;
  }

  if ( !(m_LBAvEvtsPerBX == m_LBAvEvtsPerBX) ) {
    ATH_MSG_WARNING( " Luminosity is not a number.. " << m_LBAvEvtsPerBX << "  ... set it to 0 " );
    m_LBAvEvtsPerBX=0.;
  }

  // MuToLumi gets updated in recalcPerBCIDLumi

  // Check validity of per-BCID luminosity (will issue warning in recalcPerBCIDLumi
  int perBcidValid = (m_Valid/10) % 10;
  if ((perBcidValid > 0) && m_skipInvalid) {
    return StatusCode::SUCCESS;
  }

  // Also save per-BCID blob if it exists
  for (coral::AttributeList::const_iterator attr = attrList.begin();
       attr != attrList.end(); ++attr) {
    //const std::string& name = attr->specification().name();
    if (attr->specification().name() == "BunchInstLumi") {
      if (!attrList["BunchInstLumi"].isNull())
	m_bunchInstLumiBlob = &attrList["BunchInstLumi"].data<coral::Blob>();

      break;
    }
  }

  return StatusCode::SUCCESS;
}

StatusCode
LuminosityTool::updateLBLB( IOVSVC_CALLBACK_ARGS_P(/*idx*/, /*keys*/) )
{
  ATH_MSG_DEBUG( "in updateLBLB() " );

  // Ensure zero on error
  m_LBDuration = 0.;

  // Protect against no data
  if (m_lblbFolderName.empty()) return StatusCode::SUCCESS;

  const AthenaAttributeList* attrList = 0;
  CHECK(detStore()->retrieve(attrList, m_lblbFolderName));

  if ((*attrList)["StartTime"].isNull()) {
    ATH_MSG_WARNING( "StartTime is NULL in " << m_lblbFolderName << "!" );
    return StatusCode::SUCCESS;
  }

  if ((*attrList)["EndTime"].isNull()) {
    ATH_MSG_WARNING( "EndTime is NULL in " << m_lblbFolderName << "!" );
    return StatusCode::SUCCESS;
  }

  cool::UInt63 startTime = (*attrList)["StartTime"].data<cool::UInt63>();
  cool::UInt63 endTime = (*attrList)["EndTime"].data<cool::UInt63>();
  double secPerNs = 1.E9;

  m_LBDuration = (endTime-startTime)/secPerNs;

  ATH_MSG_DEBUG( "Luminosity block duration: " << m_LBDuration << " seconds" ); 
  return StatusCode::SUCCESS;
}

//
// Main routine to re-calibrate per-BCID luminosity from raw data.
// This should be called only when inputs change (LB boundaries) and re-calculates all colliding BCIDs at once.
// This is slightly inefficient, but necessary to rescale online per-BCID values to offline calibrated luminosity.
//
// Results end up in m_LBEvtsPerBX map keyed by BCID number
//

void
LuminosityTool::recalculatePerBCIDLumi()
{
  ATH_MSG_DEBUG( "in recalculatePerBCIDLumi() for alg: " << m_preferredChannel );

  // Clear the request flag
  m_recalcPerBCIDLumi = false;

  // Clear the calibrated luminosity data
  m_LBInstLumi.assign(TOTAL_LHC_BCIDS, 0.);

  // Set some default values
  m_MuToLumi = 0.;

  // Make some sanity checks that we have everyting we need
  if (m_lumiFolderName.empty()) {
    ATH_MSG_INFO( "LumiFolderName is empty in recalculatePerBCIDLumi()!");
    return;
  }

  if (m_LBAvInstLumi <= 0.) {
    ATH_MSG_INFO( "LBAvInstLumi is zero or negative in recalculatePerBCIDLumi():" << m_LBAvInstLumi);
    return;
  }

  // Update muToLumi (check value later)
  if (!m_onlineLumiCalibrationTool.empty()) {
    // This is the only correct way to do this!
    // The division below gives average mu (over all bunches) to total lumi
    m_MuToLumi = m_onlineLumiCalibrationTool->getMuToLumi(m_calibChannel);

    // Check if this is reasonable
    if (m_MuToLumi < 0.) {
      ATH_MSG_INFO(" Found muToLumi = " << m_MuToLumi << " for channel " << m_calibChannel << ". Try backup channel..." );
      m_MuToLumi = m_onlineLumiCalibrationTool->getMuToLumi(m_calibBackupChannel);
      ATH_MSG_INFO(" Found muToLumi = " << m_MuToLumi << " for backup channel " << m_calibBackupChannel);
    }
  } else {
    ATH_MSG_WARNING(" No onlineCalibrationTool found, can't set muToLumi!");
  }

  // Check validity
  bool isValid = true;
  int perBcidValid = (m_Valid/10) % 10;
  if ((m_Valid & 0x03) || (perBcidValid > 0)) {  // Skip if either per-BCID or LBAv is invalid
    isValid = false;
    if (m_skipInvalid) {
      ATH_MSG_WARNING( " Invalid per-BCID luminosity found: " << m_Valid << "!" );
      return;
    } else {
      ATH_MSG_WARNING( " Invalid per-BCID luminosity found: " << m_Valid << " continuing because skipInvalid == FALSE" );
    }
  }


  // Now check muToLumi and report depending upon whether lumi is valid or not
  if (m_MuToLumi < 0.) {
    if (isValid) {
      ATH_MSG_ERROR(" Found invalid muToLumi = " << m_MuToLumi << " for backup channel " << m_calibBackupChannel << "!");
    } else {
      ATH_MSG_WARNING(" Found invalid muToLumi = " << m_MuToLumi << " for backup channel " << m_calibBackupChannel << "!");
    }

    // Don't keep negative values
    m_MuToLumi = 0.;
  }

  ATH_MSG_DEBUG(" Found muToLumi = " << m_MuToLumi << " for channel " << m_calibChannel );


  // Check here if we want to do this the Run1 way (hard) or the Run2 way (easy)

  if (m_bunchInstLumiBlob != NULL) { // Run2 way, easy
    ATH_MSG_DEBUG( "starting Run2 recalculatePerBCIDLumi() for alg: " << m_preferredChannel );

    // Check that the length isn't zero
    if (m_bunchInstLumiBlob->size() == 0) {
      ATH_MSG_WARNING("BunchInstLumi blob found with zero length!");
      return;
    }

    // Utility class to do the work
    //BunchLumisUtil lumiUtil;
    //if (!lumiUtil.unpackBlob(m_bunchInstLumiBlob)) {
    //  ATH_MSG_WARNING( "Failed to unpack blob: " << lumiUtil.error );
    //  return;
    //}

    //m_LBInstLumi = lumiUtil.bunchLumis();

    // Hardcode the Run2 BLOB decoding (should use CoolLumiUtilities...)
    const char* pchar = static_cast<const char*>(m_bunchInstLumiBlob->startingAddress()); // First byte holds storage size and mode
    unsigned int bss = ((*pchar) % 100) / 10;  // Byte storage size
    unsigned int smod = ((*pchar) % 10);       // Storage mode

    ATH_MSG_DEBUG( "BunchInstLumi blob found with storage mode " << smod << " and byte storage size " << bss );

    // Make sure we have what we think we have
    if (bss != 4 || smod != 1) {
      ATH_MSG_WARNING( "BunchInstLumi blob found with storage mode " << smod << " and byte storage size " << bss << " - Unknown!");
      return;
    }

    unsigned int nbcids = TOTAL_LHC_BCIDS;
    unsigned int bloblength = bss * nbcids + 1;

    if (static_cast<cool::UInt32>(m_bunchInstLumiBlob->size()) != bloblength) {
      ATH_MSG_WARNING( "BunchRawInstLumi blob found with length" << m_bunchInstLumiBlob->size() << "in storage mode" << smod <<  ", expecting " << bloblength << "!" );
      return;
    }

    // Length is correct, read raw data according to packing scheme
    // This is absolute luminosity, so just unpack values into our array
    
    ATH_MSG_DEBUG( "Unpacking lumi value from blob");
    ++pchar;  // Points to next char after header
    for (unsigned int i=0; i<nbcids; i++) {
      // Can't use assignment directly because source may be misaligned.
      memcpy (&m_LBInstLumi[i], pchar, sizeof(float));
      pchar += sizeof(float);
      ATH_MSG_DEBUG( "Bcid: " << i << " Lumi: " << m_LBInstLumi[i] );
    }
    
  } else { // Run1 way, hard!
    ATH_MSG_DEBUG( "starting Run1 recalculatePerBCIDLumi() for alg: " << m_preferredChannel );
    
    if (m_preferredChannel == 0) return;

    // Nothing to do if we don't have the ingredients
    if (m_onlineLumiCalibrationTool.empty()) {
      ATH_MSG_DEBUG( "OnlineLumiCalibrationTool.empty() is TRUE, skipping..." );
      return;
    }
    if (m_bunchLumisTool.empty()) {
      ATH_MSG_DEBUG( "BunchLumisTool.empty() is TRUE, skipping..." );
      return;
    }
    if (m_bunchGroupTool.empty()) {
      ATH_MSG_DEBUG( "BunchGroupTool.empty() is TRUE, skipping..." );
      return;
    }
    if (m_fillParamsTool.empty()) {
      ATH_MSG_DEBUG( "FillParamsTool.empty() is TRUE, skipping..." );
      return;
    }

    // Update data from FillParamsTool
    m_luminousBunches = m_fillParamsTool->nLuminousBunches();
    m_luminousBunchesVec = m_fillParamsTool->luminousBunches();

    ATH_MSG_DEBUG( "N LuminousBunches:" << m_luminousBunches );
    //ATH_MSG_DEBUG( m_luminousBunchesVec[0] << " " <<  m_luminousBunchesVec[1] );

    // Get the raw data for the preferred channel
    m_bunchLumisTool->setChannel(m_preferredChannel);
    std::vector<float> rawLumiVec = m_bunchLumisTool->rawLuminosity();


    //
    // Calibration step
    //
    
    //  Here we want to go through and calibrate raw values in the luminous bunches only.
    // This is what the OL adds up, and since these are online calibrations, we want to rescale the total
    // to agree to whatever offline tag we are using.
    std::vector<float> calLumiVec(TOTAL_LHC_BCIDS, 0.);
    
    // Update muToLumi while we are at it (also check that calibration exists)
    m_MuToLumi = m_onlineLumiCalibrationTool->getMuToLumi(m_preferredChannel);
    if (m_MuToLumi <= 0.) {
      ATH_MSG_WARNING( " dont have calibration information for preferred channel " << m_preferredChannel << "!" );
      return;
    }

    double lumiSum = 0.;
    for (unsigned int i = 0; i<m_luminousBunches; i++) {
      unsigned int bcid = m_luminousBunchesVec[i];
      
      // Don't waste time on zero lumi 
      if (rawLumiVec[bcid] <= 0.) {
	ATH_MSG_DEBUG( "Calibrate BCID " << bcid << " with raw " << rawLumiVec[bcid] << " -> skipping" );
	continue;
      }

      // Calibrate
      if (!m_onlineLumiCalibrationTool->calibrateLumi(m_preferredChannel, rawLumiVec[bcid], calLumiVec[bcid])) {
	ATH_MSG_DEBUG( "Calibrate BCID " << bcid << " with raw " << rawLumiVec[bcid] << " -> calibration failed!" );
	ATH_MSG_WARNING( "Per-BCID calibration failed for bcid " << bcid << " with raw lumi = " << rawLumiVec[bcid] );
	continue;
      }
      
      lumiSum += calLumiVec[bcid];
      
      ATH_MSG_DEBUG( "Calibrate BCID " << bcid << " with raw " << rawLumiVec[bcid] << " -> " << calLumiVec[bcid] );
    }

    // Work out scale factor between offline and online estimate
    float offlineOnlineRatio = 1.;
    if (lumiSum > 0.) offlineOnlineRatio = m_LBAvInstLumi / lumiSum;
    
    ATH_MSG_DEBUG( " Offline/Online scale factor: " << m_LBAvInstLumi << " / " << lumiSum << " = " << offlineOnlineRatio );

    // Make sure we have values for all BCIDs in the physics bunch group
    // std::vector<unsigned int> m_bgBunchesVec = m_bunchGroupTool->bunchGroup1();
    for (unsigned int i = 0; i<m_bunchGroupTool->nBunchGroup1(); i++) {
      unsigned int bcid = m_bunchGroupTool->bunchGroup1()[i];
      
      // Don't repeat if value already exists
      if (calLumiVec[bcid] > 0.) continue;
      if (rawLumiVec[bcid] <= 0.) continue;
      
      // Calibrate
      if (!m_onlineLumiCalibrationTool->calibrateLumi(m_preferredChannel, rawLumiVec[bcid], calLumiVec[bcid])) {
	if (msgLvl(MSG::DEBUG)) msg(MSG::DEBUG) << " -> Calibration failed!" << endmsg;
	ATH_MSG_WARNING( "Per-BCID calibration failed for bcid " << bcid << " with raw lumi = " << rawLumiVec[bcid] );
	continue;
      }
    }

    // Almost done, now we apply the scale factor to all BCIDs
    for (unsigned int i=0; i<calLumiVec.size(); i++) 
      calLumiVec[i] *= offlineOnlineRatio;
    
    // And finally assign this vector to our final data location
    m_LBInstLumi = calLumiVec;
  }

  ATH_MSG_DEBUG( "finished recalculatePerBCIDLumi() for alg: " << m_preferredChannel );
  return;
}

#endif
