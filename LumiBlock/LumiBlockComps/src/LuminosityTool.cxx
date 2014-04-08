/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "LuminosityTool.h"

#include "EventInfo/EventID.h"
#include "EventInfo/EventInfo.h"
#include "EventInfo/EventType.h"

#include "CoolKernel/IObject.h"
#include "CoralBase/Blob.h"
#include "AthenaPoolUtilities/CondAttrListCollection.h"
#include "AthenaPoolUtilities/AthenaAttributeList.h"

#include "AthenaKernel/errorcheck.h"

// Total number of BCIDs in one turn - this must match value used in /TDAQ/OLC/BUNCHLUMIS for storage mode 1
const unsigned int TOTAL_LHC_BCIDS = 3564;

//--------------------------------------------------

LuminosityTool::LuminosityTool(const std::string& type,
				 const std::string& name,
				 const IInterface* parent)
  : AthAlgTool(type, name, parent),
    m_lumiFolderName("/TRIGGER/OFLLUMI/LBLESTOFL"),
    m_lumiChannel(0),
    m_calibrationFolderName("/TDAQ/OLC/CALIBRATIONS"),
    m_fillparamsFolderName("/TDAQ/OLC/LHC/FILLPARAMS"),
    m_bunchlumisFolderName("/TDAQ/OLC/BUNCHLUMIS"),
    m_bunchgroupFolderName("/TRIGGER/LVL1/BunchGroupContent"),
    m_lblbFolderName("/TRIGGER/LUMI/LBLB"),
    m_recalcPerBCIDLumi(true),
    m_preferredChannel(0),
    m_luminousBunches(0),
    m_bgBunches(0)
{
  declareInterface<ILuminosityTool>(this);
  declareProperty("LumiFolderName", m_lumiFolderName);
  declareProperty("LumiChannelNumber", m_lumiChannel);
  declareProperty("CalibrationFolderName", m_calibrationFolderName);
  declareProperty("FillparamsFolderName", m_fillparamsFolderName);
  declareProperty("BunchlumisFolderName", m_bunchlumisFolderName);
  declareProperty("BunchgroupFolderName", m_bunchgroupFolderName);
  declareProperty("LBLBFolderName", m_lblbFolderName);

  m_LBAvInstLumi = 0.;
  m_LBAvEvtsPerBX = 0.;
  m_Valid = 0xFFFFFFFF;

  m_LBDuration = 0.;

  m_LBInstLumi = std::vector<float>(TOTAL_LHC_BCIDS, 0.);
  m_MuToLumi = 0.;

  m_luminousBunchesVec.clear();
  m_bgBunchesVec.clear();

  m_cali.clear();
}

