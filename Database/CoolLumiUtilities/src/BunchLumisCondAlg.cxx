/*
 * Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration.
 */
/**
 * @file CoolLumiUtilities/src/BunchLumisCondAlg.cxx
 * @author scott snyder <snyder@bnl.gov>, from existing BunchLumisTool.
 * @date May, 2019
 * @brief Conditions algorithm to unpack bunch group data from COOL.
 */


#include "BunchLumisCondAlg.h"
#include "StoreGate/ReadCondHandle.h"
#include "StoreGate/WriteCondHandle.h"
#include "CxxUtils/get_unaligned.h"
#include "CxxUtils/restrict.h"
#include "CxxUtils/vectorize.h"
#include "CoralBase/Blob.h"
#include "CoolKernel/IObject.h"
#include <stdint.h>


ATH_ENABLE_VECTORIZATION;



namespace {


// Total number of BCIDs in one turn - this must match value used in /TDAQ/OLC/BUNCHLUMIS for storage mode 1
const unsigned int TOTAL_LHC_BCIDS = 3564;


} // anonymous namespace


/**
 * @brief Gaudi initialize method.
 */
StatusCode
BunchLumisCondAlg::initialize()
{
  ATH_CHECK( m_bunchLumisFolderInputKey.initialize() );
  ATH_CHECK( m_fillParamsInputKey.initialize(SG::AllowEmpty) );
  ATH_CHECK( m_bunchLumisOutputKey.initialize() );
  return StatusCode::SUCCESS;
}


/**
 * @brief Unpack raw luminosity data for one channel.
 * @param blob Packed data to unpack.
 * @param luminousBunches BCIDs of occupied bunches, or an empty
 *                        vector if not available.
 * @param avgRawLumi Average raw luminosity, from the conditions object.
 * @param rawLumiOut[out] Unpacked per-BCID raw luminosities.
 */
StatusCode
BunchLumisCondAlg::unpackLumis (const coral::Blob& blob,
                                const std::vector<unsigned int>& luminousBunches,
                                float avgRawLumi,
                                std::vector<float>& rawLumiOut) const
{
  // Figure out Mika's blob packing mode (should really have a utility for this)
  // See description: https://twiki.cern.ch/twiki/bin/viewauth/Atlas/CoolOnlineData#Folder_TDAQ_OLC_BUNCHLUMIS

  const uint8_t* ATH_RESTRICT pchar = static_cast<const uint8_t*>(blob.startingAddress()); // First byte holds storage size and mode
  unsigned int bss = ((*pchar) % 100) / 10;  // Byte storage size
  unsigned int smod = ((*pchar) % 10);       // Storage mode 

  ATH_MSG_DEBUG( "BunchRawInstLumi blob found with storage mode " << smod << " and byte storage size " << bss );

  // Check blob length and point pchar to start of raw lumi data 
  unsigned int bloblength = 0;
  unsigned int nbcids = 0;

  ++pchar;
  const uint8_t* ATH_RESTRICT pbcids = nullptr;

  // Treat different storage modes independently  
  switch (smod) {
  case 0:
    // Packed according to luminousBunches        
    // Make sure tool is configured
    if (luminousBunches.empty()) {
      ATH_MSG_ERROR("Can't unpack using luminousBunches!");
      return StatusCode::SUCCESS;
    }

    nbcids = luminousBunches.size();
    bloblength = bss * nbcids + 1;
    break;

  case 1:
    // Full orbit stored
    nbcids = TOTAL_LHC_BCIDS;
    bloblength = bss * nbcids + 1;
    break;

  case 2:
    // Self describing length, with 2-byte length followed by 2-byte BCID vector, then data
    nbcids = CxxUtils::get_unaligned16 (pchar);
    pbcids = pchar;
    bloblength = (2+bss)*nbcids + 3;  // 2-bytes for vector plus bss plus 2 bytes for vector length, plus one byte for packing          
    pchar += 2*nbcids; // Advance pchar past bicd vector list to raw data
    // ATH_MSG_DEBUG( "Found mode 2 with " << nbcids << " BCIDs" );
    break;

  default:
    ATH_MSG_ERROR( "BunchRawInstLumi blob found with unknown storage mode " << smod << "!" );
    return StatusCode::FAILURE;
  }
  
  // Check blob size against needed length.  Give up if these don't match 
  if (static_cast<cool::UInt32>(blob.size()) != bloblength) {
    ATH_MSG_ERROR( "BunchRawInstLumi blob found with length " << blob.size() <<
                   " in storage mode " << smod <<  " with size " << bss <<
                   ", expecting " << bloblength << "!" );
    return StatusCode::FAILURE;
  }

  // Length is correct, read raw data according to packing scheme
  // Some schemes are relative and must be renormalized, while the float/double schemes are absolute values - *pay attention!*
  std::vector<float> rawLumi;
  rawLumi.clear();
  rawLumi.reserve (nbcids);

  // Different depending upon bytes allocated (this is ugly, but it works)
  // pchar is pointing to the start of the data (past first byte of blob)
  switch (bss) {

  case 1: { // 1-byte integers, just use pchar
    float scale = avgRawLumi / 100;
    for (unsigned int i=0; i<nbcids; i++) {
      float val = (*pchar++) * scale;
      rawLumi.push_back(val);
    }
    break;
  }

  case 2: { // 2-byte integers
    float scale = avgRawLumi / (100*100);
    for (unsigned int i=0; i<nbcids; i++) {
      float val = CxxUtils::get_unaligned16(pchar) * scale;
      rawLumi.push_back(val);
    }
    break;
  }

  case 4: // 4-byte floats
    for (unsigned int i=0; i<nbcids; i++) {
      rawLumi.push_back (CxxUtils::get_unaligned_float(pchar));
    }
    break;

  case 8: // 8-byte doubles
    for (unsigned int i=0; i<nbcids; i++) {
      rawLumi.push_back (CxxUtils::get_unaligned_double(pchar));
    }
    break;

  default:
    ATH_MSG_ERROR( "BunchRawInstLumi blob found with unknown byte storage size " << bss << "!" );
    return StatusCode::FAILURE;
  }

  // Now figure which BCIDs these values belong to and fill into vector indexed by BCID

  rawLumiOut.clear();
  // Remember, nbcids was set before and the blob size was checked 
  switch (smod) {
  case 0:
    // Packed according to luminous bunches, fill accordingly
    // Checked before that FillParamsTool is configured, don't need to check again
    rawLumiOut.resize (TOTAL_LHC_BCIDS, 0);
    for (unsigned int i=0; i<nbcids; i++)
      rawLumiOut[luminousBunches[i]] = rawLumi[i];
    break;

  case 1:
    // Packed according to full turn, just copy   
    rawLumiOut = std::move (rawLumi);
    break;

  case 2:
    // Packed according to private list, must read here.  pbcids points to start of this data    
    rawLumiOut.resize (TOTAL_LHC_BCIDS, 0);
    for (unsigned int i=0; i<nbcids; i++) {
      rawLumiOut[CxxUtils::get_unaligned16(pbcids)] = rawLumi[i];
      // ATH_MSG_DEBUG( "BCID: " << *p16 << " Lumi= " << rawLumi[i] );
    }
    break;

    // This error condition was dealt with before   
    //default:
  }

  return StatusCode::SUCCESS;
}



