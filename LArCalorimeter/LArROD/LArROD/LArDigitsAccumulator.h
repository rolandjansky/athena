/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

/**
 * @class LArDigitsAccumulator
 * @author Sandrine Laplace <laplace@lapp.in2p3.fr>
 * @date 18-06-2008
 * @brief Emulator of DSP code in accumulation pedestal mode
 *
 *   */


#ifndef LARDIGITSACCUMULATOR
#define LARDIGITSACCUMULATOR

#include "AthenaBaseComps/AthAlgorithm.h"
#include "GaudiKernel/ToolHandle.h"
#include "LArRawEvent/LArDigitContainer.h"
#include "LArRawEvent/LArAccumulatedDigitContainer.h"
#include "LArIdentifier/LArOnlineID.h"
#include "LArRecConditions/LArCalibLineMapping.h"
#include "StoreGate/StoreGateSvc.h"

class LArDigitsAccumulator : public AthAlgorithm
{

public:
  LArDigitsAccumulator (const std::string& name, ISvcLocator* pSvcLocator);
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
    std::vector<uint32_t>     m_samplesum;
    std::vector<uint32_t>     m_matrix;
    LArAccumulated() : m_ntrigger(0) {};
  };

private:

  const LArOnlineID* m_onlineHelper;

  typedef std::vector<LArAccumulatedDigit*> ACCUMDIGIT_VEC;
  ACCUMDIGIT_VEC m_my_vec;

  SG::ReadCondHandleKey<LArCalibLineMapping> m_calibMapKey{this,"CalibMapKey","LArCalibLineMap","SG Key of calib line mapping object"};

 /** 
   * @brief LArAccumulatedDigitContainer name.
   * */
  std::string m_AccuDigitContainerName;

 /** 
   * @brief list of key for input digit container (=gain)
   * */
  std::vector<std::string> m_keylist;

 /** 
   * @brief Number of triggers per step
   * */
  unsigned int m_NtriggersPerStep;

 /** 
   * @brief Number of steps
   * */
  unsigned int m_nStepTrigger;

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
