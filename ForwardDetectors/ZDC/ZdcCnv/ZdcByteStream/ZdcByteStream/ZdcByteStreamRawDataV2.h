/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

/*
 * ZdcByteStreamRawDataV2.h
 *
 *  Created on: Jun 29, 2009
 *      Author: leite
 *
 *
 */

#ifndef ZDCBYTESTREAMRAWDATAV2_H_
#define ZDCBYTESTREAMRAWDATAV2_H_

#include <map>
#include <string>

#include "ZdcByteStream/ZdcDefs.h"
#include "xAODTrigL1Calo/TriggerTowerContainer.h"
#include "AthenaBaseComps/AthReentrantAlgorithm.h"
#include "StoreGate/ReadHandleKey.h"
#include "AthContainers/DataVector.h"

class ISvcLocator;
class StatusCode;

class ZdcDigits;
class ZdcDigitsCollection;

#include "xAODTrigL1Calo/TriggerTowerContainer.h"

class ZdcByteStreamRawDataV2: public AthReentrantAlgorithm
{
public:
	ZdcByteStreamRawDataV2(const std::string& name, ISvcLocator* pSvcLocator);
	virtual ~ZdcByteStreamRawDataV2();

	virtual StatusCode initialize() override;
	virtual StatusCode execute(const EventContext& ctx) const override;
	virtual StatusCode finalize() override;

private:
	typedef std::map<unsigned int, const xAOD::TriggerTower*> ZdcTriggerTowersMap;

	void printVec(const std::vector<int>& vec) const;
	void printZdcTriggerTowers() const;

	/// ZDC Container and collection
	/// StoreGate key
        SG::ReadHandleKey<xAOD::TriggerTowerContainer> m_ZdcTriggerTowerContainerLocation
        { this, "ZdcTriggerTowerContainerLocation", ZdcDefs::ZdcTriggerTowerContainerLocation, "" };

	//void setupZdcTriggerTowersMap(const xAOD::TriggerTowerContainer* ttCollection);

	///REMOVE Force number of LUT slices
	///REMOVE
	int m_forceSlicesLut;
	///REMOVE Force number of FADC slices
	///REMOVE
	int m_forceSlicesFadc;
	//ZdcDigitsMap m_ZdcDigitsMap;
};

#endif /* ZDCBYTESTREAMRAWDATA_H_ */
