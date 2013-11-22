/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

////////////////////////////////////////////////////////////////////////////////
//
// filename: TrigToTrkTrackTool.h
// 
// Tool to convert TrigInDet tracks to new EDM Trk tracks
//
// author:    Dmitry EMELIYANOV <D.Emeliyanov@rl.ac.uk>
//
// date: 26/05/2004
//
// -------------------------------
// ATLAS Collaboration
////////////////////////////////////////////////////////////////////////////////


#ifndef TRIGTOTRKTRACKTOOL_H 
#define TRIGTOTRKTRACKTOOL_H

#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/AlgTool.h"
#include "TrigInDetEvent/TrigInDetTrack.h"
#include "TrigInDetEvent/TrigInDetTrackCollection.h"
#include "TrkTrack/TrackCollection.h"
#include "InDetIdentifier/SCT_ID.h"
#include "InDetIdentifier/PixelID.h" 
#include "TrkFitterInterfaces/ITrackFitter.h"
#include "TrkFitterUtils/FitterTypes.h"

class TrigTimer;

class TrigToTrkTrackTool: public AlgTool
{
 public:
  
  TrigToTrkTrackTool( const std::string&, const std::string&, const IInterface* );
  virtual ~TrigToTrkTrackTool();

  static const InterfaceID& interfaceID();
  virtual StatusCode initialize();
  virtual StatusCode finalize  ();  
  void execute(const TrigInDetTrackCollection*,TrackCollection*) const;		
 private:
  bool m_createRIO, m_doFit;
  const PixelID* m_pixelId;
  const SCT_ID* m_sctId;
#define TRIGTRACKTOOL_NTIMERS 5
  TrigTimer* m_timer[TRIGTRACKTOOL_NTIMERS];
  bool m_timers;
  bool m_useOnlineClusters;
  const Trk::ITrackFitter* m_ITrkFitter;
  std::string m_FitterName;
  std::string m_FitterInstance;
};

#endif
