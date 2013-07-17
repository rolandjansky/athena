/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/*
 * ZdcByteStreamRawData.h
 *
 *  Created on: Jun 29, 2009
 *      Author: leite
 *
 *
 */

#ifndef ZDCBYTESTREAMRAWDATA_H_
#define ZDCBYTESTREAMRAWDATA_H_

#include <map>
#include <string>

#include "AthenaBaseComps/AthAlgorithm.h"
#include "DataModel/DataVector.h"

class ISvcLocator;
class StatusCode;

class ZdcDigits;
class ZdcDigitsCollection;

class ZdcByteStreamRawData: public AthAlgorithm
{
public:
	ZdcByteStreamRawData(const std::string& name, ISvcLocator* pSvcLocator);
	virtual ~ZdcByteStreamRawData();

	virtual StatusCode initialize();
	virtual StatusCode execute();
	virtual StatusCode finalize();

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

#endif /* ZDCBYTESTREAMRAWDATA_H_ */
