/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef LARTBEVENT_LARTBSLOWCONTROL_H
#define LARTBEVENT_LARTBSLOWCONTROL_H
#include "AthContainers/DataVector.h"
#include "SGTools/CLASS_DEF.h"

#include <vector>

/** @class LArTBSlowControl LArTBSlowControl.h "LArTBEvent/LArTBSlowControl.h"
    @author A. Abdesselam and P. Ghez
*/
class LArTBSlowControl {
 public:
  
  /** Standard Constructor */
  LArTBSlowControl(int cryo_pressure, float ADC_temperature,
		   std::vector<int> FEBtemperature);
  
  /** Destructor */
  ~LArTBSlowControl();
  
  /* ------------------------------------------------------------  */
  /* ------------------------ G E T T E R ------------------------ */
  /* ------------------------------------------------------------  */  
  
  /** Get Cryostat pressure */
  inline int getCryo_pressure() const { return m_cryo_pressure;}

  /** Get ADC temperature */
  inline float getADC_temperature() const { return m_ADC_temperature;}

  /**  Get FEB temperatures  */
  inline const std::vector<int> & getFEBtemperature()
    const { return m_FEB_temperature; }
  
 private:
  LArTBSlowControl();
  
  int m_cryo_pressure;                /**< Pressure of LAr vapor (mbar) */
  float m_ADC_temperature;            /**< ADC crate temperature (Celsius) */
  std::vector<int> m_FEB_temperature; /**< FEB temperatures */

};
CLASS_DEF(LArTBSlowControl,2787,0)
#endif // LARTBEVENT_LARTBSlOWCONTROL_H


