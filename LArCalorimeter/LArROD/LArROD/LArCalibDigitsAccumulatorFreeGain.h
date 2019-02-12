/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

/**************************************************

 Class LArCalibDigitsAccumulatorFreeGain
 Author: Jean-Francois Marchand from the LArCalibDigitsAccumulator class

 As the class LArCalibDigitsAccumulator, emulates the DSP code in accumulation 
 calibration mode, but the three different gains are separated (useful for
 free gain runs)

**************************************************/
 
  

#ifndef LARCALIBDIGITSACCUMULATORFREEGAIN
#define LARCALIBDIGITSACCUMULATORFREEGAIN

#include "AthenaBaseComps/AthAlgorithm.h"
#include "GaudiKernel/ToolHandle.h"
#include "LArRawEvent/LArCalibDigitContainer.h"
#include "LArRawEvent/LArAccumulatedCalibDigitContainer.h"
#include "LArIdentifier/LArOnlineID.h"
#include "StoreGate/StoreGateSvc.h"
#include "LArRawConditions/LArCalibParams.h"
#include "LArRecConditions/LArCalibLineMapping.h"

class LArCalibDigitsAccumulatorFreeGain : public AthAlgorithm
{

public:
  LArCalibDigitsAccumulatorFreeGain (const std::string& name, ISvcLocator* pSvcLocator);
  StatusCode initialize();
  StatusCode execute();
  StatusCode finalize(){return StatusCode::SUCCESS;}

 /** 
   * @brief Class of intermediate accumulations.
   *
   * The first index of the vector of vector is the step of intermediate
   * accumulation (0 is for total accumulation, i>0 for intermediate 
   * accumulations), while the second index is for the sample.  */
  class LArAccumulated{
  public:
    unsigned int m_ntrigger;
    std::vector<unsigned int> m_sum;
    std::vector<unsigned int> m_sum2;
    LArAccumulated() : m_ntrigger(0) {};
  };

private:
  SG::ReadCondHandleKey<LArCalibLineMapping> m_calibMapKey{this,"CalibMapKey","LArCalibLineMap","SG Key of calib line mapping object"};
  const LArOnlineID* m_onlineHelper;


 /** 
   * @brief Store delay.
   * */
  int m_delay;
 /** 
   * @brief Set wether a cell is pulsed or not.
   * */
  bool m_isPulsed;

 /** 
   * @brief LArAccumulatedCalibDigitContainer name.
   * */
  std::string m_calibAccuDigitContainerName;

 /** 
   * @brief list of key for input digit container (=gain)
   * */
  std::vector<std::string> m_keylist;

 /** 
   * @brief Number of intermediate accumulations (JO property)
   * */
  unsigned int m_nStepTrigger;

 /** 
   * @brief Set delay scale.
   * */
  double m_delayScale;

 /** 
   * @brief Tells you wether you keep only pulsed cells or all cells
   * */
  bool m_keepPulsed;

 /** 
   * @brief Stores number of triggers per intermediate step
   * */
  //unsigned int m_nTriggerPerStep;

 /** 
   * @brief Vector (index=hash ID) of accumulation quantities
   * */
  std::vector<LArAccumulated> m_AccumulatedHG;
  std::vector<LArAccumulated> m_AccumulatedMG;
  std::vector<LArAccumulated> m_AccumulatedLG;

 /** 
   * @brief Event counter
   * */
  unsigned int m_event_counter;

};

#endif
