//Dear emacs, this is -*- c++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/


/** This algrithm produces a column-wise NTuple out of a
    LArCaliWaveContainer or a LArPhysWaveContainer 
  * @author M. Delmastro
  * 01. 06. 2004 
  * Last update:
  * - M. Delmastro: 19/4/2008, migrate to LArCond2NtupleBase,
  * added computation and storing of Wave-derived information.
  * - W. Lampl: 22/9/2008, introduce common base-class LArWaves2Ntuple
*/

#ifndef LARCALIWAVES2NTUPLE_H
#define LARCALIWAVES2NTUPLE_H
#include "LArCalibTools/LArWaves2Ntuple.h"

#include "LArRawConditions/LArCaliWaveContainer.h"

#include "LArRawConditions/LArWaveHelper.h"

class LArCaliWaves2Ntuple : public  LArWaves2Ntuple
{

 public:

  LArCaliWaves2Ntuple(const std::string & name, ISvcLocator * pSvcLocator);
  ~LArCaliWaves2Ntuple();

  //standard algorithm methods
  StatusCode initialize();
  virtual StatusCode stop();
  StatusCode finalize(){return StatusCode::SUCCESS;}

 private:
  bool writeEntry(const HWIdentifier chid,  const unsigned gain, const LArCaliWave& wave); 
  
  std::vector<std::string> m_keylist ; 
  std::string m_ntName;
  std::string m_ntFile;   
  bool m_dacSaturSkip;
  bool m_saveJitter;
  bool m_addCorrUndo;
  bool m_applyCorr;

  std::vector<unsigned> m_dacSaturLayer0;
  std::vector<unsigned> m_dacSaturLayer1;
  std::vector<unsigned> m_dacSaturLayer2;
  std::vector<unsigned> m_dacSaturLayer3;

   // Book specialized leaves 
  NTuple::Item<long>    m_dac;
  NTuple::Item<long>    m_gain;
  NTuple::Item<long>    m_nPulsedCalibLines;
  NTuple::Array<int>   m_pulsedCalibLines;
  NTuple::Item<double>  m_jitter;
  NTuple::Item<long>    m_corrUndo;

};

#endif
