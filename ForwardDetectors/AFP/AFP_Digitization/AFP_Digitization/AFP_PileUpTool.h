/*
  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
*/

#ifndef AFP_DIGITIZATION_TOOL_H
#define AFP_DIGITIZATION_TOOL_H

#include "PileUpTools/PileUpToolBase.h"
#include "xAODEventInfo/EventInfo.h"             // NEW EDM
#include "xAODEventInfo/EventAuxInfo.h"          // NEW EDM


#include "Gaudi/Property.h"
#include "GaudiKernel/ServiceHandle.h"
#include "GaudiKernel/ITHistSvc.h"
#include "AthenaKernel/IAthRNGSvc.h"
#include "AFP_SimEv/AFP_TDSimHitCollection.h"
#include "AFP_SimEv/AFP_SIDSimHitCollection.h"
#include "HitManagement/TimedHitCollection.h"
#include "AFP_SimEv/AFP_TDSimHit.h"
#include "AFP_SimEv/AFP_SIDSimHit.h"
#include "AFP_DigiEv/AFP_TDDigiCollection.h"
#include "AFP_DigiEv/AFP_SiDigiCollection.h"
#include "PileUpTools/PileUpMergeSvc.h"
#include "TMath.h"
#include "TH1.h"
#include "TF1.h"
#include "TFile.h"
#include <string>
#include <fstream>

#include <vector>
#include <utility> /* pair */

namespace CLHEP {
  class HepRandomEngine;
}

class AFP_PileUpTool: public PileUpToolBase {

public:
  AFP_PileUpTool(const std::string& type,
                 const std::string& name,
                 const IInterface* parent);

  virtual StatusCode initialize() override final;
  virtual StatusCode finalize() override final;

  /// called before the subevts loop. Not (necessarily) able to access SubEvents
  virtual StatusCode prepareEvent(const EventContext& ctx, const unsigned int nInputEvents) override final;

  /// called for each active bunch-crossing to process current SubEvents bunchXing is in ns
  virtual StatusCode processBunchXing(
                                      int bunchXing,
                                      SubEventIterator bSubEvents,
                                      SubEventIterator eSubEvents
                                      ) override final;
  /// return false if not interested in  certain xing times (in ns)
  /// implemented by default in PileUpToolBase as FirstXing<=bunchXing<=LastXing
  //  virtual bool toProcess(int bunchXing) const;

  /// called at the end of the subevts loop. Not (necessarily) able to access SubEvents
  virtual StatusCode mergeEvent(const EventContext& ctx) override final;

  virtual StatusCode processAllSubEvents(const EventContext& ctx) override final;

private:

  StatusCode fillTDDigiCollection(const AFP_TDSimHitCollection*,     CLHEP::HepRandomEngine*, const EventContext& ctx);
  StatusCode fillTDDigiCollection(TimedHitCollection<AFP_TDSimHit>&, CLHEP::HepRandomEngine*, const EventContext& ctx);

  StatusCode fillSiDigiCollection(const AFP_SIDSimHitCollection*, const EventContext& ctx);
  StatusCode fillSiDigiCollection(TimedHitCollection<AFP_SIDSimHit>&, const EventContext& ctx);

  void createTDDigi(int Station, int Detector, int SensitiveElement, float GlobalTime, float WafeLength, CLHEP::HepRandomEngine* rndEngine);
  StatusCode StoreTDDigi(const EventContext& ctx);

  void createSiDigi(int Station, int Detector, int PixelRow, int PixelCol, float PreStepX, float PreStepY, float PreStepZ, float PostStepX, float PostStepY, float PostStepZ, float DepEnergy);
  StatusCode StoreSiDigi(const EventContext& ctx);


  double SignalFun(double Time, double RiseTime, double FallTime);

  ServiceHandle<PileUpMergeSvc> m_mergeSvc{this, "mergeSvc", "PileUpMergeSvc", ""};
  ServiceHandle<IAthRNGSvc> m_randomSvc{this, "RndmSvc", "AthRNGSvc", ""};
  Gaudi::Property<std::string> m_randomStreamName{this, "RandomStreamName", "AFPRndEng", ""};

  std::string m_TDSimHitCollectionName{"AFP_TDSimHitCollection"};
  SG::WriteHandleKey<AFP_TDDigiCollection> m_TDDigiCollectionKey{this, "TDDigiCollectionName", "AFP_TDDigiCollection", "Name of the Collection to hold the output from the AFP digitization, TD part"};

  std::string m_SIDSimHitCollectionName{"AFP_SIDSimHitCollection"};
  SG::WriteHandleKey<AFP_SiDigiCollection> m_SiDigiCollectionKey{this, "SiDigiCollectionName", "AFP_SiDigiCollection", "Name of the Collection to hold the output from the AFP digitization, SiD part"};

  double m_CollectionEff{0.6}; // Collection efficiency
  double m_ConversionSpr{40.0}; // Photon-Electron conversion spread in ps
  double m_Gain{50000.}; // Gain
  double m_RiseTime{400.}; // Pulse rise time in ps
  double m_FallTime{1200.}; // Pulse fall time in ps
  double m_CfdThr{0.5}; // Constant fraction threshold

  AFP_TDSimHitCollection *m_mergedTDSimHitList{};

  AFP_SIDSimHitCollection *m_mergedSIDSimHitList{};


  double m_QuantumEff_PMT[7]{};

  TH1F  m_Signal[4][32][2];

  std::vector<float> m_deposited_charge;
  std::vector<float> m_deposited_energy;

  std::vector<double> m_SignalVect;

};

#endif
