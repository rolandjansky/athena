/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

/**
 * @class LArCalibDigitsAccumulator
 * @version \$Id: LArCalibDigitsAccumulator.h,v 1.8 2008-06-12 14:51:35 lafaye Exp $
 * @author Sandrine Laplace <laplace@lapp.in2p3.fr>
 * @date 22-January-2006
 * @brief Emulator of DSP code in accumulation calibration mode
 *
 * Input to this algorithm are LArCalibDigits. These LArCalibDigits are accumulated
 * (sum and squared sum)over a number of triggers given by the calibration run settings. 
 * Intermediate sums are also computed if the JO property "StepOfTriggers" is set to 
 * a larger number than one. The output of the algorithm is a LArAccumulatedCalibDigitContainer.
 *   */


#ifndef LARCALIBDIGITSACCUMULATOR
#define LARCALIBDIGITSACCUMULATOR

#include "AthenaBaseComps/AthAlgorithm.h"
#include "GaudiKernel/ToolHandle.h"
#include "LArRawEvent/LArCalibDigitContainer.h"
#include "LArRawEvent/LArAccumulatedCalibDigitContainer.h"
#include "LArIdentifier/LArOnlineID.h"
#include "StoreGate/StoreGateSvc.h"
#include "LArRawConditions/LArCalibParams.h"
 #include "LArRecConditions/LArCalibLineMapping.h"

class LArCalibDigitsAccumulator : public AthAlgorithm
{

public:
  LArCalibDigitsAccumulator (const std::string& name, ISvcLocator* pSvcLocator);
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

  SG::ReadCondHandleKey<LArCalibLineMapping> m_calibMapKey{this,"CalibCablingKey","LArCalibLineMap","SG Key of LArCalibLineMapping object"};
  const LArOnlineID* m_onlineHelper;

  typedef std::vector<LArAccumulatedCalibDigit*> ACCUMDIGIT_VEC;
  ACCUMDIGIT_VEC m_my_vec;

 /** 
   * @brief Store delay.
   * */
  int m_delay;

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
   * @brief Vector (index=hash ID) of accumulation quantities
   * */
  std::vector<LArAccumulated> m_Accumulated;

 /** 
   * @brief Event counter
   * */
  unsigned int m_event_counter;

};

#endif
