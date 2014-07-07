//Dear emacs, this is -*- c++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/


/*
   * @author S. Laplace
   * 14. 12. 2005
   * Modifications:
*/

#ifndef LARAVERAGES2NTUPLE_H
#define LARAVERAGES2NTUPLE_H
#include "GaudiKernel/Algorithm.h"
#include "GaudiKernel/MsgStream.h"
#include "StoreGate/StoreGateSvc.h"
#include "GaudiKernel/INTupleSvc.h"
#include "GaudiKernel/NTuple.h"
#include "GaudiKernel/SmartDataPtr.h"
#include "GaudiKernel/Bootstrap.h"
#include "GaudiKernel/ISvcLocator.h"
#include "GaudiKernel/IToolSvc.h"
#include "GaudiKernel/ToolHandle.h"

#include "LArTools/LArCablingService.h"
#include "CaloIdentifier/LArEM_ID.h"
#include "CaloIdentifier/CaloIdManager.h"
#include "LArIdentifier/LArOnlineID.h"

#include <fstream>
#include <math.h>
#include <string>
#include <map>

class LArAverages2Ntuple : public Algorithm
{
 public:
  LArAverages2Ntuple(const std::string & name, ISvcLocator * pSvcLocator);
  ~LArAverages2Ntuple();

  //standard algorithm methods
  StatusCode initialize();
  StatusCode execute() ;
  StatusCode finalize(){return StatusCode::SUCCESS;}
 private:
  StoreGateSvc* m_storeGateSvc;
  StoreGateSvc* m_detStore; 
  const LArEM_ID* m_emId;
  const LArOnlineID* m_onlineHelper;

  ToolHandle<LArCablingService> larCablingSvc;

  std::string ntuplePath;
  std::string m_contKey;

  unsigned int  m_Nsamples;
  bool m_keepPulsed;

  int ipass;

  NTuple::Item<long> region, layer, eta, phi;
  NTuple::Item<long> pos_neg, barrel_ec, FT, slot, channel;
  NTuple::Item<long> calibLine;
  NTuple::Item<long> m_isConnected;
  NTuple::Item<long> Nsteps;
  NTuple::Item<long> DAC;
  NTuple::Item<long> Ntrigger;
  NTuple::Item<long> delay;
  NTuple::Item<long> Nsamples;
  NTuple::Item<long> isPulsed;
  
  NTuple::Item<unsigned long> StepIndex;  
  
  NTuple::Array<long> Sum;
  NTuple::Array<long> SumSq;
  NTuple::Array<float> Mean;
  NTuple::Array<float> RMS;
};

#endif
