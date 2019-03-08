/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#ifndef ZDC_DIGITIZATION_TOOL_H
#define ZDC_DIGITIZATION_TOOL_H

#include "PileUpTools/PileUpToolBase.h"
#include "GaudiKernel/Property.h"
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
  
  StatusCode recordContainers(ServiceHandle<StoreGateSvc>& evtStore, std::string m_key_digitCnt);

  void fillStripDigitContainer(const ZDC_SimStripHit_Collection*,    CLHEP::HepRandomEngine*);
  void fillStripDigitContainer(TimedHitCollection<ZDC_SimStripHit>&, CLHEP::HepRandomEngine*);

  void fillPixelDigitContainer(const ZDC_SimPixelHit_Collection*,    CLHEP::HepRandomEngine*);
  void fillPixelDigitContainer(TimedHitCollection<ZDC_SimPixelHit>&, CLHEP::HepRandomEngine*);
  
  void createAndStoreStripDigit(int Side, int ModuleNo, int NPhotons, CLHEP::HepRandomEngine* rndEngine);
  void createAndStorePixelDigit(int Side, int ModuleNo, int PixNum, int NPhotons, CLHEP::HepRandomEngine* rndEngine);
  
  void SetDumps(bool, bool);

  ServiceHandle<PileUpMergeSvc> m_mergeSvc;
  ServiceHandle<IAtRndmGenSvc>  m_atRndmGenSvc;
  CLHEP::HepRandomEngine       *m_rndEngine;

  std::string m_SimStripHitCollectionName;
  std::string m_SimPixelHitCollectionName;
  std::string m_ZdcDigitsContainerName;

  double m_ScaleStrip, m_ScalePixel; // use this to normalize the signal from the strips;
  int    m_Pedestal;                 // Pedestal=40 ;RMS=3??(ask Andrei about this)
  int    m_MaxTimeBin;               // The number of time-slices after digitization(Typically 5 or 7)
  double m_SigmaNoiseHG_Strip, m_SigmaNoiseHG_Pixel, m_SigmaNoiseLG_Strip, m_SigmaNoiseLG_Pixel;
  bool   m_DumpStrip, m_DumpPixel;

  ZDC_SimStripHit_Collection *m_mergedStripHitList;
  ZDC_SimPixelHit_Collection *m_mergedPixelHitList;
   
  std::ofstream m_MyFile;
  ZdcDigitsCollection *m_digitContainer;
  
  double HighToLow(double signal, double gain_ratio, double gain_error, CLHEP::HepRandomEngine* rndEngine); 
  
  double m_GainRatio_Strip[8], m_GainRatioError_Strip[8];
  double m_GainRatio_Pixel,    m_GainRatioError_Pixel;

  unsigned int PixelID(int Side=-1, int Module=-1, int PixNo=-1); 
};
 
#endif
