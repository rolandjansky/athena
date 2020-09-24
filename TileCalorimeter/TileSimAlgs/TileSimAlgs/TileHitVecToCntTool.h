/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

//**************************************************************************
// Filename : TileHitVecToCntTool.h
// Author   : Vishnu Zutshi
// Created  : Dec. 2009
//
// DESCRIPTION
//
//    TileHitVecToCnt copies all TileHits from TileHitVector (AthenaHitsVector)
//    to TileHitContainer (identifiable container) without any corrections.
//    If pileup option is set, all hits are merged. The pileup is done bunchXing
//    by bunchXing (main difference from TileHitVecToCnt). If several hits have
//    the same ID, energy and time from all of them is stored in vectors
//    inside single TileHit.
//
// Properties (JobOption Parameters):
//
//    TileHitVectors            string   Name of TileHitVector to read
//    TileHitContainer          string   Name of TileHitContainer to write
//    TileInfoName              string   Name of object in TDS with all parameters
//    PileUp                    bool     Do or not pileup
//    HitTimeFlag               int      Working with hit times
//**************************************************************************

#ifndef TILESIMALGS_TILEHITVECTOCNTTOOL_H
#define TILESIMALGS_TILEHITVECTOCNTTOOL_H

// Tile includes
#include "TileIdentifier/TileFragHash.h"
#include "TileSimEvent/TileHitVector.h"
#include "TileEvent/TileHitContainer.h"
#include "TileConditions/TileCablingSvc.h"

// Athena includes
#include "PileUpTools/PileUpToolBase.h"
#include "StoreGate/WriteHandleKey.h"
#include "AthenaKernel/IAthRNGSvc.h"
// Pile up
#include "PileUpTools/PileUpMergeSvc.h"

#include "AthenaKernel/ITriggerTime.h"

// Gaudi includes
#include "GaudiKernel/ServiceHandle.h"
#include "GaudiKernel/SystemOfUnits.h"

// Avoid pushing dependencies into clients- just fwd declare the following:

class TileID;
class TileTBID;
class TileHit;
class TileInfo;
class TileHitNonConstContainer;
class TileDetDescrManager;
class Identifier;
class TileCablingService;
class TileHitCollection;


namespace CLHEP {
  class HepRandomEngine;
}

// C++ STL includes
#include <string>
#include <vector>

/**
    @class TileHitVecToCntTool
    @brief This AlgTool merges TileHits from different AthenaHitVectors and stores the result in a TileHitContainer

    TileHitVecToCntTool copies all TileHits from TileHitVector (AthenaHitsVector)
    to TileHitContainer (identifiable container) without any corrections
    If pileup option is set, all hits are merged, if several hits have
    the same ID, energy and time from all of them is stored in vectors
    inside single TileHit
*/

class TileHitVecToCntTool: public PileUpToolBase {

public:
  TileHitVecToCntTool(const std::string& type, const std::string& name, const IInterface* parent); //!< Constructor
  StatusCode initialize() override final;
  virtual StatusCode prepareEvent(const EventContext& ctx, unsigned int /*nInputEvents*/) override final;
  virtual StatusCode mergeEvent(const EventContext& ctx) override final;
  virtual StatusCode processBunchXing(int bunchXing,
                                      SubEventIterator bSubEvents,
                                      SubEventIterator eSubEvents) override final;
  virtual StatusCode processAllSubEvents(const EventContext& ctx) override final;
  StatusCode finalize() override final;

private:
  StatusCode createContainers();
  void processHitVectorForOverlay(const TileHitVector* inputHits, int& nHit, double& eHitTot);
  void processHitVectorForPileUp(const TileHitVector* inputHits, double SubEvtTimOffset, int& nHit, double& eHitTot, bool isSignal = false);
  void processHitVectorWithoutPileUp(const TileHitVector* inputHits, int& nHit, double& eHitTot, TileHitNonConstContainer* &hitCont, CLHEP::HepRandomEngine * engine);
  double applyPhotoStatistics(double energy, Identifier pmt_id, CLHEP::HepRandomEngine * engine);    //!< Method to apply photostatistics effect
  void findAndMergeE1(TileHitCollection* coll, int frag_id, TileHitNonConstContainer* &hitCont);
  void findAndMergeMBTS(TileHitCollection* coll, int frag_id, TileHitNonConstContainer* &hitCont);


  Gaudi::Property<bool> m_onlyUseContainerName{this, "OnlyUseContainerName", true, "Don't use the ReadHandleKey directly. Just extract the container name from it."};
  SG::ReadHandleKeyArray<TileHitVector> m_hitVectorKeys{this, "TileHitVectors", {"TileHitVec"},
      "Name of input hit vectors (default=TileHitVec)" };  //!< vector with the names of TileHitVectors to use
  std::vector<std::string> m_hitVectorNames{};

  SG::WriteHandleKey<TileHitContainer> m_hitContainerKey{this,"TileHitContainer","TileHitCnt",
      "Output Tile hit container key"};

  SG::WriteHandleKey<TileHitContainer> m_hitContainer_DigiHSTruthKey{this,"TileHitContainer_DigiHSTruth","TileHitCnt_DigiHSTruth",
      "Output DigiHSTruth Tile hit container key"};


