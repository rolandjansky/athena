/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRIGT1CALOCALIBUTILS_L1CALOTRIGGERTOWERSELECTOR_H
#define TRIGT1CALOCALIBUTILS_L1CALOTRIGGERTOWERSELECTOR_H

#include "AthenaBaseComps/AthAlgorithm.h"

#include "TrigT1CaloEvent/TriggerTower.h"
#include "TrigT1CaloUtils/TriggerTowerKey.h"

#include <vector>

/**
 *  This algorithm retrieves a TriggerTower container from storegate,
 *  loops through it and stores to an output container only the
 *  trigger toweres matching the slection criteria
 *
 *  @author Damien Prieur <damien.prieur@cern.ch>
 */

class L1CaloTriggerTowerSelector : public AthAlgorithm {

public:
	L1CaloTriggerTowerSelector(const std::string& name, ISvcLocator* pSvcLocator);
	virtual ~L1CaloTriggerTowerSelector();

	virtual StatusCode initialize();
	virtual StatusCode execute();
	virtual StatusCode finalize();

private:
	bool isSelected(double eta, double phi);

    // StoreGate service
    StoreGateSvc* m_storeGate;

    // Trigger tower container StoreGate key
    std::string m_inputTriggerTowerLocation;
    std::string m_outputTriggerTowerLocation;

    // Trigger tower key provider
    LVL1::TriggerTowerKey* m_towerKey;

	std::vector<double> m_vEtaSelectionRange;
	std::vector<double> m_vPhiSelectionRange;

};

#endif
