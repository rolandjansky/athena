/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#ifndef AFP_DIGITIZATION_TOOL_H
#define AFP_DIGITIZATION_TOOL_H

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
#include "PileUpTools/PileUpMergeSvc.h"
#include "TMath.h"
#include "TH1.h"
#include "TF1.h"
#include "TFile.h"
#include <string>
#include <fstream> 

#include <vector>
#include <utility> /* pair */

class IAtRndmGenSvc;
// class PileUpMergeSvc;

static const InterfaceID IID_IAFP_PileUpTool ("AFP_PileUpTool",1,0); //Temporary for back-compatibility with 17.3.X.Y

class AFP_PileUpTool: public PileUpToolBase {

 public:
  static const InterfaceID& interfaceID(); //Temporary for back-compatibility with 17.3.X.Y
  AFP_PileUpTool(const std::string& type,
		 const std::string& name,
		 const IInterface* parent);
  
  virtual StatusCode initialize() override final;
  virtual StatusCode finalize() override final;

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
  

  double SignalFun(double Time, double RiseTime, double FallTime);
//  double SiSignalFun(double Time, double RiseTime, double FallTime);

  ServiceHandle<PileUpMergeSvc> m_mergeSvc;
  ServiceHandle<IAtRndmGenSvc>  m_atRndmGenSvc;
  CLHEP::HepRandomEngine       *m_rndEngine;

  std::string m_TDSimHitCollectionName;
  std::string m_TDDigiCollectionName;
  
  std::string m_SIDSimHitCollectionName;
  std::string m_SiDigiCollectionName;  

  double m_CollectionEff;
  double m_ConversionSpr;
  double m_Gain;
  double m_RiseTime;
  double m_FallTime;
  double m_CfdThr; 

  AFP_TDSimHitCollection *m_mergedTDSimHitList;
  AFP_TDDigiCollection *m_digitCollection;
  
  AFP_SIDSimHitCollection *m_mergedSIDSimHitList;
  AFP_SiDigiCollection *m_SiDigiCollection;  
  
     
  double m_QuantumEff_PMT[7];

  TH1F  m_Signal[4][32][2];
  
  float m_deposited_charge[645120]; // = 4 x 6 x 336 x 80
  float m_deposited_energy[645120];

  std::vector<double> m_SignalVect;

};

inline const InterfaceID& AFP_PileUpTool::interfaceID() //Temporary for back-compatibility with 17.3.X.Y
{ //Temporary for back-compatibility with 17.3.X.Y
  return IID_IAFP_PileUpTool; //Temporary for back-compatibility with 17.3.X.Y
} //Temporary for back-compatibility with 17.3.X.Y
 
#endif
