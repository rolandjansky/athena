/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

/*
 * ZdcByteStreamTester.h
 *
 *  Created on: Jun 29, 2009
 *      Author: leite
 *
 *
 */

#ifndef ZDCBYTESTREAMTESTER_H_
#define ZDCBYTESTREAMTESTER_H_

#include <map>
#include <string>

#include "AthenaBaseComps/AthAlgorithm.h"
#include "AthContainers/DataVector.h"

class ISvcLocator;
class StatusCode;

class ZdcDigits;
class ZdcDigitsCollection;

class ZdcByteStreamTester: public AthAlgorithm
{
public:
	ZdcByteStreamTester(const std::string& name, ISvcLocator* pSvcLocator);

	virtual StatusCode execute() override;

private:
	typedef std::map<unsigned int, const ZdcDigits*> ZdcDigitsMap;

	void printVec(const std::vector<int>& vec) const;
	void printZdcDigits() const;

	/// ZDC Container and collection
	/// StoreGate key
	std::string m_ZdcDigitsLocation;
	std::string m_ZdcDigitsCollectionLocation;

	void setupZdcDigitsMap(const ZdcDigitsCollection* jeCollection);

	///REMOVE Force number of LUT slices
	///REMOVE
	int m_forceSlicesLut;
	///REMOVE Force number of FADC slices
	///REMOVE
	int m_forceSlicesFadc;
	ZdcDigitsMap m_ZdcDigitsMap;
};

#endif /* ZDCBYTESTREAMTESTER_H_ */