  Gaudi::Property<std::string> m_infoName{this, "TileInfoName", "TileInfo",
      "Name of TileInfo store (default=TileInfo)"};  //!< name of TileInfo object in TES
  bool m_run2{false};                                //!< if true => RUN2 geometry with E4' and merged E1
  bool m_run2plus{false};                            //!< if true => RUN2+ geometry with merged E1 (and E4' in RUN2)
  Gaudi::Property<bool> m_pileUp{this, "PileUp", false,
      "To switch on pileup (default=false)"}; //!< if true => pileup mode is activated
  Gaudi::Property<double> m_deltaT{this, "DeltaT", 1.0*Gaudi::Units::nanosecond,
      "Minimal Time granularity in TileHit (default=1ns)"}; //!< minimal time granularity for TileHit
  Gaudi::Property<int> m_timeFlag{this, "HitTimeFlag", 0,
      "Special options to deal with times of hits for cosmics and TB (default=0)"}; //!< special options to deal with times of hits for cosmics and TB
  Gaudi::Property<double> m_triggerTime{this, "TriggerTime", 0.0,
      "Fixed trigger time value (default=0)"}; //!< fixed trigger time value (default=0)
  Gaudi::Property<double> m_maxHitTime{this, "MaxHitTime", 150.5*Gaudi::Units::nanosecond,
      "All sub-hits with time above m_maxHitTime will be ignored"}; //!< all sub-hits with time above m_maxHitTime will be ignored
  Gaudi::Property<double> m_photoStatisticsWindow{this, "PhotostatWindow", 62.5*Gaudi::Units::nanosecond,
      "Sum up energy in [-m_photoStatWindow,+m_photoStatWindow] and use it for photostatistics"}; //!< sum up energy in [-m_photoStatWindow,+m_photoStatWindow] and use it for photostatistics
  Gaudi::Property<int> m_photoElectronStatistics{this, "PhotostatType", 2,
      "Method to apply photostatistics (default=2)"}; //!< photoelectron statistics type: 0 - Poisson, 1 - "new" Poisson + Gauss, 2 - Poisson->Gauss
  Gaudi::Property<bool> m_skipNoHit{this, "SkipNoHit", false,
      "Skip events with no Tile hits (default=false)"}; //!< if true => skip events with no Tile hits
  Gaudi::Property<bool> m_rndmEvtOverlay{this, "RndmEvtOverlay", false,
      "Pileup and/or noise added by overlaying random events (default=false)"}; //!< If true => overlay with random event (zero-luminosity pile-up)
  Gaudi::Property<bool> m_useTriggerTime{this, "UseTriggerTime", false,
      "Take trigger time from external tool (default=false)"}; //!< if true => take trigger time from external tool or from m_triggerTime
  Gaudi::Property<bool> m_doDigiTruth{this, "DoHSTruthReconstruction", true,
      "DigiTruth reconstruction"};                       //!
  PublicToolHandle<ITriggerTime> m_triggerTimeTool{this, "TriggerTimeTool", "",
      "Name of trigger time tool (default='')"}; //!< tool to take the time from

  ServiceHandle<PileUpMergeSvc> m_mergeSvc{this, "PileUpMergeSvc", "PileUpMergeSvc", ""};

  const TileID* m_tileID{};                     //!< Pointer to TileID helper
  const TileTBID* m_tileTBID{};                 //!< Pointer to TileID helper
  const TileInfo* m_tileInfo{};                 //!< Pointer to TileInfo
  float m_nPhotoElectrons[7];                         //!< number of photo electrons per GeV in samplings

  ServiceHandle<IAthRNGSvc> m_rndmSvc{this, "RndmSvc", "AthRNGSvc", ""};     //!< Random number generator engine to use
  /// Random Stream Name
  Gaudi::Property<std::string> m_randomStreamName{this, "RandomStreamName", "Tile_HitVecToCnt", ""};

  std::vector<TileHit*> m_allHits;           //!< vector for all TileHits
  std::vector<TileHit*> m_allHits_DigiHSTruth;           //!< vector for all TileHits
  TileHitNonConstContainer* m_hits{};          //!< pointer to hits container
  TileHitNonConstContainer* m_hits_DigiHSTruth{};   //!< pointer to hits container

  int m_mbtsOffset{0};                           //<! index of first MBTS hit in m_allHits vector
  static const int N_SIDE = 2;
  static const int N_PHI = 8;
  static const int N_ETA = 2;
  static const int N_MBTS_CELLS = N_SIDE * N_PHI * N_ETA;
  inline int mbts_index(int side, int phi, int eta) const {
    return (side * N_PHI + phi) * N_ETA + eta + m_mbtsOffset;
  }
  static const int E4_SIDE = -1;
  static const int E4_ETA  = 2;
  static const int E4_N_PHI = 4;
  static const int N_E4PRIME_CELLS = E4_N_PHI;
  inline int e4pr_index(int phi) const {
    return  phi + N_MBTS_CELLS + m_mbtsOffset;
  }

  ServiceHandle<TileCablingSvc> m_cablingSvc{this, "TileCablingSvc", "TileCablingSvc", ""};
  const TileCablingService* m_cabling{};
  TileFragHash m_fragHashFunc;
  std::vector<bool> m_E1merged;
  std::vector<bool> m_MBTSmerged;
  enum CELL_TOWER{E1_TOWER = 10};
};

#endif // TILESIMALGS_TILEHITVECTOCNTTOOL_H
