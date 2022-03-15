/*
  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
*/

#ifndef AFP_DIGITIZATION_TOOL_H
#define AFP_DIGITIZATION_TOOL_H

#include "PileUpTools/PileUpToolBase.h"

#include "Gaudi/Property.h"
#include "GaudiKernel/ServiceHandle.h"
#include "PileUpTools/PileUpMergeSvc.h"
#include "AthenaBaseComps/AthMsgStreamMacros.h"

#include "AFP_SimEv/AFP_TDSimHitCollection.h"
#include "AFP_SimEv/AFP_SIDSimHitCollection.h"
#include "HitManagement/TimedHitCollection.h"
#include "AFP_SimEv/AFP_TDSimHit.h"
#include "AFP_SimEv/AFP_SIDSimHit.h"
#include "AFP_DigiEv/AFP_TDDigiCollection.h"
#include "AFP_DigiEv/AFP_SiDigiCollection.h"
#include "xAODForward/AFPSiHit.h"
#include "xAODForward/AFPToFHit.h"
#include "xAODForward/AFPToFHitContainer.h"
#include "xAODForward/AFPToFHitAuxContainer.h"
#include "xAODForward/AFPSiHitContainer.h"
#include "xAODForward/AFPSiHitAuxContainer.h"

#include "AthenaKernel/IAthRNGSvc.h"
#include "AthenaKernel/RNGWrapper.h"
#include "CLHEP/Random/RandomEngine.h"
#include "CLHEP/Random/RandFlat.h"
#include "CLHEP/Random/RandGaussQ.h"
#include "CLHEP/Random/RandPoissonQ.h"

#include "TH1F.h"
#include "TF1.h"

#include <string>
#include <vector>
#include <utility> /* pair */
#include <memory> /* unique_ptr */


namespace CLHEP {
  class HepRandomEngine;
}


class AFP_PileUpTool: public PileUpToolBase {

public:
  AFP_PileUpTool(const std::string& type,
                 const std::string& name,
                 const IInterface* parent);

  virtual ~AFP_PileUpTool() {};
  
  virtual StatusCode initialize() override final;
  virtual StatusCode finalize() override final;

  /// Creates xAOD for silicon detector
  StatusCode recoSiHits(const EventContext& ctx, std::unique_ptr<AFP_SiDigiCollection>& siDigiCollection) const;

  /// Creates xAOD for time-of-flight detector
  StatusCode recoToFHits(const EventContext& ctx, std::unique_ptr<AFP_TDDigiCollection>& digitCollection) const;

  StatusCode recoAll(const EventContext& ctx, std::unique_ptr<AFP_TDDigiCollection>& digitCollection, std::unique_ptr<AFP_SiDigiCollection>& siDigiCollection) const;

  /// called before the subevts loop. Not (necessarily) able to access SubEvents
  virtual StatusCode prepareEvent(const EventContext& ctx, const unsigned int nInputEvents) override final;