/**
 * @brief Algorithm execute method.
 * @param ctx Event Context.
 */
StatusCode
BunchLumisCondAlg::execute (const EventContext& ctx) const
{
  auto lumis = std::make_unique<BunchLumisCondData>();

  SG::ReadCondHandle<CondAttrListCollection> bunchLumisFolder
    (m_bunchLumisFolderInputKey, ctx);
  EventIDRange range;
  ATH_CHECK( bunchLumisFolder.range (range) );

  std::vector<unsigned int> luminousBunches;
  if (!m_fillParamsInputKey.empty()) {
    SG::ReadCondHandle<FillParamsCondData> fillParams
      (m_fillParamsInputKey, ctx);
    luminousBunches = fillParams->luminousBunches();
    EventIDRange fpRange;
    ATH_CHECK( fillParams.range (fpRange) );
    range = EventIDRange::intersect (range, fpRange);
  }

  for (const auto& p : *bunchLumisFolder.retrieve()) {
    unsigned int channel = p.first;
    const coral::AttributeList& attrList = p.second;

    if (attrList["BunchRawInstLum"].isNull()) {
      ATH_MSG_ERROR( "BunchRawInstLum blob not found for channel " << channel << "!" );
      return StatusCode::FAILURE;
    }

    const coral::Blob& blob = attrList["BunchRawInstLum"].data<coral::Blob>();
    if (blob.size() == 0) {
      ATH_MSG_DEBUG( "BunchRawInstLumi blob found with zero size for channel " << channel << "!" );
      continue;
    }

    // Make sure the scale factor exists (needed below to unpack integer blob schemes)                        
    if (attrList["AverageRawInstLum"].isNull()) {
      ATH_MSG_ERROR( "AverageRawInstLum value not found for channel " << channel << "!" );
      return StatusCode::FAILURE;
    }
    float avgRawLumi = attrList["AverageRawInstLum"].data<cool::Float>();

    std::vector<float> rawLumis;
    ATH_CHECK( unpackLumis (blob, luminousBunches, avgRawLumi, rawLumis) );
    lumis->addChannel (channel, std::move (rawLumis));
  }
  
  SG::WriteCondHandle<BunchLumisCondData> bunchLumisData
    (m_bunchLumisOutputKey, ctx);
  ATH_CHECK( bunchLumisData.record (range, std::move (lumis)) );
  return StatusCode::SUCCESS;
}
