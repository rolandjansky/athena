/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

// $Id: ElectronTAccept.cxx 704615 2015-10-29 18:50:12Z wsfreund $
#ifndef RINGER_STANDALONE

#include "RingerSelectorTools/ElectronTAccept.h"

#include "RingerSelectorTools/tools/DeclareBit.h"

namespace Ringer {

// link this namespace to the static counter
COUNTER_LINK_NAMESPACE

// This is the counter which will check if we ran out of bits
DEFINE_COUNTER( ElectronNBits )

// =============================================================================
/// Define the ElectronTAcceptHolder_v1 holden TAccept:
asg::AcceptInfo ElectronTAccept_v1::m_accept("ElectronDecisionMask");

// =============================================================================
// Define the bit counter:
unsigned BitdefElectron_v1::m_nUsedBits = 0;

// =============================================================================
/// Bit set to true when no error occurs:
DECLARE_BIT(BitdefElectron_v1, NoErrorBit,
    "Bit destinated to flag error occurrance.",
    ElectronNBits )

// =============================================================================
// Bit to flag whether track CutID was run:
DECLARE_BIT(BitdefElectron_v1, ExecCutID,
    "Flag whether executed Ringer holden CutID algorithm.",
    ElectronNBits )

// =============================================================================
/// The decision position of the CutID:
DECLARE_BIT(BitdefElectron_v1, CutIDDec,
    "Ringer holden CutID algorithm decision.",
    ElectronNBits )

// =============================================================================
/// The decision position of the Ringer algorithm's discrimation chain:
DECLARE_BIT(BitdefElectron_v1, RingerChainDec,
    "Ringer algorithm discrimation chain decision." ,
    ElectronNBits )

// =============================================================================
unsigned int ElectronTAccept_v1::getAppliedCutMsk( const Requirement cut,
    bool withCutIDTrack)
{
  // Create an empty word:
  ElectronTAccept_v1::bitMskWord word(0);

  switch (cut) {
    case  Requirement::Loose_CutID_Pd:
    case  Requirement::Loose_CutID_Pf:
    case  Requirement::Loose_LH_Pd:
    case  Requirement::Loose_LH_Pf:
    case  Requirement::Loose:
      // Add for all loose configuration their cuts applied:
      ElectronTAccept_v1::addLooseAppliedCuts(word);
      break;
    case  Requirement::Medium_CutID_Pd:
    case  Requirement::Medium_CutID_Pf:
    case  Requirement::Medium_LH_Pd:
    case  Requirement::Medium_LH_Pf:
    case  Requirement::Medium_MaxSP:
    case  Requirement::Medium:
      // Add for all medium configuration their cuts applied:
      ElectronTAccept_v1::addMediumAppliedCuts(word);
      break;
    case  Requirement::Tight_CutID_Pf:
    case  Requirement::Tight_CutID_Pd:
    case  Requirement::Tight_LH_Pf:
    case  Requirement::Tight_LH_Pd:
    case  Requirement::Tight:
      // Add for all tight configuration their cuts applied:
      ElectronTAccept_v1::addTightAppliedCuts(word);
      break;
    case  Requirement::NoCut:
      // Just for safity, reset mask bits:
      ElectronTAccept_v1::resetMsk(word);
      break;
    default:
      throw std::runtime_error(std::string("Unknown required cut."));
  }
  // If we requested to add track cuts, add them:
  if (withCutIDTrack) {
    ElectronTAccept_v1::addCutIDTrackAppliedCuts(word);
  }
  return word.to_ulong();
}

// =============================================================================
void ElectronTAccept_v1::resetMsk(bitMskWord& word)
{
  word.reset();
}

// =============================================================================
void ElectronTAccept_v1::addLooseAppliedCuts(bitMskWord& word)
{
  word.set(BitdefElectron_v1::NoErrorBit);
  word.set(BitdefElectron_v1::RingerChainDec);
}

// =============================================================================
void ElectronTAccept_v1::addMediumAppliedCuts(bitMskWord& word)
{
  word.set(BitdefElectron_v1::NoErrorBit);
  word.set(BitdefElectron_v1::RingerChainDec);
}

// =============================================================================
void ElectronTAccept_v1::addTightAppliedCuts(bitMskWord& word)
{
  word.set(BitdefElectron_v1::NoErrorBit);
  word.set(BitdefElectron_v1::RingerChainDec);
}

// =============================================================================
void ElectronTAccept_v1::addCutIDTrackAppliedCuts(
    bitMskWord& word)
{
  word.set(BitdefElectron_v1::ExecCutID);
  word.set(BitdefElectron_v1::CutIDDec);
}

} // namespace Ringer

#endif

