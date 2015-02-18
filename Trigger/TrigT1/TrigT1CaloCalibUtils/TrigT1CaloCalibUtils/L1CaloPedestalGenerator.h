/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRIGT1CALOCALIBUTILS_L1CALOPEDESTALGENERATOR_H
#define TRIGT1CALOCALIBUTILS_L1CALOPEDESTALGENERATOR_H

#include <map>

#include "AthenaBaseComps/AthAlgorithm.h"

#include "CaloIdentifier/CaloIdManager.h"
#include "CaloIdentifier/CaloLVL1_ID.h"

#include "TrigT1CaloEvent/TriggerTower.h"
#include "TrigT1CaloUtils/InternalTriggerTower.h"
#include "TrigT1CaloUtils/TriggerTowerKey.h"


/**
 *  Algorithm to produce TriggerTowers filled with
 *  simulated pedestal runs
 *
 *  @author Damien Prieur <damien.prieur@cern.ch>
 */

class L1CaloPedestalGenerator : public AthAlgorithm {

public:
    L1CaloPedestalGenerator(const std::string& name, ISvcLocator* pSvcLocator);
    virtual ~L1CaloPedestalGenerator();

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

    // Properties
    std::string m_TTLocation;

    double m_pedMean;
    double m_pedRMS;
    unsigned int m_nSamples;

	// Trigger tower key provider
	LVL1::TriggerTowerKey* m_towerKey;

    std::map<unsigned int, LVL1::InternalTriggerTower*>* m_IntTTContainer;

    LVL1::InternalTriggerTower* findInternalTriggerTower(const Identifier& towerId, bool bCreate = true);
    LVL1::InternalTriggerTower* findInternalTriggerTower(double tt_phi, double tt_eta, unsigned int key, bool bCreate  = true);

    double IDeta(const Identifier& id, const CaloLVL1_ID* l1id);
    double IDphi(const Identifier& id, const CaloLVL1_ID* l1id);

};

#endif
