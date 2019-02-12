//Dear emacs, this is -*- c++ -*-

/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
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

#ifndef LARDIGITSTONTUPLE_H
#define LARDIGITSTONTUPLE_H
#include "AthenaBaseComps/AthAlgorithm.h"
#include "LArIdentifier/LArOnlineID.h"
#include <string>
#include <map>

#include "CaloIdentifier/LArEM_ID.h"
#include "CaloIdentifier/LArHEC_ID.h"
#include "CaloIdentifier/LArFCAL_ID.h"
#include "LArCabling/LArOnOffIdMapping.h"
#include "StoreGate/ReadCondHandleKey.h"

#include "GaudiKernel/INTupleSvc.h"
#include "GaudiKernel/NTuple.h"

class LArDigitsToNtuple : public AthAlgorithm
{
 public:
  LArDigitsToNtuple(const std::string & name, ISvcLocator * pSvcLocator);
  ~LArDigitsToNtuple();

  //standard algorithm methods
  StatusCode initialize();
  StatusCode execute();
  StatusCode finalize();
 private:
  SG::ReadCondHandleKey<LArOnOffIdMapping>  m_cablingKey {this,"keyCabling", "LArOnOffIdMap", "Input key for Id mapping"};
  const LArOnlineID *m_onlineHelper; 
  const LArEM_ID    *m_emId;
  const LArHEC_ID   *m_hecId;
  const LArFCAL_ID  *m_fcalId;

  NTuple::Item<long> m_nt_event;
  NTuple::Item<long> m_nt_layer, m_nt_eta, m_nt_phi, m_nt_gain;
  NTuple::Item<long> m_nt_region,m_nt_barrel_ec, m_nt_pos_neg, m_nt_FT, m_nt_slot, m_nt_channel;
  NTuple::Item<long> m_nt_detector;
  NTuple::Array<long> m_nt_samples;
  NTuple::Array<long> m_nt_sca;
  NTuple::Item<double> m_nt_tdc;
  NTuple::Item<double> m_nt_ped;
  NTuple::Item<long> m_nt_trigger;
  NTuple::Item<double> m_nt_S1;
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
