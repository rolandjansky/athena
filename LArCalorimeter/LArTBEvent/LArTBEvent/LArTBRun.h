/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef LARTBEVENT_LARTBRUN_H
#define LARTBEVENT_LARTBRUN_H

#include "AthContainers/DataVector.h"
#include "SGTools/CLASS_DEF.h"
/** @class

    @author A. Abdesselam and P. Ghez
*/
class LArTBRun {
 public:

  /** Constructor for EMEC */
  LArTBRun(int type, int runNo, int asynchronous, float temperature);
  
  /** Constructor for HEC-EMC (online) */
  LArTBRun(int type, int runNo, int asynchronous, float temperature,
	   short particleID, float ebeam, float cryoX, float tableY);
  
  ~LArTBRun();
  
  /* ------------------------------------------------------------  */
  /* ------------------------ G E T T E R ------------------------ */
  /* ------------------------------------------------------------  */  

  /** Get run type : m_type */
  inline int getType() const { return m_type; }

  /** Get run number */
  inline int getRunNumber() const { return m_runNumber; }

  /** Get asynchronous flag */
  inline int getAsynchronous() const { return m_asynchronous; }

  /** Get argon temperature */
  inline float getTemperature() const { return m_temperature; }

  /** Get particle ID (also available via LArBookkeeping) */
  inline short getParticleID() const { return m_particleID;}

  /** Get beam energy (also available via LArBookkeeping) */
  inline float getEbeam()  const { return m_ebeam;}

  /** Get impact position X (also available via LArBookkeeping) */
  inline float getCryoX()  const { return m_cryoX; }

  /** Get impact position Y (also available via LArBookkeeping) */
  inline float getTableY() const { return m_tableY;}
  
 private:
  LArTBRun();
  
  int m_type;          /**<  pedestal=0, TP=1, beamch=2, physics=3 */
  int m_runNumber;     /**<  run number */
  int m_asynchronous;  /**<  =1 when L1 is asynchronous */
  float m_temperature; /**<  argon temperature */
  short m_particleID;  /**<  particle ID, e=1, pi=2, mu=3 */
  float m_ebeam;       /**<  beam energy (unit=GeV) */
  float m_cryoX;       /**< impact position X (unit=cm) */
  float m_tableY;      /**< impact position Y (unit=cm) */
  
};
CLASS_DEF(LArTBRun,2786,0)
#endif // LARTBEVENT_LARTBRun_H


