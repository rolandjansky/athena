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
#include "AthenaBaseComps/AthAlgorithm.h"
#include "GaudiKernel/MsgStream.h"
#include "StoreGate/StoreGateSvc.h"
#include "GaudiKernel/INTupleSvc.h"
#include "GaudiKernel/NTuple.h"
#include "GaudiKernel/SmartDataPtr.h"
#include "GaudiKernel/Bootstrap.h"
#include "GaudiKernel/ISvcLocator.h"
#include "GaudiKernel/IToolSvc.h"
#include "GaudiKernel/ToolHandle.h"

#include "LArCabling/LArCablingService.h"
#include "LArCabling/LArSuperCellCablingTool.h"
#include "CaloIdentifier/LArEM_ID.h"
#include "CaloIdentifier/CaloIdManager.h"
#include "LArIdentifier/LArOnlineID.h"
#include "LArIdentifier/LArOnline_SuperCellID.h"

#include <fstream>
#include <math.h>
#include <string>
#include <map>

class LArAverages2Ntuple : public AthAlgorithm
{
 public:
  LArAverages2Ntuple(const std::string & name, ISvcLocator * pSvcLocator);
  ~LArAverages2Ntuple();

  //standard algorithm methods
  StatusCode initialize();
  StatusCode execute() ;
  StatusCode finalize(){return StatusCode::SUCCESS;}
 private:
  const LArEM_Base_ID* m_emId;
  const LArOnlineID_Base* m_onlineHelper;

  //ToolHandle<LArCablingService> larCablingSvc;
  LArCablingBase* m_larCablingSvc;
  
  bool m_isSC;

  std::string m_ntuplePath;
  std::string m_contKey;

  unsigned int  m_Nsamples;
  bool m_keepPulsed;

  int m_ipass;

  NTuple::Item<long> m_region, m_layer, m_eta, m_phi, m_onlChanId;
  NTuple::Item<long> m_pos_neg, m_barrel_ec, m_FT, m_slot, m_channel;
  NTuple::Item<long> m_calibLine;
  NTuple::Item<long> m_isConnected;
  NTuple::Item<long> m_Nsteps;
  NTuple::Item<long> m_DAC;
  NTuple::Item<long> m_Ntrigger;
  NTuple::Item<long> m_delay;
  NTuple::Item<long> m_ntNsamples;
  NTuple::Item<long> m_isPulsed;
  
  NTuple::Item<unsigned long> m_StepIndex;
  
  NTuple::Array<long> m_Sum;
  NTuple::Array<long> m_SumSq;
  NTuple::Array<float> m_Mean;
  NTuple::Array<float> m_RMS;
};

#endif
