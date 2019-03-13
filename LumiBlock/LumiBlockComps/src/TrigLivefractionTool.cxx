/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "LumiBlockComps/TrigLivefractionTool.h"

#include "AthenaPoolUtilities/AthenaAttributeList.h"

#include "AthenaKernel/errorcheck.h"

// Total number of BCIDs in one turn
const unsigned int TOTAL_LHC_BCIDS = 3564;

//--------------------------------------------------

TrigLivefractionTool::TrigLivefractionTool(const std::string& type,
				 const std::string& name,
				 const IInterface* parent)
  : AthAlgTool(type, name, parent),
    m_recalcLumiLivefraction(true),
    m_turnCounter(0),
    m_lumiLiveFractionLo(1.),
    m_lumiLiveFractionHi(1.)
{
  declareInterface<ITrigLivefractionTool>(this);

  // Initialize to 1 so we don't have divide by zero if there is no data
  m_livefractionHigh = std::vector<float>(TOTAL_LHC_BCIDS, 1.);
  m_livefractionLow  = std::vector<float>(TOTAL_LHC_BCIDS, 1.);
}

StatusCode
TrigLivefractionTool::initialize()
{
  ATH_MSG_DEBUG("TrigLivefractionTool::initialize() begin");
  ATH_CHECK(m_eventInfoKey.initialize());

  if (m_deadtimeFolderName.empty()) {
    // May not be configured, could be OK
    ATH_MSG_INFO("DeadtimeFolderName.empty is TRUE, skipping...");
  } else {
    ATH_MSG_INFO("TrigLivefractionTool::initialize() registering " << m_deadtimeFolderName);

    // In addition to local, private callback functions, also set up callbacks to updateCache any time
    // the local cached data changes.
    // This must be done with the interface (ILuminosityTool) so external code can use this to trigger
    // their own callbacks.

    // Setup callback
    if (detStore()->contains<AthenaAttributeList>(m_deadtimeFolderName)) {

      const DataHandle<AthenaAttributeList> aptr;

      // Causes updateLivefraction to be called when m_deadtimeFolderName changes
      CHECK(detStore()->regFcn(&TrigLivefractionTool::updateLivefraction, this, aptr, m_deadtimeFolderName));
      // Causes updateCache to be called when updateLivefraction is called 
      CHECK(detStore()->regFcn(&TrigLivefractionTool::updateLivefraction, this, &ITrigLivefractionTool::updateCache, dynamic_cast<ITrigLivefractionTool*>(this)));

      //    CHECK(detStore()->regFcn(&ITrigLivefractionTool::updateCache, dynamic_cast<ITrigLivefractionTool*>(this) , aptr, m_deadtimeFolderName));

      ATH_MSG_INFO( " Registered a callback for " << m_deadtimeFolderName << " COOL folder " );
    } else {
      ATH_MSG_ERROR( " cannot find " << m_deadtimeFolderName << " in DetectorStore" );
    }
  }

  // Get the luminosity tool
  if (m_lumiTool.empty()) {
    // May not be configured, could be OK
    ATH_MSG_INFO( "LuminosityTool.empty() is TRUE, skipping...");
  } else {
    ATH_MSG_INFO( "Retrieving luminosity tool handle" );
    CHECK(m_lumiTool.retrieve());

    // Also set up a callback on luminosityTool change
    ATH_MSG_INFO( "Registering callback on ILuminosityTool::updateCache" );
    CHECK(detStore()->regFcn(&ILuminosityTool::updateCache, dynamic_cast<ILuminosityTool*>(&(*m_lumiTool)), &ITrigLivefractionTool::updateCache, dynamic_cast<ITrigLivefractionTool*>(this)));
  }

  ATH_MSG_DEBUG( "TrigLivefractionTool::initialize() end" );
  return StatusCode::SUCCESS;
}

StatusCode
TrigLivefractionTool::finalize()
{
  ATH_MSG_DEBUG( "TrigLivefractionTool::finalize()" );
  return StatusCode::SUCCESS;
}

unsigned int
TrigLivefractionTool::lhcTurnCounter() const {
  return m_turnCounter;
}


const std::vector<float>&
TrigLivefractionTool::l1LivefractionVector(bool highPriority) const {
  if (highPriority) return m_livefractionHigh;
  return m_livefractionLow;
}

float
TrigLivefractionTool::livefractionPerBCID(bool highPriority) const {

  SG::ReadHandle<xAOD::EventInfo> eventInfo(m_eventInfoKey);  

  // check is only useful for serial running; remove when MT scheduler used
  if(!eventInfo.isValid()) {
    ATH_MSG_FATAL("Failed to retrieve "<< m_eventInfoKey.key());
    return 0;
  }

  float frac = livefractionPerBCID(eventInfo->bcid(), highPriority);

  ATH_MSG_DEBUG( "LB " << eventInfo->lumiBlock() << " bcid " << eventInfo->bcid() << " -> Livefrac = " << frac << " for highPriority = " << highPriority);

  return frac;
}

float
TrigLivefractionTool::livefractionPerBCID(unsigned int bcid, bool highPriority) const {
  if (bcid >= TOTAL_LHC_BCIDS) {
    ATH_MSG_WARNING( "Request for livefraction with bcid " << bcid << " > " << TOTAL_LHC_BCIDS << "!" );
    return 0.;
  }

  if (highPriority) return m_livefractionHigh[bcid];

  return m_livefractionLow[bcid];
}

