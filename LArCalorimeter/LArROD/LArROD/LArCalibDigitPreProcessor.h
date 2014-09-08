/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/


/**
 * @class LArCalibDigitPreProcessor
 * @author Rob McPherson rmcphers@triumf.ca
 * @brief Pre-processor for Calib digits
 *
 *   */


#ifndef LARCALIBDIGITPREPROCESSOR
#define LARCALIBDIGITPREPROCESSOR

#include "AthenaBaseComps/AthAlgorithm.h"
#include "StoreGate/StoreGateSvc.h"

template<class CALIBDIGITCONTAINER>
class LArCalibDigitPreProcessor : public AthAlgorithm
{

public:

  LArCalibDigitPreProcessor(const std::string& name, ISvcLocator* pSvcLocator);
  ~LArCalibDigitPreProcessor();
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
};
#include "LArROD/LArCalibDigitPreProcessor.icc"
#endif
