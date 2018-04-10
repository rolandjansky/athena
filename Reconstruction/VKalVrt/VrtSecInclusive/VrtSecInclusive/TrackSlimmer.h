/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// VKalVrt.h
//
#ifndef _VrtSecInclusive_TrackSlimmer_H
#define _VrtSecInclusive_TrackSlimmer_H


#include "VrtSecInclusive/Constants.h"

#include "AthenaBaseComps/AthAlgorithm.h"

// Gaudi includes
#include "GaudiKernel/ToolHandle.h"
#include "GaudiKernel/ITHistSvc.h"
#include "StoreGate/StoreGateSvc.h"
//
#include "TrkVKalVrtFitter/TrkVKalVrtFitter.h"

// for truth
#include "GeneratorObjects/McEventCollection.h"

#include "AthenaKernel/IThinningSvc.h"
#include "TrkToolInterfaces/ITruthToTrack.h"
#include "ITrackToVertex/ITrackToVertex.h"
#include "TrkVertexFitterInterfaces/ITrackToVertexIPEstimator.h"
#include "TrkExInterfaces/IExtrapolator.h"
#include "TrkExInterfaces/IPropagator.h"
#include "TrkSurfaces/CylinderSurface.h"
#include "TrkDetDescrInterfaces/IVertexMapper.h"
#include "GaudiKernel/ServiceHandle.h"
#include "InDetConditionsSummaryService/IInDetConditionsSvc.h"
#include "InDetIdentifier/PixelID.h"
#include "InDetIdentifier/SCT_ID.h"

// xAOD Classes
#include "xAODEventInfo/EventInfo.h"
#include "xAODTracking/TrackParticleContainer.h"
#include "xAODTracking/TrackParticleAuxContainer.h"
#include "xAODTracking/TrackParticle.h"
#include "xAODTracking/VertexContainer.h"
#include "xAODTracking/VertexAuxContainer.h"
#include "xAODTruth/TruthParticleContainer.h"
#include "xAODTruth/TruthEventContainer.h"
#include "xAODTruth/TruthVertexContainer.h"
#include "xAODMuon/MuonContainer.h"
#include "xAODMuon/Muon.h"
#include "xAODEgamma/ElectronContainer.h"
#include "xAODEgamma/Electron.h"

// Normal STL and physical vectors
#include <vector>
#include <deque>
#include <functional>


/** Forward declarations **/

class TH1;


namespace VKalVrtAthena {
  
  class TrackSlimmer : public AthAlgorithm {
    
  public:
    
    /** Standard Athena-Algorithm Constructor */
    TrackSlimmer(const std::string& name, ISvcLocator* pSvcLocator);
    ~TrackSlimmer();
  
    virtual StatusCode beginRun()   override;
    virtual StatusCode initialize() override;
    virtual StatusCode finalize()   override;
    virtual StatusCode execute()    override;
   
    virtual StatusCode initEvent();
    
  private:
    
    struct JobProperties {
      
      std::string containerName;
      
    };
    
    struct JobProperties m_jp;
    
    void declareProperties();
    
    ServiceHandle<IThinningSvc>   m_thinningSvc;
    
  };
  
} // end of namespace bracket


#endif /* _VrtSecInclusive_TrackSlimmer_H */
