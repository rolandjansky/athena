/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRIGT1CALOCALIBUTILS_L1CALOPEDESTALMAKER_H
#define TRIGT1CALOCALIBUTILS_L1CALOPEDESTALMAKER_H

#include <map>

#include "AthenaBaseComps/AthAlgorithm.h"

#include "AthenaKernel/IAthenaOutputStreamTool.h"
#include "RegistrationServices/IIOVRegistrationSvc.h"

#include "CaloIdentifier/CaloIdManager.h"

#include "CaloIdentifier/CaloLVL1_ID.h"

#include "CaloTriggerTool/CaloTriggerTowerService.h"

#include "TrigT1CaloCondSvc/L1CaloCondSvc.h"

#include "TrigT1CaloEvent/TriggerTower.h"
#include "TrigT1CaloUtils/TriggerTowerKey.h"

#include "TrigT1CaloCalibConditions/L1CaloPedestalCumul.h"

/**
 *  Algorithm to process pedestal runs and to
 *  fill the DB with pedestal data
 *  for each trigger tower
 *
 *  @author Damien Prieur <damien.prieur@cern.ch>
 */

class L1CaloPedestalMaker : public AthAlgorithm {

public:
    L1CaloPedestalMaker(const std::string& name, ISvcLocator* pSvcLocator);
    virtual ~L1CaloPedestalMaker();

    virtual StatusCode initialize();
    virtual StatusCode execute();
    virtual StatusCode finalize();

private:

    // StoreGate service
    StoreGateSvc* m_detStore;
    StoreGateSvc* m_storeGate;

    // Id managers
    const CaloIdManager*	m_caloMgr;

    // Id helpers
    const CaloLVL1_ID* m_lvl1Helper;

    // Services
    CaloTriggerTowerService* m_ttSvc;

    L1CaloCondSvc* m_l1CondSvc;

    IIOVRegistrationSvc*  m_regSvc;
    IAthenaOutputStreamTool* m_streamer;

    // Properties
	int m_nref; // number of event used to define ADC window
	double m_rms_cut; // number of sigma

	bool m_bStore2POOL;
	bool m_bStore2COOL;

	unsigned int m_iovBeginRun;
	unsigned int m_iovEndRun;
	unsigned int m_iovBeginEvent;
	unsigned int m_iovEndEvent;

	std::string m_PedestalsKey; //key in storegate
	std::string m_tagID;
	std::string m_streamName;

	std::string m_DBFolder;
	std::string m_DBTag;


    // Trigger tower container StoreGate key
    std::string m_triggerTowerLocation;

    // Trigger tower key provider
    LVL1::TriggerTowerKey* m_towerKey;

    //std::map< unsigned int, L1CaloPedestalCumul> m_mapPedestalCumul; // map of L1CaloPedestal
	//std::map< HWIdentifier, L1CaloPedestalCumul> m_mapPedestalCumul; // map of L1CaloPedestal
	std::map< Identifier, L1CaloPedestalCumul> m_mapPedestalCumul; // map of L1CaloPedestal

	int _lvl1Id_region(double eta);
	int _lvl1Id_eta(double eta);
	int _lvl1Id_phi(double eta, double phi);

};
#endif
