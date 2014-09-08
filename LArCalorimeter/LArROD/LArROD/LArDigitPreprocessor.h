/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/**
 * @class LArDigitPreProcessor
 * @author Teresa Barillari barilla@mppmu.mpg.de
 * @brief Pre-processor for Digits
 *
 *   */

#ifndef LARDIGITPREPROCESSOR
#define LARDIGITPREPROCESSOR

#include "AthenaBaseComps/AthAlgorithm.h"
#include "StoreGate/StoreGateSvc.h"

class ILArDigitOscillationCorrTool;

template<class DIGITCONTAINER>
class LArDigitPreProcessor : public AthAlgorithm
{


public:

  LArDigitPreProcessor(const std::string& name, ISvcLocator* pSvcLocator);
  ~LArDigitPreProcessor();
  StatusCode initialize();
  StatusCode execute();
  StatusCode finalize();

private:
   //Algo-properties 
  std::vector< std::string > m_inputContainers;
  std::string m_outputContainer;
  short m_lowGainUpperLimit;
  short m_medGainLowerLimit, m_medGainUpperLimit;
  short m_highGainLowerLimit;
  unsigned  m_NumberOfSamples;
  int m_firstSample;
  bool m_pickGain;
  bool m_useOscillationCorrTool;
  ILArDigitOscillationCorrTool* m_corrTool;
};
#include "LArROD/LArDigitPreprocessor.icc"
#endif
