/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/


#ifndef TRIGMUONEVENT_TRIGMUONEVENTDICT_H
#define TRIGMUONEVENT_TRIGMUONEVENTDICT_H

#include "TrigMuonEvent/MuonFeatureContainer.h"
#include "TrigMuonEvent/MuonFeatureDetailsContainer.h"
#include "TrigMuonEvent/CombinedMuonFeature.h"
#include "TrigMuonEvent/CombinedMuonFeatureContainer.h"
#include "TrigMuonEvent/IsoMuonFeature.h"
#include "TrigMuonEvent/IsoMuonFeatureContainer.h"
#include "TrigMuonEvent/TrigMuonEFContainer.h"
#include "TrigMuonEvent/TrigMuonEFCbTrack.h"
#include "TrigMuonEvent/TrigMuonEFTrack.h"
#include "TrigMuonEvent/TrigMuonEFInfoTrack.h"
#include "TrigMuonEvent/TrigMuonEFInfoTrackContainer.h"
#include "TrigMuonEvent/TrigMuonEFInfo.h"
#include "TrigMuonEvent/TrigMuonEFInfoContainer.h"
#include "TrigMuonEvent/TrigMuonEFIsolation.h"
#include "TrigMuonEvent/TrigMuonEFIsolationContainer.h"
#include "TrigMuonEvent/TrigCombDiMuonContainer.h"
#include "TrigMuonEvent/TileMuFeature.h"
#include "TrigMuonEvent/TileMuFeatureContainer.h"
#include "TrigMuonEvent/TileTrackMuFeature.h"
#include "TrigMuonEvent/TileTrackMuFeatureContainer.h"
#include "TrigMuonEvent/TrigMuonClusterFeature.h"
#include "TrigMuonEvent/TrigMuonClusterFeatureContainer.h"


void dummy_function_for_TrigMuonEvent_which_forces_dictionaries_generation ( DataVector<MuonFeature> muf,
									     DataVector<MuonFeatureContainer> mufc,
									     DataVector<MuonFeatureDetails> mufd,
									     DataVector<MuonFeatureDetailsContainer> mufdc,
									     DataVector<CombinedMuonFeature> cmuf,
									     DataVector<CombinedMuonFeatureContainer> cmufc,
									     DataVector<IsoMuonFeature> imuf,
									     DataVector<IsoMuonFeatureContainer> imufc,
                                                                             DataVector<TrigMuonClusterFeature> tmcf,
                                                                             DataVector<TrigMuonClusterFeatureContainer> tmcfc,
									     DataVector<TrigMuonEFContainer> tmuefc,
									     DataVector<TrigMuonEFCbTrack> tmefcbt,
									     DataVector<TrigMuonEFTrack> tmeft,
									     DataVector<TrigMuonEFInfoTrack> tmefit,
									     DataVector<TrigMuonEFInfoTrackContainer> tmefitc,
									     DataVector<TrigMuonEFInfo> tmefi,
									     DataVector<TrigMuonEFInfoContainer> tmefic,
									     DataVector<TrigMuonEFIsolation> tmefiso,
									     DataVector<TrigMuonEFIsolationContainer> tmefisoc,
									     DataVector<TrigCombDiMuonContainer> tdmc,
                                                                             DataVector<TileMuFeature> tilemuf,
                                                                             DataVector<TileMuFeatureContainer> tilemufc,
                                                                             DataVector<TileTrackMuFeature> tiletrackmuf,
                                                                             DataVector<TileTrackMuFeatureContainer> tiletrackmufc
									     )
{
  DataVector<MuonFeature> aamuf = muf;
  DataVector<MuonFeatureContainer> aamufc = mufc;
  DataVector<MuonFeatureDetails> aamufd = mufd;
  DataVector<MuonFeatureDetailsContainer> aamufdc = mufdc;
  DataVector<CombinedMuonFeature> aacmuf = cmuf;
  DataVector<CombinedMuonFeatureContainer> aacmufc = cmufc;
  DataVector<IsoMuonFeature> aaimuf = imuf;
  DataVector<IsoMuonFeatureContainer> aaimufc = imufc;
  DataVector<TrigMuonClusterFeature> aatmcf = tmcf;
  DataVector<TrigMuonClusterFeatureContainer> aatmcfc = tmcfc;
  DataVector<TrigMuonEFContainer> aatmufec = tmuefc;
  DataVector<TrigMuonEFCbTrack> aatmefcbt = tmefcbt;
  DataVector<TrigMuonEFTrack> aatmeft = tmeft;
  DataVector<TrigMuonEFInfoTrack> aatmefit = tmefit;
  DataVector<TrigMuonEFInfoTrackContainer> aatmefitc = tmefitc;
  DataVector<TrigMuonEFInfo> aatmefi = tmefi;
  DataVector<TrigMuonEFInfoContainer> aatmefic = tmefic;
  DataVector<TrigMuonEFIsolation> aatmefiso = tmefiso;
  DataVector<TrigMuonEFIsolationContainer> aatmefisoc = tmefisoc;
  DataVector<TrigCombDiMuonContainer> aatdmc = tdmc;
  DataVector<TileMuFeature> atilemuf = tilemuf;
  DataVector<TileMuFeatureContainer> atilemufc = tilemufc;
  DataVector<TileTrackMuFeature> atiletrackmuf = tiletrackmuf;
  DataVector<TileTrackMuFeatureContainer> atiletrackmufc = tiletrackmufc;

  std::vector<const MuonFeature*> muonFeature; muonFeature.size();
  std::vector<const CombinedMuonFeature*> cmuonFeature; cmuonFeature.size();
  std::vector<const IsoMuonFeature*> imuonFeature; imuonFeature.size();
  std::vector<const TrigMuonClusterFeature*> mcFeature; mcFeature.size();
  std::vector<const TrigMuonEF*> muonEF; muonEF.size();
  std::vector<const TrigMuonEFTrack*> muonEFT; muonEFT.size();
  std::vector<const TrigMuonEFCbTrack*> muonEFCbT; muonEFCbT.size();
  std::vector<const TrigMuonEFInfoTrack*> muonEFIT; muonEFIT.size();
  std::vector<const TrigMuonEFIsolation*> muonEFIso; muonEFIso.size();
  std::vector<const TileMuFeature*> muFeature; muFeature.size();
  std::vector<const TileTrackMuFeature*> trackmuFeature; trackmuFeature.size();

}

#endif // TRIGMUONEVENT_TRIGMUONEVENTDICT_H