float 
TrigLivefractionTool::lbAverageLivefraction(bool highPriority) {

  // Recalculate the average livefraction if some input has changed
  if (m_recalcLumiLivefraction) recalculateLumiLivefraction();

  if (highPriority) {
    return m_lumiLiveFractionHi;
  }

  return m_lumiLiveFractionLo;
}

//
// Callback functions
//
StatusCode
TrigLivefractionTool::updateCache( IOVSVC_CALLBACK_ARGS_P(/*idx*/, /*keys*/) )
{
  m_recalcLumiLivefraction = true;  
  return StatusCode::SUCCESS;
}

// Called when livefraction folder updates
StatusCode
TrigLivefractionTool::updateLivefraction( IOVSVC_CALLBACK_ARGS_P(/*idx*/, /*keys*/) )
{
  ATH_MSG_DEBUG( "in updateLivefraction() " );

  // Should be set in updateCache, but do it here just to make sure
  m_recalcLumiLivefraction = true;  

  // Ensure value won't crash monitoring on error
  m_turnCounter = 0;
  m_livefractionHigh = std::vector<float>(TOTAL_LHC_BCIDS, 1.);
  m_livefractionLow  = std::vector<float>(TOTAL_LHC_BCIDS, 1.);

  if (m_deadtimeFolderName.empty()) {
    ATH_MSG_WARNING( "updateLiveFraction called with DeadtimeFolderName.empty() = True!" );
    return StatusCode::SUCCESS;
  }

  const AthenaAttributeList* attrList = 0;
  CHECK(detStore()->retrieve(attrList, m_deadtimeFolderName));

  if ((*attrList)["TurnCounter"].isNull()) {
    ATH_MSG_WARNING( "TurnCounter is NULL in " << m_deadtimeFolderName << "!" );
    return StatusCode::SUCCESS;
  }

  m_turnCounter = (*attrList)["TurnCounter"].data<uint32_t>();

  // Nothing to do if turn counter is zero
  if (m_turnCounter == 0) {
    ATH_MSG_INFO( "TurnCounter = " << m_turnCounter << " ... setting livefraction to 1");
    return StatusCode::SUCCESS;
  }

  ATH_MSG_DEBUG( "TurnCounter = " << m_turnCounter );

  // OK, unpack deadtime counters

  // Check data availability
  if ((*attrList)["LowPriority"].isNull() || (*attrList)["HighPriority"].isNull()) {
    ATH_MSG_WARNING( " NULL veto counter information in database ... set livefraction to 1 " );
    return StatusCode::SUCCESS;
  }

  const coral::Blob& lowBlob = (*attrList)["LowPriority"].data<coral::Blob>();
  const coral::Blob& highBlob = (*attrList)["HighPriority"].data<coral::Blob>();

  fillVector(m_livefractionLow, lowBlob);
  fillVector(m_livefractionHigh, highBlob);

  if (msgLvl(MSG::DEBUG)) {
    msg(MSG::DEBUG) << "HighPriority livefraction: ";
    for (unsigned int i = 0; i < m_livefractionHigh.size(); i++) {
      msg(MSG::DEBUG) << m_livefractionHigh[i] << " "; 
    }
    msg(MSG::DEBUG) << endmsg;
  }

  return StatusCode::SUCCESS;
}

void
TrigLivefractionTool::fillVector(std::vector<float>& livevec, const coral::Blob& blob) {

  // Ensure turn counter is non-zero
  if (m_turnCounter == 0) return;

  // Verify length

  // Due to a bug, this was sometimes written with 3654 entries rather than desired 3564
  if ( static_cast<uint32_t>( blob.size() ) < 3*TOTAL_LHC_BCIDS) {
    ATH_MSG_WARNING( "Deadtime BLOB with " << blob.size() << "  < 3 * " << TOTAL_LHC_BCIDS);
    return;
  }


  // Unpack one byte at a time
  const uint8_t* p=static_cast<const uint8_t*>(blob.startingAddress());
  for (unsigned int i=0; i < TOTAL_LHC_BCIDS; i++, p+=3) {

    unsigned int busyCounter = *p | (*(p+1) << 8) | (*(p+2) << 16);
    livevec[i] = (m_turnCounter-busyCounter)/(1.*m_turnCounter);

  }
  return;
}

// Routine to recalculate the lumi-weighted live fraction if either the luminosity or live fraction data changes
void
TrigLivefractionTool::recalculateLumiLivefraction()
{
  ATH_MSG_DEBUG( "in recalculateLumiLivefraction" );

  // Clear the request flag
  m_recalcLumiLivefraction = false;

  // One more thing, lets calculate the lumi-weighted live fraction
  m_lumiLiveFractionLo = 1.;
  m_lumiLiveFractionHi = 1.;

  if (m_lumiTool.empty()) {
    // May not be configured, could be OK
    ATH_MSG_WARNING( "recalculateLumiLivefraction called with LuminosityTool.empty() == TRUE!");
    return;
  }

  double numsumlo = 0.;
  double numsumhi = 0.;
  double densum = 0.;

  // Just use physics bunch group
  for (unsigned int bcid = 1; bcid < TOTAL_LHC_BCIDS; bcid++) {
    numsumlo += m_lumiTool->lbLuminosityPerBCID(bcid) * this->livefractionPerBCID(bcid, false);
    numsumhi += m_lumiTool->lbLuminosityPerBCID(bcid) * this->livefractionPerBCID(bcid, true);
    densum   += m_lumiTool->lbLuminosityPerBCID(bcid);
  }

  if (densum > 0.) {
    m_lumiLiveFractionLo = numsumlo/densum;
    m_lumiLiveFractionHi = numsumhi/densum;
  }

  return;
}
