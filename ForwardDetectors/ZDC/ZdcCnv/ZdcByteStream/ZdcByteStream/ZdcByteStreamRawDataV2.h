/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
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

#include "AthenaBaseComps/AthAlgorithm.h"
#include "DataModel/DataVector.h"

class ISvcLocator;
class StatusCode;

class ZdcDigits;
class ZdcDigitsCollection;

#include "xAODTrigL1Calo/TriggerTowerContainer.h"

class ZdcByteStreamRawDataV2: public AthAlgorithm
{
public:
	ZdcByteStreamRawDataV2(const std::string& name, ISvcLocator* pSvcLocator);
	virtual ~ZdcByteStreamRawDataV2();

	virtual StatusCode initialize();
	virtual StatusCode execute();
	virtual StatusCode finalize();

private:
	typedef std::map<unsigned int, const xAOD::TriggerTower*> ZdcTriggerTowersMap;

	void printVec(const std::vector<int>& vec) const;
	void printZdcTriggerTowers() const;

	/// ZDC Container and collection
	/// StoreGate key
	std::string m_ZdcTriggerTowerContainerLocation;

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
