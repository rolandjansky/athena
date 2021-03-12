/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#ifndef LUCID_DIGITIZATION_TOOLBOX_H
#define LUCID_DIGITIZATION_TOOLBOX_H

#include "TTree.h"
#include "GaudiKernel/Algorithm.h"
#include "GaudiKernel/ServiceHandle.h"
#include "GaudiKernel/ITHistSvc.h"
#include "GaudiKernel/MsgStream.h"
#include "HitManagement/TimedHitCollection.h"
#include "LUCID_SimEvent/LUCID_SimHitCollection.h"
#include "LUCID_RawEvent/LUCID_DigitContainer.h"

class StoreGateSvc;

class LUCID_DigitizationToolBox {

 public:
  
  LUCID_DigitizationToolBox();
  LUCID_DigitizationToolBox(int    numTubes,
			    double qdcChannelsPerPE,
			    double qdcPedestal,
			    double qdcFedNoiseFactor,
			    double tdcPmtNoiseFactor,
			    double tdcFedNoiseFactor,
			    double TotalPMTgain,
			    double AmpFactor,
			    double Q1bin,
			    int    NoiseCharge,
			    int    numDyinodes,
			    double dynodeGammaFactor,
			    std::vector<double> pmtSmearing,
			    std::vector<double> pmtScaling,
			    std::vector<double> gasScaling,
			    std::vector<double> npeThreshold,
			    bool   fillRootTree);
  
  StatusCode recordContainers(ServiceHandle<StoreGateSvc>, std::string);
  
  StatusCode fillDigitContainer(TimedHitCollection<LUCID_SimHit>&, CLHEP::HepRandomEngine*);
  StatusCode fillDigitContainer(LUCID_SimHitCollection*          , CLHEP::HepRandomEngine*);
  
  StatusCode setDebugTree(ITHistSvc*);
  
 protected:

  TTree* getDebugTree(){ return m_tubeInfo; }  
  
  unsigned int roundoff(double x);
  
  StatusCode createAndStoreDigit(unsigned short tubeID, CLHEP::HepRandomEngine* rndEngine);

  double DynodeChainSimulation(double npe, CLHEP::HepRandomEngine* rndEngine);
  double DynodeGainSmearing   (double npe, CLHEP::HepRandomEngine* rndEngine);
  
  LUCID_DigitContainer* m_digitContainer;

  int    m_numTubes;  
  double m_qdcChannelsPerPE;
  double m_qdcPedestal;
  double m_qdcFedNoiseFactor;
  double m_tdcPmtNoiseFactor;
  double m_tdcFedNoiseFactor;

  double m_TotalPMTgain;
  double m_AmpFactor;
  double m_Q1bin;
  int    m_NoiseCharge;
  int    m_numDyinodes;
  double m_dynodeGammaFactor;

  double m_gainPerDynode;
  double m_ChargeToQdcFactor;
  
  std::vector<double> m_pmtSmearing;
  std::vector<double> m_pmtScaling;
  std::vector<double> m_gasScaling;
  std::vector<double> m_npeThreshold;

  bool   m_fillRootTree;

  unsigned short m_tubeID;
  double         m_npe;
  unsigned short m_npeGas;
  unsigned short m_npePmt;
  unsigned short m_qdc;
  double         m_tdcTot;
  unsigned short m_tdc;

  TTree* m_tubeInfo;
};

#endif
