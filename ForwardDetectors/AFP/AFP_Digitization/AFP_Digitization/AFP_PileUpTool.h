/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#ifndef AFP_DIGITIZATION_TOOL_H
#define AFP_DIGITIZATION_TOOL_H

#include "EventInfo/PileUpEventInfo.h"  /*SubEvent*/
#include "PileUpTools/PileUpToolBase.h"
#include "xAODEventInfo/EventInfo.h"             // NEW EDM
#include "xAODEventInfo/EventAuxInfo.h"          // NEW EDM


#include "GaudiKernel/Property.h"
#include "GaudiKernel/ServiceHandle.h"
#include "GaudiKernel/ITHistSvc.h"
#include "AthenaKernel/IAtRndmGenSvc.h"
#include "AFP_SimEv/AFP_TDSimHitCollection.h"
#include "AFP_SimEv/AFP_SIDSimHitCollection.h"
#include "HitManagement/TimedHitCollection.h"
#include "AFP_SimEv/AFP_TDSimHit.h"
#include "AFP_SimEv/AFP_SIDSimHit.h"
#include "AFP_DigiEv/AFP_TDDigiCollection.h"
#include "AFP_DigiEv/AFP_SiDigiCollection.h"
#include "xAODForward/AFPToFHit.h"
#include "xAODForward/AFPToFHitContainer.h"
#include "xAODForward/AFPSiHitContainer.h"

#include "TH1.h"
#include "TF1.h"
#include "TFile.h"
#include <string>
#include <fstream> 

#include <vector>
#include <utility> /* pair */

class IAtRndmGenSvc;
class PileUpMergeSvc;
class TH1F;

static const InterfaceID IID_IAFP_PileUpTool ("AFP_PileUpTool",1,0); //Temporary for back-compatibility with 17.3.X.Y

class AFP_PileUpTool: public PileUpToolBase {

 public:
  static const InterfaceID& interfaceID(); //Temporary for back-compatibility with 17.3.X.Y
  AFP_PileUpTool(const std::string& type,
		 const std::string& name,
		 const IInterface* parent);
  virtual ~AFP_PileUpTool();
  
  virtual StatusCode initialize() override final;
  virtual StatusCode finalize() override final;


  /// Creates xAOD for silicon detector
  StatusCode recoSiHits();

  /// Creates xAOD for time-of-flight detector
  StatusCode recoToFHits();

  StatusCode recoAll();
  
  /// called before the subevts loop. Not (necessarily) able to access SubEvents
  virtual StatusCode prepareEvent(const unsigned int nInputEvents) override final;
  
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
  virtual StatusCode mergeEvent() override final;

  virtual StatusCode processAllSubEvents() override final;

 private:
  
  StatusCode recordContainers(ServiceHandle<StoreGateSvc>& evtStore, std::string key_digitCnt);
  StatusCode recordSiCollection(ServiceHandle<StoreGateSvc>& evtStore, std::string key_SidigitCnt);

  void fillTDDigiCollection(const AFP_TDSimHitCollection*,     CLHEP::HepRandomEngine*);
  void fillTDDigiCollection(TimedHitCollection<AFP_TDSimHit>&, CLHEP::HepRandomEngine*);
  
  void fillSiDigiCollection(const AFP_SIDSimHitCollection*);
  void fillSiDigiCollection(TimedHitCollection<AFP_SIDSimHit>&);  
  
  void createTDDigi(int Station, int Detector, int SensitiveElement, float GlobalTime, float WafeLength, CLHEP::HepRandomEngine* rndEngine);
  void StoreTDDigi(void);
  
  void createSiDigi(int Station, int Detector, int PixelRow, int PixelCol, float PreStepX, float PreStepY, float PreStepZ, float PostStepX, float PostStepY, float PostStepZ, float DepEnergy);
  void StoreSiDigi(void); 

  void setupQuantumEff();
  void setupTDCOffsets();
  
  /// @brief Function that provides random noise (in charge units)
  double generateSiNoise() const;
  /// @brief Function that provides charge collection efficiency
  double generateSiCCE() const;
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
  TF1 m_totToChargeTransformation;

  ServiceHandle<PileUpMergeSvc> m_mergeSvc;
  ServiceHandle<IAtRndmGenSvc>  m_atRndmGenSvc;
  CLHEP::HepRandomEngine       *m_rndEngine;

  std::string m_TDSimHitCollectionName;
  std::string m_TDDigiCollectionName;
  
  std::string m_SIDSimHitCollectionName;
  std::string m_SiDigiCollectionName;  

  std::string m_AFPSiHitsContainerName;
  std::string m_AFPHitsContainerNameToF;

  double m_CollectionEff;
  double m_ConversionSpr;
  double m_RiseTime;
  double m_FallTime;
  double m_TofSignalTimeRangeLength;
  double m_TimeOffset;
  double m_CfSignalDelay;
  double m_CfdThr;
  double m_SiT_ChargeCollEff;
  double m_SiT_ChargeCollEffSigma;
  double m_SiT_NoiseMu;
  double m_SiT_NoiseSigma;
  double m_SiT_Energy2ChargeFactor;
  int m_SiT_ToTThresholdForHit;
  enum QEFF_VER { QE1, QE2, nQEffOpts };
  QEFF_VER m_QEffVer;

  AFP_TDSimHitCollection *m_mergedTDSimHitList;
  AFP_TDDigiCollection *m_digitCollection;
  
  AFP_SIDSimHitCollection *m_mergedSIDSimHitList;
  AFP_SiDigiCollection *m_SiDigiCollection;  
  
  xAOD::AFPToFHitContainer *m_TDHitCollection;
  xAOD::AFPSiHitContainer *m_SiHitCollection;

  double m_QuantumEff_PMT[82];
  double m_TDC_offsets[4][4][4]; // station, train, bar
  int m_ChargeVsTot_LUT[16]; // look-up table for charge2tot conversion, 16 = n. of bits

  const int m_ArrSize; // 645120 = 4 x 6 x 336 x 80
  float *m_deposited_charge; 
  float *m_deposited_energy;

  /// @brief vector with pre-calculated single signal from the photoelectron
  std::vector<double> m_SignalVect;
  
  /// @brief array of histograms storing final signals in the PMTs
  TH1F m_SignalHist[4][4][4]; // station, train, bar

protected:

/// Method that creates a new AFPToFHit and sets it valus according to #digi collection
  void newXAODHitToF (xAOD::AFPToFHitContainer* tofHitContainer, const AFP_TDDigiCollection* collection) const;
  
/// Method that creates a new AFPSiHit and sets it valus according to #digi collection
  void newXAODHitSi (xAOD::AFPSiHitContainer* xAODSiHit, const AFP_SiDigiCollection* collection) const;


};

inline const InterfaceID& AFP_PileUpTool::interfaceID() //Temporary for back-compatibility with 17.3.X.Y
{ //Temporary for back-compatibility with 17.3.X.Y
  return IID_IAFP_PileUpTool; //Temporary for back-compatibility with 17.3.X.Y
} //Temporary for back-compatibility with 17.3.X.Y
 
#endif
