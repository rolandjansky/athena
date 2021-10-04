/*
 * Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration.
 */
/**
 * @file LumiBlockComps/src/TrigLiveFractionCondAlg.cxx
 * @author scott snyder <snyder@bnl.gov>, from existing TrigLivefractionTool
 * @date Jun, 2019
 * @brief Conditions algorithm for trigger live fraction data.
 */


#include "TrigLiveFractionCondAlg.h"
#include "StoreGate/WriteCondHandle.h"
#include "CoolKernel/IObject.h"


/**
 * @brief Gaudi initialize method.
 */
StatusCode
TrigLiveFractionCondAlg::initialize()
{
  ATH_CHECK( m_deadtimeFolderInputKey.initialize(SG::AllowEmpty) );
  ATH_CHECK( m_luminosityInputKey.initialize(SG::AllowEmpty) );
  ATH_CHECK( m_trigLiveFractionOutputKey.initialize() );
  return StatusCode::SUCCESS;
}


/**
 * @brief Algorithm execute method.
 * @param ctx Event Context.
 */
StatusCode
TrigLiveFractionCondAlg::execute (const EventContext& ctx) const
{
  const EventIDBase::number_type UNDEFNUM = EventIDBase::UNDEFNUM;
  const EventIDBase::event_number_t UNDEFEVT = EventIDBase::UNDEFEVT;
  EventIDRange range (EventIDBase (0, UNDEFEVT, UNDEFNUM, 0, 0),
                      EventIDBase (UNDEFNUM-1, UNDEFEVT, UNDEFNUM, 0, 0));

  unsigned int turnCounter = 0;
  std::vector<float> liveFractionLo (LuminosityCondData::TOTAL_LHC_BCIDS, 1);
  std::vector<float> liveFractionHi (LuminosityCondData::TOTAL_LHC_BCIDS, 1);
  float lumiLiveFractionLo = 1;
  float lumiLiveFractionHi = 1;

  if (!m_deadtimeFolderInputKey.empty()) {
    SG::ReadCondHandle<AthenaAttributeList> deadtimeFolder
      (m_deadtimeFolderInputKey, ctx);
    ATH_CHECK( deadtimeFolder.range (range) );

    ATH_CHECK( unpackDeadtime (**deadtimeFolder,
                               turnCounter,
                               liveFractionLo,
                               liveFractionHi) );

    SG::ReadCondHandle<LuminosityCondData> luminosityInput
      (m_luminosityInputKey, ctx);
    EventIDRange range2;
    ATH_CHECK( luminosityInput.range (range2) );
    range = EventIDRange::intersect (range, range2);

    ATH_CHECK( lumiWeight (**luminosityInput,
                           liveFractionLo,
                           liveFractionHi,
                           lumiLiveFractionLo,
                           lumiLiveFractionHi) );
  }

  auto livefrac = std::make_unique<TrigLiveFractionCondData>
    (turnCounter,
     std::move (liveFractionLo),
     std::move (liveFractionHi),
     lumiLiveFractionLo,
     lumiLiveFractionHi);
  SG::WriteCondHandle<TrigLiveFractionCondData> trigLiveFractionCondData
    (m_trigLiveFractionOutputKey, ctx);
  ATH_CHECK( trigLiveFractionCondData.record (range, std::move (livefrac)) );
  return StatusCode::SUCCESS;
}


/**
 * @brief Unpack information from the deadtime COOL folder.
 * @param deadtimeFolder Input COOL folder.
 * @param turnCounter[out] Number of turns for this IOV.
 * @param liveFractionLo[out] Per-BCID live fractions, low-priority
 * @param liveFractionHi[out] Per-BCID live fractions, high-priority
 *
 * The vectors should already be sized to TOTAL_LHC_BCIDS.
 */
