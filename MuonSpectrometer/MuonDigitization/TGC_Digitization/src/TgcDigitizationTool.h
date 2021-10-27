/* -*- C++ -*- */

/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#ifndef MUONDIGITIZATION_TGC_DIGITIZATIONTOOL_H
#define MUONDIGITIZATION_TGC_DIGITIZATIONTOOL_H
/** @file TgcDigitizationTool.h
 * @brief implementation of IPileUpTool to produce TgcDigit objects from TGCSimHit
 * @author John Chapman, Yoji Hasegawa, Susumu Oda
 */

#include "PileUpTools/PileUpToolBase.h"

#include "GaudiKernel/ServiceHandle.h"
#include "AthenaKernel/IAthRNGSvc.h"
#include "PileUpTools/PileUpMergeSvc.h"

#include "HitManagement/TimedHitCollection.h"
#include "MuonSimEvent/TGCSimHit.h"
#include "MuonSimEvent/TGCSimHitCollection.h"
#include "MuonDigitContainer/TgcDigitContainer.h"
#include "MuonSimData/MuonSimDataCollection.h"
#include "MuonCondData/TgcDigitASDposData.h"

class PileUpMergeSvc;
class TgcDigitMaker;
class TgcHitIdHelper;
class TgcIdHelper;

namespace CLHEP{
  class HepRandomEngine;
}

namespace MuonGM{
  class MuonDetectorManager;
}

class TgcDigitizationTool : public PileUpToolBase {
public:
  TgcDigitizationTool(const std::string& type,
                      const std::string& name,
                      const IInterface* parent);
  /** Initialize */
  virtual StatusCode initialize() override final;

  virtual StatusCode prepareEvent(const EventContext& ctx, unsigned int /*nInputEvents*/) override final;

  /** called for each active bunch-crossing to process current SubEvents
      bunchXing is in ns */
  virtual  StatusCode processBunchXing(
                                       int bunchXing,
                                       SubEventIterator bSubEvents,
                                       SubEventIterator eSubEvents
                                       ) override final;

  /** called at the end of the subevts loop. Not (necessarily) able to access
      SubEvents (IPileUpTool) */
  virtual StatusCode mergeEvent(const EventContext& ctx) override final;

  /** alternative interface which uses the PileUpMergeSvc to obtain
  all the required SubEvents.  Reads GEANT4 hits from StoreGate in
  each of detector components corresponding to TGC modules which are
  triplets or doublets. A triplet has tree sensitive volumes and a
  double has two. This method calls TgcDigitMaker::executeDigi, which
  digitizes every hit, for every readout element, i.e., a sensitive
  volume of a chamber. */
  virtual StatusCode processAllSubEvents(const EventContext& ctx) override final;

  /** Finalize */
  virtual StatusCode finalize() override final;

private:
  /** Get next event and extract collection of hit collections */
  StatusCode getNextEvent(const EventContext& ctx);
  /** Core part of digitization used by processAllSubEvents and mergeEvent */
  StatusCode digitizeCore(const EventContext& ctx);

protected:
  ServiceHandle<PileUpMergeSvc> m_mergeSvc{this, "PileUpMergeSvc", "PileUpMergeSvc", ""}; // Pile up service
  ServiceHandle<IAthRNGSvc> m_rndmSvc{this, "RndmSvc", "AthRNGSvc", ""};      // Random number service

private:
  TgcHitIdHelper*                    m_hitIdHelper{};
  const TgcIdHelper*                 m_idHelper{};
  const MuonGM::MuonDetectorManager* m_mdManager{};
  TgcDigitMaker*                     m_digitizer{};
  TimedHitCollection<TGCSimHit>*     m_thpcTGC{};
  std::list<TGCSimHitCollection*>    m_TGCHitCollList;

  Gaudi::Property<bool> m_onlyUseContainerName{this, "OnlyUseContainerName", true, "Don't use the ReadHandleKey directly. Just extract the container name from it."};
  SG::ReadHandleKey<TGCSimHitCollection> m_hitsContainerKey{this, "InputObjectName", "TGC_Hits", "name of the input object"};
  std::string m_inputHitCollectionName{""};
  SG::ReadCondHandleKey<TgcDigitASDposData> m_readCondKey_ASDpos{this,"TGCDigitASDposKey","","ReadCondHandleKey for TGCDigitASDposData"};
  SG::WriteHandleKey<TgcDigitContainer> m_outputDigitCollectionKey{this,"OutputObjectName","TGC_DIGITS","WriteHandleKey for Output TgcDigitContainer"}; // name of the output digits
  SG::WriteHandleKey<MuonSimDataCollection> m_outputSDO_CollectionKey{this,"OutputSDOName","TGC_SDO","WriteHandleKey for Output MuonSimDataCollection"}; // name of the output SDOs

  //pileup truth veto
  Gaudi::Property<bool> m_includePileUpTruth{this, "IncludePileUpTruth", true, "Include pile-up truth info"};
};
#endif // MUONDIGITIZATION_TGC_DIGITIZATIONTOOL_H
