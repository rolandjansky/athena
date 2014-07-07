//Dear emacs, this is -*- c++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/


/** This algrithm produces a column-wise NTuple out of a
    LArPhysWaveContainer 
  * @author M. Delmastro
  * 01. 06. 2004 
  * Last update:
  * - M. Delmastro: 19/4/2008, migrate to LArCond2NtupleBase,
  * added computation and storing of Wave-derived information.
  * - W. Lampl: 22/9/2008, introduce common base-class LArWaves2Ntuple
  */

#ifndef LARPHYSWAVES2NTUPLE_H
#define LARPHYSWAVES2NTUPLE_H
#include "LArCalibTools/LArWaves2Ntuple.h"

#include "LArRawConditions/LArPhysWaveContainer.h"

#include "LArRawConditions/LArWaveHelper.h"

class LArPhysWaves2Ntuple : public  LArWaves2Ntuple
{

 public:

  LArPhysWaves2Ntuple(const std::string & name, ISvcLocator * pSvcLocator);
  ~LArPhysWaves2Ntuple();

  //standard algorithm methods
  StatusCode initialize();
  virtual StatusCode stop();
  StatusCode finalize(){return StatusCode::SUCCESS;}

 private:
  bool writeEntry(const HWIdentifier chid,  const unsigned gain, const LArPhysWave& wave); 
  
  std::vector<std::string> m_keylist ; 
  std::string m_ntName;
  bool m_addCorrUndo, m_applyCorr;

  NTuple::Item<long>    m_dac;
  NTuple::Item<long>    m_gain;
  NTuple::Item<long>    m_corrUndo;
  NTuple::Item<float>  m_timeOffset;
};

#endif
