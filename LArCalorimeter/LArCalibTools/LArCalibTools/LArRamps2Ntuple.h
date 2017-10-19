//Dear emacs, this is -*- c++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/


/** This algrithm produces a column-wise NTuple
    out of a LArRawRampContainer. Only the finalize
    method is used, initalize and execute are empty.
    The key of the container is given by the jobOption 
    'ContainerKey'. 
   * @author W. Lampl
   * 25. 2. 2004
   * Modifications:
   * - S. Laplace: 10/04: added a few fields (max sample, etc...)
   * - S. Laplace: 03/05: added individual samples (on demand, driven by jobOption)
*/

#ifndef LARRAMPS2NTUPLE_H
#define LARRAMPS2NTUPLE_H
#include "LArCalibTools/LArCond2NtupleBase.h"
#include "StoreGate/ReadCondHandleKey.h"
#include "LArElecCalib/ILArRamp.h"
#include "LArRawConditions/LArRampSym.h"

class LArRamps2Ntuple : public LArCond2NtupleBase 
{
 public:
  LArRamps2Ntuple(const std::string & name, ISvcLocator * pSvcLocator);
  ~LArRamps2Ntuple();

  //standard algorithm methods
  StatusCode initialize();
  virtual StatusCode stop();
  StatusCode finalize(){return StatusCode::SUCCESS;}

 private:
  bool m_rawRamp;
  bool m_saveAllSamples;
  bool m_applyCorr;
  bool m_addCorrUndo;
  std::vector<std::string> m_contKey;
  SG::ReadCondHandleKey<ILArRamp> m_rampKey;
  std::string m_ntName;
  bool m_isMC;

};

#endif