StatusCode
TrigLiveFractionCondAlg::unpackDeadtime (const AthenaAttributeList& deadtimeFolder,
                                         unsigned int& turnCounter,
                                         std::vector<float>& liveFractionLo,
                                         std::vector<float>& liveFractionHi) const
{
  if (deadtimeFolder["TurnCounter"].isNull()) {
    ATH_MSG_WARNING( "TurnCounter is NULL in " << m_deadtimeFolderInputKey.key() << "!" );
    return StatusCode::SUCCESS;
  }

  turnCounter = deadtimeFolder["TurnCounter"].data<uint32_t>();
  ATH_MSG_DEBUG( "TurnCounter = " << turnCounter );

  // OK, unpack deadtime counters

  // Check data availability
  if (deadtimeFolder["LowPriority"].isNull() || deadtimeFolder["HighPriority"].isNull()) {
    ATH_MSG_WARNING( " NULL veto counter information in database ... set livefraction to 1 " );
    return StatusCode::SUCCESS;
  }

  ATH_CHECK( unpackVector (deadtimeFolder["LowPriority"].data<coral::Blob>(),
                           turnCounter,
                           liveFractionLo) );
  ATH_CHECK( unpackVector (deadtimeFolder["HighPriority"].data<coral::Blob>(),
                           turnCounter,
                           liveFractionHi) );

  if (msgLvl(MSG::DEBUG)) {
    msg(MSG::DEBUG) << "HighPriority livefraction: ";
    for (unsigned int i = 0; i < liveFractionHi.size(); i++) {
      msg(MSG::DEBUG) << liveFractionHi[i] << " "; 
    }
    msg(MSG::DEBUG) << endmsg;
  }

  return StatusCode::SUCCESS;
}


/**
 * @brief Unpack a single deadtime vector.
 * @param blob Input data blob
 * @param turnCounter Number of turns in this IOV.
 * @param livevec[out] Per-BCID live fractions, low-priority
 *        Should already be sized to TOTAL_LHC_BCIDS.
 */
StatusCode
TrigLiveFractionCondAlg::unpackVector  (const coral::Blob& blob,
                                        unsigned int turnCounter,
                                        std::vector<float>& livevec) const
{
  if (turnCounter == 0) {
    return StatusCode::SUCCESS;
  }

  // Verify length.  Due to a bug, this was sometimes written with 3654 entries
  // rather than the desired 3564.
  if ( static_cast<uint32_t>( blob.size() ) < 3*LuminosityCondData::TOTAL_LHC_BCIDS) {
    ATH_MSG_WARNING( "Deadtime BLOB with " << blob.size() << "  < 3 * "
                     << LuminosityCondData::TOTAL_LHC_BCIDS);
    return StatusCode::SUCCESS;
  }

  assert (livevec.size() == LuminosityCondData::TOTAL_LHC_BCIDS);

  // Unpack one byte at a time
  const uint8_t* p = static_cast<const uint8_t*>(blob.startingAddress());
  for (unsigned int i=0; i < LuminosityCondData::TOTAL_LHC_BCIDS; i++, p+=3)
  {
    unsigned int busyCounter = *p | (*(p+1) << 8) | (*(p+2) << 16);
    livevec[i] = (turnCounter - busyCounter) / static_cast<float>(turnCounter);
  }

  return StatusCode::SUCCESS;
}


/**
 * @brief Calculate weighted average live fractions.
 * @param lumi Luminosity info.
 * @param liveFractionLo Per-BCID live fractions, low-priority
 * @param liveFractionHi Per-BCID live fractions, high-priority
 * @param lumiLiveFractionLo[out] Weighted average live fractions, low-prio.
 * @param lumiLiveFractionHi[out] Weighted average live fractions, high-prio.
 */
StatusCode
TrigLiveFractionCondAlg::lumiWeight  (const LuminosityCondData& lumi,
                                      const std::vector<float>& liveFractionLo,
                                      const std::vector<float>& liveFractionHi,
                                      float& lumiLiveFractionLo,
                                      float& lumiLiveFractionHi) 
{
  double numsumlo = 0.;
  double numsumhi = 0.;
  double densum = 0.;

  // Just use physics bunch group
  // ??? bcid 0 ignored?  Was like that in TrigLivefractionTool.
  for (unsigned int bcid = 1; bcid < LuminosityCondData::TOTAL_LHC_BCIDS; bcid++)
  {
    float bcidlum = lumi.lbLuminosityPerBCIDVector().at(bcid);
    numsumlo += bcidlum * liveFractionLo.at(bcid);
    numsumhi += bcidlum * liveFractionHi.at(bcid);
    densum   += bcidlum;
  }

  if (densum > 0.) {
    lumiLiveFractionLo = numsumlo/densum;
    lumiLiveFractionHi = numsumhi/densum;
  }
  else {
    lumiLiveFractionLo = 1;
    lumiLiveFractionHi = 1;
  }

  return StatusCode::SUCCESS;
}
