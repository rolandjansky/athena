/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

//Dear emacs, this is -*-c++-*-

/** 
 * @class LArRawChannelBuilderToolOFCIter
 * @author Bernardo Resende
 * @brief Obtain ADC_max by an iterating optimal filtering
 * 
 */

#ifndef LARROD_LARRAWCHANNELBUILDERTOOLOFCITER_H
#define LARROD_LARRAWCHANNELBUILDERTOOLOFCITER_H

#include "GaudiKernel/ToolHandle.h"
#include "LArROD/LArRawChannelBuilderToolBase.h"
#include "LArRecUtils/LArOFPeakRecoTool.h"
#include "LArElecCalib/ILArPedestal.h"
#include "LArElecCalib/ILArNoise.h"
#include "LArRawEvent/LArOFIterResultsContainer.h"
#include <vector>



class LArDigit;

class LArRawChannelBuilderToolOFCIter : public LArRawChannelBuilderToolBase
{
 public:
  
  LArRawChannelBuilderToolOFCIter(const std::string& type,
			          const std::string& name,
			          const IInterface* parent);
  
  bool buildRawChannel(const LArDigit* digit,
		       float pedestal,
		       const std::vector<float>& ramps,
		       MsgStream* pLog);
  
  StatusCode initTool();

  void initEvent();
  
 private:
  const std::vector<float>& OFC_a();
  const std::vector<float>& OFC_b();
  
  //Algo-properties  //HERE used ??
  float           m_Ecut;
  bool            m_skipSaturatedCells;
  unsigned short  m_AdcMax;
  bool            m_doQual;
  bool            m_doIter;
  float           m_defaultPhase;
  unsigned int    m_minADCforIter;
  float           m_minADCforIterInSigma;
  unsigned int    m_minSample;
  unsigned int    m_maxSample;
  unsigned int    m_nIterProp;
  std::vector<float> m_OFCtmp;
  std::vector<float> m_signal; //Pedestal-subtracted
  bool            m_doMC;

  bool            m_storeTiming;
  LArOFIterResultsContainer* m_larOFIterCont;
  std::string m_timingContKey;
  ServiceHandle<StoreGateSvc> m_storeGate; //Transient event store for timinig resutls 
  // Peak reco
  ToolHandle<LArOFPeakRecoTool> m_peakReco;

  //For threshold given in pedestal rms:
  const DataHandle<ILArPedestal> m_larPedestal;
  std::string m_pedestalKey;
  const DataHandle<ILArNoise> m_larNoise;

};

#endif