StatusCode
LuminosityTool::initialize()
{
  ATH_MSG_DEBUG("LuminosityTool::initialize() begin");

  ATH_MSG_INFO("LuminosityTool::initialize() registering " << m_lumiFolderName);

  // In addition to local, private callback functions, also set up callbacks to updateCache any time
  // the local cached data changes.
  // This must be done with the interface (ILuminosityTool) so external code can use this to trigger
  // their own callbacks.

  // Setup callback
  if (detStore()->contains<CondAttrListCollection>(m_lumiFolderName)) {

    const DataHandle<CondAttrListCollection> aptr;

    CHECK(detStore()->regFcn(&LuminosityTool::updateAvgLumi, this , aptr, m_lumiFolderName));
    CHECK(detStore()->regFcn(&LuminosityTool::updateAvgLumi, this, &ILuminosityTool::updateCache, dynamic_cast<ILuminosityTool*>(this)));

    ATH_MSG_INFO( " Registered a callback for " << m_lumiFolderName << " COOL folder " );
  } else if (!m_lumiFolderName.empty()) {
    ATH_MSG_ERROR( " cannot find " << m_lumiFolderName << " in DetectorStore" );
  }

  // Calibrations folder
  if (detStore()->contains<CondAttrListCollection>(m_calibrationFolderName)) {

    const DataHandle<CondAttrListCollection> aptr;

    CHECK(detStore()->regFcn(&LuminosityTool::updateCalibrations, this , aptr, m_calibrationFolderName));
    CHECK(detStore()->regFcn(&LuminosityTool::updateCalibrations, this , &ILuminosityTool::updateCache, dynamic_cast<ILuminosityTool*>(this)));

    ATH_MSG_INFO( " Registered a callback for " << m_calibrationFolderName << " COOL folder " );
  } else {
    ATH_MSG_ERROR( " cannot find " << m_calibrationFolderName << " in DetectorStore" );
  }

  // Fillparams folder
  if (detStore()->contains<AthenaAttributeList>(m_fillparamsFolderName)) {

    const DataHandle<AthenaAttributeList> aptr;

    CHECK(detStore()->regFcn(&LuminosityTool::updateFillparams, this , aptr, m_fillparamsFolderName));
    CHECK(detStore()->regFcn(&LuminosityTool::updateFillparams, this , &ILuminosityTool::updateCache, dynamic_cast<ILuminosityTool*>(this)));

    ATH_MSG_INFO( " Registered a callback for " << m_fillparamsFolderName << " COOL folder " );
  } else if (!m_fillparamsFolderName.empty()) {
    ATH_MSG_ERROR( " cannot find " << m_fillparamsFolderName << " in DetectorStore" );
  }

  // Bunchlumis folder
  if (detStore()->contains<CondAttrListCollection>(m_bunchlumisFolderName)) {

    const DataHandle<CondAttrListCollection> aptr;

    // No explicit function, just trigger cache update 
    CHECK(detStore()->regFcn(&ILuminosityTool::updateCache, dynamic_cast<ILuminosityTool*>(this) , aptr, m_bunchlumisFolderName));

    ATH_MSG_INFO( " Registered a callback for " << m_bunchlumisFolderName << " COOL folder " );
  } else {
    ATH_MSG_ERROR( " cannot find " << m_bunchlumisFolderName << " in DetectorStore" );
  }

  // BunchGroup folder
  if (detStore()->contains<AthenaAttributeList>(m_bunchgroupFolderName)) {

    const DataHandle<AthenaAttributeList> aptr;

    CHECK(detStore()->regFcn(&LuminosityTool::updateBunchgroup, this , aptr, m_bunchgroupFolderName));
    CHECK(detStore()->regFcn(&LuminosityTool::updateBunchgroup, this , &ILuminosityTool::updateCache, dynamic_cast<ILuminosityTool*>(this)));

    ATH_MSG_INFO( " Registered a callback for " << m_bunchgroupFolderName << " COOL folder " );
  } else if (!m_bunchgroupFolderName.empty()) {
    ATH_MSG_ERROR( " cannot find " << m_bunchgroupFolderName << " in DetectorStore" );
  }

  // LBLB folder
  if (detStore()->contains<AthenaAttributeList>(m_lblbFolderName)) {

    const DataHandle<AthenaAttributeList> aptr;

    CHECK(detStore()->regFcn(&LuminosityTool::updateLBLB, this , aptr, m_lblbFolderName));
    CHECK(detStore()->regFcn(&LuminosityTool::updateLBLB, this , &ILuminosityTool::updateCache, dynamic_cast<ILuminosityTool*>(this)));

    ATH_MSG_INFO( " Registered a callback for " << m_lblbFolderName << " COOL folder " );
  } else if (!m_lblbFolderName.empty()) {
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

  const EventInfo* eventInfo;
  StatusCode sc = evtStore()->retrieve(eventInfo);
  if (sc.isFailure()) {
    ATH_MSG_WARNING( "Cannot access event info " );
    return 0.;
  }

  float lumi = lbLuminosityPerBCID(eventInfo->event_ID()->bunch_crossing_id());

  ATH_MSG_DEBUG( "From DB, LB " << eventInfo->event_ID()->lumi_block() << " bcid " << eventInfo->event_ID()->bunch_crossing_id() << " -> " << lumi << " ub-1 s-1" );

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
LuminosityTool::muToLumi() const {
  return m_MuToLumi;
}

//
// Callback functions
//
StatusCode
LuminosityTool::updateCache( IOVSVC_CALLBACK_ARGS_P(/*idx*/, /*keys*/) )
{
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

  // Check validity
  cool::UInt32 m_Valid = attrList["Valid"].data<cool::UInt32>();
  if (m_Valid & 0x03) {
    ATH_MSG_WARNING( " Invalid luminosity ... set lumi to 0" );
    return StatusCode::SUCCESS;
  }

  // Get preferred channel (needed for per-BCID calculation)
  if (m_lumiChannel == 0) {
    m_preferredChannel = (m_Valid >> 22);
  } else {
    m_preferredChannel = m_lumiChannel;
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

  return StatusCode::SUCCESS;
}

StatusCode
LuminosityTool::updateCalibrations( IOVSVC_CALLBACK_ARGS_P(/*idx*/, /*keys*/) )
{
  ATH_MSG_DEBUG( "in updateCalibrations() " );

  // Should be set in updateCache, but do it here just to make sure
  m_recalcPerBCIDLumi = true;  

  // Clear old data
  m_cali.clear();

  const CondAttrListCollection* attrListColl = 0;
  CHECK(detStore()->retrieve(attrListColl, m_calibrationFolderName));

  // Loop over collection and save all channels
  CondAttrListCollection::const_iterator first = attrListColl->begin();
  CondAttrListCollection::const_iterator last  = attrListColl->end();
  for (; first != last; ++first) {
    cool::UInt32 channel = (*first).first;
    const coral::AttributeList& attrList = (*first).second;
    LumiCalibrator lc;
    if (!lc.setCalibration(attrList)) {
      ATH_MSG_WARNING( "error processing calibration for channel " << channel );
    } else{
      ATH_MSG_DEBUG( "Calibration for channel " << channel << ": " << lc );
      m_cali[channel] = lc;
    }
  }

  // And set muToLumi
  m_MuToLumi = m_cali[0].getMuToLumi();

  return StatusCode::SUCCESS;
}

StatusCode
LuminosityTool::updateFillparams( IOVSVC_CALLBACK_ARGS_P(/*idx*/, /*keys*/) )
{
  ATH_MSG_DEBUG( "in updateFillparams() " );

  // Should be set in updateCache, but do it here just to make sure
  m_recalcPerBCIDLumi = true;  

  // Clear old data
  m_luminousBunches = 0;
  m_luminousBunchesVec.clear(); 

  const AthenaAttributeList* attrList = 0;
  CHECK(detStore()->retrieve(attrList, m_fillparamsFolderName));

  if ((*attrList)["BCIDmasks"].isNull()) {
    ATH_MSG_WARNING( "BCIDmasks is NULL in " << m_fillparamsFolderName << "!" );
    return StatusCode::SUCCESS;
  }

  cool::UInt32 nb1 = (*attrList)["Beam1Bunches"].data<cool::UInt32>();
  cool::UInt32 nb2 = (*attrList)["Beam2Bunches"].data<cool::UInt32>();
  cool::UInt32 ncol = (*attrList)["LuminousBunches"].data<cool::UInt32>();

  ATH_MSG_DEBUG( "LuminousBunches: " << ncol );

  const coral::Blob& blob = (*attrList)["BCIDmasks"].data<coral::Blob>();

  // Verify length
  if ( static_cast<cool::UInt32>( blob.size() ) != 2 * (nb1 + nb2 + ncol)) {
    ATH_MSG_WARNING( "BCIDmasks length " << blob.size() << " != 2 * " << (nb1+nb2+ncol) );
    return StatusCode::SUCCESS;
  }

  const uint16_t* p=static_cast<const uint16_t*>(blob.startingAddress());
  p += (nb1+nb2); // Skip past beam1 and beam2 
  if (msgLvl(MSG::DEBUG)) msg(MSG::DEBUG) << "LuminousBunch list: ";
  for (unsigned int i = 0; i < ncol; i++, p++) {
    m_luminousBunchesVec.push_back(*p);
    if (msgLvl(MSG::DEBUG)) msg(MSG::DEBUG) << *p << " "; 
  }
  if (msgLvl(MSG::DEBUG)) msg(MSG::DEBUG) << endreq;

  m_luminousBunches = m_luminousBunchesVec.size();

  return StatusCode::SUCCESS;
}

StatusCode
LuminosityTool::updateBunchgroup( IOVSVC_CALLBACK_ARGS_P(/*idx*/, /*keys*/) )
{
  ATH_MSG_DEBUG( "in updateBunchgroup() " );

  // Should be set in updateCache, but do it here just to make sure
  m_recalcPerBCIDLumi = true;  

  // Clear old data
  m_bgBunches = 0;
  m_bgBunchesVec.clear(); 

  const AthenaAttributeList* attrList = 0;
  CHECK(detStore()->retrieve(attrList, m_bunchgroupFolderName));

  if ((*attrList)["BunchCode"].isNull()) {
    ATH_MSG_WARNING( "BunchCode is NULL in " << m_bunchgroupFolderName << "!" );
    return StatusCode::SUCCESS;
  }

  const coral::Blob& blob = (*attrList)["BunchCode"].data<coral::Blob>();

  ATH_MSG_DEBUG( "Bunchgroup blob length: " << blob.size() );

  // Check that we have enough data.
  // There have been many bugs in filling this folder, so just require at least TOTAL_LHC_BCIDS
  if (static_cast<cool::UInt32>(blob.size()) < TOTAL_LHC_BCIDS) {
    ATH_MSG_WARNING( "BunchCode length: " << blob.size() << " shorter than " << TOTAL_LHC_BCIDS << "!" );
    return StatusCode::SUCCESS;
  }

  if (msgLvl(MSG::DEBUG)) msg(MSG::DEBUG) << "BunchGroup list: ";
  const unsigned char* p = static_cast<const unsigned char*>(blob.startingAddress());
  for (size_t bcid = 0; bcid < static_cast<size_t>(TOTAL_LHC_BCIDS); ++bcid,++p) {
  
    // Read 8-bits at a time and decode 
    unsigned char mask = (*p);
    if (mask & 0x2) {    // Just look for physics bunch group
      m_bgBunchesVec.push_back(bcid);
      if (msgLvl(MSG::DEBUG)) msg(MSG::DEBUG) << bcid << " ";
    }
  }
  if (msgLvl(MSG::DEBUG)) msg(MSG::DEBUG) << endreq;

  m_bgBunches = m_bgBunchesVec.size();

  ATH_MSG_DEBUG( "Physics Bunch Group entries: " << m_bgBunches );
  return StatusCode::SUCCESS;
}

StatusCode
LuminosityTool::updateLBLB( IOVSVC_CALLBACK_ARGS_P(/*idx*/, /*keys*/) )
{
  ATH_MSG_DEBUG( "in updateLBLB() " );

  // Ensure zero on error
  m_LBDuration = 0.;

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
  m_LBInstLumi = std::vector<float>(TOTAL_LHC_BCIDS, 0.);

  // Make some sanity checks that we have everyting we need
  if (m_preferredChannel == 0) return;
  if (m_LBAvInstLumi == 0.) return;

  // Check if we have data
  if (m_cali.count(m_preferredChannel) == 0) {
    ATH_MSG_WARNING( " dont have calibration information for preferred channel " << m_preferredChannel << "!" );
    return;
  }

  // Get the raw data for the preferred channel
  const CondAttrListCollection* attrListColl = 0;
  StatusCode sc = detStore()->retrieve(attrListColl, m_bunchlumisFolderName);
  if (sc.isFailure() || !attrListColl) {
    ATH_MSG_WARNING( "attrListColl not found for " << m_bunchlumisFolderName );
    return;
  }

  // Loop over collection and find preferred channel
  CondAttrListCollection::const_iterator first = attrListColl->begin();
  CondAttrListCollection::const_iterator last  = attrListColl->end();
  for (; first != last; ++first) {
    cool::UInt32 channel = (*first).first;
    if (channel != m_preferredChannel) continue;
    break;
  }

  // Check if we didn't find the preferred channel
  if (first == last) {
    ATH_MSG_WARNING( "BUNCHLUMI data not found for channel " << m_preferredChannel << "!" <<endreq;
    return;
  }

  // attrList is the payload of /TDAQ/OLC/BUNCHLUMIS
  const coral::AttributeList& attrList = (*first).second;

  // Make sure the blob exists
  if (attrList["BunchRawInstLum"].isNull()) {
    ATH_MSG_WARNING( "BunchRawInstLumi blob not found for channel " << m_preferredChannel << "!" );
    return;
  }

  const coral::Blob& blob = attrList["BunchRawInstLum"].data<coral::Blob>();
  if (blob.size() == 0) {
    ATH_MSG_WARNING( "BunchRawInstLumi blob found with zero size for channel " << m_preferredChannel << "!" );
    return;
  }

  // Make sure the scale factor exists (needed below to unpack integer blob schemes)
  if (attrList["AverageRawInstLum"].isNull()) {
    ATH_MSG_WARNING( "AverageRawInstLum value not found for channel " << m_preferredChannel << "!" );
    return;
  }
  float scaleFactor = attrList["AverageRawInstLum"].data<cool::Float>(); 

  // Don't need to check validity as average value would already be invalid?  Or data wouldn't exist?  we will see

  //
  // Unpack raw luminosity data blob here
  //

  // Figure out Mika's blob packing mode (should really have a utility for this)
  // See description: https://twiki.cern.ch/twiki/bin/viewauth/Atlas/CoolOnlineData#Folder_TDAQ_OLC_BUNCHLUMIS
 
  const char* pchar = static_cast<const char*>(blob.startingAddress()); // First byte holds storage size and mode
  unsigned int bss = ((*pchar) % 100) / 10;  // Byte storage size
  unsigned int smod = ((*pchar) % 10);       // Storage mode

  ATH_MSG_DEBUG( "BunchRawInstLumi blob found with storage mode " << smod << " and byte storage size " << bss );

  // Check blob length and point pchar to start of raw lumi data
  unsigned int bloblength = 0;
  unsigned int nbcids = 0;

  // Used below for smod==2 scheme, also advance pchar past first byte
  const uint16_t* p16 = (const uint16_t*)(++pchar);  

  // Treat different storage modes independently
  switch (smod) {
  case 0:
    // Packed according to luminousBunches
    nbcids = m_luminousBunches;
    bloblength = bss * nbcids + 1;
    break;

  case 1:
    // Full orbit stored
    nbcids = TOTAL_LHC_BCIDS;
    bloblength = bss * nbcids + 1;
    break;

  case 2:
    // Self describing length, with 2-byte length followed by 2-byte BCID vector, then data
    nbcids = *p16++;
    bloblength = (2+bss)*nbcids + 3;  // 2-bytes for vector plus bss plus 2 bytes for vector length, plus one byte for packing
    pchar += 2*(nbcids+1); // Advance pchar past bicd vector list to raw data, p16 used below to get BCID vector list
    // ATH_MSG_DEBUG( "Found mode 2 with " << nbcids << " BCIDs" );
    break;

  default:
    ATH_MSG_WARNING( "BunchRawInstLumi blob found with unknown storage mode " << smod << "!" );
    return;
  }

  // Check blob size against needed length.  Give up if these don't match
  if (static_cast<cool::UInt32>(blob.size()) != bloblength) {
    ATH_MSG_WARNING( "BunchRawInstLumi blob found with length" << blob.size() << "in storage mode" << smod <<  ", expecting " << bloblength << "!" );
    return;
  }

  // Length is correct, read raw data according to packing scheme
  // Some schemes are relative and must be renormalized, while the float/double schemes are absolute values - *pay attention!*
  std::vector<float> rawLumi;
  rawLumi.clear();

  // Must define pointers outside of switch
  const uint16_t* p2 = (const uint16_t*) pchar;
  const float* p4 = (const float*) pchar;
  const double* p8 = (const double*) pchar;

  // Different depending upon bytes allocated (this is ugly, but it works)
  // pchar is pointing to the start of the data (past first byte of blob)
  switch (bss) {

  case 1: // 1-byte integers, just use pchar
    for (unsigned int i=0; i<nbcids; i++, pchar++) {
      float val = (*pchar) * scaleFactor / pow(100, bss);
      rawLumi.push_back(val);
    }
    break;

  case 2: // 2-byte integers
    for (unsigned int i=0; i<nbcids; i++, p2++) {
      float val = (*p2) * scaleFactor / pow(100, bss);
      rawLumi.push_back(val);
    }
    break;

  case 4: // 4-byte floats
    for (unsigned int i=0; i<nbcids; i++, p4++) rawLumi.push_back(*p4);
    break;

  case 8: // 8-byte doubles
    for (unsigned int i=0; i<nbcids; i++, p8++) rawLumi.push_back(*p8);
    break;

  default:
    ATH_MSG_WARNING( "BunchRawInstLumi blob found with unknown byte storage size " << bss << "!" );
    return;
  }

  // Now figure which BCIDs these values belong to and fill into temporary vector indexed by BCID
  // (not all BCIDs will end up in m_LBInstLumi)
  std::vector<float> rawLumiVec(TOTAL_LHC_BCIDS, 0.);

  // Remember, nbcids was set before and the blob size was checked
  switch (smod) {
  case 0:
    // Packed according to luminous bunches, fill accordingly
    for (unsigned int i=0; i<nbcids; i++) 
      rawLumiVec[m_luminousBunchesVec[i]] = rawLumi[i]; 
    break;

  case 1:
    // Packed according to full turn, just copy
    rawLumiVec = rawLumi;
    break;

  case 2:
    // Packed according to private list, must read here.  p16 points to start of this data
    for (unsigned int i=0; i<nbcids; i++, p16++) {
      rawLumiVec[*p16] = rawLumi[i];
      // ATH_MSG_DEBUG( "BCID: " << *p16 << " Lumi= " << rawLumi[i] );
    }
    break;

  default:
    // This error condition was delt with before
    return;
  }


  //
  // Calibration step
  //

  //  Here we want to go through and calibrate raw values in the luminous bunches only.
  // This is what the OL adds up, and since these are online calibrations, we want to rescale the total
  // to agree to whatever offline tag we are using.
  std::vector<float> calLumiVec(TOTAL_LHC_BCIDS, 0.);

  // Calibrator to use
  LumiCalibrator& lc = m_cali[m_preferredChannel];

  double lumiSum = 0.;
  for (unsigned int i = 0; i<m_luminousBunches; i++) {
    unsigned int bcid = m_luminousBunchesVec[i];

    if (msgLvl(MSG::DEBUG)) 
      msg(MSG::DEBUG) << "Calibrate BCID " << bcid << " with raw " << rawLumiVec[bcid];

    // Don't waste time on zero lumi 
    if (rawLumiVec[bcid] <= 0.) {
      if (msgLvl(MSG::DEBUG)) msg(MSG::DEBUG) << endreq; 
      continue;
    }

    // Calibrate
    if (!lc.calibrateLumi(rawLumiVec[bcid], calLumiVec[bcid])) {
      if (msgLvl(MSG::DEBUG)) msg(MSG::DEBUG) << " -> Calibration failed!" << endreq;
      ATH_MSG_WARNING( "Per-BCID calibration failed for bcid " << bcid << " with raw lumi = " << rawLumiVec[bcid] );
      continue;
    }

    lumiSum += calLumiVec[bcid];
    if (msgLvl(MSG::DEBUG)) msg(MSG::DEBUG) << " -> " << calLumiVec[bcid] );
  }

  // Work out scale factor between offline and online estimate
  float offlineOnlineRatio = 1.;
  if (lumiSum > 0.) offlineOnlineRatio = m_LBAvInstLumi / lumiSum;

  ATH_MSG_DEBUG( " Offline/Online scale factor: " << m_LBAvInstLumi << " / " << lumiSum << " = " << offlineOnlineRatio );

  // Also calibrate any luminosities in the physics bunch group (not already handled above)
  for (unsigned int i = 0; i<m_bgBunches; i++) {
    unsigned int bcid = m_bgBunchesVec[i];

    // Don't repeat if value already exists
    if (calLumiVec[bcid] > 0.) continue;
    if (rawLumiVec[bcid] <= 0.) continue;

    // Calibrate
    lc.calibrateLumi(rawLumiVec[bcid], calLumiVec[bcid]);
  }

  // Almost done, now we apply the scale factor to all BCIDs
  for (unsigned int i=0; i<calLumiVec.size(); i++) 
    calLumiVec[i] *= offlineOnlineRatio;

  // And finally assign this vector to our final data location
  m_LBInstLumi = calLumiVec;

  return;
}

