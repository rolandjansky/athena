//Dear emacs, this is -*- c++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/


/** Common base-class for wave-to-ntuple algorithms.
  * Derives from LArCond2NtupleBase and is a base-class
  * for LArCaliWave2Ntuple and LArPhysWave2Ntuple.
  * @author W. Lampl
  * @author M. Delmastro
  * Sept 22, 2008
  */

#ifndef LARWAVES2NTUPLE_H
#define LARWAVES2NTUPLE_H
#include "LArCalibTools/LArCond2NtupleBase.h"
#include "LArRawConditions/LArWaveHelper.h"

//class LArWaveHelper;
class LArWaveCumul;

class LArWaves2Ntuple : public  LArCond2NtupleBase
{

 public:

  StatusCode initialize();

  LArWaves2Ntuple(const std::string & name, ISvcLocator * pSvcLocator);
  ~LArWaves2Ntuple();

  bool fillWave(const HWIdentifier chid, const LArWaveCumul& wave); //returns true if connected

 private:
  bool m_saveDerivedInfo;

  NTuple::Item<long>    m_flag;
  NTuple::Item<long>    m_timeIndex;
  NTuple::Item<double>  m_dt;
  NTuple::Array<double> m_time;
  NTuple::Array<double> m_ampliPulse;
  NTuple::Array<double> m_errorPulse;
  NTuple::Array<int>   m_triggers;

  // derived information
  NTuple::Item<double>  m_tmaxAmp;
  NTuple::Item<double>  m_maxAmp;
  NTuple::Item<double>  m_baseline;
  NTuple::Item<double>  m_posLobe;
  NTuple::Item<double>  m_rT0;
  NTuple::Item<double>  m_width;

protected:
  LArWaveHelper m_waveHelper;

};

#endif
