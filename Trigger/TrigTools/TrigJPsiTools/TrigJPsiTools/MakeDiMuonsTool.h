/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef MakeDiMuonsTool_H
#define MakeDiMuonsTool_H

// ***********************************************************************
// MakeDiMuonsTool.h
// AUTHORS: Weina Ji (weina.ji@cern.ch)
//          Peter Wijeratne (paw@hep.ucl.ac.uk)
//          Christian Gutschow (chris.g@cern.ch)
// ***********************************************************************
#include "TrigInDetEvent/TrigInDetTrackCollection.h"
#include "GaudiKernel/StatusCode.h"
#include "TrigEffJpsiTools/ITrigEffJpsiTools.h"
#include "GaudiKernel/ToolHandle.h"
#include "muonEvent/Muon.h"
#include "muonEvent/MuonContainer.h"
#include "muonEvent/MuonParamDefs.h"
#include "TrigJPsiTools/DiMuonTool.h"
#include "TrigJPsiTools/MuonTrackSelector.h"

#include "TrigMuonEvent/TrigMuonEFTrack.h"
#include "TrigMuonEvent/TrigMuonEFCbTrack.h"
#include "TrigMuonEvent/TrigMuonEFInfoTrack.h"

class MakeDiMuonsTool
{
 public:
  MakeDiMuonsTool();
  virtual ~MakeDiMuonsTool();
  bool initializeTools();
  void initializeMembers();

  void setProbes(std::vector<const CaloCluster*> CombinedCollection, std::vector<int> roi);
  void setProbes(std::vector< const TrigMuonEFTrack*> trksIn, std::vector<int> roi);
  void setProbes(std::vector< const Rec::TrackParticle* > trksIn);
  void setProbes(const TrigInDetTrackCollection* trksIn);

  void setTags(const Analysis::MuonContainer* muonsIn);
  void setTags(std::vector<const TrigMuonEFCbTrack*> muonsIn, std::vector<int> roi);
  void setTags(std::vector< const egamma*> trksIn, std::vector<int> roi);

  void doTagAndProbe(bool doTagAndProbe);
  void useTrigger(bool useTrigger);
  void setPtcut(double ptcut);
  void setTagPtcut(double ptcut);
  void setEtacut(double etacut);
  void setTagEtacut(double etacut);
  void setTrackMass(double muonmass);
  void setUpperInvMassLimit(double upperlimit);
  void setLowerInvMassLimit(double lowerlimit);
  void makePairs(std::vector<Wrapper::MuonTrack*> Tracks);
  void makePairs(std::vector<Wrapper::MuonTrack*> taggedTracks,std::vector<Wrapper::MuonTrack*> probedTracks);
  std::vector<DiMuonTool*> execute();
  void TagMuons( std::vector< const TrigMuonEFCbTrack* > m_CombinedCollection, std::vector<int> roi );
  void TagMuons( const Analysis::MuonContainer* m_MuonCollection );

  bool isTriggeredEF(const Rec::TrackParticle* track, std::string chainName) const;
  bool isTriggeredL2(const Rec::TrackParticle* track, std::string chainName) const;
  bool isTriggeredL1(const Rec::TrackParticle* track, std::string triggerName) const;
	
  void clear();

 private:
  double m_ptcut;
  double m_tagptcut;
  double m_etacut;
  double m_tagetacut;
  bool   m_TagAndProbe;

  bool   m_runOnline;

  bool   m_useTrigger;
  double m_invmass;
  double m_trkmass;
  double m_upperlimit;
  double m_lowerlimit;
  std::vector<Wrapper::MuonTrack*> m_tracks;
  const Analysis::MuonContainer* m_MuonCollection;
  std::vector<const TrigMuonEFCbTrack*> m_CombinedCollection;
  const TrigInDetTrackCollection* m_TrigCollection;
  std::vector<Wrapper::MuonTrack*> m_taggedMuons;
  std::vector<DiMuonTool*> m_pairs; 
  ToolHandle<ITrigEffJpsiTools> m_trigEffJpsiTools; 
  MuonTrackSelector m_selectaProbe;
  MuonTrackSelector m_selectaTag;
};

#endif
