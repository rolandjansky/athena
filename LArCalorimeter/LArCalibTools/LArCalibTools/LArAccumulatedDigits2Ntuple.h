//Dear emacs, this is -*- c++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/


/*
   * @author R. Lafaye
   * 12. 06. 2008
   * Modifications:
*/

#ifndef LARACCUMULATEDDIGITS2NTUPLE_H
#define LARACCUMULATEDDIGITS2NTUPLE_H
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

class LArAccumulatedDigits2Ntuple : public Algorithm
{
 public:
  LArAccumulatedDigits2Ntuple(const std::string & name, ISvcLocator * pSvcLocator);
  ~LArAccumulatedDigits2Ntuple();

  //standard algorithm methods
  StatusCode initialize();
  StatusCode execute() ;
  StatusCode finalize(){return StatusCode::SUCCESS;}
 private:
  StoreGateSvc* m_storeGateSvc;
  const LArEM_ID* m_emId;
  const LArOnlineID* m_onlineHelper;

  ToolHandle<LArCablingService> m_larCablingSvc;

  std::string m_ntuplePath;
  std::string m_contKey;

  unsigned int  m_Nsamples;
  int m_normalize;

  int m_ipass;
  long m_event;

  NTuple::Item<long> region, layer, eta, phi;
  NTuple::Item<long> pos_neg, barrel_ec, FT, slot, channel;
  NTuple::Item<long> calibLine;
  NTuple::Item<long> m_isConnected;
  NTuple::Item<long> Ntrigger;
  NTuple::Item<long> IEvent;
  NTuple::Item<long> Nsamples;
  NTuple::Array<long>  sum;
  NTuple::Array<long>  sumsq;
  NTuple::Item<float>  mean;
  NTuple::Item<float>  rms;
  NTuple::Array<float> covr;

};

#endif
