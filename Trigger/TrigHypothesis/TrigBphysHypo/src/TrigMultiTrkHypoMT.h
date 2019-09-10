/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

/**************************************************************************
 **
 **   File: Trigger/TrigHypothesis/TrigBphysHypo/TrigMultiTrkHypoMT.h
 **
 **   Description: multi-track hypothesis algorithm 
 **
 **   Author: H. Russell
 **
 **************************************************************************/ 

#ifndef TRIG_TrigMultiTrkHypoMT_H 
#define TRIG_TrigMultiTrkHypoMT_H

// standard stuff
#include <string>
// general athena stuff
#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/IToolSvc.h"
#include "GaudiKernel/StatusCode.h"
#include "xAODMuon/MuonContainer.h"


#include "AthenaBaseComps/AthReentrantAlgorithm.h"
#include "AthViews/View.h"
#include "TrigSteeringEvent/TrigRoiDescriptorCollection.h"
#include "StoreGate/ReadHandleKey.h"
#include "StoreGate/WriteHandleKey.h"

#include "DecisionHandling/TrigCompositeUtils.h"

#include "TrigMultiTrkHypoToolMT.h"
#include "TrigBphysHelperUtilsTool.h"

#include "DecisionHandling/HypoBase.h"

#include "Constants.h"

#include "AthenaMonitoring/Monitored.h"
#include "AthenaMonitoring/GenericMonitoringTool.h"


class TrigMultiTrkHypoMT: public ::HypoBase  {
  enum { MaxNumberTools = 20 };
  public:
    TrigMultiTrkHypoMT(const std::string & name, ISvcLocator* pSvcLocator);
    ~TrigMultiTrkHypoMT();

    virtual StatusCode  initialize() override;
   virtual StatusCode  execute( const EventContext& context ) const override;
   virtual StatusCode  finalize() override;

  protected:
   //The vertexing tool is not reentrant so this algorithm cannot be used reentrantly
   bool isReEntrant() const override { return false; }

  private:

    TrigMultiTrkHypoMT();
    ToolHandleArray< TrigMultiTrkHypoToolMT > m_hypoTools {this, "HypoTools", {}, "Tools to perform selection"};

	bool passNTracks(int nObjMin,
                 const std::vector<float> & ptObjMin,
  				 const std::vector<ElementLink<xAOD::TrackParticleContainer> > & inputTrkVec,
                 std::vector<ElementLink<xAOD::TrackParticleContainer> > & outputTrkVec,
                 float mindR) const;
                 
    SG::ReadHandleKey<xAOD::TrackParticleContainer> m_trackParticleContainerKey{ this,
        "trackCollectionKey",         // property name
        "Tracks",                     // default value of StoreGate key
        "input TrackParticle container name"};

    SG::ReadHandleKey<xAOD::MuonContainer> m_muonContainerKey{ this,
        "MuonCollectionKey",         // property name
        "CBCombinedMuon",                     // default value of StoreGate key
        "input EF Muon container name"};

    SG::WriteHandleKey<xAOD::TrigBphysContainer> m_bphysObjContKey{ this,
        "bphysCollectionKey",         // property name
        "TrigBphysContainer",         // default value of StoreGate key
        "Output TrigBphysContainer name"};

  static bool sortTracks(ElementLink<xAOD::TrackParticleContainer> &l1, ElementLink<xAOD::TrackParticleContainer> &l2)
  {	
    return (*l1)->pt() > (*l2)->pt();
  }

  ToolHandle <TrigBphysHelperUtilsTool> m_bphysHelperTool { this, 
    "TrigBphysHelperUtilsTool","TrigBphysHelperUtilsTool","Select bphys helper utils tool you want to use"};
    
  //need to implement a timer still!
  //TrigTimer* m_BmmHypTot;

  Gaudi::Property<int> m_nTrk { this, "nTrk",2,"Number of tracks in the vertex"};
  Gaudi::Property<int> m_nTrkQ { this, "nTrkCharge",-1,"sum of track charges"}; // if negative - no cut
  Gaudi::Property<std::vector<double>> m_trkMasses {this, "trackMasses", {105.6583745,105.6583745}, "track masses for vertex reco (one per track!)"};
  Gaudi::Property<std::vector<float>> m_ptTrkMin { this, "trkPtThresholds", {3650, 3650} ,"minimum track pTs (one per track, sorted descending!)"};

  Gaudi::Property<std::vector<float>> m_nTrkMassMin { this, "nTrackMassMin", {0},"list of mins for nTrk mass windows"}; // both min+max need to be defined
  Gaudi::Property<std::vector<float>> m_nTrkMassMax { this, "nTrackMassMax", {100000},"list of maxes for nTrk mass windows"};

  Gaudi::Property<float> m_mindR { this, "overlapdR",0.01 ,"Minimum dR between tracks (overlap removal)"};
    
  Gaudi::Property<int> m_particleType { this, "particleType", 0, "Types of particles to use. 0 = tracks, 1 = EF Muons, 2 = tbd..."}; 
    
  ToolHandle< GenericMonitoringTool > m_monTool { this, "MonTool", "", "Monitoring tool" };

};

#endif
