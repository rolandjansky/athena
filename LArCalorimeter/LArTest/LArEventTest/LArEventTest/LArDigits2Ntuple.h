//Dear emacs, this is -*- c++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/


/** This algrithm produces a column-wise NTuple
    out of raw data. Only the finalize
    method is used, initalize and execute are empty.
    The key of the container is given by the jobOption 
    'ContainerKey' and only a maximum of 'NSamples' 
    will be written to the ntuple.
   * @author R. Lafaye
   * 19. 1. 2005


*/

#ifndef LARDIGITS2NTUPLE_H
#define LARDIGITS2NTUPLE_H
#include "GaudiKernel/Algorithm.h"
#include "GaudiKernel/MsgStream.h"
#include  "StoreGate/StoreGateSvc.h"
#include "LArIdentifier/LArOnlineID.h"
//#include <vector>
#include <string>
#include <map>

#include "GaudiKernel/Bootstrap.h"
#include "GaudiKernel/ISvcLocator.h"
#include "GaudiKernel/IToolSvc.h"

#include "LArTools/LArCablingService.h"
#include "CaloIdentifier/LArEM_ID.h"
#include "CaloIdentifier/LArHEC_ID.h"
#include "CaloIdentifier/LArFCAL_ID.h"

#include "GaudiKernel/INTupleSvc.h"
#include "GaudiKernel/NTuple.h"

class LArDigits2Ntuple : public Algorithm
{
 public:
  LArDigits2Ntuple(const std::string & name, ISvcLocator * pSvcLocator);
  ~LArDigits2Ntuple();

  //standard algorithm methods
  StatusCode initialize();
  StatusCode execute();
  StatusCode finalize();
 private:

  StoreGateSvc *m_storeGateSvc;
  StoreGateSvc *m_detStore;
  const LArOnlineID *m_onlineHelper; 
  LArCablingService *m_larCablingSvc;
  const LArEM_ID    *m_emId;
  const LArHEC_ID   *m_hecId;
  const LArFCAL_ID  *m_fcalId;

  NTuple::Item<long> event;
  NTuple::Item<long> layer, eta, phi, gain;
  NTuple::Item<long> region,barrel_ec, pos_neg, FT, slot, channel;
  NTuple::Item<long> detector;
  NTuple::Array<long> samples;
  NTuple::Array<long> sca;
  NTuple::Item<double> tdc;
  NTuple::Item<double> ped;
  NTuple::Item<long> trigger;
  NTuple::Item<double> S1;
  NTuple::Tuple* m_nt;

  std::string m_contKey;
  int  m_nsamples;
  int m_accept, m_reject;
  int m_phase;
  int m_scint;
  int m_trigger;
  int m_sca;
  int m_ped;
};

#endif
