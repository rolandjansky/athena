/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#ifndef ALFA_PILEUP_TOOL_H
#define ALFA_PILEUP_TOOL_H

#include "PileUpTools/PileUpToolBase.h"

#include "GaudiKernel/Property.h"
#include "GaudiKernel/ServiceHandle.h"
#include "GaudiKernel/ITHistSvc.h"
#include "AthenaKernel/IAtRndmGenSvc.h"

#include "HitManagement/TimedHitCollection.h"

#include "ALFA_SimEv/ALFA_HitCollection.h"
#include "ALFA_SimEv/ALFA_ODHitCollection.h"

#include "ALFA_RawEv/ALFA_DigitCollection.h"
#include "ALFA_RawEv/ALFA_ODDigitCollection.h"

#include "PileUpTools/PileUpMergeSvc.h"

#include <string>
#include <iostream>
#include <fstream> 

#include <vector>
#include <utility> /* pair */

class IAtRndmGenSvc;

class ALFA_PileUpTool: public PileUpToolBase {

 public:
  
  ALFA_PileUpTool(const std::string& type,
		 const std::string& name,
		 const IInterface* parent);
  
  virtual StatusCode initialize() override final;
  virtual StatusCode finalize() override final;
  
  /// code taken from ZDC; author (highly) probably John Chapman
  /// called before the subevts loop. Not (necessarily) able to access SubEvents
  virtual StatusCode prepareEvent(const unsigned int nInputEvents) override final;
  
  /// called for each active bunch-crossing to process current SubEvents bunchXing is in ns
  virtual  StatusCode processBunchXing(
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
  
  /*
  StatusCode recordContainers(ServiceHandle<StoreGateSvc>& evtStore, std::string m_key_digitCnt);

  void fillStripDigitContainer(const ZDC_SimStripHit_Collection*,    CLHEP::HepRandomEngine*);
  void fillStripDigitContainer(TimedHitCollection<ZDC_SimStripHit>&, CLHEP::HepRandomEngine*);

  void fillPixelDigitContainer(const ZDC_SimPixelHit_Collection*,    CLHEP::HepRandomEngine*);
  void fillPixelDigitContainer(TimedHitCollection<ZDC_SimPixelHit>&, CLHEP::HepRandomEngine*);
  
  void createAndStoreStripDigit(int Side, int ModuleNo, int NPhotons, CLHEP::HepRandomEngine* rndEngine);
  void createAndStorePixelDigit(int Side, int ModuleNo, int PixNum, int NPhotons, CLHEP::HepRandomEngine* rndEngine);
  
  */
      
   //StatusCode Truth_info();
   
   StatusCode recordCollection(ServiceHandle<StoreGateSvc>& evtStore, std::string key_digitCnt);
   //StatusCode fillDigitCollection();
   
   void ALFA_MD_info(const ALFA_HitCollection*);
   void ALFA_MD_info(TimedHitCollection<ALFA_Hit>&);
    
   //void fillDigitCollection(const ALFA_HitCollection*, CLHEP::HepRandomEngine*);
   //void fillDigitCollection(TimedHitCollection<ALFA_Hit>&, CLHEP::HepRandomEngine*);
   
   StatusCode fill_MD_DigitCollection(CLHEP::HepRandomEngine*);     

   StatusCode recordODCollection(ServiceHandle<StoreGateSvc>& evtStore, std::string key_ODdigitCnt);
   //StatusCode fillODDigitCollection();
   
   void ALFA_OD_info(const ALFA_ODHitCollection*);
   void ALFA_OD_info(TimedHitCollection<ALFA_ODHit>&);
     
   //void fillDigitODCollection(const ALFA_ODHitCollection*, CLHEP::HepRandomEngine*);
   //void fillDigitODCollection(TimedHitCollection<ALFA_ODHit>&, CLHEP::HepRandomEngine*); 
   
   StatusCode fill_OD_DigitCollection(CLHEP::HepRandomEngine*);
   StatusCode XTalk(); 
  
  /*
  void SetDumps(bool, bool);
  */
  
  ServiceHandle<PileUpMergeSvc> m_mergeSvc;
  ServiceHandle<IAtRndmGenSvc>  m_atRndmGenSvc;
  CLHEP::HepRandomEngine       *m_rndEngine;

  double m_E_fib[8][20][64];
  double m_E_ODfib[8][2][3][30];

  /*
  std::string m_SimStripHitCollectionName;
  std::string m_SimPixelHitCollectionName;
  std::string m_ZdcDigitsContainerName;
  */
  
  std::string m_SimHitCollectionName;
  std::string m_SimODHitCollectionName;
  std::string m_key_DigitCollection;
  std::string m_key_ODDigitCollection;
  
  
  ALFA_DigitCollection* m_digitCollection;
   
  ALFA_ODDigitCollection* m_ODdigitCollection;
  
  /*
  double m_ScaleStrip, m_ScalePixel; // use this to normalize the signal from the strips;
  int    m_Pedestal;                 // Pedestal=40 ;RMS=3??(ask Andrei about this)
  int    m_MaxTimeBin;               // The number of time-slices after digitization(Typically 5 or 7)
  double m_SigmaNoiseHG_Strip, m_SigmaNoiseHG_Pixel, m_SigmaNoiseLG_Strip, m_SigmaNoiseLG_Pixel;
  bool   m_DumpStrip, m_DumpPixel;

  */
  
  double m_sigma0;          
  double m_sigma1;          
  double m_meanE_dep;       
  double m_meanN_photo;      
  double m_mean;             
  double m_stdDev;
  double m_AmplitudeCut;   
  
  /*
  ZDC_SimStripHit_Collection *m_mergedStripHitList;
  ZDC_SimPixelHit_Collection *m_mergedPixelHitList;
   
  std::ofstream MyFile;
  ZdcDigitsCollection *m_digitContainer;
  */
  
  ALFA_HitCollection      *m_mergedALFA_HitList;
  ALFA_ODHitCollection    *m_mergedALFA_ODHitList;
  
  double m_cross_talk[8][127];
  int m_fibres[8][20][64];
		
  std::ifstream m_fXTalk;
  std::stringstream m_s;
  std::string m_filename;
  
};
 
#endif
