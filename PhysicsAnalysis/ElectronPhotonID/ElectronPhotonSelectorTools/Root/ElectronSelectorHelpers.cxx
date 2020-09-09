/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#include "ElectronPhotonSelectorTools/ElectronSelectorHelpers.h"
#include "xAODEgamma/Electron.h"
#include "xAODTracking/TrackParticle.h"
#include "xAODTracking/TrackParticleContainer.h"
#include "AsgTools/AsgMessaging.h"

// ==================================================================
std::size_t ElectronSelectorHelpers::numberOfPixelHitsAndDeadSensors(const xAOD::TrackParticle *tp){

  // Define an AsgMessaging instance
  static const asg::AsgMessaging msg("ElectronSelectorHelpers");

  if(!tp){
    msg.msg(MSG::WARNING) << "No track particle found!" << endmsg;
    return 0;
  }

  uint8_t nPixHits(0);
  uint8_t nPixDead(0);

  bool allFound = true;
  allFound = allFound && tp->summaryValue(nPixHits, xAOD::numberOfPixelHits);
  allFound = allFound && tp->summaryValue(nPixDead, xAOD::numberOfPixelDeadSensors);
  if(!allFound){
    msg.msg(MSG::WARNING) << "Failed to load all pixel variables" << endmsg;
  }

  return nPixHits + nPixDead;
}
// ==================================================================
std::size_t ElectronSelectorHelpers::numberOfSCTHitsAndDeadSensors(const xAOD::TrackParticle *tp){

  // Define an AsgMessaging instance
  static const asg::AsgMessaging msg("ElectronSelectorHelpers");

  if(!tp){
    msg.msg(MSG::WARNING) << "No track particle found!" << endmsg;
    return 0;
  }

  uint8_t nSCTHits(0);
  uint8_t nSCTDead(0);

  bool allFound = true;
  allFound = allFound && tp->summaryValue(nSCTHits, xAOD::numberOfSCTHits);
  allFound = allFound && tp->summaryValue(nSCTDead, xAOD::numberOfSCTDeadSensors);
  if(!allFound){
    msg.msg(MSG::WARNING) << "Failed to load all SCT variables" << endmsg;
  }

  return nSCTHits + nSCTDead;
}
// ==================================================================
std::size_t ElectronSelectorHelpers::numberOfSiliconHitsAndDeadSensors(const xAOD::TrackParticle *tp){
  return numberOfPixelHitsAndDeadSensors(tp) + numberOfSCTHitsAndDeadSensors(tp);
}
// ==================================================================
bool ElectronSelectorHelpers::passBLayerRequirement(const xAOD::TrackParticle *tp){

  // Define an AsgMessaging instance
  static const asg::AsgMessaging msg("ElectronSelectorHelpers");

  if(!tp){
    msg.msg(MSG::WARNING) << "No track particle found!" << endmsg;
    return 0;
  }

  uint8_t expectInnermostLayer(0);
  uint8_t nInnermostLayerHits(0);
  uint8_t nInnermostLayerOutliers(0);
  uint8_t expectNextToInnermostLayer(0);
  uint8_t nNextToInnermostLayerHits(0);
  uint8_t nNextToInnermostLayerOutliers(0);

  bool allFound = true;
  allFound = allFound && tp->summaryValue(expectInnermostLayer, xAOD::expectBLayerHit);
  allFound = allFound && tp->summaryValue(nInnermostLayerHits, xAOD::numberOfBLayerHits);
  allFound = allFound && tp->summaryValue(nInnermostLayerOutliers, xAOD::numberOfBLayerOutliers);
  allFound = allFound && tp->summaryValue(expectNextToInnermostLayer, xAOD::expectNextToInnermostPixelLayerHit);
  allFound = allFound && tp->summaryValue(nNextToInnermostLayerHits, xAOD::numberOfNextToInnermostPixelLayerHits);
  allFound = allFound && tp->summaryValue(nNextToInnermostLayerOutliers, xAOD::numberOfNextToInnermostPixelLayerOutliers);
  if(!allFound){
    msg.msg(MSG::WARNING) << "Failed to load all BLayer variables" << endmsg;
  }

  // If innermost and next to innermost layer are both masked off, then the
  // selectors should PASS the BL cut, so the default here is true
  bool passBLReq = true;

  if(expectInnermostLayer){
    passBLReq = ( nInnermostLayerHits + nInnermostLayerOutliers > 0 );
  }
  else if(expectNextToInnermostLayer){
    passBLReq = ( nNextToInnermostLayerHits + nNextToInnermostLayerOutliers > 0 );
  }

  return passBLReq;
}


bool ElectronSelectorHelpers::passAmbiguity(xAOD::AmbiguityTool::AmbiguityType type, const uint8_t criterion){

  // helper to check if ambiguity type is one of several that are stored in a bitmask
  //
  // Christos Rel 22 new ambi.
  //
  // The issue is that the "criterion" encodes what should
  // pass only.
  //
  // If it was not stored in the GroupData and need to keep compatibility
  // this would have been trivial but ....
  //
  // We want reject types/values/enums/"bits" 5,6.
  //
  // One way would have been  a mask
  // 0b1100000 (96)
  // and check
  // (mask & 0x1<<type) !=0
  // This would have been consistent with the rest of e/gamma
  // but this is not what is going on here.
  //
  // What seems we check is 0b11111 (31) e.g we do not specify explicitly the bit
  // to cut on. Which of course now breaks due to additional type 7
  // which we want to keep.
  //
  // For now solve ad-hoc and keep compatibility with existing config
  // tooling in GroupArea/PHYS/Derivation etc.
  //
  // But prb we need a better convention
  //
  // For now add 128 to the criterion  so we get b10011111 and bit 7 passes.
  // So we return true for types 0,1,2,3,4,7 we reject 5,6
  //
  return (128+criterion) & 0x1<<type;

}