  /// called for each active bunch-crossing to process current SubEvents bunchXing is in ns
  virtual StatusCode processBunchXing(int bunchXing,
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

  StatusCode fillTDDigiCollection(AFP_TDSimHitCollection& AFP_TDSimHitColl, CLHEP::HepRandomEngine* rndEngine, const EventContext& ctx);
  StatusCode fillTDDigiCollection(TimedHitCollection<AFP_TDSimHit>& thpcAFP, CLHEP::HepRandomEngine*, const EventContext& ctx, std::unique_ptr<AFP_TDDigiCollection>& digitCollection);

  StatusCode fillSiDigiCollection(AFP_SIDSimHitCollection& AFP_SIDSimHitColl, const EventContext& ctx);
  StatusCode fillSiDigiCollection(TimedHitCollection<AFP_SIDSimHit>& thpcAFP, const EventContext& ctx, std::unique_ptr<AFP_SiDigiCollection>& siDigiCollection);


  void createTDDigi(int Station, int Detector, int SensitiveElement, float GlobalTime, float WafeLength, CLHEP::HepRandomEngine* rndEngine);
  StatusCode StoreTDDigi(const EventContext& ctx, std::unique_ptr<AFP_TDDigiCollection>& digitCollection) const;

  void createSiDigi(const EventContext& ctx, int Station, int Detector, int PixelRow, int PixelCol, float PreStepX, float PreStepY, float PreStepZ, float PostStepX, float PostStepY, float PostStepZ, float DepEnergy);
  StatusCode StoreSiDigi(const EventContext& ctx, std::unique_ptr<AFP_SiDigiCollection>& siDigiCollection);


  void setupQuantumEff();
  void setupTDCOffsets();
  
  /// @brief Function that provides random noise (in charge units)
  double generateSiNoise(CLHEP::HepRandomEngine* rndEngine) const;
  /// @brief Function that provides charge collection efficiency
  double generateSiCCE(CLHEP::HepRandomEngine* rndEngine) const;
  /// @brief Function that converts quasi-continous charge to discrete time-over-threshold
  int charge2tot(int) const;
  /// @brief Function that converts discrete time-over-threshold to discrete charge
  int tot2charge(int) const;
  /// @brief Adds pre-calculated single photoelectron signal function stored in m_SignalVect to the passed histogram
  void addSignalFunc(TH1F &, double) const;
  /// @brief Provides quantum efficiency for given wavelength (in nm)
  double getQE(double ) const;
  /// @brief Invokes Reset() on all m_SignalHist objects
  void resetSignalHistograms();
  /// @brief Returns random response if the photon of given wavelegth induces the photoelectron
  bool isPhotoelectronInduced(double, CLHEP::HepRandomEngine*) const;
  /// @brief Modifies the hit time passed as an argument by the photoconversion time smearing
  void addPhotoconvTimeSmear(double &, CLHEP::HepRandomEngine*) const;
  /// @brief Returns the TDC determined from the signal shape passed as an argument
  double getTDC(const TH1F &) const;
  /// @brief Returns the ADC determined from the signal shape passed as an (1st) argument; 2nd arg is peak value
  double getADC(const TH1F &, const double) const;
  
  double SignalFun(double Time, double RiseTime, double FallTime, double offset = 0.0) const;
//  double SiSignalFun(double Time, double RiseTime, double FallTime);
  
  /// @brief Function that transforms time-over-threshold to charge
  ///
  /// Solution has been adopted from AFP_Raw2DigiTool. The best solution would be to have
  /// a single tool providing this method, which would be used in AFP_PileUpTool and AFP_Raw2DigiTool.
  Gaudi::Property<std::string> m_totToChargeTransfExpr{this, "TotToChargeTransfExpr", "1909 + x*363 + x*x*141", "Function that transforms time-over-threshold to charge"};
  Gaudi::Property<std::string> m_totToChargeTransfName{this, "TotToChargeTransfName", "TotToChargeTransfFunction", "Name of the function that transforms time-over-threshold to charge"};
  TF1 m_totToChargeTransformation;

  ServiceHandle<PileUpMergeSvc> m_mergeSvc{this, "mergeSvc", "PileUpMergeSvc", ""};
  ServiceHandle<IAthRNGSvc> m_randomSvc{this, "RndmSvc", "AthRNGSvc", ""};
  Gaudi::Property<std::string> m_randomStreamName{this, "RandomStreamName", "AFPRndEng", ""};

  Gaudi::Property<std::string> m_TDSimHitCollectionName{this, "TDSimHitCollectionName", "AFP_TDSimHitCollection"};
  SG::WriteHandleKey<AFP_TDDigiCollection> m_TDDigiCollectionKey{this, "TDDigiCollectionName", "AFP_TDDigiCollection", "Name of the Collection to hold the output from the AFP digitization, TD part"};
  SG::WriteHandleKey<xAOD::AFPToFHitContainer> m_AFPHitsContainerNameToF{this, "AFPHitsContainerNameToF", "AFPToFHitContainer"};

  Gaudi::Property<std::string> m_SIDSimHitCollectionName{this, "SIDSimHitCollectionName", "AFP_SIDSimHitCollection"};
  SG::WriteHandleKey<AFP_SiDigiCollection> m_SiDigiCollectionKey{this, "SiDigiCollectionName", "AFP_SiDigiCollection", "Name of the Collection to hold the output from the AFP digitization, SiD part"};
  SG::WriteHandleKey<xAOD::AFPSiHitContainer> m_AFPSiHitsContainerName{this, "AFPSiHitsContainerName", "AFPSiHitContainer",""};
  

  Gaudi::Property<double> m_CollectionEff{this, "CollectionEff", 0.6, "Collection efficiency"};
  Gaudi::Property<double> m_ConversionSpr{this, "ConversionSpr", 40.0, " Photon-Electron conversion spread in ps"};
  Gaudi::Property<double> m_RiseTime{this, "RiseTime", 400., "Pulse rise time in ps"};
  Gaudi::Property<double> m_FallTime{this, "FallTime", 1200., "Pulse fall time in ps"};
  Gaudi::Property<double> m_TofSignalTimeRangeLength{this, "TofSignalTimeRangeLength", 4000., "in ps"};
  Gaudi::Property<double> m_TimeOffset{this, "TimeOffset", 104500., "Offset reflecting time that proton travels from IP to AFP"};
  Gaudi::Property<double> m_CfSignalDelay{this, "CfSignalDelay", 200., "Pulse delay for const. frac. discrimination"};
  Gaudi::Property<double> m_CfdThr{this, "CfdThr", 0.5, "Constant fraction threshold"};
  Gaudi::Property<double> m_SiT_ChargeCollEff{this, "SiT_ChargeCollEff", 0.55, "CCE, adjusted to describe data"};
  Gaudi::Property<double> m_SiT_ChargeCollEffSigma{this, "SiT_ChargeCollEffSigma", 0.2, "sigma(CCE), adjusted to describe data"};
  Gaudi::Property<double> m_SiT_NoiseMu{this, "SiT_NoiseMu", 160., "unit = number of eh pairs"};
  Gaudi::Property<double> m_SiT_NoiseSigma{this, "SiT_NoiseSigma", 10., "unit = number of eh pairs"};
  Gaudi::Property<double> m_SiT_Energy2ChargeFactor{this, "SiT_Energy2ChargeFactor", 1000000./3.6, "number of eh pairs per MeV"};
  Gaudi::Property<int> m_SiT_ToTThresholdForHit{this, "SiT_ChargeThresholdForHit", 1, ""};
  
  enum QEFF_VER { QE1=1, QE2=2, nQEffOpts=3 };
  Gaudi::Property<int> m_QEffVer{this, "QEffVer", 1, "quantum efficiency version; QE1=1, QE2=2, nQEffOpts=3"};

  AFP_TDSimHitCollection m_mergedTDSimHitList;
  AFP_SIDSimHitCollection m_mergedSIDSimHitList;

  double m_QuantumEff_PMT[82];
  double m_TDC_offsets[4][4][4]; // station, train, bar
  int m_ChargeVsTot_LUT[16]; // look-up table for charge2tot conversion, 16 = n. of bits

  const int m_ArrSize; // 645120 = 4 x 6 x 336 x 80
  std::vector<float> m_deposited_charge; 
  std::vector<float> m_deposited_energy;

  /// @brief vector with pre-calculated single signal from the photoelectron
  std::vector<double> m_SignalVect;
  
  /// @brief array of histograms storing final signals in the PMTs
  TH1F m_SignalHist[4][4][4]; // station, train, bar

protected:

/// Method that creates a new AFPToFHit and sets it valus according to #digi collection
  void newXAODHitToF (SG::WriteHandle<xAOD::AFPToFHitContainer>& tofHitContainer, std::unique_ptr<AFP_TDDigiCollection>& collection) const;
  
/// Method that creates a new AFPSiHit and sets it valus according to #digi collection
  void newXAODHitSi (SG::WriteHandle<xAOD::AFPSiHitContainer>& xAODSiHit, std::unique_ptr<AFP_SiDigiCollection>& collection) const;

  std::unique_ptr<AFP_TDDigiCollection> m_digitCollection;
  std::unique_ptr<AFP_SiDigiCollection> m_SiDigiCollection;

};

#endif
