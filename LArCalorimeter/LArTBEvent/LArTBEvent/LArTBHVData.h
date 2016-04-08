/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef LARTBEVENT_LARTBHVDATA_H
#define LARTBEVENT_LARTBHVDATA_H

#include <vector>
#include <string>

/** @class LArTBHVData LArTBHVData.h "LArTBEvent/LArTBHVData.h"
    @author A. Abdesselam and P. Ghez
*/
class LArTBHVData {
public:

  /** Standrad Constructor */
  LArTBHVData(int moduleNumber, short detectorType, short unit, int nHVch, 
	      std::vector<float> HVdata);
  
  /** Destructor */
  virtual ~LArTBHVData();

  /* ------------------------------------------------------------  */
  /* ------------------------ G E T T E R ------------------------ */
  /* ------------------------------------------------------------  */  

  /** Get HV Module number */
  inline int getModuleNumber() const { return m_moduleNumber; }

  /** Get detector type in which HV is supplied from this HV module */
  inline short getDetectorType() const { return m_detectorType; }

  /** Get unit of m_HVdata (0= micro ampere 1= voltage */
  inline short getUnit() const { return m_unit; }

  /** Get the number of HV channels */
  inline int getNHVch() const { return m_nHVch; }

  /** Get the collection of HV data */
  inline std::vector<float> getHVdata() const { return m_HVdata; }
  
  /**  Convertion operator to a std::string  */
  virtual operator std::string() const;
  
 private:
  LArTBHVData();
  
  int m_moduleNumber;   /**< HV module number */
  short m_detectorType; /**< Detector type (0=Calorimeter 1=PMT 2=Chamber) */
  short m_unit;         /**< Unit of HV data (0=current, 1=voltage) */
  int m_nHVch;          /**< The number of hv channels used in the module */
  std::vector<float> m_HVdata; /**< HV data  (uA or V) */

};
#endif // LARTBEVENT_LARTBSlOWCONTROL_H


