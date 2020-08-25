/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#ifndef ZDC_DIGITIZATION_TOOL_H
#define ZDC_DIGITIZATION_TOOL_H

#include "PileUpTools/PileUpToolBase.h"
#include "Gaudi/Property.h"
#include "GaudiKernel/ServiceHandle.h"
#include "GaudiKernel/ITHistSvc.h"
#include "AthenaKernel/IAtRndmGenSvc.h"
#include "ZDC_SimEvent/ZDC_SimStripHit_Collection.h"
#include "ZDC_SimEvent/ZDC_SimPixelHit_Collection.h"
#include "HitManagement/TimedHitCollection.h"
#include "ZDC_SimEvent/ZDC_SimStripHit.h"
#include "ZDC_SimEvent/ZDC_SimPixelHit.h"
#include "ZdcEvent/ZdcDigitsCollection.h"
#include "PileUpTools/PileUpMergeSvc.h"
#include <string>
#include <fstream>

#include <vector>
#include <utility> /* pair */

class IAtRndmGenSvc;

class ZDC_PileUpTool: public PileUpToolBase {

 public:

  ZDC_PileUpTool(const std::string& type,
                 const std::string& name,
                 const IInterface* parent);

  virtual StatusCode initialize() override final;
  virtual StatusCode finalize() override final;

  /// called before the subevts loop. Not (necessarily) able to access SubEvents
  virtual StatusCode prepareEvent(const EventContext& ctx,const unsigned int nInputEvents) override final;

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

  StatusCode recordContainers(ServiceHandle<StoreGateSvc>& evtStore, std::string m_key_digitCnt);

  void fillStripDigitContainer(const ZDC_SimStripHit_Collection*,    CLHEP::HepRandomEngine*);
  void fillStripDigitContainer(TimedHitCollection<ZDC_SimStripHit>&, CLHEP::HepRandomEngine*);

  void fillPixelDigitContainer(const ZDC_SimPixelHit_Collection*,    CLHEP::HepRandomEngine*);
  void fillPixelDigitContainer(TimedHitCollection<ZDC_SimPixelHit>&, CLHEP::HepRandomEngine*);

  void createAndStoreStripDigit(int Side, int ModuleNo, int NPhotons, CLHEP::HepRandomEngine* rndEngine);
  void createAndStorePixelDigit(int Side, int ModuleNo, int PixNum, int NPhotons, CLHEP::HepRandomEngine* rndEngine);

  void SetDumps(bool, bool);

  ServiceHandle<PileUpMergeSvc> m_mergeSvc{this, "mergeSvc", "PileUpMergeSvc", ""};
  ServiceHandle<IAtRndmGenSvc>  m_atRndmGenSvc{this, "RndmSvc", "AtRndmGenSvc",
      "Random Number Service used in ZDC digitization"};
  CLHEP::HepRandomEngine       *m_rndEngine{nullptr};

  Gaudi::Property<std::string> m_SimStripHitCollectionName{this, "SimStripHitCollection" , "ZDC_SimStripHit_Collection",
      "Name of the input Collection of the simulated Strip Hits"};
  Gaudi::Property<std::string> m_SimPixelHitCollectionName{this, "SimPixelHitCollection" , "ZDC_SimPixelHit_Collection",
      "Name of the input Collection of the simulated Pixel Hits"};
  Gaudi::Property<std::string> m_ZdcDigitsContainerName{this, "ZdcDigitsContainerName", "ZdcDigitsCollection",
      "Name of the Container to hold the output from the ZDC digitization"};

  Gaudi::Property<double> m_ScaleStrip{this, "ScaleStrip", 3e-4, ""};
  Gaudi::Property<double> m_ScalePixel{this, "ScalePixel", 5e-3, ""}; // use this to normalize the signal from the strips;
  Gaudi::Property<int>    m_Pedestal  {this, "Pedestal"  , 100, ""};                 // Pedestal=40 ;RMS=3??(ask Andrei about this)
  Gaudi::Property<int>    m_MaxTimeBin{this, "MaxTimeBin", 5, ""};               // The number of time-slices after digitization(Typically 5 or 7)
  Gaudi::Property<double> m_SigmaNoiseHG_Strip{this, "SigmaNoiseHG_Strip", 1.5, ""};
  Gaudi::Property<double> m_SigmaNoiseHG_Pixel{this, "SigmaNoiseHG_Pixel", 1.0, ""};
  Gaudi::Property<double> m_SigmaNoiseLG_Strip{this, "SigmaNoiseLG_Strip", 1.0, ""};
  Gaudi::Property<double> m_SigmaNoiseLG_Pixel{this, "SigmaNoiseLG_Pixel", 1.0, ""};
  bool   m_DumpStrip{false}, m_DumpPixel{false};

  ZDC_SimStripHit_Collection *m_mergedStripHitList{};
  ZDC_SimPixelHit_Collection *m_mergedPixelHitList{};

  std::ofstream m_MyFile;
  ZdcDigitsCollection *m_digitContainer{};

  double HighToLow(double signal, double gain_ratio, double gain_error, CLHEP::HepRandomEngine* rndEngine);

  double m_GainRatio_Strip[8];
  double m_GainRatioError_Strip[8];
  double m_GainRatio_Pixel{10.0};
  double m_GainRatioError_Pixel{0.5};

  unsigned int PixelID(int Side=-1, int Module=-1, int PixNo=-1);
};

#endif
