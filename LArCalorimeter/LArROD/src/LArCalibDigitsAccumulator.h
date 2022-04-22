/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
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
#include "LArIdentifier/LArOnlineID_Base.h"
#include "StoreGate/StoreGateSvc.h"
#include "LArRawConditions/LArCalibParams.h"
#include "LArRecConditions/LArCalibLineMapping.h"
#include "LArCabling/LArOnOffIdMapping.h"
#include "CaloDetDescr/ICaloSuperCellIDTool.h"
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
    std::vector< uint64_t > m_sum;
    std::vector< uint64_t > m_sum2;
    LArAccumulated() : m_ntrigger(0) {};
  };

private:


  ToolHandle<ICaloSuperCellIDTool> m_sc2ccMappingTool;

  SG::ReadCondHandleKey<LArCalibLineMapping> m_calibMapKey{this,"CalibCablingKey","LArCalibLineMap","SG Key of LArCalibLineMapping object"};


  SG::ReadCondHandleKey<LArCalibLineMapping> m_calibMapSCKey{this,"CalibMapSCKey","LArCalibIdMapSC","SG Key of calib line mapping object"};


  SG::ReadCondHandleKey<LArOnOffIdMapping> m_cablingKey{this, "OnOffMap", "LArOnOffIdMap", "SG key for mapping object"};
  SG::ReadCondHandleKey<LArOnOffIdMapping> m_cablingKeySC{this,"ScCablingKey","LArOnOffIdMapSC","SG Key of SC LArOnOffIdMapping object"};

  const LArOnlineID_Base* m_onlineHelper;

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
   * @brief Tells you wether you run on SuperCells or Cells
   * */
  bool m_isSC;

 /** 
   * @brief Tells you wether you keep only fully pulsed supercells or all supercells
   * */
  bool m_keepFullyPulsedSC;


 /** 
   * @brief Vector (index=hash ID) of accumulation quantities
   * */
  std::vector<LArAccumulated> m_Accumulated;

 /** 
   * @brief Event counter
   * */
  unsigned int m_event_counter;
  unsigned int m_eventNb;

  // Information to remove certain readings if needed
  std::map<std::string, std::map<int,std::vector<int>*>*>           m_readingsMap;
  std::map<std::string, std::map<int,std::vector<int>*>*>::iterator m_readingsMap_it;
  std::map<int, std::vector<int>*>::iterator                        m_channelMap_it;


  int m_sampleShift;
};

#endif
