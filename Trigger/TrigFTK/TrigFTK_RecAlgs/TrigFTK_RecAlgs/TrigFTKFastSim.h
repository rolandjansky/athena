/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

//**************************************************************************
/**
// filename: TrigFTKFastSim.h
//
// -------------------------------
// ATLAS Collaboration
*/
//***************************************************************************

#ifndef TRIGFTKFASTSIM_H
#define TRIGFTKFASTSIM_H

#include "GaudiKernel/SystemOfUnits.h"
#include "GaudiKernel/ServiceHandle.h"
#include "GaudiKernel/MsgStream.h"

#include "AthenaKernel/IAtRndmGenSvc.h"
#include "CLHEP/Random/RandomEngine.h"
#include "CLHEP/Random/RandGauss.h"

//!< Trigger specific stuff
#include "TrigInterfaces/FexAlgo.h"
#include <vector>
#include "TROOT.h"
#include "TMath.h"
#include "TRandom3.h"
#include "TF1.h"
#include "TH1.h"
#include "TFile.h"
#include "TTree.h"
#include "TTreeReader.h"
#include "TTreeReaderValue.h"
#include "TTreeReaderArray.h"

#include "TrkSurfaces/Surface.h"

#include "TrkTrack/Track.h"
#include "TrkTrack/TrackCollection.h"
#include "TrkTrackSummary/TrackSummary.h"
#include "TrkToolInterfaces/ITrackSummaryTool.h"

#include "InDetIdentifier/SCT_ID.h"
#include "InDetIdentifier/PixelID.h"
#include "SCT_ReadoutGeometry/SCT_DetectorManager.h"

#include "InDetRIO_OnTrack/SiClusterOnTrack.h"
#include "InDetRIO_OnTrack/SCT_ClusterOnTrack.h"
#include "InDetRIO_OnTrack/PixelClusterOnTrack.h"
//#include "TrkRIO_OnTrack/RIO_OnTrack.h"

#include "FTK_DataProviderInterfaces/IFTK_UncertaintyTool.h"

namespace CLHEP {
  class HepRandomEngine;
}

class TrigFTKFastSim : public HLT::FexAlgo
{
public:
  TrigFTKFastSim(const std::string &name, ISvcLocator *pSvcLocator);
  virtual ~TrigFTKFastSim() = default;
  HLT::ErrorCode hltInitialize();
  HLT::ErrorCode hltExecute(const HLT::TriggerElement* input, HLT::TriggerElement*);    
  HLT::ErrorCode hltFinalize();
  
private:
  std::string m_collection_name;
  std::string m_smearingFilePath;
  std::string m_smearingFileName;

  const AtlasDetectorID* m_id_helper;
  const PixelID* m_pixelId;
  const SCT_ID* m_sctId;
  const InDetDD::SCT_DetectorManager* m_SCT_Manager;
  ToolHandle<Trk::ITrackSummaryTool> m_trkSumTool;
  ToolHandle<IFTK_UncertaintyTool> m_uncertaintyTool;
  
  ServiceHandle<IAtRndmGenSvc> m_rndmSvc;
  std::string m_randomEngineName;
  CLHEP::HepRandomEngine* m_randomEngine;
};

#endif
