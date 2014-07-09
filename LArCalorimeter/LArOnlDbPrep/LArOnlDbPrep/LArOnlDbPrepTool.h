/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef LARONLDBPREP_LARONLDBPREPTOOL_H
#define LARONLDBPREP_LARONLDBPREPTOOL_H

#include "GaudiKernel/AlgTool.h"
#include "LArOnlDbPrep/ILArOnlDbPrepTool.h"
#include "LArOnlDbPrep/LArOnlCalibDataContainer.h"
#include "LArOnlDbPrep/LArOnlEnergyDataContainer.h"

#include "StoreGate/DataHandle.h"
#include "LArTools/LArCablingService.h"
#include "LArIdentifier/LArOnlineID.h"
#include "CaloIdentifier/LArID.h"

#include "CaloTriggerTool/CaloTriggerTowerService.h"

class StoreGateSvc; 
class LArCablingService ;
class CaloTriggerTowerService ;

/**
   @class LArOnlDbPrepTool
   @brief Concrete AlgTool class to prepare online calibration and energy data. 

*/
class LArOnlDbPrepTool: public AlgTool,
		  virtual public ILArOnlDbPrepTool 
{
 public:
  
  /** 
      @brief constructor
  */
  LArOnlDbPrepTool(const std::string& type, 
	     const std::string& name, 
	     const IInterface* parent); 
  
  /**
     @brief destructor 
  */
  virtual ~LArOnlDbPrepTool() {}
  
  /**
     @brief access the database and prepare the calibration and energy constants for online

     This method is called by the LArOnline_Server.py (Athenaeum server).
  */
  virtual StatusCode prepare() ; 

  /**
     @brief print an XML string containing the values of online constants given a vector of FEB IDs

     This method is called by the LArOnline_Client.py (Athenaeum server client).
  */
  virtual StatusCode getXML(const std::vector<unsigned int>& febIds, std::string& XMLoutputString) ; 

  /** 
      @brief initialize method
  */
  virtual StatusCode initialize();
  virtual StatusCode finalize();

 private:

  StoreGateSvc*      m_detStore;
  const LArOnlineID* m_lar_on_id;
  LArCablingService* m_cablingService;

  CaloTriggerTowerService* m_ttService;

  LArOnlCalibDataContainer* m_calibCont;
  LArOnlEnergyDataContainer* m_energyCont;

  std::string m_keyShape; 
  std::string m_keyOFC;
  std::string m_keyRamp; 
  std::string m_keyPedestal;
  std::string m_keyuA2MeV;
  std::string m_keyDAC2uA;
  std::string m_keyMphysOverMcal;
  std::string m_keyHVScaleCorr;
  std::string m_keyDSPThresholds;
  bool m_applyRampIntercept;
  bool m_FCalInMediumGain;
  bool m_EMECIWInMediumGain;

  LArOnlDbPrepTool& operator= (const LArOnlDbPrepTool&);

};

#endif
